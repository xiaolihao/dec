#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
int
setnonblock(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL);
    if (flags < 0)
        return flags;
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0)
        return -1;

    return 0;
}

int main(){
     int      sock;
     struct sockaddr_in server_addr;
     char      *str;

     char buf[21]={0}, buf2[1024]={0};
     const char* s="app1";
     int32_t type=0, size=strlen(s);
     
     memcpy(buf, &type, 4);
     memcpy(buf+8, &size, 4);

     memcpy(buf+12, s, size);
     int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        printf("ERROR, no such host\n");
        exit(0);
    }

    struct sockaddr_in serv_addr;
    memset((char*) &serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    memcpy((char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length);
    serv_addr.sin_port = htons(9000);
    if (connect( sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        printf("error connect:%d\n", errno);
        exit(0);
    }
    

    int x=write(sockfd, buf, size+12);
    printf("send:%d\n", x);
    char buf3[1024]= {0};

    int n = read( sockfd, buf2, 35);
    printf("recv:%d\n", n);
    int type1=*(int32_t*)buf2;
   
    /* skip 4 bytes' padding */
    int size1=*(int32_t*)((char*)buf2+8);
		     
    buf2[n]='\0';
    printf("%d,%d,%s\n", type1, size1, buf2+12);

    close(sockfd);
    return 0;

     return 0;
}


