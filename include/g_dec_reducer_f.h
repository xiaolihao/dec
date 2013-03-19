
DEC_REDUCER g_dec_reducer_init(char *serv_ip,
			       char *serv_port,
			       char *app_name[],
			       int32_t count,
			       char *my_ip,
			       char *my_port,
			       char *result_root_dir,
			       int heartbeat_internal);

void g_dec_reducer_start(DEC_REDUCER reducer);

void g_dec_reducer_free(DEC_REDUCER reducer);
