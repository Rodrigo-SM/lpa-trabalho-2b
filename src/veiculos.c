
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "estacionamento.h"

// ==================================================
// FUNCOES AUXILIARES
// ==================================================

// Le uma linha de texto com seguranca.
// Retorna 1 se a leitura funcionar e 0 se falhar.
static int lerTexto(char texto[], int tamanho) {

    if (fgets(texto, tamanho, stdin) == NULL) {
        return 0;
    }

    // Verifica se o usuario digitou mais caracteres
    // do que o espaco disponivel no vetor.
    if (strchr(texto, '\n') == NULL) {

        int caractere;

        while ((caractere = getchar()) != '\n' &&
               caractere != EOF) {
        }

        return 0;
    }

    // Remove o \n do final da string.
    texto[strcspn(texto, "\n")] = '\0';

    return 1;
}


// Le um numero inteiro sem aceitar letras ou
// caracteres adicionais.
static int lerInteiro(int *numero) {

    char entrada[50];
    char *fim;

    if (!lerTexto(entrada, sizeof(entrada))) {
        return 0;
    }

    long valor = strtol(entrada, &fim, 10);

    // Ignora espacos no final da entrada.
    while (isspace((unsigned char)*fim)) {
        fim++;
    }

    // Verifica se a entrada possui somente um numero.
    if (fim == entrada || *fim != '\0') {
        return 0;
    }

    *numero = (int)valor;

    return 1;
}


