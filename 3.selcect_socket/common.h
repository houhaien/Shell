/*************************************************************************
	> File Name: common.h
	> Author: suziteng
	> Mail: 253604653@qq.com
	> Created Time: 2019年06月20日 星期四 18时26分07秒
 ************************************************************************/

#ifndef _COMMON_H

#include <curses.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>

int Socket(int family, int type, int protocal);

int socket_connect(int port, char *host);

int socket_accept(int sock_listen);

int socket_create(int port);

#define _COMMON_H
#endif
