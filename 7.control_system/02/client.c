/*************************************************************************
	> File Name: client.c
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月25日 星期四 11时11分31秒
 ************************************************************************/

#include "client.h"
#include <sys/wait.h>
#define BUFFSIZE 1024
#include <unistd.h>

void do_connect(int port, char *ip) {
    while(1) {
        int fd = socket_connect(port, ip);
        if(fd < 0) {
            close(fd);
            sleep(5);
            continue;
        }
        close(fd);
        break;
    }
}


void  do_heart(int port) {
    int fd = socket_create(port);
    if (fd < 0) {
        perror("socket_create");
        return;
    }

    while(1) {
        int newfd = accept(fd, NULL, NULL);
        printf("O(∩_∩)O~~\n");
    }
    close(fd);
}


int main() {
    int hport, cport, mport;
    char tmp[50] = {0};
    char *inarg = "./pihealthc.conf";
    char ip[50] = {0};

    get_conf_value(inarg, "Hport", tmp);
    hport = atoi(tmp);
    memset(tmp,0, sizeof(tmp));

    get_conf_value(inarg, "Cport", tmp);
    cport = atoi(tmp);
    memset(tmp,0, sizeof(tmp));
    
    get_conf_value(inarg, "Mport", tmp);
    mport = atoi(tmp);
    memset(tmp,0, sizeof(tmp));
    
    get_conf_value(inarg, "Mip", tmp);
    strcpy(ip,tmp);
    memset(tmp,0, sizeof(tmp));
    
    printf("h= %d  c = %d  m = %d\n", hport, cport, mport);
    
    int listenfd;
    
    if ((listenfd = socket_create(cport)) < 0) {
        perror("socket_create");
        exit(1);
    } 
    pid_t pid1;
    pid1 = fork();
    if (pid1 > 0) {
        while(1) {
        int fd = accept(listenfd, NULL, NULL);
        if (fd < 0) {
            perror("accept");
        }
        char buff[BUFFSIZE] = {0};
        int recvfd = recv(fd, buff, BUFFSIZE, 0);
        if (recvfd < 0) {
            perror("recv");
            printf("error recv");
            close(fd);
            continue;
        }
            printf("%s\n", buff); 
            strcpy(buff, "nihaoa!!!!!1!!!");
            int k = send(fd, buff, strlen(buff), 0);
            if (k  < 0) {
                printf("error send\n");
            }
            close(fd);
        }
        close(listenfd);
        wait(&pid1);
    }
    if (pid1 == 0) {
        pid_t pid2 = fork();
        if (pid2 > 0) {
            do_connect(mport, ip);
            wait(&pid2);
            exit(0);
        }
        if (pid2 == 0) {
            do_heart(hport);
            exit(0);
        }
    }

}
