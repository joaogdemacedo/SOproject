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
#include "estruturas.c"


void initagAuxiliar(int fd){
    int i;
    char string[15];
    for (i = 1; i < idAtualArtigos; i++)
    {   
        sprintf(string,"%s 000000\n",NumToString(i));
        write(fd,string,strlen(string));
        memset(&string[0], 0, sizeof(string)); 
    } 
}

void ag(){
    int tEntrada;
    char buf[1024];
    char linha[7];
    int linhaAtual=1;
    int server_to_ag_fifo;
    char ** strings;
    int cod;
    int l;
    int agAuxiliar = open("agAuxiliar.txt",O_CREAT | O_RDWR | O_TRUNC,0666);
    if(agAuxiliar<0){
      perror("Erro na abertura do ficheiro agAuxiliar");
    }
    
    initagAuxiliar(agAuxiliar);  
    char* timeS = strdup(timestamp());
  	int ag = open(timeS,O_CREAT | O_RDWR,0666);

    char *myfifo3 = "server_to_ag_fifo";
    server_to_ag_fifo = open(myfifo3, O_RDONLY);
    dup2(server_to_ag_fifo,0);
   


    printf("A agregar ...\n");
    while((tEntrada = readln(0,buf,sizeof(buf)))>0){
  
          strings = malloc(sizeof(char *) * 3);
          strings = splitString(buf);
       
          
          cod=atoi(strings[0]);
          lseek(agAuxiliar,14*(cod-1)+7,SEEK_SET);
          readln(agAuxiliar,linha,6);
     
          l = atoi(linha);
        
          if(l==0){

              memset(&linha[0], 0, sizeof(linha));
              lseek(agAuxiliar,14*(cod-1)+7,SEEK_SET);
              write(agAuxiliar,NumToString(linhaAtual),6);
              linhaAtual++;

              
              lseek(ag,0,SEEK_END);

              write(ag,buf,strlen(buf));
              write(ag,"\n",1);
              memset(&buf[0], 0, sizeof(buf));  
          } else {
             
              memset(&buf[0], 0, sizeof(buf)); 
              lseek(ag,21 * (l-1)+7,SEEK_SET);
              memset(&linha[0], 0, sizeof(linha));
              read(ag,linha,6);
              int quantA = atoi(linha);
              memset(&linha[0], 0, sizeof(linha));

              lseek(ag,21 * (l-1)+14,SEEK_SET);
              read(ag,linha,6);
              int montA = atoi(linha);
              memset(&linha[0], 0, sizeof(linha));

              int quantN = atoi(strings[1]) + quantA;
              int montN = atoi(strings[2]) + montA;

              lseek(ag,21 * (l-1)+7,SEEK_SET);
              sprintf(buf,"%s %s\n",NumToString(quantN),NumToString(montN));
              write(ag,buf,14);
              memset(&buf[0], 0, sizeof(buf)); 
          }

        memset(&buf[0], 0, sizeof(buf));
  }

    char bufff[21];
    lseek(ag,0,SEEK_SET);
    write(1,"Agregação final:\n",strlen("Agregação final:\n"));
    char** separa=malloc(sizeof(char*)*3);

    while((tEntrada= readln(ag,bufff,strlen(bufff)))>0){ 
         separa = splitString(bufff); 
         write(1,bufff,strlen(bufff));
         write(1,"\n",1);
         memset(&bufff[0], 0, sizeof(bufff)); 
    } 

   printf("Agregação terminada\n");
   close(agAuxiliar);
   close(server_to_ag_fifo);
   close(ag);
}


int main(int argc, char *argv[])
{
   printf("A atualizar variáveis globais...\n");
   atualizarVarGlobais();
   ag();

}