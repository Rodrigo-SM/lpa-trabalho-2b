
#ifndef ESTACIONAMENTO_H
#define ESTACIONAMENTO_H

// Quantidade de linhas e colunas do estacionamento
#define LINHAS 4
#define COLUNAS 5

// Capacidade inicial do vetor dinâmico
#define CAPACIDADE_INICIAL 2

// Representa um veículo cadastrado
typedef struct {
    char identificador[8];
    char descricao[61];
    int categoria;
    int linha;
    int coluna;
} Veiculo;

// Representa o estacionamento e seus dados
typedef struct {
    Veiculo *veiculos;
    int quantidade;
    int capacidade;
    int vagas[LINHAS][COLUNAS];
} Estacionamento;

// Funções relacionadas aos veículos
void cadastrarVeiculo(Estacionamento *est);
void consultarVeiculo(Estacionamento *est);
void atualizarVeiculo(Estacionamento *est);
void removerVeiculo(Estacionamento *est);
void listarVeiculos(Estacionamento *est);

// Funções relacionadas às vagas
void moverVeiculo(Estacionamento *est);
void exibirMapa(Estacionamento *est);
void gerarRelatorio(Estacionamento *est);

#endif

