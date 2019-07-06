/*************************************************************************
	> File Name: xcsum.cpp
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月05日 星期五 11时38分51秒
 ************************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/file.h>

#define INS 10
int sum = 0;
pthread_mutex_t mtx;

void *add(void * arg) {
    int *x = (int *) arg;
    int cnt = 0;
    int temp = (*x) - 1 ;
    printf("temp = %d    ", temp);
    for (int i = 1 + temp * 100  ; i <= temp * 100 + 100 ; i++) {
        cnt += i;
    }
    pthread_mutex_lock(& mtx);
    sum += cnt;
    printf("sum = %d\n", sum);
    pthread_mutex_unlock(&mtx);

}

int main() {
    int pnum = 0;
    for(int i = 0; i < INS; i++ ){
        pthread_t pth;
        ++pnum;
        pthread_create(&pth, NULL, add, (void *) &pnum);
        pthread_join(pth, NULL);
    }
    printf("%d\n", sum);
    return 0;
}
