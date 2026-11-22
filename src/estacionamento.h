#ifndef ESTACIONAMENTO_H
#define ESTACIONAMENTO_H

// Dimensoes do estacionamento
#define LINHAS 4
#define COLUNAS 5

// Capacidade inicial do vetor dinamico
#define CAPACIDADE_INICIAL 2

// Categorias de veiculo
#define CARRO 1
#define MOTO 2

// Representa um veiculo cadastrado
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

// Funcoes relacionadas aos veiculos
// Funcoes de veiculos
void cadastrarVeiculo(Estacionamento *est);
void consultarVeiculo(Estacionamento *est);
void atualizarVeiculo(Estacionamento *est);
void removerVeiculo(Estacionamento *est);
void listarVeiculos(Estacionamento *est);

// Funcoes relacionadas as vagas
// Funcoes de vagas
void moverVeiculo(Estacionamento *est);
void exibirMapa(const Estacionamento *est);
void gerarRelatorio(const Estacionamento *est);

// Funcoes de apoio ao mapa (linha e coluna como o usuario digita: 1 a 4 e 1 a 5)
int posicaoValida(int linha, int coluna);
int vagaLivre(const Estacionamento *est, int linha, int coluna);
int ocuparVaga(Estacionamento *est, int linha, int coluna);
int liberarVaga(Estacionamento *est, int linha, int coluna);

// Funcoes de apoio ao relatorio
int contarOcupadas(const Estacionamento *est);
int contarPorCategoria(const Estacionamento *est, int categoria);
double calcularPercentual(int ocupadas);

#endif