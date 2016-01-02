#include"huffman.h"

int main(int argc, char **argv){

	char resposta;
	char arquivo[25];
	char arquivoComprimido[25];

	printf("Deseja comprimir(digite c) ou descomprimir(digite d)?");
	resposta=getchar();

	if(resposta == 'c'){//Comprimir o arquivo
		fflush(stdin);
		printf("Digite o nome do arquivo a ser comprimido:");
		scanf("%s",arquivo);
		comprimir(arquivo);
	}else if(resposta == 'd'){//Descomprimir o arquivo
		fflush(stdin);
		printf("Digite o nome do arquivo a ser descomprimido:");
		scanf("%s", arquivoComprimido);
		descomprimir(arquivoComprimido);
	}else{//parametro desconhecido
		printf("Parametro deconhecido");
		return -1;
	}

	return 0;
}

void comprimir(char *original){

	printf("Iniciando compressão...");
	int i;
	NODE vetor[TAMANHO_DIC];

	//Inicializacao do vetor de dicionario
	for(i=0;i<TAMANHO_DIC;++i){
		vetor[i].c=i;//char na tabela ascii
		vetor[i].count=0; // frequencia
		vetor[i].esq=vetor[i].dir=vetor[i].pai=vetor[i].prox=NULL;
	}

    strcat(original, ".txt");

	//Abrir arquivo de entrada
	FILE *entrada=fopen(original,"r");
	if(entrada==NULL){//Erro ao Abrir
		printf("Erro ao abrir o arquivo\n");
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
		if(vetor[i].count != 0){
			inserir_no(&raiz,&(vetor[i]));
		}
	}

	criar_arvore_huffman(&raiz);

	//Gera os códigos
    gerar_codigos(vetor);

	//Abre o arquivo de saida
	FILE *saida=fopen("comprimido.txt","wb");
	//Escreve o total de letras e a frequencia de cada letra no inicio do arquivo
	escrever_frequencia(vetor,saida);

	//Volta o arquivo de entrada para começar a releitura
	rewind(entrada);

	//Ler char por char, obtem o codigo e escreve quando obtem um byte completo
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
	if(bits_escritos){
		fputc(cb.c,saida);
    }

	//fecha os arquivos de entrada e saida
	fclose(entrada);
	fclose(saida);
	printf("Arquivo comprimido!");
}

void descomprimir(char *arquivo_comprimido){

	printf("descomprimindo...\n");
    int i;
	int total=0;
	NODE vetor[TAMANHO_DIC];

	strcat(arquivo_comprimido, ".txt");

	//Abrir arquivo de entrada
    FILE *entrada=fopen(arquivo_comprimido,"r");
    if(!entrada){//Erro ao Abrir
    	printf("Erro ao abrir o arquivo %s\nAbortando...\n",arquivo_comprimido);
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

	//Gera os códigos
	gerar_codigos(vetor);

	FILE *saida=fopen("descomprimido.txt","w");
	if(!saida){
	printf("Erro ao abrir o arquivo de saida\nAbortando...\n");
       	exit(-1);
	}

	//Ler char por char e decodifica
    CHAR_BITS cb;
	int count=0;
	NODE *pos=raiz;
    unsigned char c;

    while((c=fgetc(entrada))!=EOF){
		if(count==total){
			break;
		}
		cb.c=c;

		for(i=0;i<8;++i){
			if(count==total){
				break;
			}
			switch(i){

				case 0:
					if(cb.b.b0==0){
                    	pos=pos->esq;
					}
					else{
						pos=pos->dir;}
                break;

                case 1:
					if(cb.b.b1==0){
                       	pos=pos->esq;
					}
					else{
						pos=pos->dir;
                    }
                 break;

				 case 2:
					if(cb.b.b2==0){
                    	pos=pos->esq;
					}
					else{
						pos=pos->dir;
                    }
                  break;

				 case 3:
				 	if(cb.b.b3==0){
                       	pos=pos->esq;
					}
					else{
						pos=pos->dir;
                    }
                  break;

				 case 4:
				  	if(cb.b.b4==0){
                    	pos=pos->esq;
					}
					else{
						pos=pos->dir;
                    }
                  break;

				  case 5:
				 	if(cb.b.b5==0){
                    	pos=pos->esq;
					}
					else{
						pos=pos->dir;
                    }
                  break;

				  case 6:
					if(cb.b.b6==0){
                    	pos=pos->esq;
					}
					else{
						pos=pos->dir;
                    }
                  break;

				  case 7:
				  	if(cb.b.b7==0){
                      	pos=pos->esq;
					}
					else{
						pos=pos->dir;
                    }
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
	printf("Arquivo descomprimido!");
}

// lista aponta pro primeiro elemento
void inserir_no(NODE **lista,NODE *no){
	if(*lista==NULL){
		*lista=no;
	}
	else{
		NODE *ant=NULL;
		NODE *p=*lista;
		while(p!=NULL&&p->count<no->count){//anda até achar a posição do nó a ser inserido
			ant=p;
			p=p->prox;
		}
		if(ant==NULL){//se for ser alocado no inicio da lista
			no->prox=*lista;
			*lista=no;
		}
		else if(p==NULL){//se for ser alocado no final da lista
			ant->prox=no;
		}
		else{//Redireciona os ponteiros se for ser alocado em uma posição intermediária da lista
			no->prox=ant->prox;
			ant->prox=no;
		}
	}
}

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
                //Eu adicionei um free, acho q não vai dar problema
                free(novo);
        }
}

//Calcula o numero de nos da lista ordenada
int tam_lista_ordenada(NODE *lista){
	int tam=0;
	while(lista->prox != NULL){
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

void gerar_codigos(NODE vetor[TAMANHO_DIC]){
	unsigned short i;
	char *codigo = (char*) malloc(sizeof(char) * TAMANHO_DIC);
	for(i=0;i<TAMANHO_DIC;++i){
		obter_codigo(vetor, i, &codigo);
		vetor[i].codigo = (char *) malloc(sizeof(char)*strlen(codigo));
		strcpy(vetor[i].codigo,codigo);
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
		if(no->pai->esq==no){
			strcat(*codigo,"0");
		}
        else{
        	strcat(*codigo,"1");
    	}
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
	fclose(saida);
}



