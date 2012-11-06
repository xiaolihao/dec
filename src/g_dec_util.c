#include "g_dec_internal.h"





/** reducer register packet format
 *
 * ----package header(12 bytes)----
 * command type(4 bytes, unsigned int)
 * reserved(4 bytes, padding by 0)
 * data size(4 bytes, unsigned int)
 *
 * ----data----
 * ip len(1 byte)
 * ip string...
 * port len(1 byte)
 * port string...
 * app count(1 byte)
 * app1 len(1 byte)
 * app1 string...
 * app2 len(1 byte)
 * app2 string...
 * ...
 */

void util_reducer_register_data_create(char **buf,
				       int32_t *size,
				       char *ip,
				       char *port,
				       char *apps[],
				       int32_t count){
  int32_t len=0, idx=0;
  unsigned char sz=0;
  int32_t offset=0;

  for(; idx<count; ++idx)
    len += strlen(apps[idx])+1;
  
  len++;
  len += strlen(ip)+strlen(port)+2;
  
  *buf = (char*)malloc(len);
  assert(*buf);

  *size = len;

  /* ip data */
  sz = strlen(ip);
  memcpy(*buf, &sz, 1);
  offset++;
  memcpy(*buf+offset, ip, sz);
  offset += sz;


  /* port data */
  sz = strlen(port);
  memcpy(*buf+offset, &sz, 1);
  offset++;
  memcpy(*buf+offset, port, sz);
  offset += sz;

  
  /* count of apps */
  
  /* count info */
  sz = count;
  memcpy(*buf+offset, &sz, 1);
  offset++;

  for(idx=0; idx<count; ++idx){

    sz = strlen(apps[idx]);
    memcpy(*buf+offset, &sz, 1);
    offset++;

    memcpy(*buf+offset, apps[idx], sz);
    offset += sz;
  }
  
}



void util_message_packet_create(char **buf,
				int32_t type,
				char *data,
				int32_t size){

  int32_t len=COM_HEADER_SIZE+size;
  
  *buf = (char*)malloc(len);
  assert(*buf);
  
  memcpy(*buf,    &type, 4);
  memcpy(*buf+8,  &size, 4);
  memcpy(*buf+12, data, size);
}

int util_mkdir_with_path(char *path){
  return g_mkdir_with_parents(path, 0755);
}

int util_file_existence(char *full_file_path){
  
  if(g_access(full_file_path, F_OK) == 0)
    return G_OK;
  else
    return G_ERROR;
}

int util_dir_empty(char *path){
  GDir *dir=g_dir_open(path, 0, NULL);
  char *file=NULL;
  
  if(!dir)
    return G_OK;

  file = g_dir_read_name(dir);
  g_dir_close(dir);

  if(file)
    return G_ERROR;
  else
    return G_OK;
}

int util_fetch_single_file(char **task_file,
			   char *path){
  
  GDir *dir=g_dir_open(path, 0, NULL);
  
  if(!dir)
    return G_ERROR;

  *task_file = g_dir_read_name(dir);
  g_dir_close(dir);

  
  if(!*task_file)
    return G_ERROR;

  return G_OK;
}

int util_load_file(char **data, 
		   int32_t *size,
		   char *full_file_path){

  FILE *fp = NULL;

  fp = fopen(full_file_path, "r+");
  if(!fp)
    return G_ERROR;

  fseek(fp, 0, SEEK_END);
  *size = ftell(fp);
  fseek(fp,0,SEEK_SET);

  *data = (char*)malloc(*size);
  if(*size != fread(*data, 1, *size, fp)){
    free(*data);
    fclose(fp);
    *size=0;
    *data = NULL;
    return G_ERROR;
  }
  
  fclose(fp);
  return G_OK;
}


int util_save_file(char *data,
		   int32_t size,
		   char *path,
		   char *file_name){

  char full_file_path[1024];
  FILE *fp=NULL;

  util_mkdir_with_path(path);
  if(file_name)
    sprintf(full_file_path, "%s/%s", path, file_name);
  else
    sprintf(full_file_path, "%s/%d", path, time(0));

  fp = fopen(full_file_path, "w+");
  
  if(!fp)
    return G_ERROR;
  
  fwrite(data, 1, size, fp);
  fclose(fp);

  return G_OK;
}
