# Nome do executável
EXEC = ArvoreBPlus

# Compilador
CC = gcc

# --- Parâmetros Configuráveis ---
# Define a ordem padrão se não for especificada
ORDEM ?= 3
# Define o número de registros para o exemplo se não for especificado
REGISTROS ?= 20

# Flags de compilação
# Adicionamos -DREGISTROS=$(REGISTROS) para passar o valor para o C
CFLAGS = -Wall -Wextra -g -DORDEM=$(ORDEM) -DREGISTROS=$(REGISTROS)

# Arquivos-fonte
SRCS = main.c BPlusTree.c fila.c

# Regra de compilação principal
all:
	$(CC) $(CFLAGS) -o $(EXEC) $(SRCS)

# Regra para limpar os arquivos gerados
clean:
	rm -f $(EXEC) *.dot *.png

.PHONY: all clean