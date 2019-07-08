/*************************************************************************
	> File Name: qiuhe.cpp
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月04日 星期四 18时07分28秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/file.h>
char sum_file[] = "./sum.txt";
char now_file[] = "./now.txt";

#define MAXN 10000
#define INS 20

void get_num(char *filename, int *num) {
    
    FILE *fp;
    ssize_t nread;
    char *line = NULL;
    size_t len = 0;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
    }
    nread = getline(&line , &len, fp);
    *num = atoi(line);
    free(line);
    fclose(fp);
}

void set_num(char *filename, int num) {
    FILE *fp;
    fp = fopen(filename, "w");
    if (fp == NULL) perror("fopen");
    fprintf(fp, "%d", num);
    fclose(fp);
}

int main() {
    
    pid_t pid;
    int x = -1;
    int zsum = 0;
    set_num(sum_file, 0);
    set_num(now_file, 0);
    for (int i = 0; i < INS; i++) {
        pid = fork();
        if (pid == 0){
            x = i;
            break;
        }
    }
    if (pid == 0) {
        while(1) {
            FILE * fp = fopen(sum_file, "r");
            flock(fp->_fileno, LOCK_EX);
            int sum = 0;
            int cnt = 0;
            get_num(sum_file, &sum);
            get_num(now_file, &cnt);
            if (cnt > MAXN) break;
            sum += cnt;
            set_num(now_file, cnt + 1);
            set_num(sum_file, sum);
            //printf("%d\n", sum);
            flock(fp->_fileno, LOCK_UN);
        }
        exit(0);
    }
    
    int n = 0;
    while(n < INS) {
        waitpid(-1,NULL, 0);
        n++;
    }

    get_num(sum_file, &zsum);
    printf("%d\n", zsum);
    //printf("%d\n",x);
    //set_num(now_file, 1111);
    return 0;
}

