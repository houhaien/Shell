/*************************************************************************
	> File Name: common.h
	> Author: 
	> Mail: 
	> Created Time: Thu 18 Jul 2019 20:19:43 CST
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

#include<stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

int get_conf_value(char *file, char *key, char *value) ;

char * my_inet_ntoa(struct in_addr in);

int socket_connect(int port, char *host);

int socket_create(int port);

int get_conf_value(char *file, char *key, char *value) {
        FILE *fp = NULL;
        char *line = NULL, *substr = NULL;
        size_t n,len = 0;
        ssize_t read;
    if (key == NULL || value == NULL) {
                printf("error\n");
                return -1;
            
    }
        fp = fopen(file, "r");
    if (fp == NULL) {
                printf("Open file failed !\n");
                return -1;
            
    }
    while((read = getline(&line, &n, fp)) >= 0) {
                substr = strstr(line, key);
                if (substr == NULL) continue;
                len = strlen(key);
                if (line[len] != '=') continue;
                strncpy(value, line + len+1, (int)read -len - 2);
                break;
            
    }
    if (value == NULL) {
                printf("not found!\n");
                return -1;
            
    }
        free(line);
        fclose(fp);
        return 0;

}

char * my_inet_ntoa(struct in_addr in){
    static char ip[20] = {0};
    char *p;
    p = (char *)&in.s_addr;
    sprintf(ip,"%d.%d.%d.%d",p[0],p[1],p[2],p[3]);
    return ip;
}

int socket_connect(int port, char *host) {
    int sockfd;
    struct sockaddr_in dest_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error");
        return -1;   
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr(host);
    connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) ;
    return sockfd;
}

int socket_create(int port) {
    int sockfd;
    int yes = 1;
    struct sockaddr_in my_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error");
        return -1;
            
    }
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t socklen = sizeof(struct sockaddr);
    //端口重用 
    if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        close(sockfd);
        perror("setsockopt");
        return -1;
    }

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
        close(sockfd);
        perror("bind() error");
        return -1;
            
    }
    if (listen(sockfd, 20) < 0) {
        close(sockfd);
        perror("listen() error");
        return -1;
            
    }
    return sockfd;
}



#endif
