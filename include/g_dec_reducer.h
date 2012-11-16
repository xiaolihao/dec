

typedef struct _dec_reducer *DEC_REDUCER;

/* struct defined in glib.h */
struct _GString;

/* struct defined in event2 */
struct event_base;
struct evconlistener;
struct bufferevent;

struct _dec_reducer{

  /* for server connection */
  int32_t fd;
  struct bufferevent *bev;


  struct _GString *serv_ip;
  int32_t serv_port;

  /* event loop manager */
  struct event_base *base;
  

   /* for local msg */
  struct bufferevent *local_bev;
  evutil_socket_t local_rfd;
  evutil_socket_t local_wfd;

  /* for writing log */
  FILE *fp_log;

  int32_t state;
  
  /* for heartbeat */
  struct event* hb_ev;

  /* for count time */
  struct timeval hb_tv;
  int32_t heartbeat_internal;

  int32_t listen_port;
  struct evconlistener *net_listener;

  struct _GString *result_root_dir;
};
