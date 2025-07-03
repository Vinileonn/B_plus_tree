#include <stdlib.h>
#include <stdio.h>
#include "fila.h" 

// Nó da fila, armazena um ponteiro para um nó da árvore B+
// Esta estrutura agora é "privada" para o módulo da fila.
typedef struct FilaNodo {
    nodo_t* nodo_arvore;
    struct FilaNodo* proximo;
} FilaNodo;

// Estrutura da Fila, agora definida completamente aqui.
struct Fila {
    FilaNodo* frente;
    FilaNodo* tras;
};

// Cria uma fila vazia
Fila* criarFila() {
    Fila* f = (Fila*)malloc(sizeof(Fila));
    if (f) {
        f->frente = f->tras = NULL;
    }
    return f;
}

// Enfileira um nó da árvore B+
void enfileirar(Fila* f, nodo_t* nodo_arvore) {
    if (!f) return;
    FilaNodo* temp = (FilaNodo*)malloc(sizeof(FilaNodo));
    if (!temp) return; // Falha na alocação

    temp->nodo_arvore = nodo_arvore;
    temp->proximo = NULL;

    if (f->tras == NULL) {
        f->frente = f->tras = temp;
        return;
    }
    f->tras->proximo = temp;
    f->tras = temp;
}

// Desenfileira um nó da árvore B+
nodo_t* desenfileirar(Fila* f) {
    if (!f || f->frente == NULL) return NULL;

    FilaNodo* temp = f->frente;
    nodo_t* nodo_retornado = temp->nodo_arvore;
    f->frente = f->frente->proximo;

    if (f->frente == NULL) {
        f->tras = NULL;
    }

    free(temp);
    return nodo_retornado;
}

// Verifica se a fila está vazia
int filaVazia(Fila* f) {
    return (f == NULL || f->frente == NULL);
}

// Libera a memória da fila
void destruirFila(Fila* f) {
    if (!f) return;
    while (!filaVazia(f)) {
        desenfileirar(f); // Apenas desenfileira para liberar os FilaNodos
    }
    free(f); // Libera a estrutura da fila em si
}

// Coloque esta função no lugar da sua 'imprimeArvore' antiga
void imprimeArvorePorNiveis(nodo_t *raiz) {
    if (raiz == NULL) {
        printf("Árvore vazia.\n");
        return;
    }

    printf("--- Impressão da Árvore B+ por Níveis ---\n\n");

    Fila* fila = criarFila();
    enfileirar(fila, raiz);
    enfileirar(fila, NULL); // Marcador de fim de nível

    int nivel = 0;
    printf("Nível %d (Raiz): ", nivel);

    while (!filaVazia(fila)) {
        nodo_t *atual = desenfileirar(fila);

        if (atual == NULL) { // Fim de um nível
            printf("\n");
            if (!filaVazia(fila)) {
                enfileirar(fila, NULL); // Adiciona marcador para o próximo nível
                nivel++;
                printf("Nível %d:         ", nivel);
            }
        } else {
            // Imprime o nó atual
            if (atual->folha) {
                printf("{Folha: ");
            } else {
                printf("[Interno: ");
            }

            for (int i = 0; i < atual->numChaves; i++) {
                printf("%llu ", atual->chaves[i]);
            }
            printf("]  ");

            // Enfileira os filhos se não for um nó folha
            if (!atual->folha) {
                for (int i = 0; i <= atual->numChaves; i++) {
                    if (atual->filhos[i] != NULL) {
                        enfileirar(fila, atual->filhos[i]);
                    }
                }
            }
        }
    }
    
    printf("\n--- Sequência de Nós Folha (Encadeamento) ---\n\n");
    // Encontra o primeiro nó folha
    nodo_t *folha_atual = raiz;
    while(folha_atual != NULL && !folha_atual->folha){
        folha_atual = folha_atual->filhos[0];
    }
    
    if(folha_atual == NULL) {
        printf("Nenhum nó folha encontrado.\n");
    }

    // Percorre a lista encadeada de folhas
    while(folha_atual != NULL){
        printf("{");
        for(int i = 0; i < folha_atual->numChaves; i++){
            printf("%llu", folha_atual->registros[i]->chave);
            if(i < folha_atual->numChaves - 1) printf(", ");
        }
        printf("}");
        
        if(folha_atual->proximo != NULL){
            printf(" -> ");
        }
        folha_atual = folha_atual->proximo;
    }
    printf("\n\n--------------------------------------------\n");
    
    destruirFila(fila);
}