/*************************************************************************
	> File Name: slp.cpp
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月09日 星期二 22时02分27秒
 ************************************************************************/



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

void hx() {
    printf("123\n");
}

int main() {
    int n;
    scanf("%d", &n);
    signal(SIGALRM, hx);
    time_t tm1, tm2;
    time(&tm1);
    printf("%s\n", ctime(&tm1));
    //alarm 设置信号传送闹钟
    alarm(n);
    //alarm(6);
    pause();
    printf("\n");
    time(&tm2);
    printf("%s\n", ctime(&tm2));
    printf("今天又是新的一天\n");
    return 0;
}
