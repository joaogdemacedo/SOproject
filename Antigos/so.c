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

    verVendasDaEstrutura();
}

void insereArtigos(){
    
    int stocks_fd = open("stocks.txt",O_RDONLY|O_CREAT|O_APPEND,0666);
    int artigos_fd = open("artigos.txt",O_RDONLY|O_CREAT|O_APPEND,0666);
    char buf[1024];
    int r;
    int i=0,j=0;
    char ** strings;

    while((r = readln(stocks_fd,buf,strlen(buf)))>0){
       // printf("-> %s\n",buf);
        colecaoArtigos[j].stock=atoi(buf);
        j++;
        memset(&buf[0], 0, sizeof(buf));
    }

    while((r = readln(artigos_fd,buf,strlen(buf)))>0){
        strings = malloc(sizeof(char *)*r);
        strings = splitString(buf);
        colecaoArtigos[i].codigoArtigo = atoi(strings[0]);
        colecaoArtigos[i].codigoNome = atoi(strings[1]);
        colecaoArtigos[i].nome = strdup(nomeArtigo(atoi(strings[1])));
        colecaoArtigos[i].preco = atoi(strings[2]);
        i++;

        idAtualArtigos = atoi(strings[0]);
        idAtualStrings = atoi(strings[1]);
        memset(&buf[0], 0, sizeof(buf));
    }
    verArtigosDaEstrutura();
   
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


void ma(){
    char buf[1024];
    int totalL;
    int artigos_fd;
    int strings_fd;
    char ** strings;
    char *newstr;
    int atualizarFicheiro=0;
    artigos_fd = open("artigos.txt",O_WRONLY | O_APPEND);
    strings_fd = open("strings.txt",O_WRONLY|O_CREAT|O_APPEND,0666);
 

    while((totalL=readln(0,buf,strlen(buf))>0)){
         strings = malloc(sizeof(char *)*totalL);
         strings = splitString(buf);

         
         if(*strings[0]=='i'){
            char stringNum[totalL];

            colecaoArtigos[idAtualArtigos-1].codigoArtigo = idAtualArtigos;
            colecaoArtigos[idAtualArtigos-1].codigoNome = idAtualStrings;
            colecaoArtigos[idAtualArtigos-1].nome = strdup(strings[1]);
            colecaoArtigos[idAtualArtigos-1].preco = atoi(strings[2]);

             newstr = malloc(strlen(strings[1]) + 2);
             strcpy(newstr, strings[1]);
             strcat(newstr, "\n");
         
             write(strings_fd,newstr,strlen(newstr));
             sprintf(stringNum,"%d %d %s\n",idAtualArtigos,idAtualStrings,strings[2]); 
             write(artigos_fd,stringNum,strlen(stringNum));

             printf("ID do artigo: %d\n",idAtualArtigos);
             idAtualArtigos++;
             idAtualVendas++;
         }

         if(*strings[0]=='n'){          
            
             newstr = malloc(strlen(strings[2]) + 2);
             strcpy(newstr, strings[2]);
             strcat(newstr, "\n");

             write(strings_fd,newstr,strlen(newstr));

             colecaoArtigos[atoi(strings[1])-1].nome = strdup(strings[2]);
             colecaoArtigos[atoi(strings[1])-1].codigoNome = idAtualStrings;
             atualizarFicheiro=1;
             idAtualStrings++;
         }

         if(*strings[0]=='p'){    
             colecaoArtigos[atoi(strings[1])-1].preco = atoi(strings[2]); 
             atualizarFicheiro=1;  
         }

        memset(&buf[0], 0, sizeof(buf));
    } 

    if(atualizarFicheiro==1){
        atualizarArtigos();  
    }
}

// ------------------------------------

void sv(){
   int p,status;
   int codigo,quant;
   int totalLido;
   char** strings; 
   char buf[1024];
   char str[50];
   int nCmds;
   int r,q;
   int vendas_fd = open("vendas.txt",O_WRONLY | O_CREAT | O_APPEND,0600);

   int client_to_server;
   char *myfifo = "client_to_server_fifo";

   int server_to_client;
   char *myfifo2 = "server_to_client_fifo";

    /* create the FIFO (named pipe) */
   mkfifo(myfifo, 0666);
   mkfifo(myfifo2, 0666);

   /* open, read, and display the message from the FIFO */
   client_to_server = open(myfifo, O_RDONLY);
   server_to_client = open(myfifo2, O_WRONLY);


   printf("----------------------\n");
   printf("------ SERVIDOR ------\n");
   printf("----------------------\n");

   while((totalLido = read(client_to_server,buf,sizeof(buf)))>0){
    buf[totalLido]='\0';
    printf("Mensagem recebida do cliente: %s\n", buf);

    strings = malloc(sizeof(char *)*2); 
    strings = splitString(buf);
   
    printf("Conteúdo do split: \n");
        for(nCmds=0;strings[nCmds]!=NULL;nCmds++){
                printf("%s\n",strings[nCmds]);
         };
    printf("----------------------\n");
        
        codigo = atoi(strings[0]);
  
        if(nCmds==1){
            if((p=fork())==0){
                sprintf(str,"Em stock: %d Preco: %d\n",colecaoArtigos[codigo-1].stock, colecaoArtigos[codigo-1].preco);
                write(server_to_client,str,strlen(str));
               _exit(1);
           }else{
              wait(&status);
              printf("STATUS: %d\n",WEXITSTATUS(status));  
           }            
        }

        if(nCmds==2){
           quant = atoi(strings[1]);
           if(quant<0){
               //efetuar venda
                if((p=fork())==0){
                        r = colecaoArtigos[codigo-1].stock;
                        colecaoArtigos[codigo-1].stock = r + quant;

                        colecaoVendas[idAtualVendas-1].codigoArtigo=codigo;
                        colecaoVendas[idAtualVendas-1].quantidade=abs(quant);
                        colecaoVendas[idAtualVendas-1].montanteTotal= abs(quant) * colecaoArtigos[codigo-1].preco;

                        sprintf(str,"%d %d %d\n",codigo,abs(quant), (abs(quant)*colecaoArtigos[codigo-1].preco));
                        write(vendas_fd,str,strlen(str)); 
                        idAtualVendas++;
            
                        sprintf(str,"Stock atual: %d\n", colecaoArtigos[codigo-1].stock);
                        write(server_to_client,str,strlen(str));
                        _exit(2);
                }else {
                    wait(&status);
                    verArtigosDaEstrutura();
                    printf("STATUS: %d\n",WEXITSTATUS(status));    
                }  
           }
           else{
               //entrada em stock
               if((p=fork())==0){
                   q = colecaoArtigos[codigo-1].stock;
                   colecaoArtigos[codigo-1].stock =  q + quant;
                   sprintf(str, "Stock atual: %d\n", colecaoArtigos[codigo-1].stock);
                   write(server_to_client, str, strlen(str));
                   _exit(3);
               }else{
                   wait(&status);
                   verArtigosDaEstrutura();
                   printf("STATUS: %d\n",WEXITSTATUS(status)); 
               }            
           } 
           atualizarStock();   
        } 
        memset(&buf[0], 0, sizeof(buf));
     }

    unlink("client_to_server_fifo");
    unlink("server_to_client_fifo");
}

// ------------------------------------


char* timestamp(){
  time_t rawtime;
  struct tm * timeinfo;
  char*tmp=malloc(sizeof(char)*30);

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
 
  tmp= strdup(asctime (timeinfo));
  return tmp;
} 



int main(int argc, char * argv[]){
    unlink("client_to_server_fifo");
    unlink("server_to_client_fifo");
    addInfo();
    insereArtigos();
    insereVendas();
    sv();
    //ma();

    return 1;
}