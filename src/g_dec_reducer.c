#include "g_dec_internal.h"
#include "g_dec_reducer.h"



static void dec_reducer_heartbeat_cb(evutil_socket_t fd,
				     short event,
				     void *p){

  DEC_REDUCER reducer=(DEC_REDUCER)p;
  char *buf=NULL;

  /* send heartbeat message */
  util_message_packet_create((char**)&buf, (int32_t)COM_R_BEAT, (char*)NULL, (int32_t)0);
  bufferevent_write(reducer->bev, buf, COM_HEADER_SIZE);
  free(buf);

  /* reset timer */
  reducer->hb_tv.tv_sec = reducer->heartbeat_internal;
  reducer->hb_tv.tv_usec = 0;
  evtimer_add(reducer->hb_ev, &reducer->hb_tv);
}


static void dec_reducer_net_message_from_worker_read_callback(struct bufferevent *bev,
							      void *p){

  char name[128];
  int32_t data;
  unsigned char len=0;
  char *buf[NET_BUF_SIZE];
  int32_t sz=0, tmp=0;
  FILE *fp=NULL;
  char recv_file[1024];

  bufferevent_read(bev, buf, 1);
  
  len=*buf;
  
  bufferevent_read(bev, buf, len);
  memcpy(name, buf, len);
  name[len]='\0';

  bufferevent_read(bev, buf, 4);
  memcpy(&sz, buf, 4);

  sprintf(recv_file, "./results/%s/%d.zip", name, time(0));
  util_mkdir_with_path("./results/app2");
  fp=fopen(recv_file, "a+");
  assert(fp);
  while(tmp+NET_BUF_SIZE <= sz){
     bufferevent_read(bev, buf, NET_BUF_SIZE);
     fwrite(buf, 1, NET_BUF_SIZE, fp);
     tmp += NET_BUF_SIZE;
  }
  
  bufferevent_read(bev, buf, sz-tmp);
  fwrite(buf, 1, sz-tmp, fp);
  fclose(fp);

  //
  util_uncompress_file_to_dir("./results/app2", recv_file);
  //
  bufferevent_free(bev);
}

static void dec_reducer_accept_incoming_request_callback(struct evconnlistener *listener, 
							 evutil_socket_t fd,
							 struct sockaddr *addr, 
							 int slen, 
							 void *p){

  DEC_REDUCER reducer=(DEC_REDUCER)p;
  struct bufferevent *bev=NULL;
  

  bev=bufferevent_socket_new(reducer->base, 
			     fd,
			     BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS|LEV_OPT_THREADSAFE);

  assert(bev);

  bufferevent_setcb(bev, 
		    dec_reducer_net_message_from_worker_read_callback,
		    NULL, 
		    NULL,
		    reducer);
  
  bufferevent_enable(bev, EV_READ|EV_WRITE);

  printf("----CONNECTING----\nconnection fd:%d\n\n", fd);
}


int dec_reducer_net_message_process(DEC_REDUCER reducer,
				    struct bufferevent *bev,
				    int32_t type,
				    void *data,
				    int32_t size){
  int32_t fd=-1;
  char *buf=NULL;
  char msg[1024];
  int32_t addr_len=0;
  struct sockaddr_storage net_addr;
  char port[128];

  fd = bufferevent_getfd(bev);

  switch(reducer->state){

    /* register ok, set heartbeat timer and send idle message */
  case REDUCER_STATE_REGISTER:
    if(type == COM_S_OK){
      
      /* set timer */
      reducer->hb_ev = evtimer_new(reducer->base, dec_reducer_heartbeat_cb, reducer);
      assert(reducer->hb_ev);
      
      reducer->hb_tv.tv_sec = reducer->heartbeat_internal;
      reducer->hb_tv.tv_usec = 0;
      evtimer_add(reducer->hb_ev, &reducer->hb_tv);

      /* set net listener to recv results packet from worker node */
      addr_len=sizeof(struct sockaddr_storage);
      sprintf(port, "%d", reducer->listen_port);
      if(evutil_parse_sockaddr_port(port, &net_addr, &addr_len)<0){
	int32_t p = reducer->listen_port;
	struct sockaddr_in *sin = &net_addr;
	sin->sin_port = htons(p);
	sin->sin_addr.s_addr = INADDR_ANY;
	sin->sin_family = AF_INET;
	addr_len = sizeof(struct sockaddr_in);
      }
      reducer->net_listener=evconnlistener_new_bind(reducer->base, 
						    dec_reducer_accept_incoming_request_callback,
						    reducer,
						    LEV_OPT_CLOSE_ON_FREE|LEV_OPT_CLOSE_ON_EXEC|LEV_OPT_REUSEABLE|LEV_OPT_THREADSAFE,
						    -1,
						    &net_addr, 
						    addr_len);
      break;
    }
    
    else if(type == COM_S_ERROR){
      strncpy(msg, data, size);
      msg[size]='\0';
      printf("server error: %s\n\n", msg);

      g_dec_reducer_free(reducer);
      exit(-1);
    }
    break;

  default:
    ;
  }

  return G_OK;
}

