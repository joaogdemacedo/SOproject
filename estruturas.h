#ifndef ESTRUTURAS_H_  
#define ESTRUTURAS_H_


char* timestamp();
int readln(int fildes, void *buf, size_t nbyte);
char** splitString(char * cmd1);
void verArtigosDaEstrutura();
void atualizarVarGlobais();
off_t avancar_offset_artigos(int linha, int fd);
off_t avancar_offset_stock(int linha, int fd);
off_t avancar_offset_strings(int linha, int fd);
char* NumToString (int i);

#endif 