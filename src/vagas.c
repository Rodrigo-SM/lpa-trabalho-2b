#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "estacionamento.h"

// ==========================
// FUNCOES AUXILIARES
// (usadas por moverVeiculo; sao static, entao cada .c tem a sua copia)
// ==========================

static int lerTexto(char texto[], int tamanho) {
    if (fgets(texto, tamanho, stdin) == NULL) {
        return 0;
    }

    size_t comprimento = strlen(texto);

    if (comprimento > 0 && texto[comprimento - 1] == '\n') {
        texto[comprimento - 1] = '\0';
        return 1;
    }

    int caractere = getchar();

    if (caractere == '\n' || caractere == EOF) {
        return 1;
    }

    while (caractere != '\n' && caractere != EOF) {
        caractere = getchar();
    }

    return 0;
}

static int lerInteiro(int *numero) {
    char texto[32];
    char *fim;
    long valor;

    if (!lerTexto(texto, sizeof(texto))) {
        return 0;
    }

    errno = 0;
    valor = strtol(texto, &fim, 10);

    while (isspace((unsigned char)*fim)) {
        fim++;
    }

    if (texto == fim || *fim != '\0' ||
        errno == ERANGE || valor < INT_MIN || valor > INT_MAX) {
        return 0;
    }

    *numero = (int)valor;
    return 1;
}

static int validarIdentificador(char identificador[]) {
    int i;

    if (strlen(identificador) != 7) {
        return 0;
    }

    for (i = 0; identificador[i] != '\0'; i++) {
        if (!isalnum((unsigned char)identificador[i])) {
            return 0;
        }

        identificador[i] = (char)toupper(
            (unsigned char)identificador[i]
        );
    }

    return 1;
}

static int buscarVeiculo(
    Estacionamento *est,
    const char identificador[]
) {
    int i;

    for (i = 0; i < est->quantidade; i++) {
        if (strcmp(
            est->veiculos[i].identificador,
            identificador
        ) == 0) {
            return i;
        }
    }

    return -1;
}

// ==========================
// APOIO AO MAPA DE VAGAS
// ==========================

// Retorna 1 se a posicao (1 a LINHAS, 1 a COLUNAS) esta dentro do estacionamento
int posicaoValida(int linha, int coluna) {
    return (linha >= 1 && linha <= LINHAS &&
            coluna >= 1 && coluna <= COLUNAS);
}

// Retorna 1 se a posicao e valida E a vaga esta livre; 0 caso contrario.
// A validacao vem antes do acesso a matriz para nao ler fora dos limites.
int vagaLivre(const Estacionamento *est, int linha, int coluna) {
    if (!posicaoValida(linha, coluna)) {
        return 0;
    }
    return est->vagas[linha - 1][coluna - 1] == 0;
}

// Marca a vaga como ocupada. Retorna 1 se ocupou, 0 se recusou (matriz intacta)
int ocuparVaga(Estacionamento *est, int linha, int coluna) {
    if (!vagaLivre(est, linha, coluna)) {
        return 0;
    }
    est->vagas[linha - 1][coluna - 1] = 1;
    return 1;
}

// Marca a vaga como livre. Retorna 1 se liberou, 0 se recusou (matriz intacta).
// Recusa tambem se a vaga ja estava livre, pois isso indica inconsistencia.
int liberarVaga(Estacionamento *est, int linha, int coluna) {
    if (!posicaoValida(linha, coluna)) {
        return 0;
    }
    if (est->vagas[linha - 1][coluna - 1] == 0) {
        return 0;
    }
    est->vagas[linha - 1][coluna - 1] = 0;
    return 1;
}

// ==========================
// MOVER VEICULO
// ==========================

