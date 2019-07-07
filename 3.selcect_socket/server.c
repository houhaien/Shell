/*************************************************************************
	> File Name: kt_server.c
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月07日 星期日 18时54分21秒
 ************************************************************************/

#include <stdio.h>
#include "common.h"

#define MAX_CLIENTS 40
#define PORT 7777
#define MAX_BUFF 1024

typedef struct Server_data {
    int client_cnt ;
    int client_fds[MAX_CLIENTS];
    fd_set allfds;
    int maxfd;
}server_data;

server_data *s_d = NULL;


void accept_client(int server_listen) {
    int client_fd = -1;
    //client_fd = socket_accept(server_listen);
    struct sockaddr_in peer_addr;
    socklen_t socklen = sizeof(struct sockaddr);
    client_fd = accept(server_listen, (struct sockaddr *)&peer_addr, &socklen) ;

    if (client_fd == -1) {
        perror("accept");
        return;
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (s_d->client_fds[i] == -1) {
            s_d->client_fds[i] = client_fd;
            s_d->client_cnt++;
            break;
        }
    }
}

void recv_send(fd_set * readfds) {
    int client_fd ;
    char buff[MAX_BUFF] = {0};
    int num ;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    for (int i = 0; i < s_d->client_cnt; i++) {
        client_fd = s_d->client_fds[i];
        if (client_fd < 0) {
            continue;
        }
        if (FD_ISSET(client_fd, readfds)) {
            num = recv(client_fd, buff, MAX_BUFF, 0);
            if(num <= 0) {
                FD_CLR(client_fd, &(s_d->allfds));
                close(client_fd);
                s_d->client_fds[i] = -1;
                s_d->client_cnt --;
                continue;
            }
            //拿客户端地址
            getpeername(client_fd, (struct sockaddr *) &client,&len);

            printf("<%s> : %s\n", inet_ntoa(client.sin_addr), buff);

            num = send(client_fd, buff, strlen(buff), 0);
            if (num > 0) {
               printf(" [sent  OK]\n");

            } else {
                printf("[sent ERROR]\n");
            }

        }

    }
}

void do_client(int server_listen) {
    int client_fd = -1;
    fd_set * readfds = &(s_d->allfds);
    struct timeval tm;
    while(1) {
        FD_ZERO(readfds);
        FD_SET(server_listen, readfds);
        s_d->maxfd = server_listen;
        
        tm.tv_sec = 5;
        tm.tv_usec = 0;
        for (int i = 0; i < s_d->client_cnt ; i++) {
            client_fd = s_d->client_fds[i];
            if (client_fd != -1){
                FD_SET(client_fd, readfds);
            }
            s_d->maxfd = (client_fd > s_d->maxfd ? client_fd : s_d->maxfd);

        }
        int ret = select(s_d->maxfd + 1, readfds, NULL, NULL, &tm);
        if (ret == -1) {
            perror("select");
            return;
        } 
        if (ret == 0) {
            printf("TIMEOUT\n");
            continue;
        }
        
        if (FD_ISSET(server_listen, readfds)) {
            printf("before accept\n");
            accept_client(server_listen);
            printf("after  accept\n");
        } else {
            recv_send(readfds);
        }


    }
}



int main() {
    int server_listen;
    
    s_d = (server_data *)malloc(sizeof(server_data));
    for (int i = 0; i < MAX_CLIENTS ; i++) {
        s_d->client_fds[i] = -1;
    }
        s_d->client_cnt = 0;
        s_d->maxfd = 0;
    server_listen = socket_create(PORT);

    if(server_listen < 0){
        perror("socket_create");
        exit(1);
    }
    do_client(server_listen); 

    return 0;
}

