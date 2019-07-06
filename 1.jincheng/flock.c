/*************************************************************************
	> File Name: flock.c
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月04日 星期四 20时35分00秒
 ************************************************************************/

#include <stdio.h>
#include <sys/file.h>
#include <unistd.h>
#include<sys/wait.h>
#define PATH "./asd.txt"
#include <stdlib.h>
int main() {
    FILE * fp = fopen(PATH, "w");
    pid_t pid;
    flock (fp->_fileno, LOCK_EX);
    printf("已加锁！\n");
    //解锁
    flock (fp->_fileno, LOCK_UN);

    pid = fork() ;
    if (pid == 0){
        fclose(fp);
        printf("in child\n");
        fp = fopen(PATH, "r");
        flock(fp->_fileno, LOCK_EX);//判断以前这个文件有没有锁，没有就加上， 有就跳过
        printf("after try flock\n");
        fflush(stdout);
        exit(0);
    }
    sleep(3);
    fclose(fp);//文件关闭，锁销毁
    wait(NULL);
    return 0;
}

