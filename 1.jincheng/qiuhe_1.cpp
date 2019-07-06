/*************************************************************************
	> File Name: qiuhe_1.cpp
	> Author: victoria 
	> Mail: 1105847344@qq.com
	> Created Time: 2019年07月04日 星期四 19时35分47秒
 ************************************************************************/

#include <iostream>
#include <algorithm>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/file.h>

using namespace std;
char sum_file[] = "./sum.txt";
char now_file[] = "./now.txt";
void get_num(char *filename, long long *num, FILE *fp){
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, fp);
    *num = atoi(line);
    free(line);
}

void set_num(char *filename, long long num, FILE *fp) {
    fprintf(fp,"%d", num);
}

int main() {
    int x = 0;
    /*get_num(now_file, &num);
    cout << num << endl;
    num = 12;
    set_num(sum_file, num);*/
    long long num = 0;
    long long sum = 0;
    FILE *fp, *fp2;
    fp = fopen(now_file, "w");
    fp2 = fopen(sum_file, "w");
    set_num(now_file,num, fp);
    set_num(sum_file,sum, fp2);
    fclose(fp);
    fclose(fp2);
    pid_t pid;
    for(int i = 1; i <= 10; i++) {
        pid = fork();
        if (pid == 0) {
            x = i;
            break;
        }
    }
    /*for (int i = 1; i <= 10; i++) {
        if (x == i) {
            fp = fopen(now_file, "r");
            flock(fp->_fileno, LOCK_EX);
            get_num(now_file, &num, fp);
            fp = fopen(sum_file, "r");
            get_num(sum_file, &sum, fp);
            for(int i = 0; i < 1000; i++){
                num ++;
                sum += num;
            }
            fp = fopen(now_file, "w");
            set_num(now_file,num, fp);
            fp = fopen(sum_file, "w");
            set_num(sum_file,sum, fp);
            fclose(fp);
            cout << sum << endl;
        } 
    }*/
    /*if (pid == 0){
    while(1){
        get_num(now_file, &num);
        get_num(sum_file, &sum);
        num ++;
        sum += num;
        set_num(now_file,num);
        set_num(sum_file,sum);

    }
    exit(0);
    }*/
    if (pid == 0) {
            fp = fopen(now_file, "r");
            flock(fp->_fileno, LOCK_EX);
            fp2 = fopen(sum_file, "r");
            flock(fp2->_fileno, LOCK_EX);
            
            get_num(now_file, &num, fp);
            get_num(sum_file, &sum, fp2);
         for (int i = 0; i < 1000; i++) {
            num ++;
            sum += num;
         }
            fp = fopen(now_file, "w");
            fp2 = fopen(sum_file, "w");
            set_num(now_file,num, fp);
            set_num(sum_file,sum, fp2);
            fclose(fp);
            fclose(fp2);
        cout << sum << " -> " << x << endl;    
    }
    int n = 0;
    while (n < 10) {
        waitpid(-1, NULL, 0);
        n++;
    }
    return 0;
}




