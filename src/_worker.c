#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "dec.h"


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

void dec_worker_cfg_init(struct _dec_worker_cfg cfg){
  memset(cfg.task_root_dir, 0, 128);
  memset(cfg.exec_root_dir, 0, 128);
  memset(cfg.server_port, 0, 128);
  memset(cfg.server_ip, 0, 128);
  memset(cfg.app_name, 0, 128);

  cfg.heartbeat_internal=10;
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
  printf("task_root_dir\t\t%s\nexec_root_dir\t\t%s\nserver_port\t\t%s\nserver_ip\t\t%s\napp_name\t\t%s\nheartbeat_internal\t%d\n",
	 cfg.task_root_dir,
	 cfg.exec_root_dir,
	 cfg.server_port,
	 cfg.server_ip,
	 cfg.app_name,
	 cfg.heartbeat_internal);
  printf("-------------------------------------------\n\n");
}


void usage(char *app_name){
  printf("\nusage: %s [worker_cfg.xml]\n\n", app_name);
}


static void text(GMarkupParseContext *context, 
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


int main(int argc, char* argv[]){
  DEC_WORKER worker=NULL;

  struct _dec_worker_cfg cfg;
  GMarkupParseContext *context;
  char *buf=NULL;
  int32_t len;

  GMarkupParser parser={ 
    . start_element=NULL, 
    . end_element=NULL, 
    . text=text, 
    . passthrough=NULL , 
    . error=NULL
  }; 
    
    
  if(argc != 2){
    usage(argv[0]);
    return 0;
  }

  dec_worker_cfg_init(cfg);

     
  g_file_get_contents(argv[1], &buf, &len, NULL); 
  context = g_markup_parse_context_new(&parser, 0, &cfg, NULL); 
  if(g_markup_parse_context_parse(context, buf, len, NULL) == FALSE){
    printf("\nparse cfg file: %s fail\n\n", argv[1]);
    g_markup_parse_context_free(context);
    return 0;
  } 

  if(dec_worker_cfg_check(cfg) != 0){
    printf("\ncfg file: %s is not correct\n\n", argv[1]);
    g_markup_parse_context_free(context);
    return 0;
  }
    
  dec_worker_cfg_dispaly(cfg);
    
  printf("DEC WORKER IS STARTING...\n");
  sleep(3);

  worker = g_dec_worker_init(cfg.server_ip,
			     cfg.server_port,
			     cfg.app_name,
			     cfg.task_root_dir,
			     cfg.exec_root_dir,
			     cfg.heartbeat_internal);
  if(!worker){
    printf("dec worker init error\n");
    return 1;
  }
  g_dec_worker_start(worker);
  
  return 0;
}
