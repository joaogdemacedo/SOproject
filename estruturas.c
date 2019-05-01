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

struct Vendas{
    int codigoArtigo;
    int quantidade;
    int montanteTotal;
};
venda colecaoVendas[1000];


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

        colecaoVendas[j].codigoArtigo = -1;
        colecaoVendas[j].quantidade = -1;
        colecaoVendas[j].montanteTotal = -1;
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

char* nomeArtigo(int posicao){
     int strings_fd = open("strings.txt",O_RDONLY);
     char buf[1024];
     int r;
     int i=0;

     while(i<posicao && ((r = readln(strings_fd,buf,strlen(buf)))>0) ){
        i++;
    }
    char* novaS = strdup(buf);
    return novaS;
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

void verVendasDaEstrutura(){
    printf("\nVENDAS\n");
    printf("%d\n",idAtualVendas);
    printf("-------------\n");

    for(int i=0;i<1000;i++){
        if(colecaoVendas[i].codigoArtigo >=0){
            printf("%d\n",colecaoVendas[i].codigoArtigo);
            printf("%d\n",colecaoVendas[i].quantidade);
            printf("%d\n",colecaoVendas[i].montanteTotal);
            printf("-------------\n");
        }
    }
}

void insereVendas(){
    
    int vendas_fd = open("vendas.txt",O_RDONLY|O_CREAT|O_APPEND,0666);
    char buf[1024];
    int r;
    int i=0;
    char** strings;

     while((r = readln(vendas_fd,buf,strlen(buf)))>0){
        strings = malloc(sizeof(char *)*r);
        strings = splitString(buf);
       
        colecaoVendas[i].codigoArtigo = atoi(strings[0]);
        colecaoVendas[i].quantidade = atoi(strings[1]);
        colecaoVendas[i].montanteTotal = atoi(strings[2]);
        
        idAtualVendas=atoi(strings[0]);
        i++;
        memset(&buf[0], 0, sizeof(buf));
    }

   // verVendasDaEstrutura();
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


void atualizarArtigos(){
    int artigos_fd = open("artigos.txt",O_WRONLY | O_TRUNC);
    char string[100];

    for(int i=0;i<1000;i++){
        if(colecaoArtigos[i].codigoArtigo>=0){
            sprintf(string,"%d %d %d\n",colecaoArtigos[i].codigoArtigo,colecaoArtigos[i].codigoNome,colecaoArtigos[i].preco);
            write(artigos_fd,string,strlen(string));
        }
    }
}

void atualizarStock(){
    int stocks_fd = open("stocks.txt",O_WRONLY | O_TRUNC);
    char string[10];

    for(int i=0;i<1000;i++){
        if(colecaoArtigos[i].codigoArtigo>=0){
            sprintf(string,"%d\n",colecaoArtigos[i].stock);
            write(stocks_fd,string,strlen(string));
        }
    }
}




