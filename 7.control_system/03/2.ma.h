/*************************************************************************
	> File Name: master.h
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月26日 星期五 20时39分47秒
 ************************************************************************/

#include "common.h"
#include <sys/wait.h>
#include <sys/epoll.h>
#include <sys/file.h>
#define MAXCLIENT 10000
#define BUFFSIZE 1024
#define CTLPORT 9732

char *logpath = "/opt/master/writlog.log";

typedef struct Node {
    struct sockaddr_in addr;
    int fd;
    struct Node *next;
} Node, *LinkList;

typedef struct Heart {
    LinkList *link;
    int ins;
    int *sum;
    long timeout;
} Heart;

struct Work {
    LinkList link;
    int index;
    int ctlport;
    int mp;
};

typedef struct All_system{
    char name[50];
    char buff[BUFFSIZE * 4];
}Shmsg;

int find_min(int *sum, int ins);
int insert(LinkList head, Node *q);
void output(LinkList head);
void *do_heart(void *arg);
void *do_work(void *arg);
void *work(void *arg);
int do_event(struct sockaddr_in addr);
int check_connect(struct sockaddr_in addr, long timeout);
int do_epoll(int listenfd, LinkList *link, int *sum, int ins, int heartport);
void *do_warn(void *arg);

int find_min(int *sum, int ins) {
    int min = 99999, ind;    
    for (int i = 0; i < ins; i++) {
        if (sum[i] < min) {
            min = sum[i];
            ind = i;
        }
    }
    return ind;
}

int insert(LinkList head, Node *q) {
    Node *p = head;
    while (p->next) {
        p = p->next;
    }
    p->next = q;
    return 1;
}

void output(LinkList head) {
    Node *p = head;
    int cnt = 0;
    while (p->next) {
        printf("[%d]:%s\n", ++cnt, inet_ntoa(p->next->addr.sin_addr));
        p = p->next;
    }
}

int check_connect(struct sockaddr_in addr, long timeout) {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 0;
    }
    int error = -1, ret = 0;
    int len = sizeof(int);
    struct timeval tm;
    tm.tv_sec = 0;
    tm.tv_usec = timeout;
    unsigned long ul = 1;
    ioctl(sockfd, FIONBIO, &ul);
    
    fd_set set;
    FD_ZERO(&set);
    FD_SET(sockfd, &set);
    
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        if (select(sockfd + 1, NULL, &set, NULL, &tm) > 0) {
            getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t*)&len);
            if (error == 0) {
                ret = 1;
            } else {
                ret = 0;
            }
        }
    }
    close(sockfd);
    return ret;
}


void *do_heart(void *arg) {
    Heart *heartarg = (Heart*)arg;
    while (1) {
        for (int i = 0; i < heartarg->ins; i++) {
            Node *p = heartarg->link[i];
            while (p->next) {
                if (!check_connect(p->next->addr, heartarg->timeout)) {
                    printf("%s : %d \033[31m  deling.....\033[0m\n", inet_ntoa(p->next->addr.sin_addr), ntohs(p->next->addr.sin_port));
                    Node *q = p->next;
                    p->next = q->next;
                    free(q);
                    heartarg->sum[i] -= 1;
                } else {
                    printf("%s \033[32m  在线ing.....\033[0m at port [%d]\n", inet_ntoa(p->next->addr.sin_addr), ntohs(p->next->addr.sin_port));
                    p = p->next;
                }
            }
        }
        sleep(5);
        printf("\n");
    }
}


void *work(void *arg) {
    struct Work *inarg = (struct Work *)arg;
    char log[50] = {0};
    sprintf(log, "./%d.log", inarg->index);
    char filename[6][50] = {"cpu.log", "disk.log", "enermy.log", "mem.log", "sys.log", "user.log"};

    while (1) {
        sleep(10);
        Node *p = inarg->link;
        while(p->next) {
            int fd;
            if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("Socket");
                write_log(logpath, "[error] : [work1] %s",strerror(errno) );
                return NULL;
            }
            struct sockaddr_in temp; 
            temp.sin_family = AF_INET;
            temp.sin_addr = p->next->addr.sin_addr;
            temp.sin_port = htons(inarg->ctlport);

            if(connect(fd, (struct sockaddr *)&temp, sizeof(temp)) < 0) {
                close(fd);
                continue;
            } 
            for (int i = 1001; i <= 1006; i++ ) {
                
                send(fd, &i, sizeof(int), 0);
                int key = 0;
                int k = recv(fd, &key, sizeof(int), 0);
                if (k <= 0) {
                    printf("error for-recv\n");
                write_log(logpath, "[error] : [work2] %s",strerror(errno) );
                    continue;
                }
                if (!key) continue;
                int recvmsg = socket_create(inarg->mp);
                if (recvmsg < 0) {
                   perror("socket_create");
                write_log(logpath, "[error] : [work3] %s",strerror(errno) );
                    break;
                }
                // key = 1;
                send(fd, &key, sizeof(int), 0);
                struct sockaddr_in addr;
                socklen_t l = sizeof(addr);
                int getfd = accept(recvmsg, (struct sockaddr *)&addr,  &l);
                if (getfd < 0) {
                    perror("error accept");
                write_log(logpath, "[error] : [work4] %s",strerror(errno) );
                    close(recvmsg);
                    continue;
                }
                char fname[50] = {0};
                char dirname[50] = {0};
                //opt/master
                sprintf(dirname, "/opt/master/%s", inet_ntoa(p->next->addr.sin_addr));
                //strcpy(dirname, inet_ntoa(addr.sin_addr));
                if (access (dirname, F_OK) < 0) {
                    int yndir = mkdir(dirname, 0755);
                    if (yndir < 0) {
                        printf("创建文件夹失败了(；′⌒`)`)\n");
                        write_log(logpath, "[warn] : [work5] %s",strerror(errno) );
                        break;
                    }
                }
                char getlog[BUFFSIZE] = {0};
                    sprintf(fname, "%s/%s", dirname, filename[i - 1001]);
                    FILE *fw = fopen(fname, "a+");
                while(1) {
                    int k = recv(getfd, getlog, sizeof(getlog), 0);
                    if (k <= 0) {
                        write_log(logpath, "[warn] : [work6] %s",strerror(errno) );
                        break;
                    }
                    fprintf(fw, "%s", getlog);
                    memset(getlog, 0, sizeof(getlog));
                }
                fclose(fw);
                close(getfd);
                close(recvmsg);
                
            }
            close(fd);
            p = p->next;
        }
    }
}

