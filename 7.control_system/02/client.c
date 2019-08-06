/*************************************************************************
	> File Name: 4.client.c
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年08月03日 星期六 22时37分22秒
 ************************************************************************/

#include <stdio.h>
#include "client.h"
#include <pthread.h>
#define BUFFSIZE 1024
#include <sys/file.h>
struct Share {
    int share_cnt;
    pthread_mutex_t smutex;
    pthread_cond_t sready; 
};

double dya = 0;
char check_info[6][BUFFSIZE*4];

pthread_mutexattr_t mattr;
pthread_condattr_t cattr;
char *config = "./pihealthc.log";

void do_con(char *ip, int port, struct Share *msg) {
    write_log(config, "[ok] :  [do_con] ");
    int tm = 2;
    printf("❤ ");
    fflush(stdout);
    while (1) {
        if (msg->share_cnt < 5) {
            printf("心跳❤ 终止\n");
            break;
        }
        int sockfd = socket_connect(port, ip);
        printf("sockfd %d\n", sockfd);
        if (sockfd < 0) {
            close(sockfd);
            sleep(tm);
            tm += 2;
            if (tm > 10) {
                tm = 10;
            }
            continue;
        }
        printf("连接成功\n");
        close(sockfd);
        break;
    }
}

void recv_heart(int port, struct Share *msg) {

    int sockfd = socket_create(port);
    if (sockfd < 0) {
        write_log(config, "[error] : ", strerror(errno));
        return ;
    }

    write_log(config, "[ok] :  [recv_data] ");
    while (1) {
        int newfd = accept(sockfd, NULL, NULL) ;
        pthread_mutex_lock(&msg->smutex);
        msg->share_cnt = 1;
        pthread_mutex_unlock(&msg->smutex);
        printf(" ❤ ");
        fflush(stdout);
        close(newfd);
    } 
    close(sockfd);
}

void recv_data(int ctlPort, int dataPort, char *ip, struct Share *msg) {
    int listenfd;
    if ((listenfd = socket_create(ctlPort)) < 0) {
        exit(1);
    }
    while (1) {
        int newfd;
        if ((newfd = accept(listenfd, NULL, NULL)) < 0) {
            perror("accept");
            continue;
        }
        for (int i = 0; i < 6; i++) {
            int ack = 0, fno;
            int k = recv(newfd, &fno, sizeof(int), 0);
            if (k <= 0) {
                close(newfd);
                continue;
            }
            char path[50] = {0};
            switch (fno) {
                case 1001 : {
                    sprintf(path, "./cpu.log");
                }break;
                case 1002 : {
                    sprintf(path, "./disk.log");
                }break;
                case 1003 : {
                    sprintf(path, "./enermy.log");
                }break;
                case 1004 : {
                    sprintf(path, "./mem.log");
                }break;
                case 1005 : {
                    sprintf(path, "./sys.log");
                }break;
                case 1006 : {
                    sprintf(path, "./user.log");
                }break;
            }
            if (access(path, F_OK) < 0) {
                ack = 0;
                send(newfd, &ack, sizeof(ack), 0);
                continue;
            }
            ack = 1;
            send(newfd, &ack, sizeof(ack), 0);
            ack = 0;
            k = recv(newfd, &ack, sizeof(ack),0);
            if (k < 0 || ack != 1) {
                close(newfd);
                continue;
            }
            int sendfd = socket_connect(dataPort, ip);
            FILE *fp = fopen(path, "r");
            flock(fp->_fileno, LOCK_EX);
            char buff[BUFFSIZE] = {0};
            while (fgets(buff, BUFFSIZE, fp) != NULL) {
                send(sendfd, buff, strlen(buff), 0);
                memset(buff, 0, sizeof(buff));
            }
            fclose(fp);
            
            close(sendfd);
            printf("<%s> is ok !!\n", path);
            
            remove(path);
        }
        pthread_mutex_lock(&msg->smutex);
        msg->share_cnt = 0;
        pthread_mutex_unlock(&msg->smutex);
        close(newfd);
    }
    close(listenfd);
}

