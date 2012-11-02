
DEC_SERVER g_dec_server_init(char *port,
			     char *task_root_dir,
			     char *exec_root_dir,
			     int conn_check_internal,
			     int conn_max_timeout);

void g_dec_server_start(DEC_SERVER server);

void g_dec_server_free(DEC_SERVER server);
