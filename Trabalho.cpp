//Lauro
#include"huffman.h"


int main(int argc, char **argv){
	
    char resposta;
    char arquivo[50];
    	
    printf("..:: Huffman Compress ::.. \n\n Comprimir aperte 'C '\n Descomprimir aperte 'D'\n");
	resposta = getchar();
    	
	if(resposta == 'c'){//Comprimir o arquivo
        printf("Favor digitar o nome do arquivo a ser comprimido: ");
		scanf("%s", arquivo);
        
        char arquivoSaida[sizeof(arquivo)];
        strcpy(arquivoSaida, arquivo);
                   
        comprimir(arquivo, strcat(arquivoSaida, ".huff"));		
	}else if(resposta == 'd'){//Descomprimir
    
        printf("Favor digitar o nome do arquivo (.huff) a ser descomprimido: ");
		scanf("%s", arquivo);
        
        char *arquivoSemExt;
        arquivoSemExt = remove_ext (arquivo, '.', '/');    
    
		descomprimir(arquivo, arquivoSemExt);
	}else{//parametro desconhecido
		printf("Parametro deconhecido: %s\n", argv[1]);
		return -1;
	}
}

/*
    Remove a extenção de um nome de arquivo
    Recebe: o nome do arquivo
    Retorna: nome sem extensao
*/
char *remove_ext (char* mystr, char dot, char sep) {
    char *retstr, *lastdot, *lastsep;

    // Error checks and allocate string.
    if (mystr == NULL)
        return NULL;
    if ((retstr = (char*)malloc (strlen (mystr) + 1)) == NULL)
        return NULL;

    // Make a copy and find the relevant characters.
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, dot);
    lastsep = (sep == 0) ? NULL : strrchr (retstr, sep);

    // If it has an extension separator.

    if (lastdot != NULL) {
        // and it's before the extenstion separator.

        if (lastsep != NULL) {
            if (lastsep < lastdot) {
                // then remove it.
                *lastdot = '\0';
            }
        } else {
            // Has extension separator with no path separator.
            *lastdot = '\0';
        }
    }

    // Return the modified string.
    return retstr;
}

/*
	Funcao para compressao
	recebe o nome do arquivo como parametro
*/
void comprimir(char *arquivo_in,char *arquivo_out){
	printf("comprimindo...\n");
	int i;
	NODE vetor[TAMANHO_DIC];

	//Inicializacao do vetor de dicionario
	for(i=0;i<TAMANHO_DIC;++i){
		vetor[i].c=i;//char na tabela ascii
		vetor[i].count=0; // frequencia
		vetor[i].esq=vetor[i].dir=vetor[i].pai=vetor[i].prox=NULL;
	}

	//Abrir arquivo de entrada
	FILE *entrada=fopen(arquivo_in,"r");
	if(entrada==NULL){//Erro ao Abrir
		printf("Erro ao abrir o arquivo %s\nAbortando...\n",arquivo_in);
		exit(-1);
	}
	//Ler char por char e incluir no vetor do dicionario
	int c;
	while((c=fgetc(entrada))!=EOF){//pega caracter um por um enquanto nao chegar ao final do arquivo(EOF)
		++vetor[c].count;//incrementa a frequencia do caracter
	}
	//Criar lista ordenada pela frequencia
	NODE *raiz=NULL;
	for(i=0;i<TAMANHO_DIC;++i){
		if(vetor[i].count>0)
			inserir_no(&raiz,&(vetor[i]));
	}
	
	//Transforma a lista em uma arvore Huffman
	criar_arvore_huffman(&raiz);

	//exibe arvore
	//imprimir_arvore(raiz,0);

	//Gera os códigos
        gerar_codigos(vetor);
	
	//Abre o arquivo de saida
	FILE *saida=fopen(arquivo_out,"wb");
	if(!saida){
		printf("Erro ao abrir o arquivo de saida\nAbortando...");
		exit(-1);
	}
	//Escreve o total de letras e a frequencia de cada letra no inicio do arquivo
	escrever_frequencia(vetor,saida);


	//Volta o arquivo de entrada
	rewind(entrada);
		

	//Ler char por char obtrem o codigo e escreve quando obtem um byte completo
        CHAR_BITS cb;
	int bits_escritos=0;
	int tam_codigo;
        while((c=fgetc(entrada))!=EOF){
		tam_codigo=strlen(vetor[c].codigo);
		for(i=0;i<tam_codigo;++i){
			switch(bits_escritos){
			        case 0:
					cb.b.b0=vetor[c].codigo[i];
					++bits_escritos;
					break;
				case 1:
					cb.b.b1=vetor[c].codigo[i];
					++bits_escritos;
					break;
				case 2:
					cb.b.b2=vetor[c].codigo[i];
					++bits_escritos;
					break;
				case 3:
					cb.b.b3=vetor[c].codigo[i];
					++bits_escritos;
					break;
				case 4:
					cb.b.b4=vetor[c].codigo[i];
					++bits_escritos;
					break;
				case 5:
					cb.b.b5=vetor[c].codigo[i];
					++bits_escritos;
					break;
				case 6:
					cb.b.b6=vetor[c].codigo[i];
					++bits_escritos;
					break;
				case 7:
					cb.b.b7=vetor[c].codigo[i];
					fputc(cb.c,saida);
					bits_escritos=0;
					break;
			}

		}
	}
	//Escreve o final, caso o arquivo codificado nao coincida de ser multiplo de 8 bits 
	if(bits_escritos)
		fputc(cb.c,saida);
        
	//fecha os arquivos de entrada e saida
	fclose(entrada);
	fclose(saida);
}