void *do_warn(void *arg) {
    int port = atoi(arg);
    char recv_buff[4096] = {0};
    struct sockaddr_in server;
    int opt;
    int sockfd;
    int len = sizeof(struct sockaddr_in);
    opt=SO_REUSEADDR;
	if (-1==(sockfd=socket(AF_INET,SOCK_DGRAM,0)))
	{
		perror("create socket error\n");
		exit(1);
	}
	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
   	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	if (-1==bind(sockfd,(struct sockaddr *)&server,sizeof(struct sockaddr))){
		perror("bind error\n");
		exit(1);
	}
    int recvfd;
    while(1) {
        if ((recvfd = recvfrom (sockfd,  recv_buff, sizeof(recv_buff), 0, (struct sockaddr*) &server, &len)) < 0) {
            perror("recvfrom");
            continue;
        } else if (recvfd > 0) {
            write_log(logpath, "[WARNING] : %s", recv_buff);
            printf("%s\n", recv_buff);
            printf("WARNING\n");
        }
    }
    close(sockfd);
    return  0;
    
}



/*
void *do_work(void *arg) {
    struct Work *inarg = (struct Work*)arg;
    char log[50] = {0};
    sprintf(log, "./%d.log", inarg->index);
    int epollfd = epoll_create(MAXCLIENT);
    struct epoll_event events[MAXCLIENT], ev;
    while (1) {
        FILE *fp = fopen(log, "w");
        Node *p = inarg->link;
        while (p->next) {
            int askfd = socket(AF_INET, SOCK_STREAM, 0);
            if (askfd < 0) {
                perror("error socket");
                continue;
            }
            unsigned long ul = 1;
            ioctl(askfd, FIONBIO, &ul);
            struct sockaddr_in naddr;
            naddr.sin_family = AF_INET;
            naddr.sin_port = htons(inarg->ctlport);
            naddr.sin_addr = p->next->addr.sin_addr;
            connect(askfd, (struct sockaddr*)&naddr, sizeof(struct sockaddr));
            fprintf(fp, "%s:%d\n", inet_ntoa(naddr.sin_addr), ntohs(naddr.sin_port));
            ev.events = EPOLLOUT;
            ev.data.fd = askfd;
            epoll_ctl(epollfd, EPOLL_CTL_ADD, askfd, &ev);
            p->fd = askfd;
            p = p->next;
        }
        int reval = epoll_wait(epollfd, events, MAXCLIENT, 3*1000);
        if (reval < 0) {
            perror("Error epoll_wait");
            fclose(fp);
            return NULL;
        } else if (reval == 0) {
            continue;
        }
        struct All_system *spt = (struct All_system*) malloc (sizeof(struct All_system));

        for (int i = 0; i < reval; i++) {
            if (events[i].events & EPOLLOUT) {
                int clientfd = events[i].data.fd;
                unsigned long ul = 0;
                ioctl(clientfd, FIONBIO, &ul);
                int ack = 0;
                struct sockaddr_in addr;
                int len = sizeof(addr);
                memset(&addr, 0, sizeof(addr));
                getpeername(clientfd, (struct sockaddr*)&addr, (socklen_t*)&len);

                for (int i = 1; i <= 2; i++) {
                    send(clientfd, &i, sizeof(i), 0);
                    int k = recv(clientfd, &ack, sizeof(ack), 0);
                    if (k <= 0) break;
                    if (!ack) continue;
                    ack = 1;
                    send(clientfd, &ack, sizeof(ack), 0);
                    memset(spt, 0, sizeof(struct All_system));
                    k = recv(clientfd, spt, sizeof(struct All_system), 0);
                    if (k <= 0) {
                        printf("recv data error on %d\n", i);
                        continue;
                    }
                    char filename[50] = {0};
                    char fdir[50] = {0};
                    strcpy(fdir, inet_ntoa(addr.sin_addr));
                    sprintf(filename, "./%s/%s", fdir, spt->name);
                    if (access(fdir, F_OK) < 0) {
                        int statu = mkdir(fdir, 0755);
                        if(statu < 0) {
                            printf("创建文件夹失败\n");
                            break;
                        }
                    }
                    FILE *fw = fopen(filename, "a+");
                    flock(fw->_fileno, LOCK_EX);
                    fprintf(fw, "%s", spt->buff);
                    fclose(fw);
                    sleep(1);
                } 
                struct epoll_event em;
                em.data.fd = clientfd;
                epoll_ctl(epollfd, EPOLL_CTL_DEL, clientfd, &em);
                close(clientfd);
            }
        }
        fclose(fp);
        sleep(5);
    }
}


void *do_work(void *arg) {
    struct Work *inarg = (struct Work*)arg;
    char log[50] = {0};
    sprintf(log, "./%d.log", inarg->index);
        int epollfd = epoll_create(MAXCLIENT);
        struct epoll_event events[MAXCLIENT], ev;
    while (1) {
        FILE *fp = fopen(log, "w");
        Node *p = inarg->link;
        while (p->next) {
            int askfd = socket(AF_INET, SOCK_STREAM, 0);
            if (askfd < 0) {
                perror("Error socket on askfd");
                continue;
            }
            unsigned long ul = 1;
            ioctl(askfd, FIONBIO, &ul);
            struct sockaddr_in naddr;
            naddr.sin_family = AF_INET;
            naddr.sin_port = htons(inarg->ctlport);
            naddr.sin_addr = p->next->addr.sin_addr;
            connect(askfd, (struct sockaddr*)&naddr, sizeof(struct sockaddr));
            fprintf(fp, "%s:%d\n", inet_ntoa(naddr.sin_addr), ntohs(naddr.sin_port));
            ev.events = EPOLLOUT;
            ev.data.fd = askfd;
            epoll_ctl(epollfd, EPOLL_CTL_ADD, askfd, &ev);
            p->fd = askfd;
            p = p->next;
        }
        int reval = epoll_wait(epollfd, events, MAXCLIENT, 3*1000);
        if (reval < 0) {
            perror("Error epoll_wait");
            fclose(fp);
            return NULL;
        } else if (reval == 0) {
            continue;
        }
        for (int i = 0; i < reval; i++) {
            if (events[i].events & EPOLLOUT) {
                int clientfd = events[i].data.fd;
                unsigned long ul = 0;
                ioctl(clientfd, FIONBIO, &ul);

                struct epoll_event em;
                for (int i = 0; i < 2; i++) {
                char buff[BUFFSIZE * 4] ;
                send(clientfd, "hello", strlen("hello"), 0);
                
                int key = 0;
                int k = recv(clientfd, &key, sizeof(key), 0);
                if (k <= 0) {
                    break;
                } else {
                    printf("%d\n", key);
                }
                key = 1;
                send(clientfd, &key, sizeof(key), 0);
                memset(buff, 0 ,sizeof(buff));
                recv(clientfd, buff, sizeof(buff), 0);
                printf("%s\n", buff);
                }
                //int n = recv(clientfd, buff, sizeof(buff), 0);
                //printf("%s\n", buff);
                em.data.fd = clientfd;
                epoll_ctl(epollfd, EPOLL_CTL_DEL, clientfd, &em);
                close(clientfd);
            }
        }
        fclose(fp);
        sleep(5);
    }
}
*/

