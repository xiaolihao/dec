
/* only works in linux/unix system */
#define evutil_socket_t int

typedef struct _dec_server *DEC_SERVER;
typedef struct _dec_worker_connection *DEC_WORKER_CONNECTION;
typedef struct _dec_reducer_connection *DEC_REDUCER_CONNECTION;


/* struct defined in event2 */
struct event_base;
struct evconlistener;
struct bufferevent;

/* struct defined in glib.h */
struct _GQueue;
struct _GHashTable;
struct _GString;
struct _GPtrArray;

/* one reducer can configure more than one different result reduce program */
struct _dec_reducer_connection{
  
  int fd;
  
  struct bufferevent *bev;
  
  int32_t heartbeat;

  struct _GString *ip;
  
  struct _GString *port;
 
  struct _GPtrArray *app_name_array;
  int32_t count;

  DEC_SERVER server;
};

/* worker node information triggered by event */
struct _dec_worker_connection{
  int fd;

  struct bufferevent *bev;

  DEC_SERVER server;

  int32_t heartbeat;

  struct _GString * app_name;
  
  long long task_cnt;
};

struct _dec_server{

  /* event loop manager */
  struct event_base *base;
  
  /* for local msg */
  struct bufferevent *local_bev;
  evutil_socket_t local_rfd;
  evutil_socket_t local_wfd;

  /* incoming msg listener */
  struct evconlistener *net_listener;
  struct sockaddr_storage *net_addr;
  int port;

  /* map from file description to connection of worker */
  struct _GHashTable *fd2worker;

  /* for writing log */
  FILE *fp_log;

  /* for checking state(heartbeat timeout) of worker and reducer */
  struct event *st_ev;

  /* for count time */
  struct timeval st_tv;

  /* map form child process id to connnection of worker */
  struct _GHashTable *pid2worker;

  /* for trigger new task */
  struct event *task_ev;

  /* new task root dir */
  struct _GString *task_root_dir;

  /* exe root dir */
  struct _GString *exe_root_dir;

  int connection_check_internal;
  int connection_max_timeout;

  /* map from file description to connection of reducer */
  struct _GHashTable *fd2reducer;

  /* map from application name to queue of reducer connection */
  struct _GHashTable *app2reducer_queue;
};
