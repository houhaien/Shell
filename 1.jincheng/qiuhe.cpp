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
int main() {
    
    FILE *fp;
    fp = fopen("./sum", "w+");
    int x = 0;
    long long sum = 0;
    fwrite(&sum, sizeof(sum), 1, fp);
    fclose(fp);

    for (int i = 0; i < 10; i++) {
        pid_t pid;
        pid = fork();
        if (pid == 0){
            x = i;
            break;
        }
    }
    for (int i = 0; i < 10; i++) {
        if (x == i) {
            fp = fopen("./sum", "w+");
            sum = fread(&sum, sizeof(sum), 1, fp);
            fclose(fp);
            for (int j = 0 + i * 1000 ; j <= 1000 + i * 1000; j++) {
                sum += j;
            } 
            fp = fopen("./sum", "w+");
            fwrite(&sum, sizeof(sum), 1, fp);
            fclose(fp);
        }
    }

    fp = fopen("./sum", "w+");
    sum = fread(&sum, sizeof(sum), 1, fp);
    fclose(fp);
    printf("%lld", sum);
    
    return 0;
}

