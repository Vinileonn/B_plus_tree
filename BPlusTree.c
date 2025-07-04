#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "BPlusTree.h"
#include "fila.h" 

// --- Estruturas Auxiliares  ---
typedef struct {
    unsigned long long chave;
    nodo_t *novoNodo;
    int ocorreuSplit; // 0 = não, 1 = sim
} SplitResult;


// --- Protótipos de Funções Estáticas/Auxiliares  ---
static int _obterIndiceChave(nodo_t *nodo, unsigned long long chave);
static void _inserirRegistroEmFolha(nodo_t *folha, unsigned long long chave, registro_t *registro);
static nodo_t *_buscarFolha(nodo_t *raiz, unsigned long long chave);
static void _imprimeNodo(nodo_t *nodo); // Usado por imprimeArvore
static SplitResult _inserirRecursivo(nodo_t *current_node, registro_t *registro, BPlusTree_t *arvore);
static void _dividirNodoFolha(nodo_t *nodoCheio, unsigned long long chaveNova, registro_t *registroNovo, SplitResult *result);
static void _dividirNodoInterno(nodo_t *nodoCheio, unsigned long long chavePromovidaFilho, nodo_t *filhoDireitoPromovido, SplitResult *result);
void gerarDotConteudoHTML(nodo_t *nodo, FILE *f); // Usado por gerarDot


// ====================================================================================
// --- Funções de Manipulação de Registro ---
// ====================================================================================

registro_t *criarRegistro(unsigned long long chave, const char *modelo, int ano, const char *cor) {
    registro_t *novoRegistro = (registro_t *)malloc(sizeof(registro_t));
    if (novoRegistro == NULL) {
        perror("Erro ao alocar registro");
        exit(EXIT_FAILURE);
    }
    novoRegistro->chave = chave;
    strncpy(novoRegistro->modelo, modelo, TAM_MODELO - 1);
    novoRegistro->modelo[TAM_MODELO - 1] = '\0';
    novoRegistro->ano = ano;
    strncpy(novoRegistro->cor, cor, TAM_COR - 1);
    novoRegistro->cor[TAM_COR - 1] = '\0';
    return novoRegistro;
}

void destruirRegistro(registro_t *registro) {
    if (registro) {
        free(registro);
    }
}

// ====================================================================================
// --- Funções de Manipulação de Nó ---
// ====================================================================================

nodo_t *criarNodo(int folha) {
    nodo_t *novoNodo = (nodo_t *)malloc(sizeof(nodo_t));
    if (novoNodo == NULL) {
        perror("Erro ao alocar nodo");
        exit(EXIT_FAILURE);
    }
    novoNodo->numChaves = 0;
    novoNodo->folha = folha;
    novoNodo->proximo = NULL; // Usado apenas para nós folha
    for (int i = 0; i < ORDEM; i++) {
        novoNodo->filhos[i] = NULL;
    }
    for (int i = 0; i < ORDEM - 1; i++) {
        novoNodo->registros[i] = NULL;
    }
    return novoNodo;
}

void destruirNodo(nodo_t *nodo) {
    if (nodo == NULL)
        return;

    // Libera os registros associados às chaves
    if (nodo->folha) {
        for (int i = 0; i < nodo->numChaves; i++) {
            destruirRegistro(nodo->registros[i]);
        }
    }
    free(nodo);
}

// ====================================================================================
// --- Funções de Manipulação da Árvore B+ (Estrutura Principal) ---
// ====================================================================================

BPlusTree_t *criarArvoreBPlus() {
    BPlusTree_t *arvore = (BPlusTree_t *)malloc(sizeof(BPlusTree_t));
    if (arvore == NULL) {
        perror("Erro ao alocar árvore B+");
        exit(EXIT_FAILURE);
    }
    arvore->raiz = criarNodo(1); // A raiz é inicialmente uma folha
    arvore->numNodos = 1;
    return arvore;
}

void destruirArvoreBPlus(nodo_t *raiz) {
    if (raiz == NULL) {
        return;
    }
    if (!raiz->folha) {
        for (int i = 0; i <= raiz->numChaves; i++) {
            destruirArvoreBPlus(raiz->filhos[i]);
        }
    }
    destruirNodo(raiz);
}


// ====================================================================================
// --- Funções Auxiliares de Busca ---
// ====================================================================================

// Retorna o índice onde a chave deveria ser inserida (para manter a ordem)
static int _obterIndiceChave(nodo_t *nodo, unsigned long long chave) {
    int i = 0;
    while (i < nodo->numChaves && chave > nodo->chaves[i]) {
        i++;
    }
    return i;
}

