import random

def extrair_chaves(nome_arquivo, qtd):
    with open(nome_arquivo, "r", encoding="utf-8") as f:
        linhas = f.readlines()
    chaves = [int(linha.split(",")[0]) for linha in linhas]
    return random.sample(chaves, qtd)

def salvar_buscas(buscas, nome="buscas.txt"):
    with open(nome, "w", encoding="utf-8") as f:
        for chave in buscas:
            f.write(f"{chave}\n")

if __name__ == "__main__":
    buscas = extrair_chaves("registros_carros.txt", 100)
    salvar_buscas(buscas)
    print("buscas.txt gerado com 100 renavams existentes.")
