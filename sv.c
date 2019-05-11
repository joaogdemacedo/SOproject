#include "estruturas.c"

int client_to_server = -1;
int server_to_client = -1;

void sv()
{
    int p, status;
    int codigo, nCmds, quant;
    int totalLido, codValido = 1;
    char **strings;
    char buf[1024];
    char str[50];
    int vendas_fd = open("vendas.txt", O_RDWR);
    if(vendas_fd<0){
        perror("Erro na abertura do ficheiro VENDAS");
    }
    int stocks_fd = open("stocks.txt", O_RDWR);
    if(stocks_fd<0){
        perror("Erro na abertura do ficheiro STOCKS");
    }
    int artigos_fd = open("artigos.txt", O_RDONLY);
    if(artigos_fd<0){
        perror("Erro na abertura do ficheiro ARTIGOS");
    }

    char *myfifo = "client_to_server_fifo";
   // char *myfifo2 = "server_to_client_fifo";
    char *myfifo3 = "server_to_ag_fifo";
    char *myfifo4 = "ma_to_server_fifo";

    if(mkfifo(myfifo, 0666)<0){
        perror("Erro na criação do pipe com nome client_to_server_fifo");
    }
   /* if(mkfifo(myfifo2, 0666)<0){
        perror("Erro na criação do pipe com nome server_to_client_fifo");
    } */
    if(mkfifo(myfifo3, 0666)<0){
        perror("Erro na criação do pipe com nome server_to_ag_fifo");
    } 
    if(mkfifo(myfifo4, 0666)<0){
        perror("Erro na criação do pipe com nome ma_to_server_fifo");
    }


    printf("----------------------\n");
    printf("------ SERVIDOR ------\n");
    printf("----------------------\n");

    int ultimaLinha = open("ultimaLinha.txt", O_CREAT | O_RDWR, 0600);
    if(ultimaLinha<0){
        perror("Erro na abertura do ficheiro ultimalinha");
    }


    int bastardo;

    bastardo = fork();
    if (bastardo == 0)
    {
        // printf("TT2\n");

        int linhaVendas = 0;
        int tL;
        char total[10];
        char buffer[1024];
        
        if ((tL = readln(ultimaLinha, buffer, sizeof(buffer))) == 0)
        {
            linhaVendas = 1;
            memset(&buffer[0], 0, sizeof(buffer));
            // printf("TT3\n");
        }
        else
        {
            linhaVendas = atoi(buffer);
            memset(&buffer[0], 0, sizeof(buffer));
            //  printf("TT4\n");
        }
        
        int ma_to_server_fifo = open(myfifo4, O_RDONLY);
        while (1)
        {
            while ((tL = readln(ma_to_server_fifo, buffer, sizeof(buffer))) > 0)
            {
                        printf("TT5\n");
                        printf("%s",buffer);
                        printf("TT6\n");
                        lseek(vendas_fd, 21 * (linhaVendas - 1), SEEK_SET);
                        printf("TT7\n");
                        int server_to_ag_fifo = open(myfifo3, O_WRONLY);
                        printf("TT4.1\n");
                        char* newstr;

                        while ((tL = readln(vendas_fd, buffer, sizeof(buffer))) > 0)
                        {
                           // printf("TT5\n");
                            newstr = malloc(strlen(buffer) + 2);
                            strcpy(newstr, buffer);
                            strcat(newstr, "\n");
                           
                            int m = fork();
                            if (m == 0)
                            {
                                printf("Entrou\n");
                                write(server_to_ag_fifo, newstr, strlen(newstr));
                                _exit(1);
                            }
                            wait(NULL);
                            memset(&buffer[0], 0, sizeof(buffer));
                            linhaVendas++;
                            free(newstr);                            
                        }
                        close(server_to_ag_fifo);
                        sprintf(total, "%s\n", NumToString(linhaVendas));
                        lseek(ultimaLinha, 0, SEEK_SET);
                        write(ultimaLinha, total,7);
                        close(ultimaLinha);
                        memset(&total[0], 0, sizeof(total));
                 
            }
        }
        close(ma_to_server_fifo);
        _exit(-1);
    }
    else{
    
    client_to_server = open(myfifo, O_RDONLY);
    //server_to_client = open(myfifo2, O_WRONLY);


    while (1)
    {

        while ((totalLido = readln(client_to_server, buf, sizeof(buf))) > 0)
        {
            // buf[totalLido] = '\0';
            printf("Mensagem recebida do cliente: %s\n", buf);

            strings = malloc(sizeof(char *) * 3);
            strings = splitString(buf);

            
            for (nCmds = 0; strings[nCmds] != NULL; nCmds++)
            {
            };
            codigo = atoi(strings[0]);

            if (codigo >= idAtualArtigos)
            {
                if ((p = fork()) == 0)
                {
                    if(nCmds==2) {
                        server_to_client = open(strings[1], O_WRONLY);
                        } 
                    else{
                        server_to_client = open(strings[2], O_WRONLY);
                    }
                    write(server_to_client, "Esse artigo não se encontra registado\n", strlen("Esse artigo não se encontra registado\n"));
                    close(server_to_client);
                    _exit(1);
                }
                else
                {
                    wait(&status);
                    codValido = 0;
                }
            }

            if (nCmds == 2 && codValido == 1)
            {
                
                if ((p = fork()) == 0)
                {
                    char buffilho[6];
                    int preco;
                    avancar_offset_stock(codigo, stocks_fd);
                    read(stocks_fd, buffilho, 6);
                    int stock = atoi(buffilho);
                    memset(&buffilho[0], 0, sizeof(buffilho));

                    
                    avancar_offset_artigos(codigo, artigos_fd);
                    lseek(artigos_fd, 14, SEEK_CUR);
                    read(artigos_fd, buffilho, 6);
                    preco = atoi(buffilho);
                    memset(&buffilho[0], 0, sizeof(buffilho));
                    
                    
                    sprintf(str, "Em stock: %d Preco: %d\n", stock, preco);
                    server_to_client = open(strings[1], O_WRONLY);
                    write(server_to_client, str, strlen(str));
                    close(server_to_client);
                    _exit(1);
                }
                else
                {
                    wait(&status);
                }
            }

            if (nCmds == 3 && codValido == 1)
            {
                quant = atoi(strings[1]);
                if (quant < 0)
                {
                    //efetuar venda
                    if ((p = fork()) == 0)
                    {
                        
                        char buffilho[6];
                        avancar_offset_stock(codigo, stocks_fd);
                        read(stocks_fd, buffilho, 6);
                        int preco;
                        int stock_antigo = atoi(buffilho);
                        int stock_novo = stock_antigo + quant;

                        if (stock_novo < 0)
                        {
                            server_to_client = open(strings[2], O_WRONLY);
                            write(server_to_client, "Quantidade indisponivel.\n", strlen("Quantidade indisponivel.\n"));
                            close(server_to_client);
                            _exit(-1);
                        }

                        avancar_offset_stock(codigo, stocks_fd);
                        write(stocks_fd, NumToString(stock_novo), 6);
                        memset(&buffilho[0], 0, sizeof(buffilho));
                        
                        avancar_offset_artigos(codigo, artigos_fd);
                        lseek(artigos_fd, 14, SEEK_CUR);
                        read(artigos_fd, buffilho, 6);
                        preco = atoi(buffilho);
                        memset(&buffilho[0], 0, sizeof(buffilho));
                        

                        lseek(vendas_fd, 0, SEEK_END);
                        sprintf(str, "%s %s %s\n", NumToString(codigo), NumToString(abs(quant)), NumToString((abs(quant) * preco)));
                        write(vendas_fd, str, strlen(str));

                        sprintf(str, "Stock atual: %d\n", stock_novo);
                        server_to_client = open(strings[2], O_WRONLY);
                        write(server_to_client, str, strlen(str));
                        close(server_to_client);

                        idAtualVendas++;

                        _exit(2);
                    }
                    else
                    {
                        wait(&status);
                    }
                }
                else
                {
                    //entrada em stock
                    if ((p = fork()) == 0)
                    {
                        
                        char buffilho[6];
                        avancar_offset_stock(codigo, stocks_fd);
                        read(stocks_fd, buffilho, 6);
                        int stock_antigo = atoi(buffilho);
                        int stock_novo = stock_antigo + quant;

                        sprintf(str, "Stock atual: %d\n", stock_novo);

                        avancar_offset_stock(codigo, stocks_fd);
                        write(stocks_fd, NumToString(stock_novo), 6);
                        memset(&buffilho[0], 0, sizeof(buffilho));

                        server_to_client = open(strings[2], O_WRONLY);
                        write(server_to_client, str, strlen(str));
                        close(server_to_client);

                        _exit(3);
                    }
                    else
                    {
                        wait(&status);
                    }
                }
            }
            codValido = 1;
            memset(&buf[0], 0, sizeof(buf));
            for(int v=0;v<3;v++){
                free(*(strings+v));
            }
            free(strings);
        }
    }
    }
}

void handler(int i)
{
    if (i == SIGINT)
    {
        write(1, "\nServidor desconectado!\n", strlen("\nServidor desconectado!\n"));
        close(client_to_server);
        close(server_to_client);
        unlink("client_to_server_fifo");
        //unlink("server_to_client_fifo");
        unlink("ma_to_server_fifo");
        unlink("server_to_ag_fifo");
        kill(getpid(), SIGKILL);
    }
}

void handler2(int i){
    if(i==SIGTERM){
        write(1,"\nServidor desconectado!\n",strlen("\nServidor desconectado!\n"));
        close(client_to_server);
        close(server_to_client);
        unlink("client_to_server_fifo");
        //unlink("server_to_client_fifo");
        unlink("ma_to_server_fifo");
        unlink("server_to_ag_fifo");
        kill(getpid(),SIGKILL);
    }
}

int main(int argc, char *argv[])
{
    if (signal(SIGINT, handler) == SIG_ERR)
    {
        perror("SIGINT failed");
    }
    if(signal(SIGTERM,handler2)==SIG_ERR){
        perror("SIGTERM failed");
    }
    atualizarVarGlobais();
    sv();
}