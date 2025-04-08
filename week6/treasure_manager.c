#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

typedef struct {
    int id;
    char name[32];
    float x,y;
    char clue[128];
    int value;
}treasure;

ssize_t safe_read_line(char *buffer, size_t size)
{
    ssize_t len = read(0, buffer, size - 1); // -1 ca să păstrăm loc pentru \0
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

void log_operation(const char * log_path,const char * what_to_write)
{
    int log_fd=open(log_path,O_WRONLY | O_APPEND | O_CREAT, 0777);
    if(log_fd!=-1)
    {
        write(log_fd,what_to_write,strlen(what_to_write));
        if(close(log_fd)==-1)
        {
            perror("eroare log_hunt");
            exit(-2);
        }
    }
    else
    {
        perror("Error open log_hunt");
        exit(-2);
    }
}

void add(char *hunt_id)
{
    char dir_path[256],file_path[256],log_path[256],link_path[256];
    snprintf(dir_path,sizeof(dir_path),"%s",hunt_id);
    snprintf(file_path,sizeof(file_path),"%s/treasures.dat",hunt_id);
    snprintf(log_path,sizeof(log_path),"%s/logged_hunt",hunt_id);
    snprintf(link_path,sizeof(link_path),"logged_hunt-%s",hunt_id);

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
    char buffer[256],*endptr;

    do
    {
        write(1,"Id: ",5);
        safe_read_line(buffer,sizeof(buffer));
        t.id=strtol(buffer,&endptr,10);
        if(buffer[0]=='\0' || *endptr !=0 || t.id<0)
        {
            write(1,"ID-ul nu este valid!\n",22);
        }
    } while (buffer[0]=='\0' || *endptr !=0 || t.id<0);
    
    write(1,"Nume: ",7);
    safe_read_line(buffer,sizeof(buffer));
    strcpy(t.name,buffer);

    do
    {
        write(1,"Latitudine: ",13);
        safe_read_line(buffer,sizeof(buffer));
        t.x=strtof(buffer,&endptr);
        if(buffer[0]=='\0' || *endptr !=0)
        {
            write(1,"Latitudinea nu este valida!\n",29);
        }
    } while (buffer[0]=='\0' || *endptr !=0);

    do
    {
        write(1,"Longitudine: ",14);
        safe_read_line(buffer,sizeof(buffer));
        t.y=strtof(buffer,&endptr);
        if(buffer[0]=='\0' || *endptr !=0)
        {
            write(1,"Longitudinea nu este valida!\n",29);
        }
    } while (buffer[0]=='\0' || *endptr !=0);
    
    write(1,"Clue: ",7);
    safe_read_line(buffer,sizeof(buffer));
    strcpy(t.clue,buffer);

    do
    {
        write(1,"Valoare: ",10);
        safe_read_line(buffer,sizeof(buffer));
        t.value=strtol(buffer,&endptr,10);
        if(buffer[0]=='\0' || *endptr !=0)
        {
            write(1,"Valoarea nu este valida!\n",26);
        }
    } while (buffer[0]=='\0' || *endptr !=0);

    //printf("%d,%s,%f,%f,%s,%d;\n",t.id,t.name,t.x,t.y,t.clue,t.value);

    //3.Deschide in fisier sau il creaza daca nu exista
    int fd=open(file_path,O_WRONLY | O_APPEND | O_CREAT, 0777);
    if(fd==-1)
    {
        perror("Eroare fisier treasure.dat");
        exit(-2);
    }

    //4. Scriere comoara in fisier binar
    ssize_t bytes=write(fd,&t,sizeof(treasure));
    if(bytes!=sizeof(treasure))
    {
        perror("Eroare write");
        close(fd);
        exit(-4);
    }

    if(close(fd)==-1)
    {
        perror("Eroare close");
        exit(-4);
    }

    //5. Scriere in logged_hunt
    char log[512];
    snprintf(log,sizeof(log),"ADD: ID=%d Name_User=%s Latitude=%.2f Longitude=%.2f Clue=\"%s\" Value=%d\n",t.id,t.name,t.x,t.y,t.clue,t.value);
    log_operation(log_path,log);

    //6. creare symlimk in caz ca nu exista deja
    if(access(link_path,F_OK)==-1)
    {
        if(symlink(log_path,link_path)==-1)
        {
            perror("eroare symlink");
            exit(-1);
        }
    }

    write(1,"Comoara a fost adaugata cu succes!\n",36);
}

void display_treasure(const treasure *t)
{
    char str[512];
    snprintf(str,sizeof(str),"ID:%d,User_Name:%s,Lat:%f,Long:%f,Clue:%s,Value:%d;\n",t->id,t->name,t->x,t->y,t->clue,t->value);
    write(1,str,strlen(str));
}
void list(char *hunt_id)
{
     char file_path[256];
     snprintf(file_path,sizeof(file_path),"%s/treasures.dat",hunt_id);

     struct stat info;
     //1.Verificare existenta director
     if(stat(hunt_id,&info)!=0)
     {
        perror("Nu exista calea catre acest director!");
        exit(-2);
     }
     else if(S_ISDIR(info.st_mode)==0)
     {
        perror("Nu este un director");
        exit(-1);
     }

     //2.Nume
     write(1,"Hunt Name: ",11);
     write(1,hunt_id,strlen(hunt_id));

     //3.Verificare existenta treasure.dat 
     if(stat(file_path,&info)!=0)
     {
        perror("Nu exista fisierul!");
        exit(-1);
     }
     else if(S_ISREG(info.st_mode)==0)
     {
        perror("Nu este fisier regular!");
        exit(-1);
     }

     //4.Total size
     char total_size[128];
     snprintf(total_size,sizeof(total_size),"\nTotal size in bytes: %ld\n",info.st_size);
     write(1,total_size,strlen(total_size));

     //5.Last modification
     char last_modification[128];
     snprintf(last_modification,sizeof(last_modification),"Last modification : %s",ctime(&info.st_mtime));
     write(1,last_modification,strlen(last_modification));

     //6.List of treasures
     treasure t;
     int fd=open(file_path,O_RDONLY);
     if(fd==-1)
     {
        perror("Eroare deschidere fisier");
        exit(-1);
     }
     ssize_t bytes_read;
     while((bytes_read=read(fd,&t,sizeof(treasure))) == sizeof(treasure))
     {
        display_treasure(&t);
     }
     
     if(bytes_read!=0)
     {
        perror("An error ocurred at reading treasures!");
     }

     if(close(fd)!=0)
     {
        perror("Error at closing file");
        exit(-1);
     }

     //7.Actualizarea logged_hunt-ului
     char log_path[256],log_message[256];
     snprintf(log_path,sizeof(log_path),"%s/logged_hunt",hunt_id);
     snprintf(log_message,sizeof(log_message),"LIST: All treasures listed from Hunt - %s\n",hunt_id);
     log_operation(log_path,log_message);

}

void view(const char *hunt_id,int id)
{
    char file_path[256];
     snprintf(file_path,sizeof(file_path),"%s/treasures.dat",hunt_id);

     struct stat info;
     //1.Verificare existenta director
     if(stat(hunt_id,&info)!=0)
     {
        perror("Nu exista calea catre acest director!");
        exit(-2);
     }
     else if(S_ISDIR(info.st_mode)==0)
     {
        perror("Nu este un director");
        exit(-1);
     }

     //2.Deschidere fisier treasure.dat
     int fd=open(file_path,O_RDONLY);
     if(fd==-1)
     {
        perror("Nu s a putut deschide fisierul sau nu exista!");
        exit(-1);
     }

     //3.Citire
     treasure t;
     ssize_t bytes;
     int found=0;
     while((bytes=read(fd,&t,sizeof(treasure)))==sizeof(treasure))
     {
        if(t.id==id)
        {
            found=1;
            break;
        }
     }
     if(found==1)
     {
        display_treasure(&t);
     }
     else
     {
        write(1,"Nu s-a gasit!\n",15);
        if(close(fd)!=0)
            {
                perror("Eroare inchidere fisier");
                exit(-1);
            }
        return;
     }

     if(close(fd)!=0)
     {
        perror("Eroare inchidere fisier");
        exit(-1);
     }

     //4.Actualizarea logged_hunt-ului
     char log_path[256],log_message[256];
     snprintf(log_path,sizeof(log_path),"%s/logged_hunt",hunt_id);
     snprintf(log_message,sizeof(log_message),"VIEW: Treasure with the ID:%d was viewed\n",id);
     log_operation(log_path,log_message);

}

void remove_treasure(const char *hunt_id,int id)
{
    char file_path[256],temp_file[256];
    snprintf(file_path,sizeof(file_path),"%s/treasures.dat",hunt_id);
    snprintf(temp_file,sizeof(temp_file),"%s/temp.dat",hunt_id);

    //1. Verificare existenta director
    struct stat info;
    if(stat(hunt_id,&info)!=0)
     {
        perror("Nu exista calea catre acest director!");
        exit(-2);
     }
     else if(S_ISDIR(info.st_mode)==0)
     {
        perror("Nu este un director");
        exit(-1);
     }

     //2. Deschidere fisier treasure
     int fd=open(file_path,O_RDONLY);
     if(fd==-1)
     {
        perror("Fisierul nu exista sau nu poate fi deschis");
        exit(-1);
     }

     //3. Crearea unui nou fisier in care punem toate comorile mai putin cea cu ID-ul dat ca parametru
     int fd_out=open(temp_file,O_WRONLY | O_CREAT | O_TRUNC,0777);
     if(fd_out==-1)
     {
        perror("An error has ocurred!");
        exit(-2);
     }

     treasure t;
     int found=0;
     ssize_t bytes;
     while((bytes=read(fd,&t,sizeof(treasure)))==sizeof(treasure))
     {
        if(t.id==id)
        {
            found=1;
            continue;
        }
        write(fd_out,&t,sizeof(treasure));
     }
     if(bytes!=0)
     {
        perror("An error has ocurred at reading!");
        exit(-2);
     }

     if(close(fd)!=0 || close(fd_out)!=0)
     {
        perror("Eroare inchidere fisire");
        exit(-2);
     }

     //4. Inlocuire fisire
     if(found==1)
     {
        if(remove(file_path)!=0 || rename(temp_file,file_path)!=0)
        {
            perror("Nu s-a putut face stergerea!");
            exit(-2);
        }
     }
     else
     {
        if(remove(temp_file)!=0)
        {
            perror("An error has ocurred!");
            exit(-2);
        }
        write(1,"Comoara nu a fost gasita!\n",27);
        return ;
     }

     //4.Actualizarea logged_hunt-ului
     char log_path[256],log_message[256];
     snprintf(log_path,sizeof(log_path),"%s/logged_hunt",hunt_id);
     snprintf(log_message,sizeof(log_message),"REMOVE_TREASURE: Treasure with the ID:%d was removed\n",id);
     log_operation(log_path,log_message);

     write(1,"Stergera a fost efectuata cu succes!\n",38);
}

void remove_hunt(const char *hunt_id)
{
    char file_path[256],log_path[256],link_path[256];
    snprintf(file_path,sizeof(file_path),"%s/treasures.dat",hunt_id);
    snprintf(log_path,sizeof(log_path),"%s/logged_hunt",hunt_id);
    snprintf(link_path,sizeof(link_path),"logged_hunt-%s",hunt_id);

    struct stat info;
    //1. Verificare existenta a fisierelor/directoarelor coresp. hunt-ului
    if(stat(hunt_id,&info)!=0 || S_ISDIR(info.st_mode)==0)
     {
        perror("Nu exista directorul sau nu este valid!");
        exit(-2);
     }
     

     if(stat(file_path,&info)==0 && S_ISREG(info.st_mode)!=0)
     {
        if(remove(file_path)!=0)
        {
            perror("Eroare stergere fisier treasure.dat");
        }
     }
     
     if(lstat(link_path,&info)==0 && S_ISLNK(info.st_mode)!=0)
     {
        if(remove(link_path)!=0)
        {
            perror("Eroare stergere fisier link-symbolic");
        }
     }

     if(stat(log_path,&info)==0 && S_ISREG(info.st_mode)!=0)
     {
        if(remove(log_path)!=0)
        {
            perror("Eroare stergere fisier logged_hunt");
        }
     }

     if(remove(hunt_id)!=0)
     {
        perror("Eroare stergere director hunt!");
        exit(-2);
     }
    
     write(1,"Stergerea hunt-ului s-a realizat cu succes.\n",45);
}

int main(int argc,char **argv)
{
    test_citire();
    if(argc==3)
    {
        if(strcmp(argv[1],"add")==0)
        {
            add(argv[2]);
        }
        else if(strcmp(argv[1],"list")==0)
        {
            list(argv[2]);
        }
        else if(strcmp(argv[1],"remove_hunt")==0)
        {
            remove_hunt(argv[2]);
        }
        else
        {
            perror("Invalid Arguments");
            exit(-2);
        }
    }
    else if(argc==4)
    {
        if(strcmp(argv[1],"view")==0)
        {
            view(argv[2],strtol(argv[3],NULL,10));
        }
        else if(strcmp(argv[1],"remove_treasure")==0)
        {
            remove_treasure(argv[2],strtol(argv[3],NULL,10));
        }
        else
        {
            perror("Invalid Arguments");
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