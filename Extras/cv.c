#include "estruturas.c"

void cv(){
   char buf[1024];
   int tLidoT,tLidoP;

   int client_to_server;
   char *myfifo = "client_to_server_fifo";

   int server_to_client;
   char *myfifo2 = "server_to_client_fifo";

   int rv;
  
   fd_set set;
   struct timeval timeout;


   while((tLidoT = readln(0,buf,sizeof(buf)))>0){
     // ((char*)buf)[strlen(buf)-1]='\0';

      client_to_server = open(myfifo, O_WRONLY);
      server_to_client = open(myfifo2, O_RDONLY);
      FD_ZERO(&set); /* clear the set */
      FD_SET(server_to_client, &set);
      write(client_to_server,buf,strlen(buf));

      memset(&buf[0], 0, sizeof(buf));
      
      timeout.tv_sec = 0;

      rv = select(server_to_client + 1, &set, NULL, NULL, &timeout);
    if(rv == -1)
       perror("select"); 
    else if(rv == 0){
       printf("Servidor desligado\n");
       break;
    }
    else{
       tLidoP = read(server_to_client,buf,sizeof(buf));
       ((char*)buf)[tLidoP]='\0'; 

       write(1,buf,strlen(buf));
        
       memset(&buf[0], 0, sizeof(buf));
    }  
  }
}


int main(int argc, char * argv[]){
    cv();
  
    return 1;
}