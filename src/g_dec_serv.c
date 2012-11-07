#include "g_dec_internal.h"
#include "g_dec_serv.h"


int dec_server_reducer_connection_padding(DEC_REDUCER_CONNECTION conn,
					  char *data,
					  int32_t size){

  int32_t offset=0;
  unsigned char sz=0, cnt=0;
  struct _GString *app=NULL;

  /* ip data */
  if(offset+1 > size)
    return G_ERROR;

  sz = *data;
  ++offset;
  
  if(offset+sz > size)
    return G_ERROR;
  
  g_string_append_len(conn->ip, data+offset, sz);
  offset += sz;

  /* port data */
  if(offset+1 > size)
    return G_ERROR;

  sz = *(data+offset);
  ++offset;

  if(offset+sz>size)
    return G_ERROR;
  g_string_append_len(conn->port, data+offset, sz);
  offset += sz;

  /* app data */
  if(offset+1>size)
    return G_ERROR;

  cnt=*(data+offset);
  ++offset;

  conn->count=cnt;

  while(cnt-->0){
    
    if(offset+1>size)
      return G_ERROR;
    
    sz = *(data+offset);
    ++offset;

    if(offset+sz>size)
      return G_ERROR;
    
    app = g_string_new("");
    g_string_append_len(app, data+offset, sz);
    offset += sz;

    g_ptr_array_add(conn->app_name_array, app);
    
  }

  return G_OK;
}

DEC_REDUCER_CONNECTION dec_server_reducer_connection_init(struct bufferevent *bev,
							  int32_t fd,
							  DEC_SERVER server){
  DEC_REDUCER_CONNECTION conn=NULL;
  
  conn = (DEC_REDUCER_CONNECTION)malloc(sizeof(struct _dec_reducer_connection));
  if(!conn)
    return NULL;

  conn->fd = fd;
  conn->bev=bev;
  conn->server=server;
  conn->heartbeat=time(0);
  conn->port=g_string_new("");
  conn->ip=g_string_new("");
  if(!conn->ip||!conn->port){
    free(conn);
    return NULL;
  }

  conn->app_name_array=g_ptr_array_new();
  if(!conn->app_name_array){
    g_string_free(conn->ip, TRUE);
    g_string_free(conn->port, TRUE);
    free(conn);
    return NULL;
  }

  return conn;
}

void dec_server_reducer_connection_display(DEC_REDUCER_CONNECTION conn){
  int idx=0;

  printf("----COM_R_REGISTRE---\n");
  printf("ip\t\t\t%s\nport\t\t\t%s\n", conn->ip->str, conn->port->str);
  printf("app_name\t\t");
  for(; idx<conn->count; ++idx){
    printf("%s", ((struct _GString*)g_ptr_array_index(conn->app_name_array, idx))->str);
    if(idx<conn->count-1)
      printf(",");
  }
  printf("\n\n");
}

void dec_server_reducer_connection_free(DEC_REDUCER_CONNECTION conn){

  int idx=0;
  bufferevent_disable(conn->bev, EV_READ|EV_WRITE);
  bufferevent_free(conn->bev);
  g_string_free(conn->ip, TRUE);
  g_string_free(conn->port, TRUE);
  for(; idx<conn->count; ++idx)
    g_string_free(g_ptr_array_index(conn->app_name_array, idx), TRUE);
  g_ptr_array_free(conn->app_name_array, TRUE);
  free(conn);
}

