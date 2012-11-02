

typedef struct _dec_worker *DEC_WORKER;

/* struct defined in glib.h */
struct _GString;

/* struct defined in event2 */
struct event_base;
struct evconlistener;
struct bufferevent;

struct _dec_worker{

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

  struct _GString *app_name;

  struct _GString *task_root_dir;
  struct _GString *exe_root_dir;
  
  /* triggler worker's state */
  int32_t state;

  /* for heartbeat */
  struct event* hb_ev;

  /* for count time */
  struct timeval hb_tv;

  /* for trigger new task */
  struct event *task_ev;

  int32_t heartbeat_internal;
};
