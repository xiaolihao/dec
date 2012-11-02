
DEC_WORKER g_dec_worker_init(char *serv_ip,
			     char *serv_port,
			     char *app_name,
			     char *task_root,
			     char *exec_root,
			     int heartbeat_internal);

void g_dec_worker_start(DEC_WORKER server);

void g_dec_worker_free(DEC_WORKER server);
