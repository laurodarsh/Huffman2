#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__
#define TAMANHO_DIC 256
 
struct node{
	char c;
        unsigned count;
	struct node* prox;
        struct node* esq;
        struct node* dir;
        struct node* pai;
	char *codigo;
};

typedef struct node NODE;

typedef struct{
	int b0:1;
	int b1:1;
	int b2:1;
	int b3:1;
	int b4:1;
	int b5:1;
	int b6:1;
	int b7:1;
}BITS;

typedef union{
	char c;
	BITS b;
}CHAR_BITS;

char *remove_ext (char* mystr, char dot, char sep);

void comprimir(char *arquivo_in,char *arquivo_out);

void descomprimir(char *arquivo_in,char *arquivo_out);

void inserir_no(NODE **lista,NODE *no);

void criar_arvore_huffman(NODE **raiz);

void imprimir_lista_ordenada(NODE *lista);

int tam_lista_ordenada(NODE *lista);

void remover_menor_frequencia(NODE** lista, NODE **no);

void imprimir_arvore(NODE *raiz,int profundidade);

void obter_codigo(NODE vetor[TAMANHO_DIC], unsigned short c, char **codigo);

void obter_codigo_aux(NODE *no,char **codigo);

void reverse_str(char *str);

void escrever_frequencia(NODE vetor[TAMANHO_DIC],FILE * saida);

void gerar_codigos(NODE vetor[TAMANHO_DIC]);
#endif
