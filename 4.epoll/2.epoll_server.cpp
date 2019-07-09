/*************************************************************************
	> File Name: 2.epoll_server.cpp
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月09日 星期二 09时43分33秒
 ************************************************************************/

#include <stdio.h>
#include "common.h"
#include <sys/epoll.h>
#include <string.h>
#include <sys/ioctl.h>
#include <iostream>
#define MAXSIZE 256

using namespace std;

void do_client(int server_listen){
    int client_fd = -1;
    int epfd = epoll_create(MAXSIZE);
    if(epfd < 0) {
        perror("epoll_create");
        return;
    }
    //保存监听套接字 
    struct epoll_event epv;
    epv.events = EPOLLIN;
    epv.data.fd = server_listen;
    //修改文件流,用于控制epoll文件描述符上的事件
    if (epoll_ctl(epfd, EPOLL_CTL_ADD,server_listen, &epv) < 0) {
        perror("epoll_ctl");
        return;
    }
    //创建等待机制
    int epvnum = 0;
    struct epoll_event ewv[66];
    int tmout = -1;
    char buff[1024] = {0};
    while(1) {
        switch(epvnum = epoll_wait(epfd, ewv, 66, tmout)) {
            case 0:{
                printf("timeout");
            }break;
            case -1 :{
                perror("error epoll_wait");
            }break;
            default : {
                for (int i = 0; i < epvnum; i++) {
                    struct sockaddr_in addr;
                    socklen_t len = sizeof(addr);
                    if (ewv[i].data.fd == server_listen && (ewv[i].events & EPOLLIN)) {
                        int new_sockfd = accept(server_listen, (struct sockaddr *)&addr, &len);
                        if (new_sockfd < 0) {
                            perror("accept");
                            continue;
                        } else {
                            epv.data.fd = new_sockfd;
                            epv.events = EPOLLIN | EPOLLET;
                            epoll_ctl(epfd, EPOLL_CTL_ADD, new_sockfd, &epv);
                        }
                    } else if(ewv[i].data.fd != server_listen  && (ewv[i].events &EPOLLIN)){
                        int num = recv(ewv[i].data.fd, buff, sizeof(buff), 0);
                        if (num > 0) {
                            struct sockaddr_in cfd;
                            socklen_t len = sizeof(cfd);
                            getpeername(ewv[i].data.fd, (struct sockaddr *)&cfd, &len);
                            printf("<%s>--->%s\n", inet_ntoa(cfd.sin_addr), buff);
                            epv.data.fd = ewv[i].data.fd;
                            epv.events = EPOLLOUT | EPOLLET;
                            //epv.events = EPOLLIN;
                            epoll_ctl(epfd, EPOLL_CTL_MOD,ewv[i].data.fd, &epv);
                        } else if (num == 0){
                            cout << "someone bye bye lou" << endl;
                            close(ewv[i].data.fd);
                            epoll_ctl(epfd, EPOLL_CTL_DEL, ewv[i].data.fd, NULL);
                        }
                    }else if (ewv[i].data.fd != server_listen && (ewv[i].events & EPOLLOUT)) {
                        char k[20] = "hello";
                        int num = send(ewv[i].data.fd, &k, sizeof(k),0);
                        if (num <= 0) 
                        cout << "no send" << endl;
                        else 
                        //cout << "k -->" << k << endl;
                        printf("k---->%s\n", k);
                        epv.events = EPOLLIN | EPOLLET;
                        epoll_ctl(epfd, EPOLL_CTL_MOD,ewv[i].data.fd,&epv );
                    }
                }
            }break;
        }
    }
    return;
}


int main(int argc, char **argv) {
    int port = atoi(argv[1]);
    int server_listen = socket_create(port);
    if(server_listen < 0) {
        perror("server_listen");
        exit(1);
    }
    do_client(server_listen);

    return 0;
}
