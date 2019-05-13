#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include "estruturas.h"

int idAtualArtigos=1;
int idAtualStrings=1; 
int idAtualVendas=1;



char* timestamp(){
  time_t rawtime;
  struct tm * timeinfo;
  char *temp;
  char*tmp=malloc(sizeof(char)*30);

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
 
  tmp= strdup(asctime (timeinfo));
  temp = strchr(tmp,'\n'); 
  *temp = '\0';  
  return tmp;
} 


int readln(int fildes, void *buf, size_t nbyte){
    int r,bytesRead=0;
    while ((r=(read(fildes,buf+bytesRead,1))>0) && ((char*)buf)[bytesRead]!='\n' ) {
        bytesRead++;
    }
    if(((char*)buf)[bytesRead]=='\n'){
        ((char*)buf)[bytesRead]='\0';
    }
    return bytesRead;
}

char** splitString(char * cmd1){
    char ** arrSt = calloc(10,sizeof(char *));
    char *r=strdup(cmd1);
    int i;
    char * pch;
    for(pch = strtok(r," "), i = 0; pch!=NULL; pch = strtok(NULL," "),i++){
        arrSt[i]=strdup(pch);
    }
    return arrSt;
}


void atualizarVarGlobais(){
    int artigos_fd = open("artigos.txt",O_RDONLY|O_CREAT,0666);
    if(artigos_fd<0){
        perror("Erro na abertura do ficheiro ARTIGOS");
    }
    int strings_fd = open("strings.txt",O_RDONLY|O_CREAT,0666);
    if(strings_fd<0){
        perror("Erro na abertura do ficheiro STRINGS");
    }
    int vendas_fd = open("vendas.txt",O_RDONLY|O_CREAT,0666);
    if(vendas_fd<0){
        perror("Erro na abertura do ficheiro VENDAS");
    }

    char buf[1024];
    
    while(readln(artigos_fd,buf,strlen(buf))>0){      
        idAtualArtigos++;
        memset(&buf[0], 0, sizeof(buf)); 
    }
    close(artigos_fd);

        
    while(readln(strings_fd,buf,strlen(buf))>0){
        idAtualStrings++;
        memset(&buf[0], 0, sizeof(buf)); 
    }
    close(strings_fd);


   while(readln(vendas_fd,buf,strlen(buf))>0){
        idAtualVendas++;
        memset(&buf[0], 0, sizeof(buf)); 
    }
    close(vendas_fd); 
     printf("%d\n",idAtualArtigos);

}


off_t avancar_offset_artigos(int linha, int fd){
    int TAM_LINHA=21;
    off_t r;
    off_t offset = TAM_LINHA *(linha-1);

    r = lseek(fd,offset,SEEK_SET);
    return r;
}


off_t avancar_offset_stock(int linha, int fd){
    int TAM_LINHA=7;
    off_t r;
    off_t offset = TAM_LINHA *(linha-1);

    r = lseek(fd,offset,SEEK_SET);
    return r;
}


off_t avancar_offset_strings(int linha, int fd){
    int TAM_LINHA=80;
    off_t r;
    off_t offset = TAM_LINHA *(linha-1);

    r = lseek(fd,offset,SEEK_SET);
    return r;
}


char* NumToString (int i){
     long long n=i;
     char str[10];
     int count=0;
     while(n != 0){
        n = n/10;
        ++count;
    }

    
    int NumZeros= 6-count;
    if(NumZeros==0){
         sprintf(str,"%d",i); 
    }
    if(NumZeros==1){
       sprintf(str,"0%d",i); 
    }
    if(NumZeros==2){
        sprintf(str,"00%d",i);
    }
    if(NumZeros==3){
        sprintf(str,"000%d",i);
    }
    if(NumZeros==4){
        sprintf(str,"0000%d",i);
    }
    if(NumZeros==5){
        sprintf(str,"00000%d",i);
    }
    if(NumZeros==6){
        sprintf(str,"00000%d",i);
    }

    char*r=strdup(str);
    memset(&str[0], 0, sizeof(str));
    return r;
}




