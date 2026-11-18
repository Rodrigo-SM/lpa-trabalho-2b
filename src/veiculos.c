
#include <stdlib.h>
#include "estacionamento.h"

// Inicializa o vetor dinamico
int inicializarVetor(Estacionamento *est) {

    if (est == NULL) {
        return 0;
    }

    est->veiculos = NULL;
    est->quantidade = 0;
    est->capacidade = 0;

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

// Aumenta a capacidade do vetor
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

// Libera a memoria do vetor
void liberarVetor(Estacionamento *est) {

    if (est == NULL) {
        return;
    }

    free(est->veiculos);

    est->veiculos = NULL;
    est->quantidade = 0;
    est->capacidade = 0;
}

