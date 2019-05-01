#ifndef ESTRUTURAS_H_  
#define ESTRUTURAS_H_

typedef struct Artigos artigos;
typedef struct Vendas venda;
typedef struct Agregacoes agregacoes;


void addInfo();
int readln(int fildes, void *buf, size_t nbyte);
char** splitString(char * cmd1);
char* nomeArtigo(int posicao);
void verArtigosDaEstrutura();
void verVendasDaEstrutura();
void insereVendas();
void insereArtigos();
void atualizarArtigos();
void atualizarStock();
char* timestamp();

#endif 