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

int safe_read_line(char *buffer, size_t size)
{
    int len = read(0, buffer, size - 1); // -1 ca să păstrăm loc pentru \0
    if (len > 0) 
    {
        if (buffer[len - 1] == '\n') 
        {
            buffer[len - 1] = '\0'; // înlocuiește \n cu \0
            len--;
        } 
        else 
        {
            buffer[len] = '\0'; // pune \0
        }
    }
    else 
    {
        buffer[0] = '\0'; // sir gol
    }
    return len;
}

void add(char *hunt_id)
{
    char dir_path[256],file_path[256],log_path[256],link_path[256];
    sprintf(dir_path,"%s",hunt_id);
    sprintf(file_path,"%s/treasures.dat",hunt_id);
    sprintf(log_path,"%s/logged_hunt",hunt_id);
    sprintf(link_path,"logged_hunt-%s",hunt_id);

    //1. Creaza directorul daca nu exista
    struct stat st;
    if(stat(dir_path,&st)==-1) 
    {
        if(mkdir(dir_path,0777)==-1)
        {
            perror("Eroare mkdir");
            exit(-1);
        }
    }

    //2. Colectare date din terminal
    treasure t;
    char buffer[256];

    write(1,"Id: ",5);
    safe_read_line(buffer,sizeof(buffer));
    t.id=strtol(buffer,NULL,10);

    write(1,"Nume: ",7);
    safe_read_line(buffer,sizeof(buffer));
    strcpy(t.name,buffer);

    write(1,"Latitudine: ",13);
    safe_read_line(buffer,sizeof(buffer));
    t.x=strtof(buffer,NULL);

    write(1,"Longitudine: ",14);
    safe_read_line(buffer,sizeof(buffer));
    t.y=strtof(buffer,NULL);

    write(1,"Clue: ",7);
    safe_read_line(buffer,sizeof(buffer));
    strcpy(t.clue,buffer);

    write(1,"Valoare: ",10);
    safe_read_line(buffer,sizeof(buffer));
    t.value=strtol(buffer,NULL,10);

    printf("%d,%s,%f,%f,%s,%d;\n",t.id,t.name,t.x,t.y,t.clue,t.value);

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