int do_epoll(int listenfd, LinkList *link, int *sum, int ins, int heartport) {
    unsigned long ul = 1;
    int nfds;
    ioctl(listenfd, FIONBIO, &ul);
    int epollfd = epoll_create(MAXCLIENT);
    if (epollfd < 0) {
        perror("epoll_create");
        exit(1);
    }
    struct epoll_event events[MAXCLIENT], ev;
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) < 0) {
        perror("epoll_ctl");
        exit(0);
    }
    while (1) {
        nfds = epoll_wait(epollfd, events, MAXCLIENT, -1);
        if (nfds < 0) {
            perror("epoll_wait");
            exit(1);
        } else if (nfds == 0) {
            continue;
        }
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listenfd && events[i].events & EPOLLIN) {
                struct sockaddr_in addr;
                socklen_t len = sizeof(addr);
                int newfd = accept(listenfd, (struct sockaddr*)&addr, &len);
                if (newfd < 0) {
                    perror("accept");
                    exit(1);
                }
                int sub = find_min(sum, ins);
                Node *p = (Node *) malloc (sizeof(Node));
                addr.sin_port = htons(heartport);
                p->addr = addr;
                p->fd = newfd;
                p->next = NULL;
                insert(link[sub], p);
                sum[sub] += 1;
                printf("加入成功\n");
            }
        }
    }
}

