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


void escreverParaStdout(agregacoes ag[],int tam){
      char str[100];

      for (int w = 0; w < tam; w++)
    {
        if (ag[w].idArtigo != -1)
        {
            sprintf(str, "%d %d %d\n", ag[w].idArtigo, ag[w].quantTotal, ag[w].montanteTotal);
            write(1, str, strlen(str));
        }
    }  
}

void escreverParaEstrutura(agregacoes ag[],int tam, char **strings){
    int j;

    for (j = 0; j < tam; j++)
                {
                    if (ag[j].idArtigo == atoi(strings[0]))
                    {
                        ag[j].quantTotal += atoi(strings[1]);
                        ag[j].montanteTotal += atoi(strings[2]);
                        j = 0;
                        break;
                    }
                    else
                    {
                        if (ag[j].idArtigo == -1)
                        {
                            ag[j].idArtigo = atoi(strings[0]);
                            ag[j].quantTotal += atoi(strings[1]);
                            ag[j].montanteTotal += atoi(strings[2]);
                            j = 0;
                            break;
                        }
                    }
                }
}



void ag(char* ficheiro){
    int tLido;
    char buf[1024];
    int tam=1;
    char **strings;
    int fd = open(ficheiro, O_RDONLY);
    while(readln(fd, buf, sizeof(buf))>=0){
      tam++; memset(&buf[0], 0, sizeof(buf)); 
    }
    printf("%d\n",tam);

    agregacoes ag[tam];
   
     for(int i=0;i<tam;i++){
        ag[i].idArtigo=-1;
        ag[i].quantTotal=0;
        ag[i].montanteTotal=0;
    }
      
     while ((tLido = readln(fd, buf, sizeof(buf))) > 0){
            strings = splitString(buf);
            escreverParaEstrutura(ag,tam,strings);

            memset(&buf[0], 0, sizeof(buf)); 
    }
    escreverParaStdout(ag,tam);
    close(fd);
}


void principal(int INICIO, int FIM){
  int tLido;
  char buf[1024];
  int linha=1;
  int fdV = open("vendas.txt",O_RDONLY);
  int fdC = open("conjunto.txt",O_WRONLY | O_CREAT, 0600);

  while( (linha<=FIM) && ((tLido = readln(fdV, buf, sizeof(buf))) > 0) ){
       if(linha>=INICIO){
           write(fdC,buf,tLido);
           write(fdC,"\n",1);
       }
       linha++;
       memset(&buf[0], 0, sizeof(buf)); 
  }

  close(fdV);
  close(fdC);
  char* ficheiro = strdup("conjunto.txt");
  ag(ficheiro);
}



int main(int argc, char* argv[]){
   principal(1,10);
}
 