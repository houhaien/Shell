/*************************************************************************
	> File Name: shm.c
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月28日 星期日 18时33分38秒
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

struct Msg{
    pthread_mutex_t sm_tex; // suo
    pthread_cond_t sm_ready;// xinhao
    char buff[1024];
};

int main() {
    char *shmaddr = NULL;
    
    //属性
    pthread_mutexattr_t m_attr;
    pthread_condattr_t c_attr;
    //初始化
    pthread_mutexattr_init(&m_attr);
    pthread_condattr_init(&c_attr);
    //设置共享 
    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);

    int shmid = shmget(IPC_PRIVATE, sizeof(struct Msg), IPC_CREAT | 0666 | IPC_EXCL);
    
    if ((shmaddr = shmat(shmid, 0,0)) == NULL) {
        perror("shmat");
        exit(1);
    }
    //char *buff = shmaddr;
    struct Msg *msg = (struct Msg *) shmaddr;
    pthread_mutex_init(&msg->sm_tex, &m_attr);
    pthread_cond_init(&msg->sm_ready, &c_attr);

        pid_t pid;
        pid = fork() ;
        if (pid < 0) {
                perror("fork");
                exit(1); 
        }
        if (pid == 0) {
            while(1) {
                scanf("%s", msg->buff);
                pthread_mutex_lock(&msg->sm_tex); 
                pthread_cond_signal(&msg->sm_ready); 
                pthread_mutex_unlock(&msg->sm_tex);
                }
        } else if (pid > 0) {
            while(1) {
                pthread_mutex_lock(&msg->sm_tex); 
                pthread_cond_wait(&msg->sm_ready, &msg->sm_tex);
                pthread_mutex_unlock(&msg->sm_tex);
                printf("%s\n", msg->buff);
                memset(msg->buff, 0, sizeof(msg->buff));
            }
        }
    return 0;
}
