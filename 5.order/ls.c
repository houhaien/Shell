/*************************************************************************
	> File Name: ls.c
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月09日 星期二 20时12分20秒
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

void showinfo(char *name) {
    struct stat st;
    if (stat(name, &st) < 0) {

        perror("stat");
    }
    printf("%s\t%d\n", name, st.st_nlink);
}


int main(int argc, char **argv) {
    struct dirent *entry;
    DIR *dir = opendir(argv[1]);
    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }
    while((entry = readdir(dir)) != NULL) {
        showinfo(entry -> d_name);
    }
    return 0;
}


