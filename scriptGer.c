#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>

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
	FILE * fp;
	fp = fopen("script.txt","w+");
	int num = atoi(argv[1]);
	int i;
	char cmd[4] = {'i','n','p','\0'};

	for (i = 0; i < num; i++)
	{
		int comando = rand() % 3;
		int price = rand() % 1000;
		int tamnome = rand() % 10;
		int codigo = rand();
		char nome[tamnome];

		if (tamnome<3){tamnome+=4;}
		rand_string(nome,tamnome); // escreve o nome

		if (comando == 0) // inserir artigo "i"
		{
			fprintf(fp,"i %s %d\n",nome, price);
		}
		if (comando == 1) // altera nome "n"
		{
			fprintf(fp,"n %d %s\n", codigo, nome);	
		}
		if (comando == 2) // altera preço "p"
		{
			fprintf(fp,"p %d %d\n", codigo, price);
		}
	}
	return 0;
}