// Busca o nó folha onde a chave deveria estar
static nodo_t *_buscarFolha(nodo_t *raiz, unsigned long long chave) {
    nodo_t *atual = raiz;
    while (!atual->folha) {
        int i = 0;
        // Encontra o filho correto para descer

        while (i < atual->numChaves && chave >= atual->chaves[i]) {
            i++;
        }
        if (atual->filhos[i] == NULL) {
             fprintf(stderr, "Erro lógico: Ponteiro de filho NULL em nó interno durante busca! Chave: %llu, Nodo: %p, Indice: %d\n", chave, (void*)atual, i);
             exit(EXIT_FAILURE);
        }
        atual = atual->filhos[i];
    }
    return atual;
}

// Função para buscar um registro na árvore B+ (apenas em nós folhas)
registro_t *buscar(BPlusTree_t *arvore, unsigned long long chave) {
    if (arvore == NULL || arvore->raiz == NULL) {
        return NULL;
    }
    nodo_t *folha = _buscarFolha(arvore->raiz, chave);
    for (int i = 0; i < folha->numChaves; i++) {
        if (folha->chaves[i] == chave) {
            return folha->registros[i];
        }
    }
    return NULL;
}


// ====================================================================================
// --- Funções Auxiliares de Inserção ---
// ====================================================================================

// Insere um registro em um nó folha que tem espaço
static void _inserirRegistroEmFolha(nodo_t *folha, unsigned long long chave, registro_t *registro) {
    int i = folha->numChaves - 1;
    // Encontra a posição correta para manter as chaves ordenadas
    while (i >= 0 && chave < folha->chaves[i]) {
        folha->chaves[i + 1] = folha->chaves[i];
        folha->registros[i + 1] = folha->registros[i];
        i--;
    }
    folha->chaves[i + 1] = chave;
    folha->registros[i + 1] = registro;
    folha->numChaves++;
}

// Divide um nó folha
static void _dividirNodoFolha(nodo_t *nodoCheio, unsigned long long chaveNova, registro_t *registroNovo, SplitResult *result) {
    nodo_t *novo = criarNodo(1);
    result->novoNodo = novo;
    result->ocorreuSplit = 1;

    int pontoMedio = (ORDEM - 1) / 2;

    unsigned long long chavesTemp[ORDEM];
    registro_t *registrosTemp[ORDEM];

    int posInsercao = _obterIndiceChave(nodoCheio, chaveNova);

    for (int i = 0, j = 0; i < ORDEM - 1; i++, j++) {
        if (j == posInsercao) {
            chavesTemp[j] = chaveNova;
            registrosTemp[j] = registroNovo;
            j++;
        }
        chavesTemp[j] = nodoCheio->chaves[i];
        registrosTemp[j] = nodoCheio->registros[i];
    }
    if (posInsercao == ORDEM - 1) {
        chavesTemp[ORDEM - 1] = chaveNova;
        registrosTemp[ORDEM - 1] = registroNovo;
    }

    for(int i = 0; i < ORDEM - 1; i++) {
        nodoCheio->chaves[i] = 0;
        nodoCheio->registros[i] = NULL;
    }

    nodoCheio->numChaves = 0;
    for (int i = 0; i <= pontoMedio; i++) {
        nodoCheio->chaves[i] = chavesTemp[i];
        nodoCheio->registros[i] = registrosTemp[i];
        nodoCheio->numChaves++;
    }

    novo->numChaves = 0;
    for (int i = pontoMedio + 1; i < ORDEM; i++) {
        novo->chaves[i - (pontoMedio + 1)] = chavesTemp[i];
        novo->registros[i - (pontoMedio + 1)] = registrosTemp[i];
        novo->numChaves++;
    }

    novo->proximo = nodoCheio->proximo;
    nodoCheio->proximo = novo;

    result->chave = novo->chaves[0];
}


