import random

def gerar_linhas_invalidas():
    linhas = [
        "abc123,Gol,2020,Branco",          # renavam não numérico
        "123456789,Gol",                   # incompleto
        "123456789,Gol,ano,Branco",        # ano não numérico
        "123456789,Gol,2020",              # faltando campo cor
        "123456789,Gol,2020,Branco,Extra", # campo a mais
        ",Gol,2020,Branco",                # renavam ausente
        "123456789,,2020,Branco",          # modelo ausente
        "123456789,Gol,,Branco",           # ano ausente
        "123456789,Gol,2020,",             # cor ausente
        ""                                 # linha vazia
    ]
    return linhas

if __name__ == "__main__":
    linhas = gerar_linhas_invalidas()
    with open("registros_invalidos.txt", "w", encoding="utf-8") as f:
        for linha in linhas:
            f.write(linha + "\n")
    print("registros_invalidos.txt gerado com dados malformados para teste.")
