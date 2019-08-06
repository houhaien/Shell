/*************************************************************************
	> File Name: client.h
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月26日 星期五 19时45分04秒
 ************************************************************************/

#include "common.h"
#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/param.h>
#include <limits.h>

#define Size 1024
#define N 6
typedef struct All_system{
    char name[20];
    char mbuff[Size * 5];
}Shmsg;
 

int get_system_value(char *filename, char *filesh, int index) {
    double dy = 0; 
    char buff[Size] = {0};
    char buffs[4 * Size] = {0};
    FILE *fp, *fw;
    fw = fopen(filename, "a+");
    if (index != 3) {
        for(int i = 1; i <= 5 ; i += 1) {
            usleep(1000);
        fp = popen(filesh, "r");
        fread(buff, sizeof(char), sizeof(buff), fp);
        strcat(buffs, buff);
        }
    } else {
        for (int i = 1; i <= 5;i++) {
        usleep(1000);
        char path[50] = {0};
        strcpy(path, filesh);
        sprintf(path, "  %.2lf", dy);
        sprintf(path, "bash %s", path);
            fp = popen(path, "r");
            if (fgets(buff, Size, fp) != NULL) {
                strcat(buffs, buff);
            }
            if (fgets(buff, Size, fp) != NULL) {
                dy = atof(buff);
            }
        }
    }
    fwrite(buffs, 1, strlen(buffs), fw);
    pclose(fp);
    fclose(fw);
    return 1;
}


