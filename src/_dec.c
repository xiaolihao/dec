#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "dec.h"


struct _dec_server_cfg{
  
  /* required */
  char task_root_dir[128];
  char exec_root_dir[128];
  char listen_port[128];

  /* optional */
  int connection_check_internal;
  int connection_max_timeout;

};

void dec_server_cfg_init(struct _dec_server_cfg *cfg){
  memset(cfg->task_root_dir, 0, 128);
  memset(cfg->exec_root_dir, 0, 128);
  memset(cfg->listen_port, 0, 128);
  
  cfg->connection_check_internal=10;
  cfg->connection_max_timeout=30;
}


int dec_server_cfg_check(struct _dec_server_cfg cfg){
  if(strlen(cfg.task_root_dir) == 0 ||
     strlen(cfg.exec_root_dir) == 0 ||
     strlen(cfg.listen_port) == 0)
    return -1;
  else
    return 0;
}


void dec_server_cfg_dispaly(struct _dec_server_cfg cfg){
  printf("\n\nDEC SERVER RUNNING CONFIGURE\n-------------------------------------------\n");
  printf("task_root_dir\t\t\t%s\nexec_root_dir\t\t\t%s\nlisten_port\t\t\t%s\nconn_check_internal\t\t%d\nconn_max_timeout\t\t%d\n",
	 cfg.task_root_dir,
	 cfg.exec_root_dir,
	 cfg.listen_port,
	 cfg.connection_check_internal,
	 cfg.connection_max_timeout);
  printf("-------------------------------------------\n\n");
}


struct _dec_worker_cfg{
  
  /* required */
  char task_root_dir[128];
  char exec_root_dir[128];
  char server_port[128];
  char server_ip[128];
  char app_name[128];


  /* optional */
  int heartbeat_internal;
};

void dec_worker_cfg_init(struct _dec_worker_cfg *cfg){
  memset(cfg->task_root_dir, 0, 128);
  memset(cfg->exec_root_dir, 0, 128);
  memset(cfg->server_port, 0, 128);
  memset(cfg->server_ip, 0, 128);
  memset(cfg->app_name, 0, 128);

  cfg->heartbeat_internal=10;
}


int dec_worker_cfg_check(struct _dec_worker_cfg cfg){
  if(strlen(cfg.task_root_dir) == 0 ||
     strlen(cfg.exec_root_dir) == 0 ||
     strlen(cfg.server_port) == 0 ||
     strlen(cfg.server_ip) == 0 ||
     strlen(cfg.app_name) == 0)
    return -1;
  else
    return 0;
}


void dec_worker_cfg_dispaly(struct _dec_worker_cfg cfg){
  printf("\n\nDEC WORKER RUNNING CONFIGURE\n-------------------------------------------\n");
  printf("task_root_dir\t\t\t%s\nexec_root_dir\t\t\t%s\nserver_port\t\t\t%s\nserver_ip\t\t\t%s\napp_name\t\t\t%s\nheartbeat_internal\t\t%d\n",
	 cfg.task_root_dir,
	 cfg.exec_root_dir,
	 cfg.server_port,
	 cfg.server_ip,
	 cfg.app_name,
	 cfg.heartbeat_internal);
  printf("-------------------------------------------\n\n");
}

struct _dec_reducer_cfg{
  char server_ip[128];
  char server_port[128];

  char my_ip[128];
  char my_port[128];

  char **apps;
  int32_t count;
  int32_t heartbeat_internal;
};

void dec_reducer_cfg_dispaly(struct _dec_reducer_cfg cfg){
  int idx=0;
  printf("\n\nDEC REDUCER RUNNING CONFIGURE\n-------------------------------------------\n");
  printf("my_ip\t\t\t%s\nmy_port\t\t\t%s\nserver_port\t\t\t%s\nserver_ip\t\t\t%s\nheartbeat_internal\t\t%d\n",
	 cfg.my_ip,
	 cfg.my_port,
	 cfg.server_port,
	 cfg.server_ip,
	 cfg.heartbeat_internal);

  printf("apps\t\t");
  for(; idx<cfg.count; ++idx){
    printf("%s", cfg.apps[idx]);
    if(idx < cfg.count-1)
      printf(",");
  }
  printf("\n");
  printf("-------------------------------------------\n\n");
}

void dec_reducer_cfg_init(struct _dec_reducer_cfg *cfg){
  memset(cfg->my_ip, 0, 128);
  memset(cfg->my_port, 0, 128);
  memset(cfg->server_port, 0, 128);
  memset(cfg->server_ip, 0, 128);

  cfg->heartbeat_internal=10;

  cfg->count=0;
  cfg->apps = (char**)malloc(20*sizeof(char*));
}

