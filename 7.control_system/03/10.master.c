/*************************************************************************
	> File Name: master.c
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月26日 星期五 20时43分03秒
 ************************************************************************/

//#include "3.ma.h"
#include "2.ma.h"
#include <limits.h>
#include <sys/param.h>
int main() {
   // FILE *fd = fopen(logpath, "a+");
    //fclose(fd);
    
pid_t tpid = fork();
    if (tpid < 0) {
        perror("tpid");
        exit(0);
    } else if (tpid > 0) {
        exit(0);
    }
    setsid();
    umask(0);

    for (int i = 0; i < 1024; i++) {
        close(i);
    }
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);


    int listenfd;
    char *config = "/opt/master/pihealthd.conf";
    char *writelog= "/opt/master/writlog.log";
    //char *writelog= "./writlog.log";
    int Port, Ins, ClientHeart_port, CtlPort, Errport;
    int Msgport;
    long Timeout;
    char FromIp[20] = {0}, ToIp[20] = {0};
    pthread_t pth_heart;


    char tmp[20] = {0};
    get_conf_value(config, "INS", tmp);
    Ins = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "ClientHeart", tmp);
    ClientHeart_port = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "MasterPort", tmp);
    Port = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "MSGPORT", tmp);
    Msgport = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "ERRPORT", tmp);
    Errport = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "CTPORT", tmp);
    CtlPort = atoi(tmp);

    printf("H = %d , MP = %d, CP= %d\n", ClientHeart_port, Port, CtlPort);

    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "TimeOut", tmp);
    Timeout = atol(tmp);

    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "From", tmp);
    strcpy(FromIp, tmp);

    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "To", tmp);
    strcpy(ToIp, tmp);
    
    //chdir("/home/hou/haizei/2.Shell/7.control_system/");
    chdir("/opt");
    
    LinkList *linklist = (LinkList*) malloc (sizeof(LinkList) * Ins);
    int *sum = (int*) malloc (sizeof(int) * Ins);
    memset(sum, 0, sizeof(int) * Ins);

    struct sockaddr_in initaddr;
    initaddr.sin_family = AF_INET;
    initaddr.sin_port = htons(ClientHeart_port);
    initaddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    for (int i = 0; i < Ins; i++) {
        Node *p = (Node*) malloc (sizeof(Node));
        p->addr = initaddr;
        p->fd = -1;
        p->next = NULL;
        linklist[i] = p;
    }

    unsigned int sip, eip;
    sip = ntohl(inet_addr(FromIp));
    eip = ntohl(inet_addr(ToIp));
    for (unsigned int i = sip; i <= eip; i++) {
        if (i % 256 == 0 || i % 256 == 255) continue;
        initaddr.sin_addr.s_addr = htonl(i);
        Node *p = (Node*) malloc (sizeof(Node));
        p->addr = initaddr;
        p->fd = -1;
        p->next = NULL;
        int sub = find_min(sum, Ins);
        if (insert(linklist[sub], p)) {
            sum[sub] += 1;
        }
    }

    for (int i = 0; i < Ins; i++) {
        printf("Ins <%d> :\n", i);
        output(linklist[i]);
    }

    Heart heartArg;
    heartArg.sum = sum;
    heartArg.link = linklist;
    heartArg.ins = Ins;
    heartArg.timeout = Timeout;
    pthread_create(&pth_heart, NULL, do_heart, (void*)&heartArg);

    struct Work workarg[Ins];
    pthread_t pth[Ins];

    for (int i = 0; i < Ins; i++) {
        workarg[i].link = linklist[i];
        workarg[i].index = i;
        workarg[i].ctlport = CtlPort;
        workarg[i].mp = Msgport;
        pthread_create(&pth[i], NULL,work, (void*)&workarg[i]);
    }
   
    pthread_t pth_warn;
    pthread_create(&pth_warn, NULL, do_warn, (void *) &Errport);
   // pthread_join(pth_warn, NULL);

    if ((listenfd = socket_create(Port)) < 0) {
        perror("socket_create");
        write_log(writelog, "[error] : [socket_create]  %s", strerror(errno));
        exit(0);
    }

    do_epoll(listenfd, linklist, sum, Ins, ClientHeart_port);

    for (int i = 0; i < Ins; i++) {
        pthread_join(pth[i], NULL);
    }

    pthread_join(pth_heart, NULL);
    return 0;
}

