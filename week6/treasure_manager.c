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

void add(char *hunt_id)
{
    char dir_path[256],file_path[256],log_path[256],link_path[256];
    sprintf(dir_path,"%s",hunt_id);
    sprintf(file_path,"%s/treasures.dat",hunt_id);
    sprintf(log_path,"%s/logged_hunt",hunt_id);
    sprintf(link_path,"logged_hunt-%s",hunt_id);

    //Creaza directorul daca nu exista
    struct stat st;
    if(stat(dir_path,&st)==-1) 
    {
        if(mkdir(dir_path,0777)==-1)
        {
            perror("Eroare mkdir");
            exit(-1);
        }
    }



}
int main(int argc,char **argv)
{
    printf("Hello world!\n");
    if(argc==3)
    {
        if(strcmp(argv[1],"add")==0)
        {
            add(argv[2]);
        }
        else
        {
            perror("Argument invalid");
            exit(-2);
        }
    }
    else
    {
        perror("Argumente insuficiente");
        exit(-3);
    }
    return 0;
}