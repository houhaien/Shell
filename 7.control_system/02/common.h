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
#include <sys/file.h>
#include <time.h>
#include <stdarg.h>


int get_conf_value(char *file, char *key, char *value) ;

char * my_inet_ntoa(struct in_addr in);

int socket_connect(int port, char *host);

int socket_create(int port);

void write_log( char *path, const char *format,...) ;

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

int socket_connect1(int port, char *host) {
	int fd;
    struct sockaddr_in socket_addr;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        close(fd);
        return -1;
    }
    int ul = 1;
    if (ioctl(fd, FIONBIO, &ul) < 0) {
        close(fd);
        return -1;
    }
    memset(&socket_addr, 0, sizeof(socket_addr));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons(port);
    socket_addr.sin_addr.s_addr = inet_addr(host);
    
    if (connect(fd, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) < 0) {
        //调用connect连接一般的超时时间是75s, 但是在程序中我们一般不希望等这么长时间采取采取动作。 
        //可以在调用connect之前设置套接字非阻塞,然后调用connect,此时connect会立刻返回, 
        //如果连接成功则直接返回0（成功）， 如果没有连接成功，也会立即返回并且会设置errno为EINPROCESS,
        //这并不是一个致命错误,仅仅是告知你已经在连接了,你只要判断是它就继续执行后面的逻辑就行了
        // 我们可以用select来设置连接超时的定时器
        if (errno == EINPROGRESS) {
            int err;
            int len = sizeof(int);
            struct timeval tv;
            tv.tv_sec  = 2;
            tv.tv_usec = 0;
            fd_set set;
            FD_ZERO(&set);
            FD_SET(fd, &set);
            if(select(fd + 1, NULL, &set, NULL, &tv) > 0) {
                int ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, (socklen_t *) &len) ;
                if (ret == -1 || err != 0) {
                    close(fd);
                    return -1;
                }
            } else {
                close(fd);
                return -1;
            }
        } else {
            printf("connect false\n");
            close(fd);
            return -1;
        }
    }
    ul = 0;
    if (ioctl(fd, FIONBIO, &ul) < 0) {
        close(fd);
        return -1;
    }
    return fd;
    
}

int socket_connect(int port, char *host) {
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        close(fd);
        perror("Socket");
    }
    struct sockaddr_in temp; 
    temp.sin_family = AF_INET;
    temp.sin_addr.s_addr = inet_addr(host);
    temp.sin_port = htons(port);
    if(connect(fd, (struct sockaddr *)&temp, sizeof(temp)) >= 0) {
        return fd;
    } else {
        return -1;
    } 
    close(fd);
    return 0;
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
    if (listen(sockfd, 200) < 0) {
        close(sockfd);
        perror("listen() error");
        return -1;
            
    }
    return sockfd;
}

void write_log( char *path, const char *format,...) {
    FILE *fd = NULL;
    va_list arg;
    int ret;
    va_start(arg, format);
    fd = fopen(path, "a+");
    flock(fd->_fileno, LOCK_EX);
    char buff[1024] = {0};
   //获取时间 
    time_t timer = time(NULL);
    struct tm *tblock;
    tblock=localtime(&timer);
    //printf("Local time is: %s",asctime(tblock));
    //printf("%d:%02d:%02d\n", tblock->tm_year+1900, tblock->tm_mon+1,tblock->tm_mday);
    //printf("%02d:%02d:%02d\n", tblock->tm_hour, tblock->tm_min, tblock->tm_sec);
    
    fprintf (fd, "%d:%02d:%02d   %02d:%02d:%02d ", tblock->tm_year+1900,tblock->tm_mon+1,tblock->tm_mday, tblock->tm_hour, tblock->tm_min, tblock->tm_sec);

    ret = vfprintf(fd, format, arg);
    fprintf(fd, "pid = [%d]", getpid());
    fprintf(fd, "\n");
    va_end(arg);   
    fflush(fd);
    fclose(fd);
    return ;
} 


#endif