// Verifica se o identificador possui exatamente
// 7 caracteres entre A-Z e 0-9.
static int validarIdentificador(char identificador[]) {

    if (strlen(identificador) != 7) {
        return 0;
    }

    for (int i = 0; i < 7; i++) {

        char c = identificador[i];

        if (!((c >= 'A' && c <= 'Z') ||
              (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9'))) {

            return 0;
        }

        // Converte letras minusculas em maiusculas.
        identificador[i] = (char)toupper((unsigned char)c);
    }

    return 1;
}


// Verifica se a descricao nao esta vazia
// ou preenchida somente com espacos.
static int validarDescricao(char descricao[]) {

    if (strlen(descricao) == 0) {
        return 0;
    }

    for (int i = 0; descricao[i] != '\0'; i++) {

        if (!isspace((unsigned char)descricao[i])) {
            return 1;
        }
    }

    return 0;
}


// Procura um identificador no vetor.
// Retorna a posicao encontrada ou -1.
static int buscarVeiculo(Estacionamento *est,
                         const char identificador[]) {

    for (int i = 0; i < est->quantidade; i++) {

        if (strcmp(est->veiculos[i].identificador,
                   identificador) == 0) {

            return i;
        }
    }

    return -1;
}


// ==================================================
// GERENCIAMENTO DO VETOR DINAMICO
// ==================================================

// Inicializa o vetor dinamico.
int inicializarVetor(Estacionamento *est) {

    if (est == NULL) {
        return 0;
    }

    est->veiculos = NULL;
    est->quantidade = 0;
    est->capacidade = 0;

    // Inicializa todas as vagas como livres.
    for (int i = 0; i < LINHAS; i++) {

        for (int j = 0; j < COLUNAS; j++) {
            est->vagas[i][j] = 0;
        }
    }

    Veiculo *vetor = malloc(
        CAPACIDADE_INICIAL * sizeof(Veiculo)
    );

    if (vetor == NULL) {
        return 0;
    }

    est->veiculos = vetor;
    est->capacidade = CAPACIDADE_INICIAL;

    return 1;
}


// Aumenta a capacidade do vetor.
int aumentarCapacidade(Estacionamento *est) {

    if (est == NULL || est->veiculos == NULL) {
        return 0;
    }

    int novaCapacidade = est->capacidade * 2;

    Veiculo *novoVetor = realloc(
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


// Libera a memoria do vetor.
void liberarVetor(Estacionamento *est) {

    if (est == NULL) {
        return;
    }

    free(est->veiculos);

    est->veiculos = NULL;
    est->quantidade = 0;
    est->capacidade = 0;
}


// ==================================================
// CADASTRO DE VEICULOS
// ==================================================

void cadastrarVeiculo(Estacionamento *est) {

    if (est == NULL || est->veiculos == NULL) {
        printf("\nEstacionamento nao inicializado!\n");
        return;
    }

    // Verifica o limite fisico de vagas.
    if (est->quantidade >= LINHAS * COLUNAS) {
        printf("\nEstacionamento lotado!\n");
        return;
    }

    Veiculo novoVeiculo;

    // ----------------------------------------------
    // IDENTIFICADOR
    // ----------------------------------------------

    printf("\nDigite o identificador (7 caracteres): ");

    if (!lerTexto(novoVeiculo.identificador,
                  sizeof(novoVeiculo.identificador))) {

        printf("Entrada invalida!\n");
        return;
    }

    if (!validarIdentificador(novoVeiculo.identificador)) {
        printf("Identificador invalido! Use 7 letras ou numeros.\n");
        return;
    }

    // Verifica se ja existe um veiculo com esse ID.
    if (buscarVeiculo(est, novoVeiculo.identificador) != -1) {
        printf("Ja existe um veiculo com esse identificador!\n");
        return;
    }

    // ----------------------------------------------
    // DESCRICAO
    // ----------------------------------------------

    printf("Digite a descricao do veiculo (maximo 60 caracteres): ");

    if (!lerTexto(novoVeiculo.descricao,
                  sizeof(novoVeiculo.descricao))) {

        printf("Descricao muito longa ou entrada invalida!\n");
        return;
    }

    if (!validarDescricao(novoVeiculo.descricao)) {
        printf("A descricao nao pode ficar vazia!\n");
        return;
    }

    // ----------------------------------------------
    // CATEGORIA
    // ----------------------------------------------

    printf("Categoria (1 - Carro | 2 - Moto): ");

    if (!lerInteiro(&novoVeiculo.categoria)) {
        printf("Categoria invalida!\n");
        return;
    }

    if (novoVeiculo.categoria != CARRO &&
        novoVeiculo.categoria != MOTO) {

        printf("Escolha 1 para carro ou 2 para moto!\n");
        return;
    }

    // ----------------------------------------------
    // LINHA
    // ----------------------------------------------

    printf("Digite a linha da vaga (1 a %d): ", LINHAS);

    if (!lerInteiro(&novoVeiculo.linha)) {
        printf("Linha invalida!\n");
        return;
    }

    if (novoVeiculo.linha < 1 ||
        novoVeiculo.linha > LINHAS) {

        printf("Linha fora do estacionamento!\n");
        return;
    }

    // ----------------------------------------------
    // COLUNA
    // ----------------------------------------------

    printf("Digite a coluna da vaga (1 a %d): ", COLUNAS);

    if (!lerInteiro(&novoVeiculo.coluna)) {
        printf("Coluna invalida!\n");
        return;
    }

    if (novoVeiculo.coluna < 1 ||
        novoVeiculo.coluna > COLUNAS) {

        printf("Coluna fora do estacionamento!\n");
        return;
    }

    // ----------------------------------------------
    // VERIFICA SE A VAGA ESTA LIVRE
    // ----------------------------------------------

    int linha = novoVeiculo.linha - 1;
    int coluna = novoVeiculo.coluna - 1;

    if (est->vagas[linha][coluna] == 1) {
        printf("Essa vaga ja esta ocupada!\n");
        return;
    }

    // ----------------------------------------------
    // VERIFICA SE PRECISA AUMENTAR O VETOR
    // ----------------------------------------------

    if (est->quantidade == est->capacidade) {

        if (!aumentarCapacidade(est)) {
            printf("Erro ao aumentar a capacidade do vetor!\n");
            return;
        }
    }

    // ----------------------------------------------
    // CADASTRA O VEICULO
    // ----------------------------------------------

    est->veiculos[est->quantidade] = novoVeiculo;

    est->vagas[linha][coluna] = 1;

    est->quantidade++;

    printf("\nVeiculo cadastrado com sucesso!\n");
    printf("Identificador: %s\n", novoVeiculo.identificador);
    printf("Vaga: Linha %d, Coluna %d\n",
           novoVeiculo.linha,
           novoVeiculo.coluna);
}

