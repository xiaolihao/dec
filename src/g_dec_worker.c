#include "g_dec_internal.h"
#include "g_dec_worker.h"



static void dec_worker_heartbeat_cb(evutil_socket_t fd,
			 short event,
			 void *p){

  DEC_WORKER worker=(DEC_WORKER)p;
  char *buf=NULL;

  /* send heartbeat message */
  util_message_packet_create((char**)&buf, (int32_t)COM_W_BEAT, (char*)NULL, (int32_t)0);
  bufferevent_write(worker->bev, buf, COM_HEADER_SIZE);
  free(buf);

  /* reset timer */
  worker->hb_tv.tv_sec = worker->heartbeat_internal;
  worker->hb_tv.tv_usec = 0;
  evtimer_add(worker->hb_ev, &worker->hb_tv);
}


static void dec_worker_task_check_cb(evutil_socket_t fd,
				     short event,
				     void *p){
  DEC_WORKER worker = (DEC_WORKER)p;
  char *buf=NULL;
  
  waitpid(-1, NULL, 0);
  sleep(1);

  /* send idle mesage */
  util_message_packet_create((char**)&buf, COM_W_IDLE, (char*)NULL, (int32_t)0);
  bufferevent_write(worker->bev, buf, COM_HEADER_SIZE);

  worker->state=WORKER_STATE_IDLE;
  free(buf);
}



int dec_worker_net_message_process(DEC_WORKER worker,
				   struct bufferevent *bev,
				   int32_t type,
				   void *data,
				   int32_t size){
  int32_t fd=-1;
  char *buf=NULL;
  char task_path[1024], exe_path[1024];
  pid_t pid=-1;


  fd = bufferevent_getfd(bev);

  switch(worker->state){

    /* register ok, set heartbeat timer and send idle message */
  case WORKER_STATE_REGISTER:
    if(type == COM_S_OK){
      
      /* set timer */
      worker->hb_ev = evtimer_new(worker->base, dec_worker_heartbeat_cb, worker);
      assert(worker->hb_ev);
      
      worker->hb_tv.tv_sec = worker->heartbeat_internal;
      worker->hb_tv.tv_usec = 0;
      evtimer_add(worker->hb_ev, &worker->hb_tv);

      /* send idle mesage */
      util_message_packet_create((char**)&buf, COM_W_IDLE, (char*)NULL, (int32_t)0);
      bufferevent_write(worker->bev, buf, COM_HEADER_SIZE);

      worker->state=WORKER_STATE_IDLE;
      free(buf);
    }
    break;

    /* server get idle message, worker wait to download task file */
  case WORKER_STATE_IDLE:
    if(type == COM_W_TASK){

      worker->state=WORKER_STATE_BUSY;

      /* save file to task dir */
      sprintf(task_path, "%s/%s", worker->task_root_dir->str, worker->app_name->str);
      util_save_file(data, size, task_path, NULL);

      pid=fork();
      if(pid > 0)
	return G_OK;
      
      if(pid == 0){
	sprintf(exe_path, "%s/%s", worker->exe_root_dir->str, worker->app_name->str);
	execl(exe_path, worker->app_name->str, task_path, NULL);
	printf("exe:%s fail\n", exe_path);
	exit(0);
      }
     
    }

    /* no more task on server right now */
    else if(type == COM_W_TASK_NONE||type == COM_S_BUSY){
      sleep(5);

      /* send idle mesage again */
      util_message_packet_create((char**)&buf, COM_W_IDLE, (char*)NULL, (int32_t)0);
      bufferevent_write(worker->bev, buf, COM_HEADER_SIZE);

      worker->state=WORKER_STATE_IDLE;
      free(buf);
    }
    
    break;

  default:
    ;
  }

  return G_OK;
}

static void dec_worker_net_message_read_callback(struct bufferevent *bev,
						 void *p){
  DEC_WORKER worker=(DEC_WORKER)p;
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
    if(dec_worker_net_message_process(worker, bev, type, src_buffer, size) != G_OK){
       free(src_buffer);
       return;
    }
    
    free(src_buffer);
  
    src = bufferevent_get_input(bev);
    len = evbuffer_get_length(src);
  }
}