int get_info(char *bsname, char *destfile, int cnt, int index) {
    FILE *fw, *fp;
    char filename[100] = {0};
    sprintf(filename, "bash %s", bsname);
    fw = popen(filename, "r");
    if (!fw) {
        return 0;
    }
    char buff[BUFFSIZE] = {0};
    if (index == 2) {
        if( fgets(buff, BUFFSIZE, fw) != NULL)
            strcat(check_info[index], buff);
        if( fgets(buff, BUFFSIZE, fw) != NULL) {
             dya= atof(buff);
        }
    } else {
        while(fgets(buff, BUFFSIZE, fw) != NULL) {
            strcat(check_info[index], buff);
        }
    }
    if (cnt == 5) {
        fp = fopen(destfile, "a+");
        flock(fp->_fileno, LOCK_EX);
        fprintf(fp,"%s", check_info[index]);
        memset(check_info[index], 0, sizeof(check_info[index]));
        fclose(fp);
    }
    pclose(fw);
    return 1;
}
/*
void bj() {
    int port =  9520;
    char ip[20] = "192.168.2.214";
    FILE *fp = fopen("./enermy.log", "r");
    char buff[4096] = {0};
    char cnt [521] = {0};
    while(fgets(cnt, sizeof(cnt), fw) != NULL) {
        strcat(buff, cnt);
    }
    socket_connect_udp(port, ip, buff);
    return;
}

*/
void do_check(struct Share *msg, int cnt) {
    int flag;
    flag = get_info("cpu_info.sh", "./cpu.log", cnt, 0); 
    flag &= get_info("Control_Disk.sh", "./disk.log", cnt, 1);
    char buff[50] = {0};
    sprintf(buff, "Memlog.sh %.2lf", dya);
    flag &= get_info(buff, "./mem.log", cnt, 2); 
    flag &= get_info("Users.sh", "./user.log", cnt, 3); 
    flag &= get_info("SysInfo.sh", "./sys.log", cnt, 4); 
    
   /* 
    int k = get_info("Error_course.sh", "./enermy.log", cnt, 5); 
    if(k > 0) {
        FILE *fp = NULL;
        fp = fopen("./enermy.log", "r");
        char ch;
        ch = fgetc(fp);
        if (ch == EOF) {
            fclose(fp);
            bj();  
            return ;
        } 
        fclose (fp);
    } 
    */

    flag &= get_info("Error_course.sh", "./enermy.log", cnt, 5); 
   
    if (flag == 0) {
        printf("error get_info \n");
        return ;
    } else {
        printf(" ok ");
    }
    sleep(2);
    
    if (msg->share_cnt >= 5) return ;

    pthread_mutex_lock(&msg->smutex);
    msg->share_cnt += 1;
    if (msg->share_cnt >= 5) {
        pthread_cond_signal(&msg->sready);
    }
    pthread_mutex_unlock(&msg->smutex);
}

int main() {
 int pid0 = fork();
if (pid0 == 0) {
    setsid();
    umask(0);
    for (int i = 0; i < NOFILE; i++) {
        close(i);
    }
    
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    int heartPort, ctlPort, loadPort, dataPort;
    char tmp[20]={0};
    char *config = "./pihealthc.conf";
    char ip[20]={0};
   
    get_conf_value(config, "Hport", tmp);
    heartPort = atoi(tmp);
    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "Mip", tmp);
    strcpy(ip, tmp);
    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "Cport", tmp);
    ctlPort = atoi(tmp);
    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "Mport", tmp);
    loadPort = atoi(tmp);
    memset(tmp, 0, sizeof(tmp));
    get_conf_value(config, "MSPORT", tmp);
    dataPort = atoi(tmp);
    
    chdir("/tmp");
    
    struct Share *share_msg = NULL;
    pthread_mutexattr_init(&mattr);
    pthread_condattr_init(&cattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
    
    int shmid = shmget(IPC_PRIVATE, sizeof(struct Share), IPC_CREAT | 0666);
    share_msg = (struct Share*) shmat (shmid, NULL, 0); 
    pthread_mutex_init(&(share_msg->smutex), &mattr);
    pthread_cond_init(&(share_msg->sready), &cattr);
    
    pid_t pid = fork();
    if (pid > 0) {
        recv_data(ctlPort, dataPort, ip, share_msg);
        shmdt((void*)share_msg);
        wait(&pid);
    }else if (pid == 0) {
        pid_t npid = fork();
        if (npid > 0) {  
            recv_heart(heartPort, share_msg);
            wait(&npid);
            exit(0);
        }
        if (npid == 0) {
            pid_t mpid = fork();
            if (mpid > 0) {
                while (1) {
                    pthread_mutex_lock(&share_msg->smutex);
                    pthread_cond_wait(&share_msg->sready, &share_msg->smutex);
                    pthread_mutex_unlock(&share_msg->smutex);
                    do_con(ip, loadPort, share_msg);
                    pthread_mutex_lock(&share_msg->smutex);
                    share_msg->share_cnt = 0;
                    pthread_mutex_unlock(&share_msg->smutex);
                }
                wait(&mpid);
                exit(0);
            } else if (mpid == 0) {
                int cnt = 0;
                while (1) {
                    cnt ++;
                    do_check(share_msg, cnt);
                    if(cnt >= 5) cnt = 0;
                }
                exit(0);
            }
        }
    }

} else if (pid0 > 0){
    exit(0);
} else {
    exit(1);
}

    return 0;
}

