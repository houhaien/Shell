
#ifndef _LINKLIST_H
#define _LINKLIST_H
typedef struct Node {
    char name[20];
    struct Node *next;
}Node, *LinkedList;

LinkedList linkedlist;

//判断是否登录
int is_in(LinkedList head, char *name) {
    Node *node = head;
    while(node->next != NULL) {
        if(!strcmp(node->next->name,name)) {
           // printf("登录失败，该用户已在线\n");
            return 1;
        }
    }
    return 0;
}

//插入节点
LinkedList insert (LinkedList head, Node *nod) {
    Node *node = head; 
    while(node->next != NULL) {
        node = node->next;
    }
    node ->next = nod;
   // printf("您的好友%s已上线\n",nod->name);
    return head;
}

//删除节点
LinkedList delete_node(LinkedList head, char *dest_name) {
    Node *node = head;
    while (node->next != NULL) {
        if (!strcmp (node->next->name, dest_name)){
            Node *temp_node = node->next;
            node->next = temp_node->next;
            printf("您的好友%s已下线\n",temp_node->name);
            free(temp_node);
            return head;
        }
        node = node->next;
    }
    printf("该用户不存在或未在线\n");
    return head;
}
//遍历打印
void show (LinkedList head) {
    Node *nod = head->next;
    printf("在线人员：");
    while (nod != NULL) {
        printf("[ %s ] ",nod->name);
        nod = nod->next;
    }
    printf("\n");
}
//销毁函数释放空间
void l_clear(LinkedList head) {
    Node *node =head->next ;
    while(node != NULL) {
        Node *tmp = node->next;
        printf("销毁%s\n",node->name);
        free(node);
        node = tmp; 
    }
    free(head);
}

LinkedList CreateNode( char *name) {
    Node *p = (Node *) malloc (sizeof(Node));
    p->next = NULL;
    strcpy(p->name,name);
    return p;
}

int getNum(LinkedList head) {
    Node *nod = head->next;
    int temp = 0;
    while(nod != NULL) {
        temp ++;
        nod = nod->next;
    }
    return temp;
}


#endif
