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

void escreverParaFicheiro(int fd,agregacoes ag[],int tam){
      char str[100];

      for (int w = 0; w < tam; w++)
    {
        if (ag[w].idArtigo != -1)
        {
            sprintf(str, "%d %d %d\n", ag[w].idArtigo, ag[w].quantTotal, ag[w].montanteTotal);
            write(fd, str, strlen(str));
        }
    }  
    close(fd);
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


void agAux(int INICIO, int FIM, int iFicheiro){

    int tLido;
    int tam = FIM-INICIO+1;
    agregacoes ag[tam];
    char buf[1024];
    char nomeF[20];
    sprintf(nomeF,"agregacaoN%d.txt",iFicheiro);
    int i=1;
    char **strings;
    int fd = open("vendas.txt", O_RDONLY);
   
   
     for(int i=0;i<tam;i++){
        ag[i].idArtigo=-1;
        ag[i].quantTotal=0;
        ag[i].montanteTotal=0;
    }
    
    
     while ((tLido = readln(fd, buf, sizeof(buf))) > 0){

        if (i < INICIO){ i++; }
        else
        {
            if(i<=FIM)
            {
                strings = splitString(buf);
                escreverParaEstrutura(ag,tam,strings);
                i++;
            }
            else 
            {
              break;
            }
        }
      memset(&buf[0], 0, sizeof(buf)); 
    }


    int fdR = open(nomeF, O_WRONLY | O_CREAT, 0666);
    escreverParaFicheiro(fdR,ag,tam);

}


void agregarFicheiros(int tam){
    char buf[1024];
    int fdO = open("temp.txt",O_RDWR | O_CREAT, 0666);
    char fich[25];
    int totalLido;
    int p;
    char **strings;
    agregacoes ag[tam];
    for(int i=0;i<tam;i++){
        ag[i].idArtigo=-1;
        ag[i].quantTotal=0;
        ag[i].montanteTotal=0; 
    }
 
    for(int j=1;j<=4;j++){
        sprintf(fich,"agregacaoN%d.txt",j);
        int fdN = open(fich,O_RDONLY);
        while((totalLido = readln(fdN, buf, strlen(buf))) > 0){
            write(fdO,buf,totalLido);    
            write(fdO,"\n",1);    
            memset(&buf[0], 0, sizeof(buf));   
        }

        close(fdN);
        if((p=fork())==0){
            execlp("rm","rm",fich,NULL);
            _exit(-1);
        }  
    }
   
    close(fdO);
    fdO = open("temp.txt",O_RDWR);

    while ((totalLido = readln(fdO, buf, strlen(buf))) > 0){
                strings = splitString(buf);
                escreverParaEstrutura(ag,tam,strings);
               
                memset(&buf[0], 0, sizeof(buf));
        }


    char* timeS = strdup(timestamp());
    int fdR = open(timeS, O_WRONLY | O_CREAT,0600);
    escreverParaFicheiro(fdR,ag,tam);
}



void ag(int INICIO, int FIM){
    int TAM = FIM - INICIO + 1;
    int i, j, p, f=INICIO-1+TAM/4, ini=INICIO, dv=TAM/4;
    int status;

    if (TAM % 4 == 0)
    {
        for (i = 0; i < 4; i++)
        {
            p = fork();
            if (p == 0)
            {
                agAux(ini, f, ++i);
                _exit(++i);
            }
            ini += dv;
            f += dv;
        }
        for (j = 0; j < 4; j++)
        {
            wait(&status);
            printf("STATUS: %d\n", WEXITSTATUS(status));
        }
    }
    else
    {
        dv++;
        f++;
        for (i = 0; i < 4; i++)
            {
                p = fork();
                if (p == 0)
                {
                    agAux(ini, f, ++i);
                    _exit(++i);
                }
                ini += dv;

                if(i==2){ f=FIM; }
                else { f += dv; }              
            }
            for (j = 0; j < 4; j++)
            {
                wait(&status);
                printf("STATUS: %d\n", WEXITSTATUS(status));
            }
    }
   agregarFicheiros(TAM);
}






int main(int argc, char* argv[]){
   ag(1,15); 
}
 