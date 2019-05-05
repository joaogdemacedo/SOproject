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

/**
O agregador é um programa que recebe, pelo stdin, um conjunto de entradas do ficheiro VENDAS com a 
finalidade de agregar e enviar o resultado para o stdout. Ora, este conjunto de entradas do ficheiro 
de VENDAS é um subconjunto do ficheiro completo, isto é, é um intervalo de registos do ficheiro VENDAS.
Assim, uma vez que o enunciado indica que o agregador recebe essas entradas, este não deve sequer ter
qualquer conhecimento sobre o ficheiro nem sobre a última agregação, devendo atribuir esta responsabilidade
ao programa que o vai invocar.
*/

void puta(int fd){
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
    //printf("TT6\n");
    int tEntrada;
    char buf[1024];
    char linha[7];
    int linhaAtual=1;
    int server_to_ag_fifo;
    char ** strings;
    int cod;
    int l;
    int agAuxiliar = open("agAuxiliar.txt",O_CREAT | O_RDWR | O_TRUNC,0666);
    puta(agAuxiliar);  
    char* timeS = strdup(timestamp());
	int ag = open(timeS,O_CREAT | O_RDWR,0666);

    char *myfifo3 = "server_to_ag_fifo";
    server_to_ag_fifo = open(myfifo3, O_RDONLY);
    dup2(server_to_ag_fifo,0);
    //printf("TT7\n");


    // entrada = codigo quantidade montante
    while((tEntrada = read(0,buf,sizeof(buf)))>0){
       // printf("%s\n",buf);
          strings = malloc(sizeof(char *) * 2);
          strings = splitString(buf);
     //     printf("TT8\n");
          
          cod=atoi(strings[0]);
          lseek(agAuxiliar,14*(cod-1)+7,SEEK_SET);
          readln(agAuxiliar,linha,6);
      //    printf("%s\n",linha);
          l = atoi(linha);
        
          if(l==0){
            //  printf("TT9\n");
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
            //  printf("TT10\n");
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
   }

   close(agAuxiliar);
   close(server_to_ag_fifo);
   close(ag);
}


int main(int argc, char *argv[])
{
   atualizarVarGlobais();
   ag();
}