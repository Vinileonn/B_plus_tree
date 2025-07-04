#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "BPlusTree.h"
#include "fila.h" 

// Constantes Globais
#define MAX_LINHA 256
#define MAX_REGISTROS 10000

// Carrega registros de um arquivo para a árvore, validando cada linha.
void carregarRegistros(const char *nomeArquivo, BPlusTree_t *arvore, int numRegistros, unsigned long long *chaves) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    char linha[MAX_LINHA];
    int count = 0;
    while (fgets(linha, sizeof(linha), arquivo) && count < numRegistros) {
        unsigned long long chave;
        int ano;
        char modelo[TAM_MODELO] = {0};
        char cor[TAM_COR] = {0};
        int campos_lidos = 0;
        char char_extra;

        linha[strcspn(linha, "\n")] = 0;

        campos_lidos = sscanf(linha, "%llu,%19[^,],%d,%19[^,]%c", &chave, modelo, &ano, cor, &char_extra);

        if (campos_lidos == 4) {
             registro_t *registro = criarRegistro(chave, modelo, ano, cor);
             inserir(arvore, registro);
             if (chaves != NULL) {
                 chaves[count] = chave;
             }
             count++;
        } else {
            if (strlen(linha) > 0) {
                 fprintf(stderr, "AVISO: Linha malformada ignorada -> \"%s\"\n", linha);
            }
        }
    }
    fclose(arquivo);
}

// Testa o desempenho da busca lendo 100 chaves do arquivo 'buscas.txt'.
void testarDesempenho(BPlusTree_t *arvore, int totalRegistros) {
    const int NUM_BUSCAS = 100;
    unsigned long long chavesParaBuscar[NUM_BUSCAS];

    FILE* f_buscas = fopen("buscas.txt", "r");
    if (!f_buscas) {
        perror("ERRO: Não foi possível abrir 'buscas.txt'. Execute 'python gerar_testes_busca.py' primeiro");
        return;
    }

    int chavesLidas = 0;
    for (int i = 0; i < NUM_BUSCAS; i++) {
        if (fscanf(f_buscas, "%llu", &chavesParaBuscar[i]) != 1) {
            break;
        }
        chavesLidas++;
    }
    fclose(f_buscas);

    if (chavesLidas == 0) {
        fprintf(stderr, "AVISO: Nenhuma chave lida de 'buscas.txt'. Teste de desempenho cancelado.\n");
        return;
    }
    
    clock_t inicio = clock();

    for (int i = 0; i < chavesLidas; i++) {
        buscar(arvore, chavesParaBuscar[i]);
    }

    clock_t fim = clock();

    double tempoTotal = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    double tempoMedio = tempoTotal / chavesLidas;

    printf("ORDEM: %-3d | Registros: %-5d | Tempo médio de busca: %.10f segundos (usando buscas.txt)\n",
           ORDEM, totalRegistros, tempoMedio);
}

int main() {
    //const char *nomeArquivo = "registros_invalidos.txt";
    const char *nomeArquivo = "registros_carros.txt";
    int tamanhosTeste[] = {100, 1000, 10000};
    int numTamanhos = sizeof(tamanhosTeste) / sizeof(int);

    srand(time(NULL));

    printf("--- Iniciando Teste de Desempenho da Árvore B+ ---\n");

    for (int i = 0; i < numTamanhos; i++) {
        int numRegistros = tamanhosTeste[i];
        BPlusTree_t *arvore = criarArvoreBPlus();
        
        carregarRegistros(nomeArquivo, arvore, numRegistros, NULL);
        
        testarDesempenho(arvore, numRegistros);

        destruirArvoreBPlus(arvore->raiz);
        free(arvore);
    }

    printf("--- Teste de Desempenho Finalizado ---\n\n");
    
    printf("--- Exemplo de Impressão e Visualização (ORDEM=%d, %d registros) ---\n", ORDEM, REGISTROS);
    BPlusTree_t *arvoreExemplo = criarArvoreBPlus();
    carregarRegistros(nomeArquivo, arvoreExemplo, REGISTROS, NULL); 
    
    // --- 1. Imprime a árvore no console ---
    printf("\n[Impressão no Terminal via Fila]\n");
    imprimeArvorePorNiveis(arvoreExemplo->raiz);

    // --- 2. Gera o arquivo .dot para a imagem ---
    char nomeArquivoDot[100];
    // O nome do arquivo agora também inclui a ordem e os registros
    sprintf(nomeArquivoDot, "arvore_ordem_%d_regs_%d.dot", ORDEM, REGISTROS);
    
    char nomeArquivoPng[100];
    sprintf(nomeArquivoPng, "saida_ordem_%d_regs_%d.png", ORDEM, REGISTROS);
    
    printf("\n[Geração de Arquivo para Visualização Gráfica]\n");
    gerarDot(arvoreExemplo, nomeArquivoDot);
    
    // Limpa a memória
    destruirArvoreBPlus(arvoreExemplo->raiz);
    free(arvoreExemplo);

    // Imprime a instrução exata para o usuário gerar a imagem
    printf("\nPara gerar a imagem desta árvore, execute no terminal:\n");
    printf("dot -Tpng %s -o %s\n", nomeArquivoDot, nomeArquivoPng);

    return 0;
}