int dec_reducer_cfg_check(struct _dec_reducer_cfg cfg){
  if(strlen(cfg.my_ip) == 0 ||
     strlen(cfg.my_port) == 0 ||
     strlen(cfg.server_port) == 0 ||
     strlen(cfg.server_ip) == 0)
    return -1;
  else
    return 0;
}


void usage(){
  printf("Usage:\n");
  printf("\tDEC SERVER: \t_dec -s -cfg-file [configure-file]\n");
  printf("\tDEC WORKER: \t_dec -w -cfg-file [configure-file]\n");
  printf("\tDEC REDUCER:\t_dec -r -cfg-file [configure-file]\n");
}


static void server_text(GMarkupParseContext *context, 
			const gchar *text, 
			gsize text_len, 
			gpointer user_data, 
			GError **error ){

  struct _dec_server_cfg *cfg=(struct _dec_server_cfg*)user_data;
  char *element=NULL;

  element = g_markup_parse_context_get_element(context);
  
  if(strcmp(element, "TaskRootDir") == 0){
    strncpy(cfg->task_root_dir, text, text_len);
    cfg->task_root_dir[text_len]='\0';
  }

  else if(strcmp(element, "ExecRootDir") == 0){
    strncpy(cfg->exec_root_dir, text, text_len);
    cfg->exec_root_dir[text_len]='\0';
  }
  else if(strcmp(element, "ListenPort") == 0){
    strncpy(cfg->listen_port, text, text_len);
    cfg->listen_port[text_len]='\0';
  }
  else if(strcmp(element, "ConnCheckInternal") == 0 && text_len > 0)
    cfg->connection_check_internal = atoi(text);

  else if(strcmp(element, "ConnMaxTimeout") == 0 && text_len > 0)
    cfg->connection_max_timeout = atoi(text);

}

static void worker_text(GMarkupParseContext *context, 
		 const gchar *text, 
		 gsize text_len, 
		 gpointer user_data, 
		 GError **error ){

  struct _dec_worker_cfg *cfg=(struct _dec_worker_cfg*)user_data;
  char *element=NULL;

  element = g_markup_parse_context_get_element(context);
  
  if(strcmp(element, "TaskRootDir") == 0){
    strncpy(cfg->task_root_dir, text, text_len);
    cfg->task_root_dir[text_len]='\0';
  }

  else if(strcmp(element, "ExecRootDir") == 0){
    strncpy(cfg->exec_root_dir, text, text_len);
    cfg->exec_root_dir[text_len]='\0';
  }

  else if(strcmp(element, "ServerPort") == 0){
    strncpy(cfg->server_port, text, text_len);
    cfg->server_port[text_len]='\0';
  }

  else if(strcmp(element, "ServerIP") == 0){
    strncpy(cfg->server_ip, text, text_len);
    cfg->server_ip[text_len]='\0';
  }

  else if(strcmp(element, "AppName") == 0){
    strncpy(cfg->app_name, text, text_len);
    cfg->app_name[text_len]='\0';
  }

  else if(strcmp(element, "HeartbeatInternal") == 0 && text_len > 0)
    cfg->heartbeat_internal = atoi(text);

}


static void reducer_text(GMarkupParseContext *context, 
			 const gchar *text, 
			 gsize text_len, 
			 gpointer user_data, 
			 GError **error ){

  struct _dec_reducer_cfg *cfg=(struct _dec_reducer_cfg*)user_data;
  char *element=NULL;
  static int idx=0;

  element = g_markup_parse_context_get_element(context);
  
  if(strcmp(element, "MyIP") == 0){
    strncpy(cfg->my_ip, text, text_len);
    cfg->my_ip[text_len]='\0';
  }

  else if(strcmp(element, "MyPort") == 0){
    strncpy(cfg->my_port, text, text_len);
    cfg->my_port[text_len]='\0';
  }

  else if(strcmp(element, "ServerPort") == 0){
    strncpy(cfg->server_port, text, text_len);
    cfg->server_port[text_len]='\0';
  }

  else if(strcmp(element, "ServerIP") == 0){
    strncpy(cfg->server_ip, text, text_len);
    cfg->server_ip[text_len]='\0';
  }

  else if(strcmp(element, "AppName") == 0){
    cfg->apps[idx] = (char*)malloc(128);
    
    strncpy(cfg->apps[idx], text, text_len);
    cfg->apps[idx][text_len]='\0';
    ++idx;
    cfg->count = idx;
  }

  else if(strcmp(element, "HeartbeatInternal") == 0 && text_len > 0)
    cfg->heartbeat_internal = atoi(text);

}

