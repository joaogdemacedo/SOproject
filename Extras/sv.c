#include "estruturas.c"

int client_to_server = -1;
int server_to_client = -1;
int linhaVendas = 0;


 void agregarConc(int l_ini, int l_fim, int it){
               
        char pipe[30];
        char itt[5];
        char buffer[1024];
        sprintf(pipe,"server_to_ag_fifo_%d",it);
        sprintf(itt,"%d",it);

        if(mkfifo(pipe, 0666)<0){
            perror("Erro na criação do pipe");
        } 
        int p,m,tL;
        
        int vendas_fd = open("vendas.txt", O_RDONLY);
        if(vendas_fd<0){
            perror("Erro na abertura do ficheiro VENDAS");
        }
        
        lseek(vendas_fd, 21 * (l_ini - 1), SEEK_SET);

        if((p=fork())==0){
            execlp("./ag","./ag",pipe,itt,NULL);
        }
        
        printf("Nome do pipe: %s\n",pipe);

        int server_to_ag_fifo = open(pipe, O_WRONLY);
        while ( (l_ini<=l_fim) && ((tL = readln(vendas_fd, buffer, sizeof(buffer))) > 0) )
        {
            printf("[SV]\n"); 
            m = fork();
            if (m == 0)
            {    
                write(server_to_ag_fifo, buffer, tL);
                memset(&buffer[0], 0, sizeof(buffer));
                _exit(-1);
            }
            wait(NULL);
            l_ini++;
           // memset(&buffer[0], 0, sizeof(buffer));
        }  
        close(server_to_ag_fifo); 
        unlink(pipe);  
        close(vendas_fd); 
}
            
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
    char *myfifo2 = "server_to_client_fifo";
    //char *myfifo3 = "server_to_ag_fifo";
    char *myfifo4 = "ma_to_server_fifo";

    if(mkfifo(myfifo, 0666)<0){
        perror("Erro na criação do pipe com nome client_to_server_fifo");
    }
    if(mkfifo(myfifo2, 0666)<0){
        perror("Erro na criação do pipe com nome server_to_client_fifo");
    }
   // if(mkfifo(myfifo3, 0666)<0){
   //     perror("Erro na criação do pipe com nome server_to_ag_fifo");
   // } 
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
        int tL, i, j;
        int h;
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
            while ((tL = read(ma_to_server_fifo, buffer, sizeof(buffer))) > 0)
            {
                int TAM = idAtualVendas - linhaVendas;
                int it=1;
                int ini = linhaVendas;
                int fim = idAtualVendas - 1;

                int f = ini-1 + (TAM/4);
                int dv = TAM/4;

                if (TAM % 4 == 0)
                {
                    for (i = 0; i < 4; i++)
                    {
                        h = fork();
                        if (h == 0)
                        {
                            printf("PROCESSO: %d - INI: %d\n",i,ini);
                            printf("PROCESSO: %d - FIM: %d\n",i,f);
                            agregarConc(ini, f, it); 
                            _exit(1);
                        }
                        it++;
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
                        h = fork();
                        if (h == 0)
                        {
                            printf("PROCESSO: %d - INI: %d\n",i,ini);
                            printf("PROCESSO: %d - FIM: %d\n",i,f);
                            agregarConc(ini, f, it);
                            _exit(1);
                        }
                        it++;
                        ini += dv;

                        if (i == 2)
                        {
                            f = fim;
                        }
                        else
                        {
                            f += dv;
                        }
                    }
                    for (j = 0; j < 4; j++)
                    {
                        wait(&status);
                        printf("STATUS: %d\n", WEXITSTATUS(status));
                    }
                }

                linhaVendas+=TAM;
                sprintf(total, "%d\n", linhaVendas);
                lseek(ultimaLinha, 0, SEEK_SET);
                write(ultimaLinha, total, strlen(total));
                close(ultimaLinha);
                memset(&total[0], 0, sizeof(total));
                printf("TT4.1\n");
            }
        }
        close(ma_to_server_fifo);
        _exit(-1);
    }
    else
    {
        client_to_server = open(myfifo, O_RDONLY);
        server_to_client = open(myfifo2, O_WRONLY);

        while (1)
        {
            while ((totalLido = read(client_to_server, buf, sizeof(buf))) > 0)
            {
                // buf[totalLido] = '\0';
                printf("Mensagem recebida do cliente: %s\n", buf);

                strings = malloc(sizeof(char *) * 2);
                strings = splitString(buf);

                for (nCmds = 0; strings[nCmds] != NULL; nCmds++)
                {
                };
                codigo = atoi(strings[0]);

                if (codigo >= idAtualArtigos)
                {
                    if ((p = fork()) == 0)
                    {
                        write(server_to_client, "Esse artigo não se encontra registado\n", strlen("Esse artigo não se encontra registado\n"));
                        _exit(1);
                    }
                    else
                    {
                        wait(&status);
                        codValido = 0;
                    }
                }

                if (nCmds == 1 && codValido == 1)
                {
                    if ((p = fork()) == 0)
                    {
                        char buffilho[6];
                        avancar_offset_stock(codigo, stocks_fd);
                        read(stocks_fd, buffilho, 6);
                        int stock = atoi(buffilho);
                        memset(&buffilho[0], 0, sizeof(buffilho));
                        avancar_offset_artigos(codigo, artigos_fd);
                        lseek(artigos_fd, 14, SEEK_CUR);
                        read(artigos_fd, buffilho, 6);
                        int preco = atoi(buffilho);
                        memset(&buffilho[0], 0, sizeof(buffilho));

                        sprintf(str, "Em stock: %d Preco: %d\n", stock, preco);
                        write(server_to_client, str, strlen(str));
                        _exit(1);
                    }
                    else
                    {
                        wait(&status);
                    }
                }

                if (nCmds == 2 && codValido == 1)
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
                            int stock_antigo = atoi(buffilho);
                            int stock_novo = stock_antigo + quant;

                            if (stock_novo < 0)
                            {
                                write(server_to_client, "Quantidade indisponivel.\n", strlen("Quantidade indisponivel.\n"));
                                _exit(-1);
                            }

                            avancar_offset_stock(codigo, stocks_fd);
                            write(stocks_fd, NumToString(stock_novo), 6);
                            memset(&buffilho[0], 0, sizeof(buffilho));

                            avancar_offset_artigos(codigo, artigos_fd);
                            lseek(artigos_fd, 14, SEEK_CUR);
                            read(artigos_fd, buffilho, 6);
                            int preco = atoi(buffilho);
                            memset(&buffilho[0], 0, sizeof(buffilho));

                            lseek(vendas_fd, 0, SEEK_END);
                            sprintf(str, "%s %s %s\n", NumToString(codigo), NumToString(abs(quant)), NumToString((abs(quant) * preco)));
                            write(vendas_fd, str, strlen(str));

                            sprintf(str, "Stock atual: %d\n", stock_novo);
                            write(server_to_client, str, strlen(str));

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
                            write(server_to_client, str, strlen(str));

                            avancar_offset_stock(codigo, stocks_fd);
                            write(stocks_fd, NumToString(stock_novo), 6);
                            memset(&buffilho[0], 0, sizeof(buffilho));
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
        unlink("server_to_client_fifo");
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
        unlink("server_to_client_fifo");
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
    //addInfo();
    atualizarVarGlobais();
    //insereArtigos();
    sv();
}