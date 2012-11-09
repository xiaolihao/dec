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

void util_remove_file_parent(char *file){
  
  int32_t len=strlen(file);
  int32_t idx=len-1;
  
  while(file[idx] != '/')
    --idx;

  file[idx]='\0';
  remove(file);
  
}

int util_compress_dir_to_file(char *dest_file,
			      char *prefix,
			      char *src_dir){

  GDir *dir=g_dir_open(src_dir, 0, NULL);
  char *file=NULL;
  char name_prefix[128];
  char *tmp_dir_name;
  char dest[1024];
  char src[1024];
  zipFile zf;
  int flag = 0;
  FILE *fp=NULL;
  char buf[NET_BUF_SIZE];
  int32_t len=0;

  if(!prefix)
    tmp_dir_name = g_dir_make_tmp(NULL, NULL);
  else{
    sprintf(name_prefix,"%s-XXXXXX", prefix);
    tmp_dir_name = g_dir_make_tmp(name_prefix, NULL);
  }
  
  if(!tmp_dir_name || !dir){
    g_dir_close(dir);
    g_free(tmp_dir_name);
    return G_ERROR;
  }

  sprintf(dest, "%s/%d.zip",tmp_dir_name, time(0));
  memcpy(dest_file, dest, strlen(dest));
  dest_file[strlen(dest)]='\0';

  zf = zipOpen(dest, APPEND_STATUS_CREATE);
  
  if(!zf){
    g_dir_close(dir);
    g_free(tmp_dir_name);
    return G_ERROR;
  }


  /* move all file to tmp dir first */
  while((file=g_dir_read_name(dir)) != NULL){

    flag = 1;

    sprintf(dest, "%s/%s", tmp_dir_name, file);
    sprintf(src, "%s/%s", src_dir, file);
    rename(src, dest);

    zip_fileinfo zi;
    memset(&zi, 0, sizeof(zi));
    
    if(zipOpenNewFileInZip(zf,
			   dest,
			   &zi,
			   NULL,
			   0,
			   NULL,
			   0,
			   NULL,
			   Z_DEFLATED,
			   Z_BEST_COMPRESSION) != ZIP_OK){

      g_dir_close(dir);
      g_free(tmp_dir_name);
      zipClose(zf, NULL);
      return G_ERROR;


    }

    fp=fopen(dest, "r");
    if(!fp)
      continue;

    while((len=fread(buf, 1, NET_BUF_SIZE, fp)) > 0){
      assert(zipWriteInFileInZip(zf, buf, len) == ZIP_OK);
    }

    fclose(fp);
    remove(dest);
    
  }
  
  zipClose(zf, NULL);
  g_dir_close(dir);
  g_free(tmp_dir_name);

  if(!flag){
    remove(zf);
    return G_ERROR;
  }
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

int util_get_file_length(char *file){

  FILE *fp = NULL;
  int len=0;

  fp = fopen(file, "r");
  if(!fp)
    return -1;

  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp,0,SEEK_SET);

  fclose(fp);

  return len;
}

int util_send_file_to_host_with_prepadding(char *ip,
					   char *port,
					   char *file,
					   char *pre_padding,
					   int32_t size){
  struct sockaddr_in addr;
  int32_t addr_len=0;
  int32_t offset=0, len=0;
  int fd;
  FILE *fp=NULL;
  char buf[NET_BUF_SIZE];

  if((fd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return G_ERROR;
  
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=inet_addr(ip);
  addr.sin_port=htons(atoi(port));
  
  if(connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0){
    close(fd);
    return G_ERROR;
  }
  
  fp = fopen(file, "r");
  if(!fp){
    close(fd);
    return G_ERROR;
  }

  /* send padding first */
  if(send(fd, pre_padding, size, 0) != size){
    close(fd);
    fclose(fp);
    return G_ERROR;
  }


  /* send file */
  while(!feof(fp)){
    len=fread(buf, 1, NET_BUF_SIZE, fp);
    
    if(ferror(fp)){
      close(fd);
      fclose(fp);
      return G_ERROR;
    }
    
    if(send(fd, buf, len, 0) != len){
      close(fd);
      fclose(fp);
      return G_ERROR;
    }
  }

  close(fd);
  fclose(fp);
  return G_OK;
}


int util_send_data_to_host(char *ip,
			   char *port,
			   char *data,
			   int32_t size){
  struct sockaddr_in addr;
  int32_t addr_len=0;
  int32_t offset=0, len=0;
  int fd;
  
  if((fd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return G_ERROR;
  
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=inet_addr(ip);
  addr.sin_port=htons(atoi(port));
  
  if(connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0){
    close(fd);
    return G_ERROR;
  }

  while(offset+NET_BUF_SIZE <= size){
    if(send(fd, data+offset, NET_BUF_SIZE, 0) != NET_BUF_SIZE){
      close(fd);
      return G_ERROR;
    }
    
    offset += NET_BUF_SIZE;
  }

  if(offset < size){
    if(send(fd, data+offset, size-offset, 0) != size-offset){
      close(fd);
      return G_ERROR;
    }
  }


  close(fd);
  return G_OK;
}
