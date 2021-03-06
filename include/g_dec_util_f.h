

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

int util_file_existence(char *full_file_path);

int util_send_data_to_host(char *ip,
			   char *port,
			   char *data,
			   int32_t size);

int util_get_file_length(char *file);

int util_send_file_to_host_with_prepadding(char *ip,
					   char *port,
					   char *file,
					   char *pre_padding,
					   int32_t size);


void util_remove_file_parent(char *file);

int util_uncompress_file_to_dir(char *dest_dir,
				char zip_file);

int util_compress_dir_to_file(char *dest_file,
			      char *prefix,
			      char *src_dir);
