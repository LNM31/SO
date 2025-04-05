#include <stdio.h>      // pentru printf, scanf, perror
#include <stdlib.h>     // pentru exit, malloc (dacă vei folosi)
#include <string.h>     // pentru strcpy, strcmp, fgets, strcspn
#include <fcntl.h>      // open, O_CREAT, O_WRONLY etc.
#include <unistd.h>     // close, read, write, lseek, unlink, rmdir
#include <sys/stat.h>   // mkdir, stat, lstat, struct stat
#include <dirent.h>     // manipularea directoarelor (dacă vei citi conținutul)
#include <time.h>       // pentru afișarea timpului ultimei modificări (cu stat)

typedef struct {
    int id;
    char name[32];
    float x,y;
    char clue[128];
    int value;
}treasure;
int main()
{
    printf("Hello world!\n");
    return 0;
}