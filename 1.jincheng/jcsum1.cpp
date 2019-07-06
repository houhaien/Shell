/*************************************************************************
	> File Name: jcsum.cpp
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月05日 星期五 10时13分24秒
 ************************************************************************/

#include <iostream>
#include <cstdio>
#include <sys/wait.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/file.h>
using namespace std;

char sum_file[] = "./sum.txt";
char now_file[] = "./now.txt";

#define MAXN 10000
#define INS 10

void getnum(char *filename, int *num, FILE *fp) {
    char *line = NULL;
    size_t len = 0;
    if (fp == NULL) perror("fopen");
    getline(&line, &len, fp);
    *num = atoi(line);
    free(line);
    //fclose(fp);
}
void setnum(char *filename, int num, FILE *fpw) {
    if (fpw == NULL) perror("fopen");
    fprintf(fpw, "%d", num);
}
int main() {
    pid_t pid;
    int zsum = 0;
    FILE *fps, *fpn, *fpf;
    fps = fopen(sum_file, "w");
    fpn = fopen(now_file, "w");
    setnum(sum_file, 0,fps);
    setnum(now_file, 0,fpn);
    fclose(fps);
    fclose(fpn);

    for (int i = 0; i <INS; i++) {
        pid = fork();
        if (pid == 0) break;
    }
    int sum  = -1;
    int cnt = 0;
    if (pid == 0) {
        while(1) {
            fpf = fopen(sum_file, "r");
            flock(fpf->_fileno, LOCK_EX);

            fps = fopen(sum_file, "r");
            fpn = fopen(now_file, "r");
            getnum(sum_file, &sum, fps);
            getnum(now_file, &cnt, fpn);
            fclose(fps);
            fclose(fpn);
            cnt ++;
            if(cnt > MAXN) break;
            sum += cnt;
            fps = fopen(sum_file, "w");
            setnum(sum_file, sum, fps);
            fpn = fopen(now_file, "w");
            setnum(now_file, cnt, fpn);
            fclose(fps);
            fclose(fpn);
            flock(fpf->_fileno, LOCK_UN);
            fclose(fpf);
        }
    }
    //等待子进程
    int n = 0;
    while (n < INS) {
        waitpid(-1,NULL, 0);
        n++;
    }
    fps = fopen(sum_file, "r");
    //getnum(sum_file, zsum, fps);
    if (sum != -1)
    printf("%d\n", sum);
    return 0;
}