void descomprimir(char *arquivo_in, char *arquivo_out){

	printf("descomprimindo...\n");
        int i;
	int total=0;
        NODE vetor[TAMANHO_DIC];

        //Abrir arquivo de entrada
        FILE *entrada=fopen(arquivo_in,"rb");
        if(!entrada){//Erro ao Abrir
                printf("Erro ao abrir o arquivo %s\nAbortando...\n",arquivo_in);
                exit(-1);
        }

	unsigned tamanhos[TAMANHO_DIC]; 
	fread(tamanhos, sizeof(unsigned), sizeof(tamanhos), entrada);
	
	//Inicializacao do vetor de dicionario
        for(i=0;i<TAMANHO_DIC;++i){
		total+=tamanhos[i];
                vetor[i].c=i;
		vetor[i].count = tamanhos[i];
                vetor[i].esq=vetor[i].dir=vetor[i].pai=vetor[i].prox=NULL;
        }
	
	
	//Criar lista ordenada pela frequencia
        NODE *raiz=NULL;
        for(i=0;i<TAMANHO_DIC;++i){
                if(vetor[i].count>0)
                        inserir_no(&raiz,&(vetor[i]));
        }

        //Transforma a lista em uma arvore Huffman
        criar_arvore_huffman(&raiz);

	//Exibe arvore
//	imprimir_arvore(raiz,0);

	//Gera os códigos
	gerar_codigos(vetor);

	FILE *saida=fopen(arquivo_out,"w");
	if(!saida){
		printf("Erro ao abrir o arquivo de saida\nAbortando...\n");
                exit(-1);

	}

	//Ler char por char e decodifica
        CHAR_BITS cb;
	int count=0;
	NODE *pos=raiz;
	int c;
        while((c=fgetc(entrada))!=EOF){
		if(count==total)
			break;
		cb.c=c;
		for(i=0;i<8;++i){
			if(count==total)
				break;
			switch(i){
                                case 0:
					if(cb.b.b0==0)
                                        	pos=pos->esq;
					else
						pos=pos->dir;
                                        break;
                                case 1:
					if(cb.b.b1==0)
                                        	pos=pos->esq;
					else
						pos=pos->dir;
                                        break;
                                case 2:
					if(cb.b.b2==0)
                                        	pos=pos->esq;
					else
						pos=pos->dir;
                                        break;
                                case 3:
					if(cb.b.b3==0)
                                        	pos=pos->esq;
					else
						pos=pos->dir;
                                        break;
                                case 4:
					if(cb.b.b4==0)
                                        	pos=pos->esq;
					else
						pos=pos->dir;
                                        break;
                                case 5:
					if(cb.b.b5==0)
                                        	pos=pos->esq;
					else
						pos=pos->dir;
                                        break;
                                case 6:
					if(cb.b.b6==0)
                                        	pos=pos->esq;
					else
						pos=pos->dir;
                                        break;
                                case 7:
					if(cb.b.b7==0)
                                        	pos=pos->esq;
					else
						pos=pos->dir;
                                        break;
                        }
			if(pos->esq==NULL&&pos->dir==NULL){
				fputc(pos->c,saida);	
				pos=raiz;
				++count;
			}
		}
	}

	//Fechar os arquivos
	fclose(entrada);
	fclose(saida);


}