int main(int argc, char *argv[]){

    DEC_SERVER server=NULL;
    DEC_WORKER worker=NULL;
    DEC_REDUCER reducer=NULL;
    struct _dec_server_cfg server_cfg;
    struct _dec_worker_cfg worker_cfg;
    struct _dec_reducer_cfg reducer_cfg;

    GMarkupParseContext *context;
    char *buf=NULL;
    int32_t len;
    int flag=0;
    
    GMarkupParser server_parser={ 
      . start_element=NULL, 
      . end_element=NULL, 
      . text=server_text, 
      . passthrough=NULL , 
      . error=NULL
    }; 
    
     GMarkupParser worker_parser={ 
      . start_element=NULL, 
      . end_element=NULL, 
      . text=worker_text, 
      . passthrough=NULL , 
      . error=NULL
    }; 

     GMarkupParser reducer_parser={ 
      . start_element=NULL, 
      . end_element=NULL, 
      . text=reducer_text, 
      . passthrough=NULL , 
      . error=NULL
    }; 


    if(argc != 4){
      usage();
      return 0;
    }

    if(strcmp(argv[1],"-s") == 0){
      dec_server_cfg_init(&server_cfg);
      flag = 1;
    }
    else if(strcmp(argv[1],"-w") == 0){
       dec_worker_cfg_init(&worker_cfg);
       flag = 2;
    }
    else if(strcmp(argv[1],"-r") == 0){
      dec_reducer_cfg_init(&reducer_cfg);
      flag = 3;
    }
    else{
      usage();
      return 0;
    }

    if(strcmp(argv[2], "-cfg-file") != 0){
      usage();
      return 0;
    }

    
    g_file_get_contents(argv[3], &buf, &len, NULL); 
    
    if(flag == 1)
      context = g_markup_parse_context_new(&server_parser, 0, &server_cfg, NULL); 
    else if(flag == 2)
      context = g_markup_parse_context_new(&worker_parser, 0, &worker_cfg, NULL); 
    else if(flag == 3)
      context = g_markup_parse_context_new(&reducer_parser, 0, &reducer_cfg, NULL); 

    if(g_markup_parse_context_parse(context, buf, len, NULL) == FALSE){
      printf("\nparse cfg file: %s fail\n\n", argv[3]);
      g_markup_parse_context_free(context);
      return 0;
    } 

    if(flag == 1 ? (dec_server_cfg_check(server_cfg) != 0):
       (flag == 2 ? (dec_worker_cfg_check(worker_cfg) != 0):
	(dec_reducer_cfg_check(reducer_cfg) != 0))){
      printf("\ncfg file: %s is not correct\n\n", argv[3]);
      g_markup_parse_context_free(context);
      return 0;
    }
    
    if(flag == 1){
      dec_server_cfg_dispaly(server_cfg);
      printf("DEC SERVER IS STARTING...\n");
    
      server = g_dec_server_init(server_cfg.listen_port,
				 server_cfg.task_root_dir,
				 server_cfg.exec_root_dir,
				 server_cfg.connection_check_internal,
				 server_cfg.connection_max_timeout);
      if(!server){
	printf("dec server init error\n");
	return 1;
      }

    g_dec_server_start(server);
    }

    else if(flag == 2){
       dec_worker_cfg_dispaly(worker_cfg);
       printf("DEC WORKER IS STARTING...\n");
       sleep(3);

       worker = g_dec_worker_init(worker_cfg.server_ip,
				  worker_cfg.server_port,
				  worker_cfg.app_name,
				  worker_cfg.task_root_dir,
				  worker_cfg.exec_root_dir,
				  worker_cfg.heartbeat_internal);
       if(!worker){
	 printf("dec worker init error\n");
	 return 1;
       }
       g_dec_worker_start(worker);
    }

    else if(flag == 3){
      printf("DEC REDUCER IS STARTING...\n");
      reducer = g_dec_reducer_init(reducer_cfg.server_ip,
				   reducer_cfg.server_port,
				   reducer_cfg.apps,
				   reducer_cfg.count,
				   reducer_cfg.my_ip,
				   reducer_cfg.my_port,
				   reducer_cfg.heartbeat_internal);
      if(!reducer){
	 printf("dec reducer init error\n");
	 return 1;
      }

      g_dec_reducer_start(reducer);
    }

    g_markup_parse_context_free(context);
    return 0;
}
