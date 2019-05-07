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

void ag(char *nomePipe,int n){
   // printf("TT6\n");
    int tEntrada;
    char buf[1024];
    char linha[7];
    int linhaAtual=1;
    int server_to_ag_fifo;
    char ** strings;
    int cod;
    int l;
    char str[20];
    sprintf(str,"agAuxiliar%d.txt",n);
    int agAuxiliar = open(str,O_CREAT | O_RDWR | O_TRUNC,0666);
    if(agAuxiliar<0){
      perror("Erro na abertura do ficheiro agAuxiliar");
    }
    
    initagAuxiliar(agAuxiliar);  
    //char* timeS = strdup(timestamp());
    sprintf(str,"agregacao%d.txt",n);
  	int ag = open(str,O_CREAT | O_RDWR,0666);

    server_to_ag_fifo = open(nomePipe, O_RDONLY);
    dup2(server_to_ag_fifo,0);
   // printf("TT7\n");
   


    // entrada = codigo quantidade montante
    printf("A agregar ...\n");
    while((tEntrada = read(0,buf,sizeof(buf)))>0){
          printf("PIPE: %s - LinhaRecebida: %s\n",nomePipe,buf);
          strings = malloc(sizeof(char *) * 3);
          strings = splitString(buf);
          printf("[AG]\n");
          
          cod=atoi(strings[0]);
          lseek(agAuxiliar,14*(cod-1)+7,SEEK_SET);
          readln(agAuxiliar,linha,6);
      //    printf("%s\n",linha);
          l = atoi(linha);
        
          if(l==0){
             // printf);("TT9\n");
              memset(&linha[0], 0, sizeof(linha));
              lseek(agAuxiliar,14*(cod-1)+7,SEEK_SET);
              write(agAuxiliar,NumToString(linhaAtual),6);
              linhaAtual++;

              lseek(ag,0,SEEK_END);
              memset(&buf[0], 0, sizeof(buf));
              sprintf(buf,"%s %s %s\n",NumToString(cod),NumToString(atoi(strings[1])),NumToString(atoi(strings[2])));
              write(ag,buf,strlen(buf));
              memset(&buf[0], 0, sizeof(buf));  
          } else {
              //printf("TT10\n");
              memset(&buf[0], 0, sizeof(buf)); 
              lseek(ag,21 * (l-1)+7,SEEK_SET);
              memset(&linha[0], 0, sizeof(linha));
              readln(ag,linha,6);
              int quantA = atoi(linha);
              memset(&linha[0], 0, sizeof(linha));

              lseek(ag,21 * (l-1)+14,SEEK_SET);
              readln(ag,linha,6);
              int montA = atoi(linha);
              memset(&linha[0], 0, sizeof(linha));

              int quantN = atoi(strings[1]) + quantA;
              int montN = atoi(strings[2]) + montA;

              lseek(ag,21 * (l-1)+7,SEEK_SET);
              sprintf(buf,"%s %s",NumToString(quantN),NumToString(montN));
              write(ag,buf,13);
              memset(&buf[0], 0, sizeof(buf)); 
          }
        memset(&buf[0], 0, sizeof(buf));
        for(int v=0;v<3;v++){
                free(*(strings+v));
            }
        free(strings); 
   }
  /* char bufff[21];
    lseek(ag,0,SEEK_SET);
    while((tEntrada= readln(ag,bufff,strlen(bufff)))>0){
         write(1,"Agregação final:\n",strlen("Agregação final:\n"));
         write(1,bufff,strlen(bufff));
         write(1,"\n",1);
         memset(&bufff[0], 0, sizeof(bufff)); 
    }  */
   printf("Agregação terminada\n");
   close(agAuxiliar);
   close(server_to_ag_fifo); 
   close(ag);
   
}


int main(int argc, char *argv[])
{
   atualizarVarGlobais();
   ag(argv[1],atoi(argv[2]));
}