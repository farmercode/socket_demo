#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define SERVER_PORT 5555

void main(int argc, char const *argv[]){
    char server_host[50];
    memset(server_host,0,50);
    //命令行参数处理
    if(argc > 1){
        strcpy(server_host,argv[1]);
    }else{
        strcpy(server_host,"127.0.0.1");
    }
        int client_socket,sock_err;
        struct sockaddr_in server_ip;
        char *name = "Hello,this is king!";

        server_ip.sin_family = AF_INET;
        server_ip.sin_port=htons(SERVER_PORT);
        server_ip.sin_addr.s_addr = inet_addr(server_host);

        //创建socket
        client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(client_socket < 0){
                printf("socket create error!\n" );
                exit(1);
        }

        sock_err = connect(client_socket,(struct sockaddr *)&server_ip,sizeof(server_ip));
        if(sock_err != 0){
                printf("connect server failed\n" );
                exit(2);
        }

        sock_err = send(client_socket,name,strlen(name),0);
        if(sock_err<0){
                printf("send message to server failed\n" );
                exit(3);
        }
        char recv_buffer[512];
        memset(recv_buffer,0,512);
        recv(client_socket,recv_buffer,512,0);
        printf("server recv:%s\n",recv_buffer);

        close(client_socket);
        //return 0;
}
