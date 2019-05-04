#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include "estruturas.h"

int idAtualArtigos=1;
int idAtualStrings=1; 
int idAtualVendas=1;

struct Artigos{
    int codigoArtigo;
    int codigoNome;
    char* nome; 
    int preco;
    int stock;
};
artigos colecaoArtigos[1000];

struct Agregacoes{
    int idArtigo;
    int quantTotal;
    int montanteTotal;
};


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

void addInfo() {
    for(int j=0;j<1000;j++){
        colecaoArtigos[j].codigoArtigo = -1;
        colecaoArtigos[j].codigoNome = -1;
        colecaoArtigos[j].nome = NULL;
        colecaoArtigos[j].preco = -1;
        colecaoArtigos[j].stock = 0;
    }
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
    int i;
    char * pch;
    for(pch = strtok(cmd1," "), i = 0; pch!=NULL; pch = strtok(NULL," "),i++){
        arrSt[i]=strdup(pch);
    }
    return arrSt;
}

void verArtigosDaEstrutura(){
    printf("\nARTIGOS\n");
    printf("%d\n",idAtualArtigos);
    printf("%d\n",idAtualStrings);
    printf("-------------\n");

    for(int i=0;i<1000;i++){
        if(colecaoArtigos[i].codigoArtigo>=0){
            printf("%d\n",colecaoArtigos[i].codigoArtigo);
            printf("%d\n",colecaoArtigos[i].codigoNome);
            printf("%s\n",colecaoArtigos[i].nome);
            printf("%d\n",colecaoArtigos[i].preco);
            printf("%d\n",colecaoArtigos[i].stock);
            printf("-------------\n");
        }
    }
}

void insereArtigos(){
    
    int stocks_fd = open("stocks.txt",O_RDONLY|O_CREAT|O_APPEND,0666);
    int artigos_fd = open("artigos.txt",O_RDONLY|O_CREAT|O_APPEND,0666);
    int strings_fd = open("strings.txt",O_RDONLY|O_CREAT|O_APPEND,0666);
    char buf[1024];
    int r;
    int nS=1;
    int i=0,j=0;
    char ** strings;

    while((r = readln(stocks_fd,buf,strlen(buf)))>0){
        colecaoArtigos[j].stock=atoi(buf);
        j++;
        memset(&buf[0], 0, sizeof(buf));
    }

      while((r = readln(strings_fd,buf,strlen(buf)))>0){
        nS++;
        memset(&buf[0], 0, sizeof(buf));
    }
    
    idAtualStrings = nS;

    while((r = readln(artigos_fd,buf,strlen(buf)))>0){
        strings = malloc(sizeof(char *)*r);
        strings = splitString(buf);
        colecaoArtigos[i].codigoArtigo = atoi(strings[0]);
        colecaoArtigos[i].codigoNome = atoi(strings[1]);
        colecaoArtigos[i].nome = strdup(nomeArtigo(atoi(strings[1])));
        colecaoArtigos[i].preco = atoi(strings[2]);
        i++;

        idAtualArtigos = atoi(strings[0]);
        memset(&buf[0], 0, sizeof(buf));
    }
    idAtualArtigos++;
    
  // verArtigosDaEstrutura();
   
}



// varGlobais
void atualizarVarGlobais(){
    int artigos_fd = open("artigos.txt",O_RDONLY|O_CREAT|O_CREAT,0666);
    int strings_fd = open("strings.txt",O_RDONLY|O_CREAT|O_CREAT,0666);
    int vendas_fd = open("vendas.txt",O_RDONLY|O_CREAT|O_CREAT,0666);
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
    
}


// "xxxxxx xxxxxx xxxxxx\n" 21
off_t avancar_offset_artigos(int linha, int fd){
    int TAM_LINHA=21;
    off_t r;
    off_t offset = TAM_LINHA *(linha-1);

    r = lseek(fd,offset,SEEK_SET);
    return r;
}

// "xxxxxx\n" 7
off_t avancar_offset_stock(int linha, int fd){
    int TAM_LINHA=7;
    off_t r;
    off_t offset = TAM_LINHA *(linha-1);

    r = lseek(fd,offset,SEEK_SET);
    return r;
}


//80
off_t avancar_offset_strings(int linha, int fd){
    int TAM_LINHA=80;
    off_t r;
    off_t offset = TAM_LINHA *(linha-1);

    r = lseek(fd,offset,SEEK_SET);
    return r;
}

// buscar o nome do artigo ao ficheiro strings
char* nomeArtigo(int idArtigo){
    off_t r;
    char* nome;
    char buf[90];
    int strings_fd = open("strings.txt",O_RDONLY); 
    
    r = avancar_offset_strings(idArtigo,strings_fd);
    readln(strings_fd,buf,strlen(buf));
    close(strings_fd);

    nome=strdup(buf);
    return nome;
}


char* NumToString (int i){
     long long n=i;
     char str[10];
     int count=0;
     while(n != 0)
    {
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
    char*r=strdup(str);
    return r;
}




