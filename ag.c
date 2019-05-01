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


void ag(int INICIO, int FIM,int iFicheiro){

    int tLido;
    int tam = FIM-INICIO+1;
    agregacoes ag[tam];

    char buf[1024];
    char str[100];
    char nomeF[20];
    sprintf(nomeF,"agregacaoN%d.txt",iFicheiro);
    int j,i=1;
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
    for (int w = 0; w < tam; w++)
    {
        if (ag[w].idArtigo != -1)
        {
            sprintf(str, "%d %d %d\n", ag[w].idArtigo, ag[w].quantTotal, ag[w].montanteTotal);
            write(fdR, str, strlen(str));
        }
    }  
    close(fdR);
}


void agConc(int INICIO, int FIM){
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
                ag(ini, f, ++i);
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
                    ag(ini, f, ++i);
                    _exit(++i);
                }
                ini += dv;

                if(i==2){f=FIM;}
                else { f += dv; }              
            }
            for (j = 0; j < 4; j++)
            {
                wait(&status);
                printf("STATUS: %d\n", WEXITSTATUS(status));
            }
    }
    
    

}



int main(int argc, char* argv[]){
   agConc(1,8);
  
}
 