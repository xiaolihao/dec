#include "g_dec_internal.h"
#include "g_dec_serv.h"



DEC_SERVER_CONNECTION dec_server_connection_init(struct bufferevent *bev,
						 int32_t fd,
						 DEC_SERVER server){
  DEC_SERVER_CONNECTION conn=NULL;

  conn=(DEC_SERVER_CONNECTION)malloc(sizeof(struct _dec_server_connection));
  if (!conn)
    return NULL;

  conn->fd=fd;
  conn->server=server;
  conn->bev=bev;
  conn->heartbeat=time(0);
  conn->task_cnt=0;

  conn->app_name=g_string_new("");
  if(!conn->app_name){
    free(conn);
    return NULL;
  }
  return conn;
}

void dec_server_connection_free(DEC_SERVER_CONNECTION conn){

  bufferevent_disable(conn->bev, EV_READ|EV_WRITE);
  bufferevent_free(conn->bev);
  g_string_free(conn->app_name, TRUE);
  free(conn);
}

void dec_server_worker_add_by_fd(DEC_SERVER server,
				 int32_t fd,
				 DEC_SERVER_CONNECTION conn){
 
  /* MUST use dynamic memory */
  int32_t *key=NULL;
  key = (int32_t*)malloc(sizeof(int32_t));

  assert(key);

  *key = fd;
  g_hash_table_insert(server->fd2worker, key, conn);
}


void dec_server_int_key_destroy(gpointer key){
  free(key);
}

void dec_server_fd2worker_value_destroy(gpointer value){
  dec_server_connection_free((DEC_SERVER_CONNECTION)value);
}

gboolean _dec_server_state_check(gpointer key,
		  gpointer value,
		  gpointer p){
  
   DEC_SERVER_CONNECTION conn=(DEC_SERVER_CONNECTION)value;
   
   /* if timeout, delete the connection */
   if(time(0)-conn->heartbeat >= conn->server->connection_max_timeout)
     return TRUE;
   else
     return FALSE;
}

static void dec_server_state_check_cb(evutil_socket_t fd,
			   short event,
			   void *p){

  DEC_SERVER server=(DEC_SERVER)p;
  
  g_hash_table_foreach_remove(server->fd2worker, _dec_server_state_check, server);

  /* reset timer */
  server->st_tv.tv_sec = server->connection_check_internal;
  server->st_tv.tv_usec = 0;
  evtimer_add(server->st_ev, &server->st_tv);
}

static void dec_server_send_task(DEC_SERVER_CONNECTION conn){

  char task_path[1024]={0}, task_file[1024]={0};
  char *file_name=NULL;
  char *data=NULL;
  int32_t size=0;
  char *buf=NULL;

  sprintf(task_path, "%s/%s", conn->server->task_root_dir->str, conn->app_name->str);

  /* no more task can be send */
  if(util_fetch_single_file(&file_name, task_path) != G_OK){
    util_message_packet_create((char**)&buf, COM_W_TASK_NONE, (char*)NULL, (int32_t)0);

    bufferevent_write(conn->bev, buf, COM_HEADER_SIZE);
    free(buf);

    printf("no more task for fd:%d\n\n", conn->fd);
    return;
  }

  sprintf(task_file, "%s/%s", task_path,  file_name);
  util_load_file(&data, &size, task_file);
  
  printf("send:\"%s\" to fd:%d\n\n", task_file, conn->fd);

  /* send task file */
  util_message_packet_create(&buf, (int32_t)COM_W_TASK, data, size);

  bufferevent_write(conn->bev, buf, COM_HEADER_SIZE+size);
  remove(task_file);

  
  if(data)
    free(data);

  if(buf)
  free(buf);

}

static void dec_server_task_check_cb(evutil_socket_t fd,
			   short event,
			   void *p){

  DEC_SERVER server=(DEC_SERVER)p;
  DEC_SERVER_CONNECTION conn=NULL;


  int pid=-1;
  while((pid=waitpid(-1, NULL, 0)) > 0){

    conn=(DEC_SERVER_CONNECTION)g_hash_table_lookup(server->pid2worker, &pid);
    if(!conn)
      break;

    dec_server_send_task(conn);

    /* remove pid from pid2worker */
    g_hash_table_remove(server->pid2worker, &pid);
  }
}