//Insere no de forma ordenada na lista encadeada
// lista aponta pro primeiro elemento
void inserir_no(NODE **lista,NODE *no){
	if(*lista==NULL)
		*lista=no;
	else{
		NODE *ant=NULL;
		NODE *p=*lista;
		while(p!=NULL&&p->count<no->count){
			ant=p;
			p=p->prox;
		}
		if(ant==NULL){
			no->prox=*lista;
			*lista=no;
		}else if(p==NULL){
			ant->prox=no;
		}else{
			no->prox=ant->prox;
			ant->prox=no;
		}
	}	
}

        //Transforma a lista em uma arvore Huffman
void criar_arvore_huffman(NODE **raiz){
	NODE *no1;
        NODE *no2;
        while(tam_lista_ordenada(*raiz)>1){
                //remove os dois nos de menor frequencia da lista
                remover_menor_frequencia(raiz, &no1);
                remover_menor_frequencia(raiz, &no2);
                //Cria um novo no com frequencia igual a soma dos dois nos retirados 
                NODE *novo=(NODE *)malloc(sizeof(NODE));
                novo->c=EOF;
                novo->count=no1->count+no2->count;
                novo->esq=no1;
                no1->pai=novo;
                novo->dir=no2;
                no2->pai=novo;
                //insere o novo no na lista
                inserir_no(raiz,novo);
        }
}

//Exibe lista encadeada
void imprimir_lista_ordenada(NODE *lista){
	while(lista){
		printf("%c:%u->",lista->c,lista->count);
		lista=lista->prox;
	}
	printf("//\n");
}


//Calcula o numero de nos da lista ordenada
int tam_lista_ordenada(NODE *lista){
	int tam=0;
	while(lista){
		++tam;
		lista=lista->prox;
	}
	return tam;
}

//remove o primeiro no da lista encadeada
void remover_menor_frequencia(NODE** lista, NODE **no){
        *no=*lista;
        *lista=(*lista)->prox;
}


//exibe arvore de huffman
void imprimir_arvore(NODE *raiz,int profundidade){
	if(raiz!=NULL){
		int i;
		imprimir_arvore(raiz->dir,profundidade+1);
		for(i=0;i<profundidade;++i)
			printf("   ");
		printf("  %c\n",raiz->c);
		imprimir_arvore(raiz->esq,profundidade+1);
	}
}

//Obtem o codigo binario em uma string dada letra
void obter_codigo(NODE vetor[TAMANHO_DIC],unsigned short c, char **codigo){
	*codigo[0]='\0';
	obter_codigo_aux(&(vetor[c]),codigo);
	reverse_str(*codigo);
}

void obter_codigo_aux(NODE *no,char **codigo){
	if(no->pai){
		if(no->pai->esq==no)
                        strcat(*codigo,"0");
                else
                        strcat(*codigo,"1");
                obter_codigo_aux(no->pai,codigo);
	}
}

void reverse_str(char *str){
	int i;
	int tam=strlen(str);
	char aux;
	for(i=0;i<tam/2;++i){
		aux=str[i];
		str[i]=str[tam-1-i];
		str[tam-1-i]=aux;
	}
}

void escrever_frequencia(NODE vetor[TAMANHO_DIC],FILE * saida){
	//Escreve as frequencias dos caracteres para reconstruir a arvore
	int i;
	unsigned tamanhos[TAMANHO_DIC]; 
	for(i=0;i<TAMANHO_DIC;++i){
		tamanhos[i] = vetor[i].count;
	}
	fwrite(tamanhos, sizeof(unsigned), sizeof(tamanhos), saida);
}

void gerar_codigos(NODE vetor[TAMANHO_DIC]){
	unsigned short i;
	char *codigo = (char*) malloc(sizeof(char) * TAMANHO_DIC);
	for(i=0;i<TAMANHO_DIC;++i){
		obter_codigo(vetor, i, &codigo);
		vetor[i].codigo = (char *) malloc(sizeof(char)*strlen(codigo));
		strcpy(vetor[i].codigo,codigo);
	}
}
