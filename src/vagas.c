#include <stdio.h>
#include <stdlib.h>
#include "estacionamento.h"

// Inicializa o estacionamento e aloca a capacidade inicial (2 registros)
void inicializarEstacionamento(Estacionamento *est) {

    int i, j;

    est->quantidade = 0;
    est->capacidade = CAPACIDADE_INICIAL;
    est->veiculos = (Veiculo *) malloc(est->capacidade * sizeof(Veiculo));

    if (est->veiculos == NULL) {
        printf("Erro critico: Falha ao alocar memoria inicial!\n");
        exit(1);
    }

    // Inicializa todas as vagas do mapa como livres (0)
    for (i = 0; i < LINHAS; i++) {
        for (j = 0; j < COLUNAS; j++) {
            est->vagas[i][j] = 0;
        }
    }
}

// Libera a memoria dinamica ao encerrar
void liberarEstacionamento(Estacionamento *est) {
    if (est->veiculos != NULL) {
        free(est->veiculos);
        est->veiculos = NULL;
    }
    est->quantidade = 0;
    est->capacidade = 0;
}

// ---------- Funcoes de apoio ao mapa de vagas ----------

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

// ---------- Funcoes temporarias para evitar erro de compilacao ----------
void cadastrarVeiculo(Estacionamento *est) {
    (void) est;
    printf("\n[Em construcao] Cadastrar veiculo\n");
}
void consultarVeiculo(Estacionamento *est) {
    (void) est;
    printf("\n[Em construcao] Consultar veiculo\n");
}
void atualizarVeiculo(Estacionamento *est) {
    (void) est;
    printf("\n[Em construcao] Atualizar veiculo\n");
}
void moverVeiculo(Estacionamento *est) {
    (void) est;
    printf("\n[Em construcao] Mover veiculo\n");
}
void listarVeiculos(Estacionamento *est) {
    (void) est;
    printf("\n[Em construcao] Listar veiculos\n");
}
void removerVeiculo(Estacionamento *est) {
    (void) est;
    printf("\n[Em construcao] Remover veiculo\n");
}
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
// ---------- Funcoes de apoio ao relatorio ----------

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

// Conta os veiculos de uma categoria (1 = carro, 2 = moto) percorrendo o VETOR.
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
    printf("Carros         : %d\n", contarPorCategoria(est, 1));
    printf("Motos          : %d\n", contarPorCategoria(est, 2));
}

int main() {
    Estacionamento est;
    int opcao = -1; // valor que nao e opcao valida, para o 'continue' repetir o menu

    inicializarEstacionamento(&est);


    do {
        printf("\n========================================\n");
        printf("   SISTEMA DE GESTAO DE ESTACIONAMENTO  \n");
        printf("========================================\n");
        printf("1. Cadastrar veiculo\n");
        printf("2. Consultar veiculo\n");
        printf("3. Atualizar veiculo\n");
        printf("4. Mover veiculo\n");
        printf("5. Listar veiculos\n");
        printf("6. Remover veiculo\n");
        printf("7. Exibir mapa de vagas\n");
        printf("8. Exibir relatorio de ocupacao\n");
        printf("0. Encerrar\n");
        printf("----------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("\n[Erro] Entrada invalida. Digite um numero.\n");
            while (getchar() != '\n'); // Limpa o buffer do teclado
            opcao = -1;
            continue;
        }

        switch (opcao) {
            case 1: cadastrarVeiculo(&est); break;
            case 2: consultarVeiculo(&est); break;
            case 3: atualizarVeiculo(&est); break;
            case 4: moverVeiculo(&est);     break;
            case 5: listarVeiculos(&est);   break;
            case 6: removerVeiculo(&est);   break;
            case 7: exibirMapa(&est);       break;
            case 8: gerarRelatorio(&est);   break;
            case 0:
                liberarEstacionamento(&est);
                printf("\nEncerrando o programa e liberando memoria...\n");
                break;
            default:
                printf("\n[Aviso] Opcao inexistente! Escolha entre 0 e 8.\n");
        }

    } while (opcao != 0);

    return 0;
}