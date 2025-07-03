#ifndef BPLUSTREE_H
#define BPLUSTREE_H

//define a capacidade máxima de filhos

//#define ORDEM 3 ela deve ser variável

#define TAM_MODELO 20
#define TAM_COR 20

//estrutura para armazenar os dados de um automóvel
typedef struct {
    unsigned long long chave; //chave única (renavam)
    char modelo[TAM_MODELO];  //modelo do veículo
    int ano; //ano de fabricação
    char cor[TAM_COR]; //cor do veículo
} registro_t;

//estrutura de um nó da árvore B+
typedef struct nodo_t {
    unsigned long long chaves[ORDEM - 1]; //chaves armazenadas no nó
    struct nodo_t *filhos[ORDEM]; //ponteiros para os filhos
    registro_t *registros[ORDEM - 1]; //registros associados às chaves
    struct nodo_t *proximo; //ponteiro para o próximo nó
    unsigned short numChaves; //número de chaves atuais no nó
    char folha; //indica se o nó é folha (1) ou não (0) 
} nodo_t;

//estrutura da árvore B+
typedef struct {
    nodo_t *raiz; //ponteiro para a raiz da árvore
    int numNodos; //número total de nós na árvore
} BPlusTree_t;

registro_t *criarRegistro(unsigned long long chave, const char *modelo, int ano, const char *cor);
void destruirRegistro(registro_t *registro); //protótipo de função para destruir um registro
nodo_t *criarNodo(); //protótipo de função para criar um novo nó (folha ou interno)
void destruirNodo(nodo_t *nodo); //protótipo de função para destruir um nó
BPlusTree_t *criarArvoreBPlus(); //protótipo de função para criar uma nova árvore B+
void destruirArvoreBPlus(nodo_t *raiz); //protótipo de função para destruir a árvore B+
void inserir(BPlusTree_t *arvore, registro_t *registro); //protótipo de função para inserir um registro na árvore B+
registro_t *buscar(BPlusTree_t *arvore, unsigned long long chave); //protótipo de função para buscar um registro na árvore B+
void imprimeArvore(nodo_t *nodo); //protótipo de função para imprimir a árvore B+ (para depuração).


void gerarDot(BPlusTree_t *arvore, const char* nomeArquivo);

#endif //BPlusTree.h