static void dec_server_net_event_callback(struct bufferevent *bev, 
					  short what, 
					  void *p){
  int fd=-1;
  DEC_SERVER server=(DEC_SERVER)p;

  fd = bufferevent_getfd(bev);
  printf("Got an event in net server: %s\n",
	 evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
  
  if (what & (BEV_EVENT_EOF|BEV_EVENT_ERROR)) {
    if (what & BEV_EVENT_ERROR) {
      if (errno)
	perror("connection error");
    }

    g_hash_table_remove(server->fd2worker, &fd);
  }
}

static void dec_server_net_message_read_callback(struct bufferevent *bev,
						  void *p){
  DEC_SERVER server=(DEC_SERVER)p;
  struct evbuffer *src=NULL;
  size_t len=0;
  int32_t size=0, type=0;
  unsigned char *src_buffer=NULL, head[12]={0};
  

  src=bufferevent_get_input(bev);
  len=evbuffer_get_length(src);

  while(len>=COM_HEADER_SIZE){

    /* read the package header */
    if(COM_HEADER_SIZE!=bufferevent_read(bev, head, COM_HEADER_SIZE))
      return;

    type=*(int32_t*)head;
   
    /* skip 4 bytes' padding */
    size=*(int32_t*)((char*)head+2*sizeof(int32_t));
   
    printf("recv net message from fd:%d, ", bufferevent_getfd(bev));
    printf("total size:%d, command type:%d, data size:%d\n", (int)len, (int)type, (int)size);
    if(size > (int32_t)len)
      return;

    
    if(size > 0){
      /* read the command body */
      src_buffer=(unsigned char*)malloc((size+1)*sizeof(char));
      memset(src_buffer, 0, size+1);
      if (size!=(int32_t)bufferevent_read(bev, src_buffer, size*sizeof(char))){
	free(src_buffer);
	return;
      }
    }

    /* process the command */
    if(dec_server_net_message_process(server, bev, type, src_buffer, size) != G_OK){
       free(src_buffer);
       return;
    }
    
    free(src_buffer);
  
    src = bufferevent_get_input(bev);
    len = evbuffer_get_length(src);
  }
}



int dec_server_net_message_process(DEC_SERVER server,
				    struct bufferevent *bev,
				    int32_t type,
				    void *data,
				    int32_t size){
  DEC_SERVER_CONNECTION conn=NULL;
  int32_t fd=-1;
  char *buf=NULL;
  char task_path[1024]={0}, exe_path[1024]={0}, task_file[1024]={0};
  pid_t pid=-1;
  int32_t *pid_key=NULL;
 
  fd = bufferevent_getfd(bev);

  switch(type){

  case COM_W_REGISTER:
    if(size == 0 || data == NULL)
      return G_OK;
    /* create a new connection and add to worker table */
    conn=dec_server_connection_init(bev, fd, server);
    assert(conn);
    dec_server_worker_add_by_fd(server, fd, conn);
    
    /* get application name */
    g_string_append_len(conn->app_name, (char*)data, size);
    printf("----COM_W_REGISTRE---\napp_name:%s\n\n", conn->app_name->str);

    /* send back a confirm information */
    util_message_packet_create((char**)&buf, (int32_t)COM_S_OK, (char*)NULL, (int32_t)0);
    bufferevent_write(bev, buf, COM_HEADER_SIZE);
    free(buf);
    break;

  case COM_W_BEAT:
    printf("----COM_W_BEAT---\nbeat fd:%d\n\n", fd);
   
    conn=(DEC_SERVER_CONNECTION)g_hash_table_lookup(server->fd2worker, &fd);
    if(conn)
      conn->heartbeat=time(0);
    break;

    /* create task file and send to worker */
  case COM_W_IDLE:
   
    conn=(DEC_SERVER_CONNECTION)g_hash_table_lookup(server->fd2worker, &fd);
    if(conn){
      printf("----COM_W_IDLE---\nfd:%d\n\n", conn->fd);
      sprintf(task_path, "%s/%s", server->task_root_dir->str, conn->app_name->str);

      /* read old task first */
      if(util_dir_empty(task_path) != G_OK){
	dec_server_send_task(conn);
	return G_OK;
      }

      sprintf(exe_path, "%s/%s", server->exe_root_dir->str, conn->app_name->str);
      sprintf(task_file, "%s/task_%d_%d", task_path, conn->fd, conn->task_cnt++);

      
      
      /* create a directory if it doesn't already exist. create intermediate parent directories as needed, too. */
      if(util_mkdir_with_path(task_path) == 0){
	pid = -1;

	pid=fork();
	if(pid == -1){
	  /* send server busy message to worker */
	  util_message_packet_create((char**)&buf, (int32_t)COM_S_BUSY, (char*)NULL, (int32_t)0);
	  bufferevent_write(conn->bev, buf, COM_HEADER_SIZE);
	  free(buf);
	  return G_OK;
	}
		
	/* in parent process */
	if(pid > 0){

	  pid_key = (int32_t*)malloc(sizeof(int32_t));
	  *pid_key=pid;

	  /* add pid to trigger table */
	  g_hash_table_insert(server->pid2worker, pid_key, conn);
	  return G_OK;
	}


	/* execute task create program in child process */
	if(pid == 0){	  
	  execl(exe_path, conn->app_name->str, task_file, NULL);
	  exit(0);
	}
      }
    }
    break;
    
  default:
    return G_ERROR;
  }
  
  return G_OK;
}



static void dec_server_accept_incoming_request_callback(struct evconnlistener *listener, 
							evutil_socket_t fd,
							struct sockaddr *addr, 
							int slen, 
							void *p){
  DEC_SERVER server=(DEC_SERVER)p;
  struct bufferevent *bev=NULL;
  

  bev=bufferevent_socket_new(server->base, 
			     fd,
			     BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS|LEV_OPT_THREADSAFE);

  assert(bev);

  bufferevent_setcb(bev, 
		    dec_server_net_message_read_callback,
		    NULL, 
		    dec_server_net_event_callback,
		    server);
  
  bufferevent_enable(bev, EV_READ|EV_WRITE);

  printf("----CONNECTING----\nconnection fd:%d\n\n", fd);
}


DEC_SERVER g_dec_server_init(char *port,
			     char *task_root_dir,
			     char *exec_root_dir,
			     int conn_check_internal,
			     int conn_max_timeout){
  evutil_socket_t fds[2];
  int32_t addr_len=0;

  DEC_SERVER server= (struct _dec_server*)malloc(sizeof(struct _dec_server));
  if(!server)
    return NULL;
  
  
  server->base=event_base_new();
  if(!server->base)
    return NULL;

  /* set local event */
  if(evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, fds)!=0)
    return NULL;
  
  server->local_rfd=fds[0];
  server->local_wfd=fds[1];

  server->local_bev=bufferevent_socket_new(server->base, 
					   server->local_rfd,
					   BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

  if(!server->local_bev)
    return NULL;


  bufferevent_setcb(server->local_bev, 
		    NULL,
		    NULL, 
		    NULL,
		    server);

  bufferevent_enable(server->local_bev, EV_READ|EV_WRITE);

  /* set incoming net event */
  server->net_addr=malloc(sizeof(struct sockaddr_storage));
  if(!server->net_addr)
    return NULL;


  addr_len=sizeof(struct sockaddr_storage);
  if(evutil_parse_sockaddr_port(port,
				 (struct sockaddr*)server->net_addr, &addr_len)<0){
    int32_t p = atoi(port);
    struct sockaddr_in *sin = (struct sockaddr_in*)server->net_addr;
    sin->sin_port = htons(p);
    sin->sin_addr.s_addr = INADDR_ANY;
    sin->sin_family = AF_INET;
    addr_len = sizeof(struct sockaddr_in);
  }
  server->port = atoi(port);
  server->net_listener=evconnlistener_new_bind(server->base, 
					       dec_server_accept_incoming_request_callback,
					       server,
					       LEV_OPT_CLOSE_ON_FREE|LEV_OPT_CLOSE_ON_EXEC|LEV_OPT_REUSEABLE|LEV_OPT_THREADSAFE,
					       -1,
					       (const struct sockaddr*)server->net_addr, 
					       addr_len);

  server->fd2worker = g_hash_table_new_full(g_int_hash, 
					    g_int_equal,
					    dec_server_int_key_destroy,
					    dec_server_fd2worker_value_destroy);

  server->pid2worker = g_hash_table_new_full(g_int_hash, 
					     g_int_equal,
					     dec_server_int_key_destroy,
					     NULL);/* can't delete connnection */
  if(!server->fd2worker||!server->pid2worker)
    return NULL;


  server->connection_check_internal=conn_check_internal;
  server->connection_max_timeout=conn_max_timeout;

  /* set timer */
  server->st_ev = evtimer_new(server->base, dec_server_state_check_cb, server);
  if(!server->st_ev)
    return NULL;

  server->st_tv.tv_sec = server->connection_check_internal;
  server->st_tv.tv_usec = 0;

  evtimer_add(server->st_ev, &server->st_tv);

  /* task trigger */
  server->task_ev=evsignal_new(server->base, SIGCHLD, dec_server_task_check_cb, server);
  if(!server->task_ev)
    return NULL;
  
  evsignal_add(server->task_ev, NULL);


  server->task_root_dir = g_string_new("");
  server->exe_root_dir  = g_string_new("");

  if(!server->task_root_dir||!server->exe_root_dir)
    return NULL;

  g_string_append_len(server->task_root_dir, task_root_dir, strlen(task_root_dir));
  g_string_append_len(server->exe_root_dir, exec_root_dir, strlen(exec_root_dir));

  
  return server;
}


void g_dec_server_start(DEC_SERVER server){
  event_base_dispatch(server->base);
}



void g_dec_server_free(DEC_SERVER server){

  g_string_free(server->task_root_dir, TRUE);
  g_string_free(server->exe_root_dir,  TRUE);

  evconnlistener_free(server->net_listener);
  
  event_base_free(server->base);  

  free(server->net_addr);  

  g_hash_table_destroy(server->fd2worker); 
  g_hash_table_destroy(server->pid2worker); 

  free(server);
}