static void dec_reducer_net_message_from_server_read_callback(struct bufferevent *bev,
						  void *p){
  DEC_REDUCER reducer=(DEC_REDUCER)p;
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
    if(dec_reducer_net_message_process(reducer, bev, type, src_buffer, size) != G_OK){
       free(src_buffer);
       return;
    }
    
    free(src_buffer);
  
    src = bufferevent_get_input(bev);
    len = evbuffer_get_length(src);
  }
}

static void dec_reducer_net_event_from_server_callback(struct bufferevent *bev, 
					   short what, 
					   void *p){
  DEC_REDUCER reducer=(DEC_REDUCER)p;

  printf("Got an event in net server: %s\n",
	 evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
  
  if (what & (BEV_EVENT_EOF|BEV_EVENT_ERROR)) {
    if (what & BEV_EVENT_ERROR) {
      if (errno)
	perror("connection error\n");
    }

    printf("server error, reducer exit\n");
    g_dec_reducer_free(reducer);
    exit(-1);
  }
}

DEC_REDUCER g_dec_reducer_init(char *serv_ip,
			       char *serv_port,
			       char *app_name[],
			       int32_t count,
			       char *my_ip,
			       char *my_port,
			       int heartbeat_internal){
  evutil_socket_t fds[2];
  struct sockaddr_in sin;
  char *buf=NULL;
  char *data=NULL;
  int32_t data_size=0;


  DEC_REDUCER reducer= (struct _dec_reducer*)malloc(sizeof(struct _dec_reducer));
  if(!reducer)
    return NULL;
  
  
  reducer->base=event_base_new();
  if(!reducer->base)
    return NULL;

  /* set local event */
  if(evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, fds)!=0)
    return NULL;
  
  reducer->local_rfd=fds[0];
  reducer->local_wfd=fds[1];

  reducer->local_bev=bufferevent_socket_new(reducer->base, 
					   reducer->local_rfd,
					   BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

  if(!reducer->local_bev)
    return NULL;


  bufferevent_setcb(reducer->local_bev, 
		    NULL,
		    NULL, 
		    NULL,
		    reducer);

  bufferevent_enable(reducer->local_bev, EV_READ|EV_WRITE);
  
  util_reducer_register_data_create(&data, &data_size, my_ip, my_port, app_name, count);
  util_message_packet_create((char**)&buf, COM_R_REGISTER, data, data_size);

  /* set server connection */
  memset(&sin, 0, sizeof(sin));
  if((sin.sin_port=htons(atoi(serv_port))) == 0)
    return NULL;

  if((sin.sin_addr.s_addr=inet_addr(serv_ip)) == 0xffffffff)
    return NULL;

  
  
  if((reducer->fd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return NULL;

  if(evutil_make_socket_nonblocking(reducer->fd) < 0)
    return NULL;

  if(connect(reducer->fd,
	     (struct sockaddr*)&sin,
	     sizeof(sin)) < 0){
    /* connection can be retried later */
    if(errno != EINTR && errno != EINPROGRESS)
      return NULL;
  }
  
  reducer->bev=bufferevent_socket_new(reducer->base,
				     reducer->fd,
				     BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

  if(!reducer->bev)
    return NULL;

  bufferevent_setcb(reducer->bev,
		    dec_reducer_net_message_from_server_read_callback,
		    NULL,
		    dec_reducer_net_event_from_server_callback,
		    reducer);

  bufferevent_enable(reducer->bev, EV_READ|EV_WRITE);
  
  
  /* send register message */
  bufferevent_write(reducer->bev, buf, COM_HEADER_SIZE+data_size);
  reducer->state = REDUCER_STATE_REGISTER;
  free(buf);
  free(data);

  reducer->heartbeat_internal=heartbeat_internal;
  
  reducer->listen_port=atoi(my_port);
  reducer->net_listener=NULL;
  
  return reducer;
}


void g_dec_reducer_start(DEC_REDUCER reducer){
  event_base_dispatch(reducer->base);
}

void g_dec_reducer_free(DEC_REDUCER reducer){
  event_base_free(reducer->base);  
  evconnlistener_free(reducer->net_listener);
  free(reducer);
}
