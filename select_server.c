#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 5555
#define MAX_SET_NUM 50
int fd_arr[MAX_SET_NUM];
int conn_num;

int main(int argc,const char **argv){
    int server_fd,client_fd,tmp_fd;
    struct sockaddr_in server_addr,client_addr;
    char buffer[512];
    int reuse_addr = 1;
    int ret,recv_len;
    socklen_t sin_size;
    char *welcome = "welcome to select server";
    conn_num = 0;

    if((server_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        printf("create socket failed\n");
        return 1;
    }
    
    if(setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&reuse_addr,sizeof(int)) == -1){
        printf("set socket opt failed\n");
        return 2;
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    memset(server_addr.sin_zero,'\0',sizeof(server_addr.sin_zero));

    if(bind(server_fd,(struct sockaddr *) &server_addr,sizeof(server_addr)) == -1){
        printf("bind socket failed\n");
        return 3;
    }

    if(listen(server_fd,MAX_SET_NUM) == -1){
        printf("listen socket failed\n");
        return 4;
    }
    printf("listen 0.0.0.0:%d,fd:%d\n",SERVER_PORT,server_fd);

    fd_set  my_sets;
    int     max_sock,i;
    struct timeval tv;

    conn_num = 0;
    sin_size = sizeof(client_addr);
    max_sock = server_fd+1;
    tv.tv_sec = 30;
    tv.tv_usec = 0;
        
    for(i=0;i<MAX_SET_NUM;i++)
        fd_arr[i] = -1;

    while(1){
        //初始化描述符集合
        FD_ZERO(&my_sets);
        FD_SET(server_fd,&my_sets);

        //添加活动到连接
        for(i=0;i < MAX_SET_NUM;i++){
            if(fd_arr[i] != -1){
                FD_SET(fd_arr[i],&my_sets);
            }
        }
        
        ret = select(max_sock,&my_sets,NULL,NULL,&tv);
        if(ret < 0 ){
            perror("select error\n");
            break;
        }else if(ret == 0){
            printf("select timeout\n");
            continue;
        }
        
        //新客户端接入
        if(FD_ISSET(server_fd,&my_sets)){
           client_fd = accept(server_fd,(struct sockaddr *)&client_addr,&sin_size);
           for(i=0;i<MAX_SET_NUM;i++){
               if(fd_arr[i]<0){
                   fd_arr[i] = client_fd;
                   //退出break将数组指针移向下一个元素，下一个指针为数组真实大小
                   i++;
                   break;
               }
           }

           if(i == MAX_SET_NUM){
                printf("too many clients\n");
                return 5;
           }
           //将客户端描述符加入set
           FD_SET(client_fd,&my_sets);
           //客户端描述符大于select最大监听描述符时进行赋值
           if(client_fd>max_sock)
               max_sock = client_fd;

           if(i > conn_num){
                conn_num = i;
           }

           //select返回监听集合中有事件发生到数量，如果为0说明没有需要处理的，就回到头部继续select
        }
        
        //处理client中发生的事件
        for(i=0;i<conn_num;i++){
            if((tmp_fd = fd_arr[i])<0)
                continue;
            if(FD_ISSET(tmp_fd,&my_sets)){
               memset(buffer,0,512); 
               recv_len = recv(tmp_fd,buffer,512,0);
               if(recv_len > 0 ){
                    printf("recv content:%s\n",buffer);
                    send(tmp_fd,welcome,strlen(welcome),0);
               }else{
                   printf("recv nothig\n");
               }
               close(tmp_fd);
               fd_arr[i] = -1;
            }
        }
    }
    close(server_fd);
    return 0;
}