// Move um veiculo para outra vaga livre. Em qualquer recusa, veiculo e mapa
// ficam exatamente como estavam.
void moverVeiculo(Estacionamento *est) {
    char identificador[8];
    int indice, novaLinha, novaColuna;
    Veiculo *v;

    if (est == NULL || est->veiculos == NULL) {
        printf("\nEstacionamento nao inicializado.\n");
        return;
    }

    printf("\n===== MOVER VEICULO =====\n");

    if (est->quantidade == 0) {
        printf("Nenhum veiculo cadastrado.\n");
        return;
    }

    printf("Identificador do veiculo: ");
    if (!lerTexto(identificador, sizeof(identificador)) ||
        !validarIdentificador(identificador)) {
        printf("Identificador invalido. Use 7 caracteres alfanumericos.\n");
        return;
    }

    indice = buscarVeiculo(est, identificador);
    if (indice == -1) {
        printf("Veiculo nao localizado.\n");
        return;
    }
    v = &est->veiculos[indice];

    printf("Nova linha (1 a %d): ", LINHAS);
    if (!lerInteiro(&novaLinha)) {
        printf("Entrada invalida. Movimentacao cancelada.\n");
        return;
    }

    printf("Nova coluna (1 a %d): ", COLUNAS);
    if (!lerInteiro(&novaColuna)) {
        printf("Entrada invalida. Movimentacao cancelada.\n");
        return;
    }

    // 1. destino dentro do estacionamento (antes de tocar na matriz)
    if (!posicaoValida(novaLinha, novaColuna)) {
        printf("Posicao fora do estacionamento. Movimentacao cancelada.\n");
        return;
    }

    // 2. destino igual a posicao atual: so informa
    if (novaLinha == v->linha && novaColuna == v->coluna) {
        printf("O veiculo %s ja esta na vaga L%d/C%d. Nada foi alterado.\n",
               v->identificador, v->linha, v->coluna);
        return;
    }

    // 3. ocupa o destino; se falhar, o destino ja estava ocupado
    if (!ocuparVaga(est, novaLinha, novaColuna)) {
        printf("Destino L%d/C%d ocupado. Movimentacao recusada.\n",
               novaLinha, novaColuna);
        return;
    }

    // 4. libera a origem; se falhar, desfaz o passo 3 para nao deixar o mapa inconsistente
    if (!liberarVaga(est, v->linha, v->coluna)) {
        liberarVaga(est, novaLinha, novaColuna);
        printf("Erro de consistencia no mapa. Movimentacao cancelada.\n");
        return;
    }

    // 5. so agora atualiza o registro
    v->linha = novaLinha;
    v->coluna = novaColuna;
    printf("Veiculo %s movido para L%d/C%d.\n",
           v->identificador, v->linha, v->coluna);
}

// ==========================
// EXIBIR MAPA
// ==========================

void exibirMapa(const Estacionamento *est) {
    int i, j;

    printf("\n===== MAPA DE VAGAS =====\n");

    // Cabecalho com as colunas (rotulos comecam em 1 na tela)
    printf("   ");
    for (j = 0; j < COLUNAS; j++) {
        printf("C%d ", j + 1);
    }
    printf("\n");

    // Uma linha por fileira
    for (i = 0; i < LINHAS; i++) {
        printf("F%d ", i + 1);
        for (j = 0; j < COLUNAS; j++) {
            if (est->vagas[i][j] == 1) {
                printf("1  ");
            } else {
                printf("0  ");
            }
        }
        printf("\n"); // quebra a linha so depois de percorrer todas as colunas
    }

    printf("1 = ocupada; 0 = livre\n");
}

// ==========================
// RELATORIO DE OCUPACAO
// ==========================

// Conta as vagas ocupadas percorrendo a MATRIZ (estado real do mapa)
int contarOcupadas(const Estacionamento *est) {
    int i, j;
    int total = 0;

    for (i = 0; i < LINHAS; i++) {
        for (j = 0; j < COLUNAS; j++) {
            if (est->vagas[i][j] == 1) {
                total++;
            }
        }
    }
    return total;
}

// Conta os veiculos de uma categoria percorrendo o VETOR.
// Vai ate 'quantidade' e nao ate 'capacidade': depois dela ha espaco reservado
// que ainda nao tem veiculo (lixo de memoria).
int contarPorCategoria(const Estacionamento *est, int categoria) {
    int i;
    int total = 0;

    for (i = 0; i < est->quantidade; i++) {
        if (est->veiculos[i].categoria == categoria) {
            total++;
        }
    }
    return total;
}

// Percentual de ocupacao: (ocupadas / total de vagas) * 100.
// O 100.0 forca a conta em ponto flutuante (2 / 20 entre inteiros daria 0).
double calcularPercentual(int ocupadas) {
    return (ocupadas * 100.0) / (LINHAS * COLUNAS);
}

// Exibe o relatorio de ocupacao. So imprime; os calculos vem das funcoes acima.
void gerarRelatorio(const Estacionamento *est) {
    int total = LINHAS * COLUNAS;
    int ocupadas = contarOcupadas(est);
    int livres = total - ocupadas;

    printf("\n===== RELATORIO DE OCUPACAO =====\n");
    printf("Total de vagas : %d\n", total);
    printf("Vagas ocupadas : %d\n", ocupadas);
    printf("Vagas livres   : %d\n", livres);
    printf("Ocupacao       : %.2f%%\n", calcularPercentual(ocupadas));
    printf("Carros         : %d\n", contarPorCategoria(est, CARRO));
    printf("Motos          : %d\n", contarPorCategoria(est, MOTO));
}