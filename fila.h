#ifndef FILA_H
#define FILA_H

#include "BPlusTree.h" //

// Estrutura da Fila
typedef struct Fila Fila;

// Funções Públicas (API da Fila)
Fila* criarFila();
void enfileirar(Fila* f, nodo_t* nodo_arvore);
nodo_t* desenfileirar(Fila* f);
int filaVazia(Fila* f);
void destruirFila(Fila* f);

void imprimeArvorePorNiveis(nodo_t *raiz);

#endif // FILA_H