// Divide um nó interno
static void _dividirNodoInterno(nodo_t *nodoCheio, unsigned long long chavePromovidaFilho, nodo_t *filhoDireitoPromovido, SplitResult *result) {
    nodo_t *novo = criarNodo(0);
    result->novoNodo = novo;
    result->ocorreuSplit = 1;

    int pontoMedio = ORDEM / 2;

    unsigned long long chavesTemp[ORDEM];
    nodo_t *filhosTemp[ORDEM + 1];

    int posInsercao = _obterIndiceChave(nodoCheio, chavePromovidaFilho);

    for (int i = 0, j = 0; i < ORDEM - 1; i++, j++) {
        if (j == posInsercao) {
            chavesTemp[j] = chavePromovidaFilho;
            j++;
        }
        chavesTemp[j] = nodoCheio->chaves[i];
    }
    if (posInsercao == ORDEM - 1) {
        chavesTemp[ORDEM - 1] = chavePromovidaFilho;
    }

    for (int i = 0, j = 0; i < ORDEM; i++, j++) {
        if (j == posInsercao + 1) {
            filhosTemp[j] = filhoDireitoPromovido;
            j++;
        }
        filhosTemp[j] = nodoCheio->filhos[i];
    }
    if (posInsercao + 1 == ORDEM) {
        filhosTemp[ORDEM] = filhoDireitoPromovido;
    }

    result->chave = chavesTemp[pontoMedio];

    for(int i = 0; i < ORDEM - 1; i++) nodoCheio->chaves[i] = 0;
    for(int i = 0; i < ORDEM; i++) nodoCheio->filhos[i] = NULL;

    nodoCheio->numChaves = 0;
    for (int i = 0; i < pontoMedio; i++) {
        nodoCheio->chaves[i] = chavesTemp[i];
        nodoCheio->numChaves++;
    }
    for (int i = 0; i <= pontoMedio; i++) {
        nodoCheio->filhos[i] = filhosTemp[i];
    }

    novo->numChaves = 0;
    for (int i = pontoMedio + 1; i < ORDEM; i++) {
        novo->chaves[i - (pontoMedio + 1)] = chavesTemp[i];
        novo->numChaves++;
    }
    for (int i = pontoMedio + 1; i <= ORDEM; i++) {
        novo->filhos[i - (pontoMedio + 1)] = filhosTemp[i];
    }
}

// Função recursiva principal para inserção que retorna um SplitResult
static SplitResult _inserirRecursivo(nodo_t *current_node, registro_t *registro, BPlusTree_t *arvore) {
    SplitResult result = {0, NULL, 0}; // Inicializa com sem split

    if (current_node->folha) {
        if (current_node->numChaves < ORDEM - 1) {
            _inserirRegistroEmFolha(current_node, registro->chave, registro);
        } else {
            _dividirNodoFolha(current_node, registro->chave, registro, &result);
            arvore->numNodos++;
        }
    } else { // Nó interno
        int child_index = _obterIndiceChave(current_node, registro->chave);

        SplitResult child_split_result = _inserirRecursivo(current_node->filhos[child_index], registro, arvore);

        if (child_split_result.ocorreuSplit) {
            if (current_node->numChaves < ORDEM - 1) {
                int i = current_node->numChaves - 1;
                while (i >= 0 && child_split_result.chave < current_node->chaves[i]) {
                    current_node->chaves[i + 1] = current_node->chaves[i];
                    current_node->filhos[i + 2] = current_node->filhos[i + 1];
                    i--;
                }
                current_node->chaves[i + 1] = child_split_result.chave;
                current_node->filhos[i + 2] = child_split_result.novoNodo;
                current_node->numChaves++;
            } else {
                _dividirNodoInterno(current_node, child_split_result.chave, child_split_result.novoNodo, &result);
                arvore->numNodos++;
            }
        }
    }
    return result;
}


// ====================================================================================
// --- Função Principal de Inserção ---
// ====================================================================================

void inserir(BPlusTree_t *arvore, registro_t *registro) {
    if (arvore == NULL || registro == NULL) {
        fprintf(stderr, "Erro: Árvore ou registro nulo na inserção.\n");
        return;
    }

    if (arvore->raiz->folha) {
         for (int i = 0; i < arvore->raiz->numChaves; i++) {
            if (arvore->raiz->chaves[i] == registro->chave) {
                fprintf(stderr, "Chave %llu já existe. Inserção ignorada.\n", registro->chave);
                destruirRegistro(registro);
                return;
            }
        }
    } else {
        registro_t *folhaExistente = buscar(arvore, registro->chave);
        if (folhaExistente != NULL) {
            fprintf(stderr, "Chave %llu já existe. Inserção ignorada.\n", registro->chave);
            destruirRegistro(registro);
            return;
        }
    }

    SplitResult final_result = _inserirRecursivo(arvore->raiz, registro, arvore);

    if (final_result.ocorreuSplit) {
        nodo_t *novaRaiz = criarNodo(0);
        arvore->numNodos++;
        novaRaiz->chaves[0] = final_result.chave;
        novaRaiz->filhos[0] = arvore->raiz;
        novaRaiz->filhos[1] = final_result.novoNodo;
        novaRaiz->numChaves = 1;
        arvore->raiz = novaRaiz;
    }
}


// ====================================================================================
// --- Funções de Impressão e Visualização (DOT) ---
// ====================================================================================