static void dec_worker_net_event_callback(struct bufferevent *bev, 
					  short what, 
					  void *p){
  DEC_WORKER worker=(DEC_WORKER)p;

  printf("Got an event in net server: %s\n",
	 evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
  
  if (what & (BEV_EVENT_EOF|BEV_EVENT_ERROR)) {
    if (what & BEV_EVENT_ERROR) {
      if (errno)
	perror("connection error\n");
    }

    printf("server error, worker exit\n");
    g_dec_worker_free(worker);
    exit(-1);
  }
}

DEC_WORKER g_dec_worker_init(char *serv_ip,
			     char *serv_port,
			     char *app_name,
			     char *task_root,
			     char *exec_root,
			     int heartbeat_internal){
  evutil_socket_t fds[2];
  struct sockaddr_in sin;
  char *buf=NULL;


  DEC_WORKER worker= (struct _dec_worker*)malloc(sizeof(struct _dec_worker));
  if(!worker)
    return NULL;
  
  
  worker->base=event_base_new();
  if(!worker->base)
    return NULL;

  /* set local event */
  if(evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, fds)!=0)
    return NULL;
  
  worker->local_rfd=fds[0];
  worker->local_wfd=fds[1];

  worker->local_bev=bufferevent_socket_new(worker->base, 
					   worker->local_rfd,
					   BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

  if(!worker->local_bev)
    return NULL;


  bufferevent_setcb(worker->local_bev, 
		    dec_worker_net_message_read_callback,
		    NULL, 
		    NULL,
		    worker);

  bufferevent_enable(worker->local_bev, EV_READ|EV_WRITE);

 
  util_message_packet_create((char**)&buf, COM_W_REGISTER, app_name, strlen(app_name));

  /* set server connection */
  memset(&sin, 0, sizeof(sin));
  if((sin.sin_port=htons(atoi(serv_port))) == 0)
    return NULL;

  if((sin.sin_addr.s_addr=inet_addr(serv_ip)) == 0xffffffff)
    return NULL;

  
  
  if((worker->fd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return NULL;

  if(evutil_make_socket_nonblocking(worker->fd) < 0)
    return NULL;

  if(connect(worker->fd,
	     (struct sockaddr*)&sin,
	     sizeof(sin)) < 0){
    /* connection can be retried later */
    if(errno != EINTR && errno != EINPROGRESS)
      return NULL;
  }
  
  worker->bev=bufferevent_socket_new(worker->base,
				     worker->fd,
				     BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

  if(!worker->bev)
    return NULL;

  bufferevent_setcb(worker->bev,
		    dec_worker_net_message_read_callback,
		    NULL,
		    dec_worker_net_event_callback,
		    worker);

  bufferevent_enable(worker->bev, EV_READ|EV_WRITE);
  
  
  worker->app_name = g_string_new("");
  if(!worker->app_name)
    return NULL;
  
  g_string_append_len(worker->app_name, app_name, strlen(app_name));
  
  /* send register message */
  bufferevent_write(worker->bev, buf, COM_HEADER_SIZE+strlen(app_name));
  worker->state = WORKER_STATE_REGISTER;
  free(buf);

  worker->task_root_dir = g_string_new("");
  worker->exe_root_dir  = g_string_new("");

  if(!worker->task_root_dir||!worker->exe_root_dir)
    return NULL;

  g_string_append_len(worker->task_root_dir, task_root, strlen(task_root));
  g_string_append_len(worker->exe_root_dir, exec_root, strlen(exec_root));

  /* task trigger */
  worker->task_ev=evsignal_new(worker->base, SIGCHLD, dec_worker_task_check_cb, worker);
  if(!worker->task_ev)
    return NULL;
  
  evsignal_add(worker->task_ev, NULL);
  
  worker->heartbeat_internal=heartbeat_internal;
  
  return worker;
}


void g_dec_worker_start(DEC_WORKER worker){
  event_base_dispatch(worker->base);
}

void g_dec_worker_free(DEC_WORKER worker){
  event_base_free(worker->base);  
  g_string_free(worker->task_root_dir, TRUE);
  g_string_free(worker->exe_root_dir,  TRUE);
  g_string_free(worker->app_name, TRUE);
  free(worker);
}
