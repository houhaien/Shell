/*************************************************************************
	> File Name: client.cpp
	> Author: houhaien 
	> Mail: github.com/houhaien 
	> Created Time: 2019年07月09日 星期二 10时52分52秒
 ************************************************************************/

#include "common.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main() {
    int port = 9999;
    char ip[20] = "192.168.2.214";
    int socket = socket_connect(port, ip);
    char buff[100];
    int k=1 ;
    int num = 1;
    cout << "请输入你要发送的内容：" << endl;
    while(cin >> buff){
    if (!strcmp(buff, "EOF")) break;
    if (send(socket, buff, strlen(buff), 0) < 0) {
        perror("send");
    }
    if (recv(socket, buff,1024, 0) < 0) {
        perror("recv");
    }
    //memset(buff, 0, sizeof(buff));
    cout <<"服务端 ：" << buff  << endl;
    }
    close(socket);
    return 0;
}

