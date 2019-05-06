#include "estruturas.c"



void ma(){
    char buf[1024];
    int totalL;
    int artigos_fd;
    int strings_fd;
    int stocks_fd;
    char ** strings;
    char *newstr;
    int atualizarFicheiro=0;
    artigos_fd = open("artigos.txt",O_WRONLY);
    strings_fd = open("strings.txt",O_WRONLY | O_APPEND);
    stocks_fd = open("stocks.txt",O_WRONLY | O_CREAT | O_APPEND,0600);

    while((totalL=readln(0,buf,strlen(buf))>0)){
         strings = malloc(sizeof(char *)*totalL);
         strings = splitString(buf);
        
         
         if(*strings[0]=='i'){
            char stringNum[totalL];

             newstr = malloc(strlen(strings[1]) + 2);
             strcpy(newstr, strings[1]);
             strcat(newstr, "\n");
             
            
             write(strings_fd,newstr,strlen(newstr));     
             sprintf(stringNum,"%s %s %s\n",NumToString(idAtualArtigos),NumToString(idAtualStrings),NumToString(atoi(strings[2]))); 
             write(stocks_fd,"000000\n",7);

             lseek(artigos_fd,0,SEEK_END);
             write(artigos_fd,stringNum,strlen(stringNum));

             printf("ID do artigo: %d\n",idAtualArtigos);
             idAtualArtigos++;
             idAtualStrings++;
         }

         if(*strings[0]=='n'){   

             if(atoi(strings[1])<idAtualArtigos){       
            
                newstr = malloc(strlen(strings[2]) + 2);
                strcpy(newstr, strings[2]);
                strcat(newstr, "\n");

                write(strings_fd,newstr,strlen(newstr));

                avancar_offset_artigos(atoi(strings[1]),artigos_fd);
                lseek(artigos_fd,7,SEEK_CUR);
                write(artigos_fd,NumToString(idAtualStrings),6);
                idAtualStrings++;

             }else{
                printf("Esse artigo não se encontra registado\n");
             }  
             
         }

         if(*strings[0]=='p'){   
   
            if(atoi(strings[1])<idAtualArtigos){   
                avancar_offset_artigos(atoi(strings[1]),artigos_fd);
                lseek(artigos_fd,14,SEEK_CUR);
                write(artigos_fd,NumToString(atoi(strings[2])),6);

            }else{
                printf("Esse artigo não se encontra registado\n");
             } 
         }

        if (*strings[0] == 'a'){
            //printf("TT1\n");
            
            int filho;
            filho = fork();
            
            if(filho==0){ 
                //printf("TT2\n"); 
                char *myfifo4 = "ma_to_server_fifo";
                int ma_to_server_fifo = open(myfifo4,O_WRONLY);  
                write(ma_to_server_fifo,"a\0",1);
                close(ma_to_server_fifo);
                execlp("./ag","./ag",NULL,NULL);
            } else {
               wait(NULL);
              
           }
         }  

        memset(&buf[0], 0, sizeof(buf));
    } 
}


int main(int argc,  char * argv[]){
      addInfo();
      atualizarVarGlobais();
      ma();
}