// Helper para imprimir o conteúdo de um nó
static void _imprimeNodo(nodo_t *nodo) {
    if (nodo->folha) {
        printf("Folha: [");
        for (int i = 0; i < nodo->numChaves; i++) {
            printf("%llu (mod: %s)", nodo->chaves[i], nodo->registros[i]->modelo);
            if (i < nodo->numChaves - 1) {
                printf(", ");
            }
        }
        printf("] -> (Prox: %p)\n", (void*)nodo->proximo);
    } else {
        printf("Interno: [");
        for (int i = 0; i < nodo->numChaves; i++) {
            printf("%llu", nodo->chaves[i]);
            if (i < nodo->numChaves - 1) {
                printf(", ");
            }
        }
        printf("]\n");
    }
}

// Função recursiva para imprimir a árvore
void imprimeArvore(nodo_t *nodo) {
    if (nodo == NULL) {
        return;
    }

    _imprimeNodo(nodo); // Imprime o nó atual

    if (!nodo->folha) {
        for (int i = 0; i <= nodo->numChaves; i++) {
            // Adiciona indentação para melhor visualização dos níveis
            for (int j = 0; j < (i) * 4; j++) {
                printf("  ");
            }
            imprimeArvore(nodo->filhos[i]);
        }
    }
}


void gerarDotConteudoHTML(nodo_t *nodo, FILE *f) {
    if (nodo == NULL) return;

    // Etapa 1: Definir o nó atual usando sintaxe de tabela HTML
    fprintf(f, "  node%p [shape=none, margin=0, label=<\n", (void*)nodo);
    fprintf(f, "    <TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n");

    // Linha 1: Células para os ponteiros de filhos
    fprintf(f, "      <TR><TD PORT=\"f0\"> </TD>");
    for (int i = 0; i < nodo->numChaves; i++) {
        fprintf(f, "<TD PORT=\"f%d\"> </TD>", i + 1);
    }
    fprintf(f, "</TR>\n");

    // Linha 2: Chaves e dados
    fprintf(f, "      <TR>");
    for (int i = 0; i < nodo->numChaves; i++) {
        fprintf(f, "<TD BGCOLOR=\"%s\">%llu", nodo->folha ? "lightyellow" : "lightblue", nodo->chaves[i]);
        if (nodo->folha && nodo->registros[i] != NULL) {
            fprintf(f, "<BR/>%s<BR/>%d, %s",
                    nodo->registros[i]->modelo,
                    nodo->registros[i]->ano,
                    nodo->registros[i]->cor);
        }
        fprintf(f, "</TD>");
    }
    // Adiciona uma célula vazia no final se for nó interno
    if (nodo->numChaves > 0 || nodo->folha == 0) {
        fprintf(f, "<TD BGCOLOR=\"%s\"> </TD>", nodo->folha ? "lightyellow" : "lightblue");
    }
    fprintf(f, "</TR>\n");

    fprintf(f, "    </TABLE>>];\n");


    // Etapa 2: Chamar recursivamente para os filhos
    if (!nodo->folha) {
        for (int i = 0; i <= nodo->numChaves; i++) {
            gerarDotConteudoHTML(nodo->filhos[i], f);
        }
    }

    // Etapa 3: Criar as arestas
    if (!nodo->folha) {
        for (int i = 0; i <= nodo->numChaves; i++) {
            if (nodo->filhos[i] != NULL) {
                fprintf(f, "  \"node%p\":f%d -> \"node%p\";\n", (void*)nodo, i, (void*)nodo->filhos[i]);
            }
        }
    }
    if (nodo->folha && nodo->proximo != NULL) {
        fprintf(f, "  \"node%p\" -> \"node%p\" [style=dashed, color=gray, constraint=false];\n", (void*)nodo, (void*)nodo->proximo);
    }
}


void gerarDot(BPlusTree_t *arvore, const char* nomeArquivo) {
    if (arvore == NULL || arvore->raiz == NULL) return;

    FILE *f = fopen(nomeArquivo, "w");
    if (f == NULL) {
        perror("Erro ao abrir arquivo para escrita do .dot");
        return;
    }

    fprintf(f, "digraph BPlusTree {\n");
    fprintf(f, "  rankdir=TB;\n");
    fprintf(f, "  node [fontname=\"Arial\"];\n\n");

    // Define que as folhas ficarão na mesma linha
    fprintf(f, "  { rank=same; ");
    nodo_t *folha = arvore->raiz;
    while(folha != NULL && !folha->folha) folha = folha->filhos[0];
    while(folha != NULL) {
        fprintf(f, "node%p; ", (void*)folha);
        folha = folha->proximo;
    }
    fprintf(f, "}\n");

    gerarDotConteudoHTML(arvore->raiz, f);

    fprintf(f, "}\n");
    fclose(f);
    printf("Arquivo de visualização '%s' gerado com sucesso (usando HTML-like).\n", nomeArquivo);
}