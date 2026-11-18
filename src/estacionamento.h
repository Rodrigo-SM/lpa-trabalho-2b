
#ifndef ESTACIONAMENTO_H
#define ESTACIONAMENTO_H

// Dimensoes do estacionamento
#define LINHAS 4
#define COLUNAS 5

// Capacidade inicial do vetor
#define CAPACIDADE_INICIAL 2

// Categorias de veiculos
#define CARRO 1
#define MOTO 2

// Estrutura de um veiculo
typedef struct {
    char identificador[8];
    char descricao[61];
    int categoria;
    int linha;
    int coluna;
} Veiculo;

// Estrutura principal do estacionamento
typedef struct {
    Veiculo *veiculos;
    int quantidade;
    int capacidade;
    int vagas[LINHAS][COLUNAS];
} Estacionamento;

// Gerenciamento do vetor dinamico
int inicializarVetor(Estacionamento *est);
int aumentarCapacidade(Estacionamento *est);
void liberarVetor(Estacionamento *est);

// Funcoes de veiculos
void cadastrarVeiculo(Estacionamento *est);
void consultarVeiculo(Estacionamento *est);
void atualizarVeiculo(Estacionamento *est);
void removerVeiculo(Estacionamento *est);
void listarVeiculos(Estacionamento *est);

// Funcoes de vagas
void moverVeiculo(Estacionamento *est);
void exibirMapa(Estacionamento *est);
void gerarRelatorio(Estacionamento *est);

#endif

