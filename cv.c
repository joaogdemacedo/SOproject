#include "estruturas.c"


void cv(){
   char buf[1024];
   char nomeP[50];
   int tLidoT,tLidoP;
   char msg[200];

   int client_to_server;
   char *myfifo = "client_to_server_fifo";

   int server_to_client;
   sprintf(nomeP,"server_to_client_fifo%d",getpid());

   if(mkfifo(nomeP, 0666)<0){
        perror("Erro na criação do pipe com nome server_to_client_fifo");
    }
   //char *myfifo2 = "server_to_client_fifo";

   int rv;
  
   fd_set set;
   struct timeval timeout;

   int tt1 = open("scriptCV_1.txt",O_RDONLY);

   while((tLidoT = readln(tt1,buf,sizeof(buf)))>0){
     // ((char*)buf)[strlen(buf)-1]='\0';

      client_to_server = open(myfifo, O_WRONLY);
     
   //   FD_ZERO(&set); /* clear the set */
   //   FD_SET(server_to_client, &set);
      sprintf(msg,"%s %s",buf,nomeP);
      write(client_to_server,msg,strlen(msg));
      close(client_to_server);
     

      memset(&buf[0], 0, sizeof(buf));
      
  //    timeout.tv_sec = 0;

  /*    rv = select(server_to_client + 1, &set, NULL, NULL, &timeout);
    if(rv == -1)
       perror("select"); 
    else if(rv == 0){
       printf("Servidor desligado\n");
       break;
    }
    else{*/
       server_to_client = open(nomeP, O_RDONLY);
       tLidoP = read(server_to_client,buf,sizeof(buf));
       close(server_to_client);
       ((char*)buf)[tLidoP]='\0'; 

       write(1,buf,strlen(buf));
        
       memset(&buf[0], 0, sizeof(buf));
   // }  
  }

  unlink(nomeP);
}


int main(int argc, char * argv[]){ 
    cv();
  
    return 1;
}