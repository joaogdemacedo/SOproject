#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>



int fdPrincipal;
int insereFlag= 0;
int execFlag= 0;
int writeFlag= 0;
char* original;


typedef struct Info{
    int idcmd;
    int idcmdDep;
    char * desc;
    char * cmds[10];
    char * outP;
}info;

info notebook[1000];




/*
* Função que inicializa os parâmetros da estrutura
*/
void addInfo() {
    for(int j=0;j<1000;j++){
        notebook[j].idcmd = -1;
        notebook[j].idcmdDep= -1;
        notebook[j].desc=NULL;
        for(int i=0;i<10;i++){
            notebook[j].cmds[i]=NULL;
        }
        notebook[j].outP=NULL;
    }
}



/*
* Função que lê todas as linhas de um dado descritor de ficheiro
*/
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


/*
* Função que dado um comando parte-o nos
* seus argumentos devolvendo um array com eles
*  Exemplo: ordena("ls -l") -> ["ls","-l"]
*/
char** parteComando(char * cmd1){
    char ** arrSt = calloc(10,sizeof(char *));
    int i;
    char * pch;
    for(pch = strtok(cmd1," "), i = 0; pch!=NULL; pch = strtok(NULL," "),i++){
        arrSt[i]=strdup(pch);
    }
    return arrSt;
}



/*
* Função que insere na estrutura toda a
* informação, de um determinado ficheiro,
* exceto os comandos
*/
char ** retiraCmd(int file){

    int flagOut=1;
    int r;
    char buf[1024];
    char ** cmdss = calloc(1000,sizeof(char *));
    int i=0,q=0,a=-2;

    while((r = readln(file,buf,strlen(buf)))>0){
        if(buf[0]=='>'){  // caso a linha seja o início de um output
            flagOut=0;
        }
        else if(buf[0]=='<'){  // caso a linha seja o fim de um output
            flagOut=1;
        }
        else if(buf[0] != '$' && flagOut==1){  // caso seja a descrisão de um comando
            notebook[q].desc = strdup(buf);
            q++;
        }
        else if(buf[0] == '$' && flagOut==1){ // caso seja um comando
                notebook[q].idcmd=i;
                cmdss[i]=strdup(buf);
                a=atoi(&(buf[1])); // inteiro que indica se é preciso ler um output anterior específico

                  if(a==1||a==2||a==3||a==4||a==5||a==6||a==7||a==8||a==9){
                  notebook[q].idcmdDep=i-(atoi(&(buf[1])));
              }

                  if(buf[1]=='|') notebook[q].idcmdDep=i-1; // caso seja para ler o output do comando anterior
                      i++;
                      q++;
        }
        memset(&buf[0], 0, sizeof(buf));
    }
    return cmdss;
}



/*
* Função que insere todos os comandos, com o seu
* devido tratamento, na estrutura
*/
void inserecmds(int fildes){

    char ** cmds1 = malloc(sizeof(char *)*100);
    char ** cmdsor = malloc(sizeof(char *)*10);

    cmds1 = retiraCmd(fildes);
    for(int i=0 ; cmds1[i]!=NULL; i++){

        cmdsor=parteComando(cmds1[i]);
        for(int j=0;j<1000;j++){

            if(notebook[j].idcmd==i){
                for(int c=0;cmdsor[c]!=NULL;c++ ){
                    notebook[j].cmds[c]= strdup(cmdsor[c]);
            }
        }
    }
   insereFlag = 1;
 }
}



/*
* Função responsável por executar os comandos
* e atualizar o output dos mesmos na estrutura
*/
void execmd(){


  for(int i = 0 ; i<1000;i++){

      if((notebook[i].idcmd)!=-1){
        int fp[2],pf[2];
        int out = notebook[i].idcmdDep;

        pipe(fp);
        pipe(pf);
        pid_t child;
        child=fork();
        if(!child){

          close(fp[0]);
          close(pf[1]);
          if(out!=-1){

            dup2(pf[0],0);
            close(pf[0]);
          }

          dup2(fp[1],1);
          execvp(notebook[i].cmds[1],&(notebook[i].cmds[1]));
          _exit(errno);
        }
        else{
          close(pf[0]);
          if(out!=-1){
            for(int i=0;i<1000;i++){
              if(notebook[i].idcmd==out){
                if(notebook[i].outP != NULL){
                  write(pf[1],notebook[i].outP,strlen(notebook[i].outP));
                }
              }
            }
          }

          int status;
          close(pf[1]);
          close(fp[1]);
          waitpid(child,&status,0);
          if(status!=0){
            unlink("temp.nb");
            kill(getpid(),SIGINT);
          }

          char buf[1024];
          read(fp[0],buf,1024);
          notebook[i].outP=strdup(buf);
          memset(&buf[0], 0, sizeof(buf));
        }
      }
    }

  execFlag = 1;
}



/*
* Função que escreve no ficheiro os
* resultados da execução do código ou
* comandos nele embebidos
*/
void escreveResult(int descFd){


    for(int i=0;i<1000;i++){

        if(notebook[i].desc==NULL && notebook[i].outP==NULL){
            break ;
        }

        if(notebook[i].idcmd==-1){
            write(descFd,notebook[i].desc,strlen(notebook[i].desc));
            write(descFd,"\n",1);

        } else{

            for(int j=0;j<10;j++){
                if(notebook[i].cmds[j]!=NULL){
                    ssize_t t = strlen(notebook[i].cmds[j]);
                    write(descFd,notebook[i].cmds[j],t);

                  if (notebook[i].cmds[j+1]!=NULL) {
                      write(descFd," ",1);
                    }

                }
            }

            write(descFd,"\n>>>\n",5);
            write(descFd,notebook[i].outP,strlen(notebook[i].outP));
            write(descFd,"<<<\n",4);
        }
    }
    unlink("temp.nb");
    writeFlag=1;
}



/*
* Função que copia o ficheiro input
* para um ficheiro auxiliar
*/
void copyFile(){
    if(!fork()){
      execlp("cp", "cp", original, "temp.nb",NULL);
      _exit(-1);
    }
}



/*
*  Função que trata dos sinais
*/
void handler(int j) {

   if(j==SIGINT){

      if(insereFlag==1 && execFlag==1 && writeFlag==0) {
        pid_t child1=fork();

        if(!child1){
          execlp("cp", "cp", "temp.nb", original,NULL);
          _exit(-1);
        }
        else{
          int status;
          waitpid(child1,&status,0);
          unlink("temp.nb");
        }
      }
       kill(getpid(),SIGKILL);
    }
}



int main(int argc, char *argv[]){
    signal(SIGINT,handler);

 for(int i=1;i<argc;i++){

    original=strdup(argv[i]);

    fdPrincipal = open(argv[i],O_RDONLY);
    if(fdPrincipal<0){
          perror("Erro na abertura do ficheiro\n");
          exit(-1);
      }

      copyFile();         // copia o ficheiro "original" para um ficheiro auxiliar "tem.nb"

      addInfo();          // inicializa a estrutura

      inserecmds(fdPrincipal);     // insere os dados na estrutura

      execmd();           // executa os comandos

      close(fdPrincipal);


      int fpp = open(argv[i],O_WRONLY | O_TRUNC);
      if(fpp<0){
          perror("Erro na abertura do ficheiro\n");
          exit(-1);
      }

      escreveResult(fpp);   // escreve os resultados no ficheiro

  }
    return 0;
}

