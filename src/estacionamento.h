#ifndef ESTACIONAMENTO_H
#define ESTACIONAMENTO_H

// Quantidade de linhas e colunas do estacionamento
#define LINHAS 4
#define COLUNAS 5

// Capacidade inicial do vetor dinamico
#define CAPACIDADE_INICIAL 2

// Representa um veiculo cadastrado
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

// Gerenciamento do estacionamento
void inicializarEstacionamento(Estacionamento *est);
void liberarEstacionamento(Estacionamento *est);

// Funcoes relacionadas aos veiculos
void cadastrarVeiculo(Estacionamento *est);
void consultarVeiculo(Estacionamento *est);
void atualizarVeiculo(Estacionamento *est);
void removerVeiculo(Estacionamento *est);
void listarVeiculos(Estacionamento *est);

// Funcoes relacionadas as vagas
void moverVeiculo(Estacionamento *est);
void exibirMapa(Estacionamento *est);
void gerarRelatorio(Estacionamento *est);

// Funcoes de apoio ao mapa (linha e coluna como o usuario digita: 1 a 4 e 1 a 5)
int posicaoValida(int linha, int coluna);
int vagaLivre(const Estacionamento *est, int linha, int coluna);
int ocuparVaga(Estacionamento *est, int linha, int coluna);
int liberarVaga(Estacionamento *est, int linha, int coluna);

#endif