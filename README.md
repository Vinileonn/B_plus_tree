## ✒️ Autores

* Elen Pasqualli Gesser - elengesser@ufpr.br

* Laisa Marcelino Santos Rodrigues - laisamarcelino@ufpr.br

* Vinicius Leon Paula - vinicius.leon@ufpr.br

# Análise de Desempenho de Árvore B+: O Impacto do Dimensionamento de Nós

Este projeto implementa uma **Árvore B+** em linguagem C e realiza uma análise de desempenho focada no impacto do dimensionamento dos nós (definido pela `ORDEM` da árvore) na eficiência das operações de busca e inserção.

A Árvore B+ é uma estrutura de dados auto-balanceada otimizada para sistemas que gerenciam grandes volumes de dados, especialmente em memória secundária, minimizando o número de acessos a disco.

---

## 🚀 Funcionalidades

* **Inserção de Registros**: Adiciona novos registros à árvore, realizando divisões (splits) de nós folha e internos conforme necessário para manter as propriedades da Árvore B+.
* **Busca de Registros**: Permite a recuperação eficiente de registros com base em sua chave única.
* **Visualização Gráfica**: Gera arquivos `.dot` que são convertidos em imagens PNG usando o Graphviz, permitindo visualizar a estrutura da árvore em diferentes estágios.
* **Teste de Desempenho**: Avalia o tempo de execução das operações de inserção e busca para diferentes volumes de dados e valores de `ORDEM`, fornecendo métricas de tempo total e médio.

---

## 🛠️ Como Usar

### Pré-requisitos

Certifique-se de ter as seguintes ferramentas instaladas em seu sistema:

* **GCC** (GNU Compiler Collection): Para compilar o código C.
* **Make**: Para automatizar o processo de compilação.
* **Graphviz**: Para gerar as imagens da árvore a partir dos arquivos `.dot`.
* **Python 3**: Para executar os scripts de geração de dados de teste (se utilizados).

### Construção do Projeto

O projeto utiliza um `Makefile` para facilitar a compilação. Você pode definir a `ORDEM` da árvore (capacidade de chaves por nó) e o número de `REGISTROS` para a visualização diretamente no comando `make`.

Para compilar, navegue até o diretório raiz do projeto e execute:

```bash
make ORDEM=5 REGISTROS=100
./ArvoreBPlus
```

O programa executará automaticamente os testes de desempenho para diferentes volumes de registros (100, 1.000, 100.000) e gerará um arquivo .dot e uma imagem .png da árvore para a configuração de ORDEM e REGISTROS definida no Makefile (ou os valores padrão, se não especificados).

### Exemplo de Saída no Terminal

```bash
--- Iniciando Testes de Desempenho da Árvore B+ ---
Configuração (ORDEM definida via Makefile/compilação)
-----------------------------------------------------------------------------------------------------------
Realizando testes para 100 registros:
ORDEM:   3 | Registros Inseridos: 100    | Tempo Total Inserção: 0.000123 segundos | Tempo Médio por Inserção: 0.0000012300 segundos
ORDEM:   3 | Registros Inseridos: 100    | Tempo Total Busca (100 chaves): 0.000006 segundos | Tempo Médio por Busca: 0.0000000600 segundos
-----------------------------------------------------------------------------------------------------------
... (outros resultados para diferentes tamanhos de teste) ...
-----------------------------------------------------------------------------------------------------------
--- Impressão e Visualização (ORDEM=3, 20 registros) ---

Arquivo de visualização 'arvore_ordem_3_regs_20.dot' gerado com sucesso (usando HTML-like).
Imagem 'saida_ordem_3_regs_20.png' criada com sucesso!
```

Após a execução, um arquivo .dot (ex: arvore_ordem_3_regs_20.dot) e uma imagem .png (ex: saida_ordem_3_regs_20.png) serão gerados no diretório do projeto. Você pode abrir o arquivo .png para visualizar a estrutura da árvore.

## 📂 Estrutura do Projeto

* **BPlusTree.h**: Contém as definições de estruturas (registro_t, nodo_t, BPlusTree_t) e protótipos de funções para manipulação da Árvore B+.

* **BPlusTree.c**: Implementação das funções da Árvore B+, incluindo criação, destruição, inserção, busca e as funções auxiliares de split e impressão/geração DOT.

* **main.c**: Responsável por carregar os dados, executar os testes de desempenho de inserção e busca, e gerar os arquivos de visualização.

* **fila.h**: Contém protótipos para uma estrutura de fila, usada para impressão em níveis ou depuração.

* **fila.c**: Implementação das funções da fila.

* **Makefile**: Define as regras de compilação do projeto e permite configurar ORDEM e REGISTROS.

* **registros_carros.txt**: Arquivo de entrada com os dados dos carros (gerado externamente).

* **buscas.txt**: Arquivo de entrada com as chaves para teste de busca (gerado externamente).

* **arvore_ordem_X_regs_Y.dot**: Arquivo gerado pelo programa para visualização com Graphviz.

* **saida_ordem_X_regs_Y.png**: Imagem da árvore gerada a partir do arquivo .dot.