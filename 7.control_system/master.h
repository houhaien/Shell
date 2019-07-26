/*************************************************************************
	> File Name: master.h
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月26日 星期五 20时39分47秒
 ************************************************************************/

#include "common.h"
#include <sys/wait.h>
#include <sys/epoll.h>

#define MAXCLIENT 10000
#define BUFFSIZE 1024

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
};

int find_min(int *sum, int ins);
int insert(LinkList head, Node *q);
void output(LinkList head);
void *do_heart(void *arg);
void *do_work(void *arg);
int check_connect(struct sockaddr_in addr, long timeout);
int listen_epoll(int listenfd, LinkList *link, int *sum, int ins, int heartport);

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

void *do_work(void *arg) {
    struct Work *inarg = (struct Work*)arg;
    char log[50] = {0};
    sprintf(log, "./%d.log", inarg->index);
    while (1) {
        FILE *fp = fopen(log, "w");
        Node *p = inarg->link;
        int epollfd = epoll_create(MAXCLIENT);
        struct epoll_event events[MAXCLIENT], ev;
        
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
                char buff[BUFFSIZE] = {0};
                send(clientfd, "hello", strlen("hello"), 0);
                int k = recv(clientfd, buff, BUFFSIZE, 0);
                if (k > 0) {
                    printf("%s\n", buff);
                }

                em.data.fd = clientfd;
                epoll_ctl(epollfd, EPOLL_CTL_DEL, clientfd, &em);
                close(clientfd);
            }
        }
        fclose(fp);
        sleep(5);
    }
}


int listen_epoll(int listenfd, LinkList *link, int *sum, int ins, int heartport) {
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