DEC_WORKER_CONNECTION dec_server_worker_connection_init(struct bufferevent *bev,
							int32_t fd,
							DEC_SERVER server){
  DEC_WORKER_CONNECTION conn=NULL;

  conn=(DEC_WORKER_CONNECTION)malloc(sizeof(struct _dec_worker_connection));
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

void dec_server_worker_connection_free(DEC_WORKER_CONNECTION conn){

  bufferevent_disable(conn->bev, EV_READ|EV_WRITE);
  bufferevent_free(conn->bev);
  g_string_free(conn->app_name, TRUE);
  free(conn);
}


void dec_server_hash_table_int_key_add(struct _GHashTable *hash_table,
				       int32_t key,
				       void* value){
 
  /* MUST use dynamic memory */
  int32_t *tmp=NULL;
  tmp = (int32_t*)malloc(sizeof(int32_t));

  assert(tmp);

  *tmp = key;
  g_hash_table_insert(hash_table, tmp, value);
}


void dec_server_key_simple_destroy(gpointer key){
  free(key);
}

void dec_server_fd2worker_value_destroy(gpointer value){
  dec_server_worker_connection_free((DEC_WORKER_CONNECTION)value);
}

void dec_server_fd2reducer_value_destroy(gpointer value){
  dec_server_reducer_connection_free((DEC_REDUCER_CONNECTION)value);
}

gboolean _dec_server_worker_state_check(gpointer key,
					gpointer value,
					gpointer p){
  
   DEC_WORKER_CONNECTION conn=(DEC_WORKER_CONNECTION)value;
   
   /* if timeout, delete the connection */
   if(time(0)-conn->heartbeat >= conn->server->connection_max_timeout)
     return TRUE;
   else
     return FALSE;
}

gboolean _dec_server_reducer_state_check(gpointer key,
					 gpointer value,
					 gpointer p){
  
   DEC_REDUCER_CONNECTION conn=(DEC_REDUCER_CONNECTION)value;
   int32_t idx=0;
   struct _GQueue *queue=NULL;

   /* if timeout, delete the connection */
   if(time(0)-conn->heartbeat >= conn->server->connection_max_timeout){
     for(idx=0; idx<conn->count; ++idx){
       queue=g_hash_table_lookup(conn->server->app2reducer_queue, 
				 ((struct _GString*)g_ptr_array_index(conn->app_name_array, idx))->str);
       /* remove from queue */
       if(queue)
	 g_queue_remove(queue, conn);

     }
     return TRUE;
   }
   else
     return FALSE;
}

static void dec_server_state_check_cb(evutil_socket_t fd,
				      short event,
				      void *p){

  DEC_SERVER server=(DEC_SERVER)p;
  
  g_hash_table_foreach_remove(server->fd2worker, _dec_server_worker_state_check, server);
  g_hash_table_foreach_remove(server->fd2reducer, _dec_server_reducer_state_check, server);

  /* reset timer */
  server->st_tv.tv_sec = server->connection_check_internal;
  server->st_tv.tv_usec = 0;
  evtimer_add(server->st_ev, &server->st_tv);
}


DEC_REDUCER_CONNECTION dec_server_high_priority_reducer_get(DEC_WORKER_CONNECTION worker_conn){
  DEC_REDUCER_CONNECTION reducer_conn=NULL;
  struct _GQueue *queue=NULL;
  int idx=0, len=0;

  queue = g_hash_table_lookup(worker_conn->server->app2reducer_queue, worker_conn->app_name->str);
  if(!queue)
    return NULL;
  
  len = g_queue_get_length(queue);
  idx = rand()%len;
  
  return g_queue_peek_nth(queue, idx);
}

static void dec_server_send_task(DEC_WORKER_CONNECTION conn){

  char task_path[1024]={0}, task_file[1024]={0};
  char *file_name=NULL;
  char *data=NULL, *tmp=NULL;
  int32_t size=0, offset=0;
  unsigned char len=0;
  char *buf=NULL;
  DEC_REDUCER_CONNECTION reducer_conn=NULL;

  sprintf(task_path, "%s/%s", conn->server->task_root_dir->str, conn->app_name->str);

  /* no more task can be send */
  if(util_fetch_single_file(&file_name, task_path) != G_OK){
    util_message_packet_create((char**)&buf, COM_W_TASK_NONE, (char*)NULL, (int32_t)0);

    bufferevent_write(conn->bev, buf, COM_HEADER_SIZE);
    free(buf);

    printf("no more task for fd:%d\n\n", conn->fd);
    return;
  }

  reducer_conn=dec_server_high_priority_reducer_get(conn);
  if(!reducer_conn){
    /* no reducer */
    util_message_packet_create((char**)&buf, 
			       (int32_t)COM_S_ERROR, 
			       (char*)COM_S_ERROR_MSG_NO_REDUCER, 
			       (int32_t)strlen(COM_S_ERROR_MSG_NO_REDUCER));
    bufferevent_write(conn->bev, buf, COM_HEADER_SIZE+strlen(COM_S_ERROR_MSG_NO_REDUCER));
    free(buf);

    printf("no reducer for %s is registered\n", conn->app_name->str);
    return;
  }

  
  sprintf(task_file, "%s/%s", task_path,  file_name);
  util_load_file(&tmp, &size, task_file);
  

  data = malloc(size+strlen(reducer_conn->ip->str)+strlen(reducer_conn->port->str)+2);
  assert(data);

  /* add reucer ip and port first */
  len = strlen(reducer_conn->ip->str);
  memcpy(data, &len, 1);
  offset++;
  
  memcpy(data+offset, reducer_conn->ip->str, len);
  offset += len;

  len=strlen(reducer_conn->port->str);
  memcpy(data+offset, &len, 1);
  offset++;
  
  memcpy(data+offset, reducer_conn->port->str, len);
  offset += len;

  /*padding with task file data */
  memcpy(data+offset, tmp, size);
  
  printf("send:\"%s\" to fd:%d\n\n", task_file, conn->fd);

  /* send task file */
  util_message_packet_create(&buf, (int32_t)COM_W_TASK, data, size+offset);

  bufferevent_write(conn->bev, buf, COM_HEADER_SIZE+size+offset);
  remove(task_file);

  if(tmp)
    free(tmp);
  
  if(data)
    free(data);

  if(buf)
  free(buf);

}

static void dec_server_task_check_cb(evutil_socket_t fd,
				     short event,
				     void *p){

  DEC_SERVER server=(DEC_SERVER)p;
  DEC_WORKER_CONNECTION conn=NULL;


  int pid=-1;
  while((pid=waitpid(-1, NULL, 0)) > 0){

    conn=(DEC_WORKER_CONNECTION)g_hash_table_lookup(server->pid2worker, &pid);
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
  DEC_REDUCER_CONNECTION conn=NULL;
  struct _GQueue *queue=NULL;
  int32_t idx=0;

  fd = bufferevent_getfd(bev);
  printf("Got an event in net server: %s\n",
	 evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
  
  if (what & (BEV_EVENT_EOF|BEV_EVENT_ERROR)) {
    if (what & BEV_EVENT_ERROR) {
      if (errno)
	perror("connection error");
    }

    /* worker node error */
    if(g_hash_table_contains(server->fd2worker, &fd))
      g_hash_table_remove(server->fd2worker, &fd);

    /* reducer node error */
    if(g_hash_table_contains(server->fd2reducer, &fd)){
      conn=g_hash_table_lookup(server->fd2reducer, &fd);

      for(idx=0; idx<conn->count; ++idx){
	queue=g_hash_table_lookup(server->app2reducer_queue, 
				  ((struct _GString*)g_ptr_array_index(conn->app_name_array, idx))->str);
	/* remove from queue */
	if(queue)
	  g_queue_remove(queue, conn);
      }

      g_hash_table_remove(server->fd2reducer, &fd);
    }
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
  DEC_WORKER_CONNECTION worker_conn=NULL;
  DEC_REDUCER_CONNECTION reducer_conn=NULL;
  int32_t fd=-1;
  char *buf=NULL;
  char task_path[1024]={0}, exe_path[1024]={0}, task_file[1024]={0};
  pid_t pid=-1;
  int32_t *pid_key=NULL;
  char name[1024]={0};
  int32_t idx=0;
  struct _GQueue *queue=NULL;


  fd = bufferevent_getfd(bev);

  switch(type){

    /* worker register */
  case COM_W_REGISTER:
    if(size == 0 || data == NULL)
      return G_OK;

    /* check exe existence */
    strncpy(name, (char*)data, size);
    name[size]='\0';
    sprintf(exe_path, "%s/%s_create", server->exe_root_dir->str, name);

    if(util_file_existence(exe_path) != G_OK){
      
      /* no exe file on server */
      util_message_packet_create((char**)&buf, (int32_t)COM_S_ERROR, (char*)COM_S_ERROR_MSG_NO_EXE, (int32_t)strlen(COM_S_ERROR_MSG_NO_EXE));
      bufferevent_write(bev, buf, COM_HEADER_SIZE+strlen(COM_S_ERROR_MSG_NO_EXE));
      free(buf);

      printf("----COM_W_REGISTRE---\napp_name:%s_create\t\t[ERROR]\n\n", name);
      break;
    }
    
    /* create a new connection and add to worker table */
    worker_conn=dec_server_worker_connection_init(bev, fd, server);
    assert(worker_conn);
    dec_server_hash_table_int_key_add(server->fd2worker, fd, worker_conn);
    
    /* get application name */
    g_string_append_len(worker_conn->app_name, (char*)data, size);
    printf("----COM_W_REGISTRE---\napp_name:%s_create\t\t [OK]\n\n", worker_conn->app_name->str);
    
    /* send back a confirm information */
    util_message_packet_create((char**)&buf, (int32_t)COM_S_OK, (char*)NULL, (int32_t)0);
    bufferevent_write(bev, buf, COM_HEADER_SIZE);
    free(buf);
    break;

    /* reducer register */
  case COM_R_REGISTER:
    if(size == 0 || data == NULL)
      return G_OK;
    reducer_conn = dec_server_reducer_connection_init(bev, fd, server);
    assert(reducer_conn);
    
    if(dec_server_reducer_connection_padding(reducer_conn, data, size) != G_OK){
      dec_server_reducer_connection_free(reducer_conn);
      
      util_message_packet_create((char**)&buf, 
				 (int32_t)COM_S_ERROR, 
				 (char*)COM_S_ERROR_MSG_FAIL_EX_MSG, 
				 (int32_t)strlen(COM_S_ERROR_MSG_FAIL_EX_MSG));
      bufferevent_write(bev, buf, COM_HEADER_SIZE+strlen(COM_S_ERROR_MSG_FAIL_EX_MSG));
      free(buf);

      printf("----COM_R_REGISTRE---\nextract register msg\t\t[ERROR]\n\n");
      break;
    }
    
    dec_server_reducer_connection_display(reducer_conn);
    dec_server_hash_table_int_key_add(server->fd2reducer, fd, reducer_conn);

    for(; idx<reducer_conn->count; ++idx){
      queue=g_hash_table_lookup(server->app2reducer_queue,
				((struct _GString*)g_ptr_array_index(reducer_conn->app_name_array, idx))->str);
      
      /* queue for this app is existence */
      if(queue){
	g_queue_push_tail(queue, reducer_conn);
      }
      
      /* not existence */
      else{
	queue = g_queue_new();
	assert(queue);
	g_hash_table_insert(server->app2reducer_queue, 
			    ((struct _GString*)g_ptr_array_index(reducer_conn->app_name_array, idx))->str,
			    queue);
	g_queue_push_tail(queue, reducer_conn);
      }
    }

    /* send back a confirm information */
    util_message_packet_create((char**)&buf, (int32_t)COM_S_OK, (char*)NULL, (int32_t)0);
    bufferevent_write(bev, buf, COM_HEADER_SIZE);
    free(buf);

    break;

  case COM_W_BEAT:
    printf("----COM_W_BEAT---\nbeat fd:%d\n\n", fd);
   
    worker_conn=(DEC_WORKER_CONNECTION)g_hash_table_lookup(server->fd2worker, &fd);
    if(worker_conn)
      worker_conn->heartbeat=time(0);
    break;

  case COM_R_BEAT:
    printf("----COM_R_BEAT---\nbeat fd:%d\n\n", fd);
   
    reducer_conn=(DEC_REDUCER_CONNECTION)g_hash_table_lookup(server->fd2reducer, &fd);
    if(reducer_conn)
      reducer_conn->heartbeat=time(0);
    break;

    /* create task file and send to worker */
  case COM_W_IDLE:
   
    worker_conn=(DEC_WORKER_CONNECTION)g_hash_table_lookup(server->fd2worker, &fd);
    if(worker_conn){
      printf("----COM_W_IDLE---\nfd:%d\n\n", worker_conn->fd);
      sprintf(task_path, "%s/%s", server->task_root_dir->str, worker_conn->app_name->str);

      /* read old task first */
      if(util_dir_empty(task_path) != G_OK){
	dec_server_send_task(worker_conn);
	return G_OK;
      }
      sprintf(name, "%s_create", worker_conn->app_name->str);
      sprintf(exe_path, "%s/%s", server->exe_root_dir->str, name);
      sprintf(task_file, "%s/task_%d_%d", task_path, worker_conn->fd, worker_conn->task_cnt++);

      
      
      /* create a directory if it doesn't already exist. create intermediate parent directories as needed, too. */
      if(util_mkdir_with_path(task_path) == 0){
	pid = -1;

	pid=fork();
	if(pid == -1){
	  /* send server busy message to worker */
	  util_message_packet_create((char**)&buf, (int32_t)COM_S_BUSY, (char*)NULL, (int32_t)0);
	  bufferevent_write(worker_conn->bev, buf, COM_HEADER_SIZE);
	  free(buf);
	  return G_OK;
	}
		
	/* in parent process */
	if(pid > 0){

	  pid_key = (int32_t*)malloc(sizeof(int32_t));
	  *pid_key=pid;

	  /* add pid to trigger table */
	  g_hash_table_insert(server->pid2worker, pid_key, worker_conn);
	  return G_OK;
	}


	/* execute task create program in child process */
	if(pid == 0){	  
	  execl(exe_path, name, task_file, NULL);
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
					    dec_server_key_simple_destroy,
					    dec_server_fd2worker_value_destroy);

  server->pid2worker = g_hash_table_new_full(g_int_hash, 
					     g_int_equal,
					     dec_server_key_simple_destroy,
					     NULL);/* can't delete connnection */

  server->fd2reducer = g_hash_table_new_full(g_int_hash,
					     g_int_equal,
					     dec_server_key_simple_destroy,
					     dec_server_fd2reducer_value_destroy);
  server->app2reducer_queue = g_hash_table_new_full(g_str_hash,
						    g_str_equal,
						    dec_server_key_simple_destroy,
						    NULL);
  if(!server->fd2worker  ||
     !server->pid2worker ||
     !server->fd2reducer ||
     !server->app2reducer_queue)
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
