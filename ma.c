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
#include "estruturas.c"



void ma(){
    char buf[1024];
    int totalL;
    int artigos_fd;
    int strings_fd;
    char ** strings;
    char *newstr;
    int atualizarFicheiro=0;
    artigos_fd = open("artigos.txt",O_WRONLY | O_CREAT | O_APPEND,0666);
    strings_fd = open("strings.txt",O_WRONLY |O_CREAT |O_APPEND,0666);
 

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
             idAtualStrings++;
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


int main(int argc,  char * argv[]){
    addInfo();
    insereArtigos();
    insereVendas();
    ma();
}
