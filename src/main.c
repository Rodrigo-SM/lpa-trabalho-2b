
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "estacionamento.h"

int main(void) {
    Estacionamento est;
    char entrada[20];
    char *fim;
    long valor;
    int opcao = -1;

    // Inicializa o vetor dinamico e a matriz de vagas.
    if (!inicializarVetor(&est)) {
        printf("Erro ao inicializar o estacionamento.\n");
        return 1;
    }

    do {
        printf("\n========== ESTACIONAMENTO ==========\n");
        printf("1. Cadastrar veiculo\n");
        printf("2. Consultar veiculo\n");
        printf("3. Atualizar veiculo\n");
        printf("4. Mover veiculo\n");
        printf("5. Remover veiculo\n");
        printf("6. Listar veiculos\n");
        printf("7. Exibir mapa de vagas\n");
        printf("8. Relatorio de ocupacao\n");
        printf("0. Sair\n");
        printf("====================================\n");
        printf("Escolha uma opcao: ");

        // Le a opcao como texto para evitar problemas com fgets.
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            printf("\nErro na leitura. Encerrando o programa.\n");
            break;
        }

        errno = 0;
        valor = strtol(entrada, &fim, 10);

        // Ignora espacos e caracteres de quebra de linha.
        while (*fim == ' ' || *fim == '\t' ||
               *fim == '\n' || *fim == '\r') {
            fim++;
        }

        // Verifica se a entrada e um numero inteiro valido.
        if (fim == entrada || *fim != '\0' ||
            errno == ERANGE ||
            valor < INT_MIN || valor > INT_MAX) {
            printf("\nOpcao invalida! Digite um numero.\n");
            continue;
        }

        opcao = (int) valor;

        switch (opcao) {
            case 1:
                cadastrarVeiculo(&est);
                break;

            case 2:
                consultarVeiculo(&est);
                break;

            case 3:
                atualizarVeiculo(&est);
                break;

            case 4:
                moverVeiculo(&est);
                break;

            case 5:
                removerVeiculo(&est);
                break;

            case 6:
                listarVeiculos(&est);
                break;

            case 7:
                exibirMapa(&est);
                break;

            case 8:
                gerarRelatorio(&est);
                break;

            case 0:
                printf("\nEncerrando o estacionamento...\n");
                break;

            default:
                printf("\nOpcao inexistente! Tente novamente.\n");
                break;
        }

    } while (opcao != 0);

    // Libera a memoria alocada para o vetor.
    liberarVetor(&est);

    printf("Memoria liberada. Programa encerrado.\n");

    return 0;
}

