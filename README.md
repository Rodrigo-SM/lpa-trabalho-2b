# Sistema de Gestão de Estacionamento

**Disciplina:** Lógica de Programação e Algoritmos  
**Semestre:** 2026/2  
**Trabalho:** Trabalho do 2º Bimestre (B2)  
**Linguagem:** C (padrão C11), executado em terminal

## Integrantes

| Nome | Perfil GitHub |
|---|---|
| Rodrigo | [@Rodrigo-SM](https://github.com/Rodrigo-SM) |
| Thiago Amaral | [@tz-thiagodev](https://github.com/tz-thiagodev) |

---

## 1. Cenário e escopo

O sistema controla as vagas e os veículos de um estacionamento fictício com **4 fileiras de 5 vagas (20 vagas no total)**. O programa começa sem veículos cadastrados, recebe operações por um menu em terminal e conserva os dados somente enquanto estiver aberto (não há persistência em arquivos).

Cada vaga é identificada na tela por uma **fileira de 1 a 4** e uma **coluna de 1 a 5**. Internamente, o programa utiliza índices de 0 a 3 e de 0 a 4, convertendo os valores digitados (`vagas[linha - 1][coluna - 1]`).

Todos os identificadores e descrições usados nos testes são fictícios.

## 2. Funcionalidades e menu

```text
========== ESTACIONAMENTO ==========
1. Cadastrar veiculo
2. Consultar veiculo
3. Atualizar veiculo
4. Mover veiculo
5. Remover veiculo
6. Listar veiculos
7. Exibir mapa de vagas
8. Relatorio de ocupacao
0. Sair
```

| Opção | O que faz |
|---|---|
| 1 | Pede identificador, descrição, categoria, fileira e coluna; valida cada campo e só grava no final. |
| 2 | Busca por identificador e mostra descrição, categoria e posição. |
| 3 | Altera a descrição **ou** a categoria de um veículo existente (uma alteração por vez; para mudar as duas, repete-se a opção). O identificador e a vaga não mudam. |
| 4 | Move o veículo para outra vaga livre. Em qualquer recusa, veículo e mapa ficam como estavam. |
| 5 | Libera a vaga e remove o registro do vetor, sem deixar lacunas. |
| 6 | Lista todos os veículos ativos (ou informa que não há nenhum). |
| 7 | Exibe o mapa 4 × 5 com legenda (`1` = ocupada, `0` = livre). |
| 8 | Mostra o total de vagas, ocupadas, livres, percentual de ocupação (2 casas decimais), carros e motos. |
| 0 | Libera a memória dinâmica e encerra. |

### Regras de validação

- **Identificador:** exatamente 7 caracteres de A–Z e 0–9. Letras minúsculas são convertidas para maiúsculas, então `abc1d23` e `ABC1D23` são reconhecidos como o mesmo veículo, e a duplicidade é recusada.
- **Descrição:** não pode ser vazia nem conter apenas espaços, e tem no máximo 60 caracteres. Texto maior é recusado, não cortado.
- **Categoria:** 1 (carro) ou 2 (moto).
- **Fileira e coluna:** valores fora dos limites de 1–4 e 1–5 são recusados antes de qualquer acesso à matriz.
- **Vaga ocupada:** no cadastro ou na movimentação, a operação é recusada sem alterar os dados.
- **Lotação:** com as 20 vagas ocupadas, novos cadastros são recusados.
- **Falha no `realloc`:** o cadastro é cancelado e o ponteiro original é preservado, mantendo a coleção e o mapa íntegros.
- **Leitura de texto:** feita com `fgets` e verificação de tamanho. A função `gets` não é utilizada. Entradas longas demais são descartadas até o fim da linha.
- **Opção de menu inválida:** o programa avisa e apresenta o menu novamente.

## 3. Estruturas utilizadas

### Matriz 4 × 5 (mapa de vagas)

```c
int vagas[LINHAS][COLUNAS];   // 0 = livre, 1 = ocupada
```

É uma matriz de tamanho fixo, não alocada dinamicamente. Todas as posições começam com valor 0. Ela é percorrida com laços aninhados no mapa (`exibirMapa`) e na contagem de vagas ocupadas (`contarOcupadas`).

### Registro (`struct`)

```c
typedef struct {
    char identificador[8];   // 7 caracteres + '\0'
    char descricao[61];      // até 60 caracteres + '\0'
    int categoria;           // 1 = carro, 2 = moto
    int linha;               // 1 a 4
    int coluna;               // 1 a 5
} Veiculo;
```

### Coleção dinâmica

```c
typedef struct {
    Veiculo *veiculos;       // vetor alocado com malloc/realloc
    int quantidade;          // registros ativos
    int capacidade;          // espaço reservado
    int vagas[LINHAS][COLUNAS];
} Estacionamento;
```

- Criada com `malloc`, com **capacidade inicial de 2 registros**.
- Quando `quantidade == capacidade`, a função `aumentarCapacidade` dobra a capacidade com `realloc`, utilizando um ponteiro temporário. Se o `realloc` falhar, o ponteiro original não é perdido.
- A capacidade do vetor é independente do limite físico de 20 vagas. O cadastro é recusado ao atingir 20 veículos, mesmo que ainda exista memória disponível.
- Na remoção, os registros seguintes são deslocados uma posição para trás. A capacidade não é reduzida.
- No encerramento, `liberarVetor` executa `free` e zera o ponteiro.

### Consistência dos dados

Em qualquer momento, o número de registros ativos deve corresponder à quantidade de células com valor 1 na matriz e ao total de vagas ocupadas apresentado no relatório.

Cada veículo ocupa exatamente uma posição, e cada posição marcada como ocupada corresponde a um único registro ativo. Por isso, as operações validam os dados antes de alterar o vetor e a matriz.

## 4. Modularização

| Arquivo | Conteúdo |
|---|---|
| `src/estacionamento.h` | Constantes (`LINHAS`, `COLUNAS`, `CAPACIDADE_INICIAL`, `CARRO`, `MOTO`), estruturas (`struct`) e declarações das funções. |
| `src/main.c` | Menu, leitura da opção e chamada das operações. |
| `src/veiculos.c` | Gerenciamento do vetor dinâmico (`inicializarVetor`, `aumentarCapacidade`, `liberarVetor`) e operações sobre os registros (`cadastrarVeiculo`, `consultarVeiculo`, `atualizarVeiculo`, `removerVeiculo`, `listarVeiculos`), com funções auxiliares de leitura e validação. |
| `src/vagas.c` | Matriz de vagas e funções de apoio: `posicaoValida`, `vagaLivre`, `ocuparVaga`, `liberarVaga`, `moverVeiculo`, `exibirMapa`, `contarOcupadas`, `contarPorCategoria`, `calcularPercentual` e `gerarRelatorio`. |

O estado do programa fica em uma variável `Estacionamento`, declarada na função `main` e passada por **ponteiro** às funções, sem utilização de variáveis globais.

## 5. Compilação e execução

Na raiz do repositório, execute o comando:

```bash
gcc -std=c11 -Wall -Wextra -pedantic src/main.c src/veiculos.c src/vagas.c -o estacionamento
```

Para executar no Linux:

```bash
./estacionamento
```

No Windows com MinGW, utilize:

```bash
gcc -std=c11 -Wall -Wextra -pedantic src/main.c src/veiculos.c src/vagas.c -o estacionamento.exe
```

E execute no PowerShell:

```powershell
.\estacionamento.exe
```

A compilação foi realizada com sucesso, sem erros ou avisos, utilizando os parâmetros apresentados.

## 6. Casos de teste

Todos os testes abaixo foram executados e aprovados.

| ID | Entrada / operação | Resultado esperado | Resultado observado | Situação |
|---|---|---|---|---|
| T01 | Abrir o programa e escolher 8, 7 e 6 sem cadastrar nada. | Relatório com 0 ocupadas, 20 livres e 0,00%; mapa todo livre; mensagem de lista vazia. | Igual ao esperado. | Aprovado |
| T02 | Cadastrar `abc1d23`, "Carro azul", categoria 1, F1/C1 e consultar `abc1d23`. | Cadastro confirmado; consulta mostra `ABC1D23` em maiúsculas, categoria Carro, linha 1, coluna 1. | Igual ao esperado. | Aprovado |
| T03 | Com `ABC1D23` cadastrado, tentar cadastrar `abc1d23`. | Duplicidade recusada; quantidade de registros não muda. | Igual ao esperado. | Aprovado |
| T04a | Cadastrar outro veículo na vaga F1/C1, que já está ocupada. | Operação recusada; nenhum dado alterado. | Igual ao esperado. | Aprovado |
| T04b | Cadastrar um veículo com fileira 5. | Linha inválida; nenhum veículo cadastrado. | Igual ao esperado. | Aprovado |
| T05 | Cadastrar 3 veículos, considerando a capacidade inicial 2. | Após o terceiro cadastro, a capacidade passa de 2 para 4 e os registros são mantidos. | Capacidade 2, 2 e 4 após os cadastros; 3 registros mantidos. | Aprovado |
| T06 | Atualizar a descrição de `ABC1D23` e depois sua categoria para moto. | Dados alterados; permanece apenas 1 registro e a vaga é preservada. | Igual ao esperado. | Aprovado |
| T07a | Mover `ABC1D23` de F1/C1 para F2/C3, que está livre. | F1/C1 fica livre, F2/C3 fica ocupada e o registro é atualizado. | Igual ao esperado. | Aprovado |
| T07b | Mover `ABC1D23` para F1/C2, que está ocupada. | Movimentação recusada; mapa e registro permanecem inalterados. | Igual ao esperado. | Aprovado |
| T07c | Tentar mover para a própria posição, para coluna 9 e mover `ZZZ9999`. | O sistema informa que a posição é inválida ou que o veículo não foi localizado, sem alterar os dados. | Igual ao esperado. | Aprovado |
| T08 | Remover `XYZ9K87` e listar os veículos. | Registro removido, vaga liberada e demais veículos mantidos corretamente. | Igual ao esperado. | Aprovado |
| T09 | Cadastrar 20 veículos, ocupando todas as vagas, e tentar cadastrar o 21º. | 21ª inclusão recusada; relatório com 20 ocupadas e 100,00% de ocupação. | Igual ao esperado. | Aprovado |
| Cenário do roteiro | Executar os itens 1 a 8 da seção 7.1 do roteiro: 3 cadastros, duplicidade, tentativa de movimentação para vaga ocupada, movimentação para F2/C3, remoção e relatório. | 20 vagas, 2 ocupadas, 18 livres, 10,00%, 2 carros e 0 motos. | Igual ao esperado. | Aprovado |
| T10 | Escolher a opção 0 para encerrar. | Memória da coleção liberada e programa encerrado normalmente. | Mensagem de memória liberada e execução sem vazamentos na verificação com sanitizer. | Aprovado |

A verificação com sanitizer também foi executada e não apresentou erros.

## 7. Distribuição das responsabilidades e integração

### Responsabilidades

| Integrante | Responsabilidades | Branch |
|---|---|---|
| Rodrigo | Menu e fluxo principal (`main.c`); vetor dinâmico e controle de memória; cadastro, consulta, atualização, remoção e listagem de veículos (`veiculos.c`). | `veiculos` |
| Thiago Amaral | Mapa de vagas e funções de apoio à matriz; movimentação; exibição do mapa e relatório de ocupação (`vagas.c`). | `alteracoesVagas` |

### Integração das branches

Cada integrante desenvolveu suas funcionalidades em uma branch própria. Na branch principal (`master`), a branch `alteracoesVagas` foi integrada primeiro e, posteriormente, a branch `veiculos`.

Durante a integração surgiram conflitos reais nos arquivos `estacionamento.h` e `main.c`. Os conflitos foram resolvidos manualmente, preservando o menu principal e as declarações necessárias para as funcionalidades desenvolvidas por ambos.

Também foi necessário revisar o arquivo `vagas.c`, removendo cópias de funções que já estavam implementadas em `veiculos.c` e uma segunda função `main`, que causavam erros de definição duplicada (`multiple definition`) durante a compilação.

Os arquivos binários gerados durante os testes foram removidos do repositório e adicionados ao `.gitignore`, mantendo apenas os arquivos necessários ao projeto.

Durante o processo de integração e resolução dos conflitos, a dupla utilizou ChatGPT e Claude como ferramentas de apoio para compreender os conflitos, analisar as alterações e auxiliar na organização da versão final do código.

## 8. Decisões e limitações conhecidas

- As funções auxiliares de leitura e validação (`lerTexto`, `lerInteiro`, `validarIdentificador`, `buscarVeiculo`) são declaradas como `static` e existem em duas cópias, uma em `veiculos.c` e outra em `vagas.c`. A implementação funciona, mas essas funções poderiam ser centralizadas em um arquivo comum.
- Um identificador fora do formato esperado, como um identificador com 6 caracteres, é informado como inválido, e não como "não localizado".
- A atualização altera um campo por vez.
- O cadastro com as 20 vagas ocupadas é recusado antes de solicitar os demais campos.
- O mapa utiliza `1` e `0`, acompanhados de legenda, para representar vagas ocupadas e livres.
- Não há persistência em arquivos ou banco de dados. Os dados existem somente durante a execução do programa.

## 9. Declarações individuais de autoria e uso de IA

### Rodrigo

**(a) Funcionalidades em que atuei:**

Atuei no desenvolvimento do menu e do fluxo principal do programa, no gerenciamento do vetor dinâmico e da memória, além das funcionalidades de cadastro, consulta, atualização, remoção e listagem de veículos. Também participei da integração das branches e da resolução dos conflitos encontrados durante o merge.

**(b) Utilizei inteligência artificial generativa?**

Sim.

**(c) Ferramentas utilizadas, finalidade e sugestões incorporadas:**

Utilizei o ChatGPT como ferramenta de apoio durante o desenvolvimento. A ferramenta foi utilizada para esclarecer dúvidas sobre a linguagem C, compreender trechos do código, identificar e corrigir erros de compilação e ligação, revisar a leitura de dados com `fgets` e `scanf`, compreender o gerenciamento de memória dinâmica e auxiliar na organização dos arquivos do projeto.

Também utilizei o ChatGPT para receber orientações sobre Git, branches e resolução de conflitos durante a integração. Em conjunto com a dupla, o ChatGPT e o Claude foram utilizados como apoio para analisar e resolver os conflitos reais de merge.

Exemplos de solicitações realizadas:

- "Me ajude com este erro de compilação."
- "Pode me ajudar a entender a diferença entre essas duas implementações?"
- "Como posso corrigir a leitura de dados com `fgets`?"
- "Pode me explicar as partes mais difíceis desse código?"
- "Como resolver os conflitos que apareceram durante o merge?"

As sugestões incorporadas foram analisadas e adaptadas ao projeto. O código foi escrito e ajustado com base no entendimento das explicações, e as funcionalidades foram testadas durante o desenvolvimento.

**(d) Código externo adaptado:**

Foi utilizado código externo como referência e houve adaptação para o projeto.

**Fonte e trecho utilizado:** [PREENCHER COM O LINK OU IDENTIFICAÇÃO DA FONTE E DESCREVER QUAL PARTE DO CÓDIGO FOI ADAPTADA.]

### Thiago Amaral

**(a) Funcionalidades em que atuei:**

Atuei no desenvolvimento do mapa de vagas, nas funções de apoio à matriz, na movimentação de veículos, na exibição do mapa e no relatório de ocupação, concentrando essas funcionalidades no arquivo `vagas.c`. Também participei da integração das branches e da resolução dos conflitos encontrados durante o merge.

**(b) Utilizei inteligência artificial generativa?**

Sim.

**(c) Ferramentas utilizadas, finalidade e sugestões incorporadas:**

Foram utilizadas ferramentas de inteligência artificial generativa como apoio ao desenvolvimento e à integração do projeto.

**Ferramentas utilizadas e finalidades específicas:** [COMPLETAR COM AS FERRAMENTAS UTILIZADAS PELO THIAGO E COMO FORAM UTILIZADAS.]

**Exemplos de solicitações e sugestões incorporadas:** [COMPLETAR COM EXEMPLOS REAIS DE PROMPTS E COM AS SUGESTÕES QUE FORAM UTILIZADAS NO CÓDIGO.]

**(d) Código externo adaptado:**

[INFORMAR SE HOUVE ADAPTAÇÃO DE CÓDIGO EXTERNO PELO THIAGO. SE SIM, INDICAR A FONTE E A PARTE UTILIZADA. SE NÃO, DECLARAR QUE NÃO HOUVE.]

---

**Observação:** as ferramentas de IA foram utilizadas como apoio ao aprendizado, à revisão, à resolução de dúvidas e à integração do projeto. As implementações foram analisadas e testadas pelos integrantes, que participaram do desenvolvimento e devem compreender as funcionalidades entregues.
