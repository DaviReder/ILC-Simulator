#ifndef CODIFICAR_H_INCLUDED
#define CODIFICAR_H_INCLUDED

#define TAM 256

// =========== STRUCTS GERAIS ===========

typedef struct no {
    unsigned char caracter;
    unsigned int frequencia;
    struct no *prox, *dir, *esq;
} No;

typedef struct lista {
    No *inicio;
    int tam;
} Lista;

// =========== FUNCOES DE BITS ===========

unsigned int testa_bit(unsigned char byte, int i);

// =========== FUNCOES TABELA DE FREQUENCIA ===========

void inicializa_com_zero(unsigned int vet[]);
void preenche_tabela(unsigned char texto[], unsigned int tabela[]);
void imprime_tabela(unsigned int tabela[]);

// =========== FUNCOES PARA LISTA 'ENCADEADA' ===========

void criar_lista(Lista *lista);
void inserir_ordenado(Lista *lista, No *no);
void preencher_lista(unsigned int tab[], Lista *lista);
void imprimir_lista(Lista *lista);

// =========== FUNCOES PARA ARVORE DE HUFFMAN ===========

No* remover_inicio(Lista *lista);
No* montar_arvore(Lista *lista);
void imprimir_arvore(No *raiz, int tam);
void liberar_arvore(No *raiz); // Adicionado para limpeza

// =========== MONTAGEM DO DICIONARIO ===========

int altura_arvore(No *raiz);
char** aloca_dicionario(int colunas);
void gerar_dicionario(char **dicionario, No *raiz, char *caminho, int colunas);
void imprimir_dicionario(char **dicionario);
void liberar_dicionario(char **dicionario); // Adicionado para limpeza

// =========== CODIFICAR ===========

int calc_tamanho_string(char **dicionario, unsigned char *texto);
char* codificar(char **dicionario, unsigned char *texto);

// =========== ESCREVER ARQUIVO ===========

// ATUALIZADO: Agora recebe a tabela para gravar o header
void compactar(const char *nome_saida, unsigned char codificado[], unsigned int tabela[]);

// =========== ENTRADA DE TEXTO E LEITURA DE ARQUIVO ===========

int descobrir_tamanho(const char *caminho);
void ler_texto(const char *caminho, unsigned char *frase);

// =========== FACILITAR NA MAIN ===========

void executar_compactacao_autonoma(const char *arquivo_origem, const char *arquivo_destino);

#endif // CODIFICAR_H_INCLUDED
