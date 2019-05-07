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

void rand_string(char *str, size_t size)
{
    char charset[] = "abcdefghijklmnopqrstuvwxyz";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
}

int main(int argc, char const *argv[])
{
	
	int scriptMA = open("scriptMA.txt",O_CREAT | O_RDWR,0666);
	int scriptCV = open("scriptCV.txt",O_CREAT | O_RDWR,0666);
	//fp = fopen("scriptMA.txt","w+");
	//int num = atoi(argv[1]);
	int i;
	char cmd[4] = {'i','n','p','\0'};

	for (i = 0; i < 500000; i++)
	{
		int tamnome = rand() % 80;
		if (tamnome<3){tamnome+=4;}
		char nome[tamnome];
		rand_string(nome,tamnome); // escreve o nome
		int preco = rand() % 1001;
		char linha[100];

		sprintf(linha,"i %s %d\n",nome,preco);
		write(scriptMA,linha,strlen(linha));
		memset(&linha,0,sizeof(linha));
	}
	for (i = 0; i < 10000; i++)
	{
		int opcao = rand() % 2;
		int codigo = rand() % 500000;
		char linha[100];
		char string[20];

		if (opcao == 0) 
		{
			int tamnome = rand() % 80;
			if (tamnome<3){tamnome+=4;}
			char nome[tamnome];
			rand_string(nome,tamnome); // escreve o nome

			// alterar o nome
			sprintf(linha,"n %d %s\n",codigo,nome);
			write(scriptMA,linha,strlen(linha));
			memset(&linha,0,sizeof(linha));
			// pedir para mostrar stock e preço
			sprintf(string,"%d\n",codigo);
			write(scriptCV,string,strlen(string));
			memset(&string,0,sizeof(string));
		}
		if (opcao == 1)
		{
			int preco = rand() % 1001;
			int quantidade = ((rand() % 100) - (rand()%100));

			// alterar o preço
			sprintf(linha,"p %d %d\n",codigo,preco);
			write(scriptMA,linha,strlen(linha));
			memset(&linha,0,sizeof(linha));
			// pedir para actualizar stock 
			sprintf(string,"%d %d\n",codigo,quantidade);
			write(scriptCV,string,strlen(string));
			memset(&string,0,sizeof(string));
		}
	}
	return 0;
}

