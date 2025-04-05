#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>

typedef struct {
    int id;
    char name[32];
    float x,y;
    char text[128];
    int value;
}treasure;
int main()
{
    printf("Hello world!\n");
    return 0;
}