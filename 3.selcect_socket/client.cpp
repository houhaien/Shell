/*************************************************************************
	> File Name: client.cpp
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月07日 星期日 10时23分06秒
 ************************************************************************/
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <sys/time.h>

int main(int argc, char **argv) {
    int port = 7777;
    char ip[20];
    strcpy(ip, argv[1]);
    char name[20] = {0};
	struct passwd *pwd;
	pwd = getpwuid(getuid());
    strcpy(name, pwd->pw_name);

    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    
    ioctl(sockfd, FIONBIO, 1); //1为非阻塞，0为阻塞

    fd_set wfds;
    struct timeval tvl;
    int retval;

    FD_ZERO(&wfds);
    FD_SET(sockfd,&wfds);
    tvl.tv_sec = 5;
    tvl.tv_usec = 0;

    if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
    }

    retval = select(sockfd+1, NULL, &wfds, NULL, &tvl);

    if (retval == -1) {
        perror("select()");
    } else if (retval) {
        printf(" success:\n");
        if(FD_ISSET(sockfd, &wfds)){
            if( (send(sockfd, name, sizeof(name), 0)) < 0) {
                perror("error send : socketfd");
            }
            char k[1024] = {0};
            recv(sockfd, &k, sizeof(k), 0);
            printf("k == %d\n", k);
            printf("send: %s\n", name);
            }
        }
    else {
        printf("out of time...\n");
    }
    return 0;
}
