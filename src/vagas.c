#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "estacionamento.h"

// ==========================
// FUNCOES AUXILIARES
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

static int validarDescricao(char descricao[]) {
    int i;

    for (i = 0; descricao[i] != '\0'; i++) {
        if (!isspace((unsigned char)descricao[i])) {
            return 1;
        }
    }

    return 0;
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
// GERENCIAMENTO DO VETOR
// ==========================

int inicializarVetor(Estacionamento *est) {
    int i, j;

    if (est == NULL) {
        return 0;
    }

    est->veiculos = NULL;
    est->quantidade = 0;
    est->capacidade = 0;

    for (i = 0; i < LINHAS; i++) {
        for (j = 0; j < COLUNAS; j++) {
            est->vagas[i][j] = 0;
        }
    }

    est->veiculos = malloc(
        CAPACIDADE_INICIAL * sizeof(Veiculo)
    );

    if (est->veiculos == NULL) {
        return 0;
    }

    est->capacidade = CAPACIDADE_INICIAL;

    return 1;
}

int aumentarCapacidade(Estacionamento *est) {
    int novaCapacidade;
    Veiculo *novoVetor;

    if (est == NULL || est->veiculos == NULL) {
        return 0;
    }

    novaCapacidade = est->capacidade * 2;

    novoVetor = realloc(
        est->veiculos,
        novaCapacidade * sizeof(Veiculo)
    );

    if (novoVetor == NULL) {
        return 0;
    }

    est->veiculos = novoVetor;
    est->capacidade = novaCapacidade;

    return 1;
}

void liberarVetor(Estacionamento *est) {
    if (est == NULL) {
        return;
    }

    free(est->veiculos);

    est->veiculos = NULL;
    est->quantidade = 0;
    est->capacidade = 0;
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
// CADASTRAR VEICULO
// ==========================

void cadastrarVeiculo(Estacionamento *est) {
    Veiculo novo;
    int linha, coluna;

    if (est == NULL || est->veiculos == NULL) {
        printf("\nEstacionamento nao inicializado.\n");
        return;
    }

    if (est->quantidade >= LINHAS * COLUNAS) {
        printf("\nEstacionamento lotado.\n");
        return;
    }

    printf("\n===== CADASTRAR VEICULO =====\n");

    printf("Identificador (7 caracteres): ");

    if (!lerTexto(novo.identificador, sizeof(novo.identificador)) ||
        !validarIdentificador(novo.identificador)) {
        printf("Identificador invalido.\n");
        return;
    }

    if (buscarVeiculo(est, novo.identificador) != -1) {
        printf("Ja existe um veiculo com esse identificador.\n");
        return;
    }

    printf("Descricao: ");

    if (!lerTexto(novo.descricao, sizeof(novo.descricao)) ||
        !validarDescricao(novo.descricao)) {
        printf("Descricao invalida.\n");
        return;
    }

    printf("Categoria (1 - Carro / 2 - Moto): ");

    if (!lerInteiro(&novo.categoria) ||
        (novo.categoria != CARRO && novo.categoria != MOTO)) {
        printf("Categoria invalida.\n");
        return;
    }

    printf("Linha (1 a %d): ", LINHAS);

    if (!lerInteiro(&linha) || linha < 1 || linha > LINHAS) {
        printf("Linha invalida.\n");
        return;
    }

    printf("Coluna (1 a %d): ", COLUNAS);

    if (!lerInteiro(&coluna) || coluna < 1 || coluna > COLUNAS) {
        printf("Coluna invalida.\n");
        return;
    }

    if (est->vagas[linha - 1][coluna - 1] == 1) {
        printf("Essa vaga ja esta ocupada.\n");
        return;
    }

    if (est->quantidade == est->capacidade) {
        if (!aumentarCapacidade(est)) {
            printf("Erro ao aumentar a capacidade do vetor.\n");
            return;
        }
    }

    novo.linha = linha;
    novo.coluna = coluna;

    est->veiculos[est->quantidade] = novo;

    est->vagas[linha - 1][coluna - 1] = 1;

    est->quantidade++;

    printf("\nVeiculo cadastrado com sucesso!\n");
}

// ==========================
// CONSULTAR VEICULO
// ==========================

void consultarVeiculo(Estacionamento *est) {
    char identificador[8];
    int indice;

    if (est == NULL || est->veiculos == NULL) {
        printf("\nEstacionamento nao inicializado.\n");
        return;
    }

    printf("\n===== CONSULTAR VEICULO =====\n");
    printf("Digite o identificador: ");

    if (!lerTexto(identificador, sizeof(identificador)) ||
        !validarIdentificador(identificador)) {
        printf("Identificador invalido. Use 7 caracteres alfanumericos.\n");
        return;
    }

    indice = buscarVeiculo(est, identificador);

    if (indice == -1) {
        printf("\nVeiculo nao encontrado.\n");
        return;
    }

    printf("\n===== VEICULO ENCONTRADO =====\n");
    printf("Identificador: %s\n",
           est->veiculos[indice].identificador);

    printf("Descricao: %s\n",
           est->veiculos[indice].descricao);

    if (est->veiculos[indice].categoria == CARRO) {
        printf("Categoria: Carro\n");
    } else {
        printf("Categoria: Moto\n");
    }

    printf("Linha: %d\n", est->veiculos[indice].linha);
    printf("Coluna: %d\n", est->veiculos[indice].coluna);
}

// ==========================
// LISTAR VEICULOS
// ==========================

void listarVeiculos(Estacionamento *est) {
    int i;

    if (est == NULL || est->veiculos == NULL) {
        printf("\nEstacionamento nao inicializado.\n");
        return;
    }

    printf("\n===== LISTA DE VEICULOS =====\n");

    if (est->quantidade == 0) {
        printf("Nenhum veiculo cadastrado.\n");
        return;
    }

    for (i = 0; i < est->quantidade; i++) {
        printf("\n-----------------------------\n");
        printf("Veiculo %d\n", i + 1);

        printf("Identificador: %s\n",
               est->veiculos[i].identificador);

        printf("Descricao: %s\n",
               est->veiculos[i].descricao);

        if (est->veiculos[i].categoria == CARRO) {
            printf("Categoria: Carro\n");
        } else {
            printf("Categoria: Moto\n");
        }

        printf("Linha: %d\n", est->veiculos[i].linha);
        printf("Coluna: %d\n", est->veiculos[i].coluna);
    }

    printf("\nTotal de veiculos cadastrados: %d\n",
           est->quantidade);
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
// FUNCOES TEMPORARIAS (parceiro)
// ==========================

void atualizarVeiculo(Estacionamento *est) {
    (void) est;
    printf("\n[Em construcao] Atualizar veiculo\n");
}

void removerVeiculo(Estacionamento *est) {
    (void) est;
    printf("\n[Em construcao] Remover veiculo\n");
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

// ==========================
// MAIN
// ==========================

int main(void) {
    Estacionamento est;
    int opcao = -1; // valor que nao e opcao valida, para o 'continue' repetir o menu
    int c;

    if (!inicializarVetor(&est)) {
        printf("Erro critico: falha ao alocar memoria inicial!\n");
        return 1;
    }

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
            while ((c = getchar()) != '\n' && c != EOF); // limpa o buffer
            opcao = (c == EOF) ? 0 : -1; // se a entrada acabou, encerra
            continue;
        }

        // Descarta o Enter que sobrou, para o proximo fgets nao ler linha vazia
        while ((c = getchar()) != '\n' && c != EOF);

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
                printf("\nEncerrando o programa e liberando memoria...\n");
                break;
            default:
                printf("\n[Aviso] Opcao inexistente! Escolha entre 0 e 8.\n");
        }

    } while (opcao != 0);

    liberarVetor(&est); // libera uma unica vez, depois do laco

    return 0;
}