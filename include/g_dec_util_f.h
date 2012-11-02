

void util_message_packet_create(char **buf,
				int32_t type,
				char* data,
				int32_t size);

int util_mkdir_with_path(char *path);

int util_dir_empty(char *path);

int util_fetch_single_file(char** task_file,
			   char *path);

int util_load_file(char** data, 
		   int32_t *size,
		   char* full_file_path);

void util_save_file(char* data,
		    int32_t size,
		    char* path,
		    char* file_name);
