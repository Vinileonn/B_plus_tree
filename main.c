#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include "BPlusTree.h"
#include "fila.h"

#define MAX_LINHA 256

// Carrega registros de um arquivo para a árvore.
void carregarRegistros(const char *nomeArquivo, BPlusTree_t *arvore, int numRegistros, unsigned long long *chaves) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo para carregar registros");
        exit(EXIT_FAILURE);
    }

    char linha[MAX_LINHA];
    int count = 0;
    while (fgets(linha, sizeof(linha), arquivo) && count < numRegistros) {
        unsigned long long chave;
        int ano;
        char modelo[TAM_MODELO] = {0};
        char cor[TAM_COR] = {0};

        linha[strcspn(linha, "\n")] = 0;

        if (sscanf(linha, "%llu,%19[^,],%d,%19[^,]", &chave, modelo, &ano, cor) == 4) {
             registro_t *registro = criarRegistro(chave, modelo, ano, cor);
             inserir(arvore, registro);
             if (chaves != NULL) {
                 chaves[count] = chave;
             }
             count++;
        }
    }
    fclose(arquivo);
}

// Testa o desempenho da busca lendo chaves do arquivo 'buscas.txt'.
// O número de buscas é fixo em 100 
void testarDesempenhoBusca(BPlusTree_t *arvore, int totalRegistros) {
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
        fprintf(stderr, "AVISO: Nenhuma chave lida de 'buscas.txt'. Teste de desempenho de busca cancelado.\n");
        return;
    }

    clock_t inicio = clock();

    for (int i = 0; i < chavesLidas; i++) {
        buscar(arvore, chavesParaBuscar[i]);
    }

    clock_t fim = clock();

    double tempoTotal = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    double tempoMedio = tempoTotal / chavesLidas;

    printf("ORDEM: %-3d | Registros Inseridos: %-6d | Tempo Total Busca (%d chaves): %.6f segundos | Tempo Médio por Busca: %.10f segundos\n",
           ORDEM, totalRegistros, chavesLidas, tempoTotal, tempoMedio);
}

// Testa o desempenho da inserção de registros.
void testarDesempenhoInsercao(BPlusTree_t *arvore, const char *nomeArquivo, int numRegistros) {

    clock_t inicio = clock();

    carregarRegistros(nomeArquivo, arvore, numRegistros, NULL);

    clock_t fim = clock();

    double tempoTotal = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    double tempoMedio = tempoTotal / numRegistros;

    printf("ORDEM: %-3d | Registros Inseridos: %-6d | Tempo Total Inserção: %.6f segundos | Tempo Médio por Inserção: %.10f segundos\n",
           ORDEM, numRegistros, tempoTotal, tempoMedio);
}


int main() {
    const char *nomeArquivoDados = "registros_carros.txt";
    int tamanhosTeste[] = {100, 1000, 100000};
    int numTamanhos = sizeof(tamanhosTeste) / sizeof(int);

    srand(time(NULL));

    printf("--- Iniciando Testes de Desempenho da Árvore B+ ---\n");
    printf("Configuração (ORDEM definida via Makefile/compilação)\n");
    printf("-----------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < numTamanhos; i++) {
        int numRegistros = tamanhosTeste[i];

        printf("Realizando testes para %d registros:\n", numRegistros);

        // Teste de Desempenho de Inserção
        BPlusTree_t *arvoreInsercao = criarArvoreBPlus();
        testarDesempenhoInsercao(arvoreInsercao, nomeArquivoDados, numRegistros);
        destruirArvoreBPlus(arvoreInsercao->raiz);
        free(arvoreInsercao);

        // Teste de Desempenho de Busca
        BPlusTree_t *arvoreBusca = criarArvoreBPlus();
        carregarRegistros(nomeArquivoDados, arvoreBusca, numRegistros, NULL);
        testarDesempenhoBusca(arvoreBusca, numRegistros);

        int altura = alturaArvoreBPlus(arvoreBusca->raiz);
        printf("\nAltura da Árvore B+ com REGISTRO %d = %d\n\n", numRegistros, altura);

        destruirArvoreBPlus(arvoreBusca->raiz);
        free(arvoreBusca);

        printf("-----------------------------------------------------------------------------------------------------------\n");
    }

    // Seção de Visualização

    printf("--- Impressão e Visualização (ORDEM=%d, %d registros) ---\n\n", ORDEM, REGISTROS);
    BPlusTree_t *arvoreExemplo = criarArvoreBPlus();
    carregarRegistros(nomeArquivoDados, arvoreExemplo, REGISTROS, NULL);

    /*
    // Descomente se quiser a impressão no console
    // Imprime a árvore no console 
    printf("\n[Impressão no Terminal via Fila]\n");

    int altura = alturaArvoreBPlus(arvoreExemplo->raiz);
    printf("\nAltura da Árvore B+ com REGISTRO %d = %d\n\n", REGISTROS, altura);

    imprimeArvorePorNiveis(arvoreExemplo->raiz); 
    */

    // Gera o arquivo .dot para a imagem
    char nomeArquivoDot[100];
    char nomeArquivoPng[100];

    // O nome do arquivo agora também inclui a ordem e os registros, usando REGISTROS passados
    sprintf(nomeArquivoDot, "arvore_ordem_%d_regs_%d.dot", ORDEM, REGISTROS);
    sprintf(nomeArquivoPng, "saida_ordem_%d_regs_%d.png", ORDEM, REGISTROS);

    gerarDot(arvoreExemplo, nomeArquivoDot); 

    // Chamada ao sistema para gerar o PNG
    char command_buffer[256];
    sprintf(command_buffer, "dot -Tpng %s -o %s", nomeArquivoDot, nomeArquivoPng);
    int sys_result = system(command_buffer); 
    if (sys_result == 0) {
        printf("\nImagem '%s' criada com sucesso!\n", nomeArquivoPng);
    } 
    else {
        fprintf(stderr, "ERRO: Falha ao gerar a imagem PNG. Verifique se o Graphviz está instalado e no PATH.\n");
    }

    destruirArvoreBPlus(arvoreExemplo->raiz);
    free(arvoreExemplo);


    return 0;
}