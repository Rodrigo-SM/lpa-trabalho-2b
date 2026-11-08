#include <stdio.h>
#include <stdlib.h>
#include "estacionamento.h"

// Função para inicializar o estacionamento e alocar a capacidade inicial (2 registros)
void inicializarEstacionamento(Estacionamento *est) {
    int i, j; // Variáveis declaradas no início do bloco
    
    est->quantidade = 0;
    est->capacidade = CAPACIDADE_INICIAL;
    est->veiculos = (Veiculo *) malloc(est->capacidade * sizeof(Veiculo));
    
    if (est->veiculos == NULL) {
        printf("Erro crítico: Falha ao alocar memória inicial!\n");
        exit(1);
    }
    
    // Inicializa todas as vagas do mapa como livres (0)
    for (i = 0; i < LINHAS; i++) {
        for (j = 0; j < COLUNAS; j++) {
            est->vagas[i][j] = 0;
        }
    }
}

// Função para liberar a memória dinâmica ao encerrar
void liberarEstacionamento(Estacionamento *est) {
    if (est->veiculos != NULL) {
        free(est->veiculos);
        est->veiculos = NULL;
    }
    est->quantidade = 0;
    est->capacidade = 0;
}

// Funções temporárias para evitar erro de compilação
void cadastrarVeiculo(Estacionamento *est) { 
    printf("\n[Em construcao] Cadastrar veiculo\n"); 
}
void consultarVeiculo(Estacionamento *est) { 
    printf("\n[Em construcao] Consultar veiculo\n"); 
}
void atualizarVeiculo(Estacionamento *est) { 
    printf("\n[Em construcao] Atualizar veiculo\n"); 
}
void moverVeiculo(Estacionamento *est) { 
    printf("\n[Em construcao] Mover veiculo\n"); 
}
void listarVeiculos(Estacionamento *est) { 
    printf("\n[Em construcao] Listar veiculos\n"); 
}
void removerVeiculo(Estacionamento *est) { 
    printf("\n[Em construcao] Remover veiculo\n"); 
}
void exibirMapa(Estacionamento *est) { 
    printf("\n[Em construcao] Exibir mapa\n"); 
}
void gerarRelatorio(Estacionamento *est) { 
    printf("\n[Em construcao] Gerar relatorio\n"); 
}

//

int main() {
    Estacionamento est;
    inicializarEstacionamento(&est);

    int opcao;

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
            continue;
        }

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
                listarVeiculos(&est);
                break;
            case 6:
                removerVeiculo(&est);
                break;
            case 7:
                exibirMapa(&est);
                break;
            case 8:
                gerarRelatorio(&est);
                break;
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
