#include "estruturas.c"

void sv()
{
    int p,r,q,status;
    int codigo,nCmds,quant;
    int totalLido,codValido=1;
    char **strings;
    char buf[1024];
    char str[50];
    int vendas_fd = open("vendas.txt", O_WRONLY | O_CREAT | O_APPEND, 0600);

    int client_to_server;
    char *myfifo = "client_to_server_fifo";

    int server_to_client;
    char *myfifo2 = "server_to_client_fifo";

    mkfifo(myfifo, 0666);
    mkfifo(myfifo2, 0666);

    printf("----------------------\n");
    printf("------ SERVIDOR ------\n");
    printf("----------------------\n");

    client_to_server = open(myfifo, O_RDONLY);
    server_to_client = open(myfifo2, O_WRONLY);

    while ((totalLido = read(client_to_server, buf, sizeof(buf))) > 0)
    {
        buf[totalLido] = '\0';
        printf("Mensagem recebida do cliente: %s\n", buf);

        strings = malloc(sizeof(char *) * 2);
        strings = splitString(buf);

        for (nCmds = 0; strings[nCmds] != NULL; nCmds++){};
        codigo = atoi(strings[0]);
       
        if(codigo<idAtualArtigos){
            codValido=0;
            printf("Esse artigo não se encontra registado\n");
        }

        if (nCmds == 1 && codValido==1)
        {
            if ((p = fork()) == 0)
            {
                sprintf(str, "Em stock: %d Preco: %d\n", colecaoArtigos[codigo - 1].stock, colecaoArtigos[codigo - 1].preco);
                write(server_to_client, str, strlen(str));
                _exit(1);
            }
            else
            {
                wait(&status);
            }
        }

        if (nCmds == 2 && codValido==1)
        {
            quant = atoi(strings[1]);
            if (quant < 0)
            {
                //efetuar venda
                if ((p = fork()) == 0)
                {
                    colecaoVendas[idAtualVendas - 1].codigoArtigo = codigo;
                    colecaoVendas[idAtualVendas - 1].quantidade = abs(quant);
                    colecaoVendas[idAtualVendas - 1].montanteTotal = abs(quant) * colecaoArtigos[codigo - 1].preco;

                    sprintf(str, "%d %d %d\n", codigo, abs(quant), (abs(quant) * colecaoArtigos[codigo - 1].preco));
                    write(vendas_fd, str, strlen(str));

                    r = colecaoArtigos[codigo - 1].stock;
                    sprintf(str, "Stock atual: %d\n", r + quant);
                    write(server_to_client, str, strlen(str));

                    idAtualVendas++;

                    _exit(2);
                }
                else
                {
                    wait(&status);
                    r = colecaoArtigos[codigo - 1].stock;
                    colecaoArtigos[codigo - 1].stock = r + quant;
                }
            }
            else
            {
                //entrada em stock
                if ((p = fork()) == 0)
                {
                    sprintf(str, "Stock atual: %d\n", colecaoArtigos[codigo - 1].stock + quant);
                    write(server_to_client, str, strlen(str));
                    _exit(3);
                }
                else
                {
                    wait(&status);
                    q = colecaoArtigos[codigo - 1].stock;
                    colecaoArtigos[codigo - 1].stock = q + quant;
                }
            }
            atualizarStock();
        }
        memset(&buf[0], 0, sizeof(buf));
    }

    
    close(client_to_server);
    close(server_to_client);

    unlink("client_to_server_fifo");
    unlink("server_to_client_fifo");
}

int main(int argc, char *argv[])
{
    unlink("client_to_server_fifo");
    unlink("server_to_client_fifo");
    addInfo();
    insereArtigos();
    insereVendas();
    sv();
}