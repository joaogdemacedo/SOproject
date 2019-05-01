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



int readln(int fildes, void *buf, size_t nbyte){
    int r,bytesRead=0;
    while ((r=(read(fildes,buf+bytesRead,1))>0) && ((char*)buf)[bytesRead]!='\n' ) {
        bytesRead++;
    }
    return bytesRead;
}


void cv(){
   char buf[1024];
   int tLidoT,tLidoP;

   int client_to_server;
   char *myfifo = "client_to_server_fifo";

   int server_to_client;
   char *myfifo2 = "server_to_client_fifo";


   while((tLidoT = readln(0,buf,sizeof(buf)))>0){
      ((char*)buf)[strlen(buf)-1]='\0';


      client_to_server = open(myfifo, O_WRONLY);
      server_to_client = open(myfifo2, O_RDONLY);
      write(client_to_server,buf,strlen(buf));

      memset(&buf[0], 0, sizeof(buf));
      
      tLidoP = read(server_to_client,buf,sizeof(buf));
      ((char*)buf)[tLidoP]='\0'; 

      write(1,buf,strlen(buf));
        
      memset(&buf[0], 0, sizeof(buf));
  }
    
}


int main(int argc, char * argv[]){
    cv();

    return 1;
}