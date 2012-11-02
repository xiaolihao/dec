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

void dec_server_cfg_init(struct _dec_server_cfg cfg){
  memset(cfg.task_root_dir, 0, 128);
  memset(cfg.exec_root_dir, 0, 128);
  memset(cfg.listen_port, 0, 128);
  
  cfg.connection_check_internal=10;
  cfg.connection_max_timeout=30;
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
  printf("task_root_dir\t\t%s\nexec_root_dir\t\t%s\nlisten_port\t\t%s\nconn_check_internal\t%d\nconn_max_timeout\t%d\n",
	 cfg.task_root_dir,
	 cfg.exec_root_dir,
	 cfg.listen_port,
	 cfg.connection_check_internal,
	 cfg.connection_max_timeout);
  printf("-------------------------------------------\n\n");
}


void usage(char *app_name){
  printf("\nusage: %s [serv_cfg.xml]\n\n", app_name);
}


static void text(GMarkupParseContext *context, 
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

int main(int argc, char *argv[]){

    DEC_SERVER server=NULL;
    struct _dec_server_cfg cfg;
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

    dec_server_cfg_init(cfg);

     
    g_file_get_contents(argv[1], &buf, &len, NULL); 
    context = g_markup_parse_context_new(&parser, 0, &cfg, NULL); 
    if(g_markup_parse_context_parse(context, buf, len, NULL) == FALSE){
      printf("\nparse cfg file: %s fail\n\n", argv[1]);
      g_markup_parse_context_free(context);
      return 0;
    } 

    if(dec_server_cfg_check(cfg) != 0){
      printf("\ncfg file: %s is not correct\n\n", argv[1]);
      g_markup_parse_context_free(context);
      return 0;
    }
    
    dec_server_cfg_dispaly(cfg);
    
    printf("DEC SERVER IS STARTING...\n");
    
    server = g_dec_server_init(cfg.listen_port,
			       cfg.task_root_dir,
			       cfg.exec_root_dir,
			       cfg.connection_check_internal,
			       cfg.connection_max_timeout);
    if(!server){
	printf("dec server init error\n");
	return 1;
    }

    g_dec_server_start(server);
    return 0;
}
