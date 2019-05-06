#ifndef ESTRUTURAS_H_  
#define ESTRUTURAS_H_

typedef struct Artigos artigos;


char* timestamp();
void addInfo();
int readln(int fildes, void *buf, size_t nbyte);
char** splitString(char * cmd1);
void verArtigosDaEstrutura();
void insereArtigos();
void atualizarVarGlobais();
off_t avancar_offset_artigos(int linha, int fd);
off_t avancar_offset_stock(int linha, int fd);
off_t avancar_offset_strings(int linha, int fd);
char* nomeArtigo(int posicao);
char* NumToString (int i);


#endif 