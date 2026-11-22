
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
// ATUALIZAR VEICULO
// ==========================

void atualizarVeiculo(Estacionamento *est) {
    char identificador[8];
    char novaDescricao[61];
    int indice;
    int novaCategoria;
    int opcao;

    if (est == NULL || est->veiculos == NULL) {
        printf("\nEstacionamento nao inicializado.\n");
        return;
    }

    printf("\n===== ATUALIZAR VEICULO =====\n");
    printf("Digite o identificador: ");

    if (!lerTexto(identificador, sizeof(identificador)) ||
        !validarIdentificador(identificador)) {
        printf("Identificador invalido.\n");
        return;
    }

    indice = buscarVeiculo(est, identificador);

    if (indice == -1) {
        printf("\nVeiculo nao encontrado.\n");
        return;
    }

    printf("\nVeiculo encontrado: %s\n",
           est->veiculos[indice].descricao);

    printf("\nO que deseja atualizar?\n");
    printf("1. Descricao\n");
    printf("2. Categoria\n");
    printf("Opcao: ");

    if (!lerInteiro(&opcao)) {
        printf("Opcao invalida.\n");
        return;
    }

    switch (opcao) {
        case 1:
            printf("Nova descricao: ");

            if (!lerTexto(novaDescricao, sizeof(novaDescricao)) ||
                !validarDescricao(novaDescricao)) {
                printf("Descricao invalida.\n");
                return;
            }

            strcpy(est->veiculos[indice].descricao, novaDescricao);

            printf("\nDescricao atualizada com sucesso!\n");
            break;

        case 2:
            printf("Nova categoria (1 - Carro / 2 - Moto): ");

            if (!lerInteiro(&novaCategoria) ||
                (novaCategoria != CARRO && novaCategoria != MOTO)) {
                printf("Categoria invalida.\n");
                return;
            }

            est->veiculos[indice].categoria = novaCategoria;

            printf("\nCategoria atualizada com sucesso!\n");
            break;

        default:
            printf("Opcao inexistente.\n");
            break;
    }
}

// ==========================
// REMOVER VEICULO
// ==========================

void removerVeiculo(Estacionamento *est) {
    char identificador[8];
    int indice;
    int i;
    int linha;
    int coluna;

    if (est == NULL || est->veiculos == NULL) {
        printf("\nEstacionamento nao inicializado.\n");
        return;
    }

    if (est->quantidade == 0) {
        printf("\nNao existem veiculos cadastrados.\n");
        return;
    }

    printf("\n===== REMOVER VEICULO =====\n");
    printf("Digite o identificador: ");

    if (!lerTexto(identificador, sizeof(identificador)) ||
        !validarIdentificador(identificador)) {
        printf("Identificador invalido.\n");
        return;
    }

    indice = buscarVeiculo(est, identificador);

    if (indice == -1) {
        printf("\nVeiculo nao encontrado.\n");
        return;
    }

    linha = est->veiculos[indice].linha - 1;
    coluna = est->veiculos[indice].coluna - 1;

    // Libera a vaga ocupada pelo veiculo.
    est->vagas[linha][coluna] = 0;

    // Desloca os registros seguintes para preencher o espaco.
    for (i = indice; i < est->quantidade - 1; i++) {
        est->veiculos[i] = est->veiculos[i + 1];
    }

    // Atualiza a quantidade de registros.
    est->quantidade--;

    printf("\nVeiculo removido com sucesso!\n");
}

