#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

//matriz do mapa
#define LINHAS 5
#define COLUNAS 5
//Elementos do mapa
#define PAREDE 1
#define CHAO 0

#define BAIXO -1
#define CIMA -2

#define ITEM 2
#define TESOURO 3

#define MONSTRO 4
#define JOGADOR 5



// Definição das estruturas
typedef struct {
    char nome[50];
    int energia;
    int basedamage;
    int damage;
    int linha;
    int coluna;
    int objeto;
    int tesouro;
    int chanceCrit;
} Jogador;

typedef struct {
    char descricao[100];
    int objeto;
    int tesouro;
    int linha;
    int coluna;
} Sala;

typedef struct {
    int energia;
    int linha;
    int coluna;
    int id;
} Monstro;

// Estrutura para passar argumentos para a thread do jogador
typedef struct {
    Jogador *jogador;
    Monstro *monstros;
    Sala *localAventura;
    int numMonstros;
    int mapa[LINHAS][COLUNAS];
} ThreadArgs;

void imprimirMapa(int mapa[LINHAS][COLUNAS]){
    for (int i = 0; i < LINHAS; ++i){
        for (int j = 0; j < COLUNAS; ++j) {
            switch (mapa[i][j])
            {
                case PAREDE:
                    printf("#");
                    break;
                case CHAO:
                    printf(".");
                    break;
                case BAIXO:
                    printf("D");
                    break;
                case CIMA:
                    printf("S");
                    break;
                case TESOURO:
                    printf("$");
                case ITEM:
                    printf("I");
                    break;
                case MONSTRO:
                    printf("M");
                    break;
                case JOGADOR:
                    printf("P");
                    break;
                default:
                    break;
            }
        }

        printf("\n");
    }
}
// Cria chao e bordas do mapa
void inicializarMapa(int mapa[LINHAS][COLUNAS]) {
    // Preencher o mapa com paredes
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            // Colocar paredes nas bordas
            if (i == 0 || i == LINHAS - 1 || j == 0 || j == COLUNAS - 1) {
                mapa[i][j] = PAREDE;
            } else {
                mapa[i][j] = CHAO;
            }
        }
    }
}

// Função para verificar se uma posição é válida no mapa
int posicaoValida(int linha, int coluna) {
    return linha >= 0 && linha < LINHAS && coluna >= 0 && coluna < COLUNAS;
}

// Função para verificar se uma posição é válida e não é uma parede
int posicaoAndavel(int linha, int coluna, int mapa[LINHAS][COLUNAS]) {
    return posicaoValida(linha, coluna) && mapa[linha][coluna] != PAREDE;
}

void adicionarElementosMapa(int mapa[LINHAS][COLUNAS], int linha, int coluna, int elemento){mapa[linha][coluna] = elemento;}

// Funções para inicialização
void inicializarJogador(Jogador *jogador) {
    printf("Digite o nome do jogador: ");
    scanf("%s", jogador->nome);
    jogador->energia = 100;
    jogador -> basedamage = 20;
    jogador->chanceCrit = 10;
    jogador->linha = 1;
    jogador->coluna = 1;
    jogador->objeto = -1;
    jogador->tesouro = -1;
}

void inicializarLocalAventura(Sala *localAventura, int numSalas) {
    for (int i = 0; i < numSalas; i++) {
        localAventura[i].linha = rand() % (LINHAS - 2) + 1; // Evitar bordas
        localAventura[i].coluna = rand() % (COLUNAS - 2) + 1; // Evitar bordas

        localAventura[i].objeto = rand() % 2 == 0 ? -1 : rand() % numSalas; // 50% de chance de ter um objeto
        localAventura[i].tesouro = rand() % 2 == 0 ? -1 : 1; // 50% de chance de ter um tesouro
    }
}

void inicializarMonstro(Monstro *monstros, int numMonstros, int mapa[LINHAS][COLUNAS]) {
    for (int i = 0; i < numMonstros; i++) {
        monstros[i].energia = 50;

        int linha = 0;
        int coluna = 0;

        while (mapa[linha][coluna] != CHAO) {
            linha = rand() % (LINHAS - 2) + 1; // Evitar bordas
            coluna = rand() % (COLUNAS - 2) + 1; // Evitar bordas
        }

        monstros[i].linha = linha;
        monstros[i].coluna = coluna;
        monstros[i].id = i + 1;

        mapa[linha][coluna] = MONSTRO;
    }
}

// Funções para manipulação do jogo
void *movimentarMonstro(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    Monstro *monstros = threadArgs->monstros;
    int numMonstros = threadArgs->numMonstros;
    int mapa[LINHAS][COLUNAS];
    memcpy(mapa, threadArgs->mapa, sizeof(mapa));

    // Enquanto pelo menos um monstro estiver vivo
    while (1) {
        int monstroVivo = 0; // Flag para verificar se pelo menos um monstro está vivo

        for (int i = 0; i < numMonstros; ++i) {
            // Verificar se o monstro está vivo
            if (monstros[i].energia > 0) {
                monstroVivo = 1; // Pelo menos um monstro está vivo

                // Movimentar o monstro
                int direcao = rand() % 4;

                // Obter a próxima posição com base na direção da sala
                int novaLinha = monstros[i].linha;
                int novaColuna = monstros[i].coluna;

                switch (direcao) {
                    case 0:  // Norte
                        novaLinha--;
                        printf("Monstro %d movimentou-se para Norte \n", monstros->id);
                        break;
                    case 1:  // Sul
                        novaLinha++;
                        printf("Monstro %d movimentou-se para Sul \n", monstros->id);
                        break;
                    case 2:  // Oeste
                        novaColuna--;
                        printf("Monstro %d movimentou-se para Oeste \n", monstros->id);
                        break;
                    case 3:  // Este
                        novaColuna++;
                        printf("Monstro %d movimentou-se para Este \n", monstros->id);
                        break;
                    default:
                        continue;
                }

                // Verificar se a nova posição é válida e não é uma parede
                if (posicaoAndavel(novaLinha, novaColuna, mapa)) {
                    // Remover a posição anterior do monstro
                    mapa[monstros[i].linha][monstros[i].coluna] = CHAO;
                    // Atualizar as novas coordenadas do monstro
                    monstros[i].linha = novaLinha;
                    monstros[i].coluna = novaColuna;
                    mapa[novaLinha][novaColuna] = MONSTRO;
                }
            }
        }

        // Se nenhum monstro estiver vivo, sair do loop
        if (!monstroVivo) {
            break;
        }

        // Atualizar o mapa
        memcpy(threadArgs->mapa, mapa, sizeof(mapa));

        // Imprimir o mapa após o movimento do monstro
        imprimirMapa(mapa);

        usleep(3000000);
    }

    return NULL;
}

void descreverMonstro(Monstro monstro) {
    printf("Monstro %d | Energia: %d\n", monstro.id, monstro.energia);
}

void descreverLocalizacao(Jogador jogador, Sala *localAventura) {
    printf("Jogador %s | Energia: %d | Local: %s\n", jogador.nome, jogador.energia, localAventura[jogador.linha * COLUNAS + jogador.coluna].descricao);
}

void aceitarComando(Jogador *jogador, Sala *localAventura, int mapa[LINHAS][COLUNAS]) {
    printf("Digite o comando (ex: 'n' para norte, 's' para sul, 'e' para este, 'o' para oeste): \n");
    char comando;
    scanf(" %c", &comando);

    int novaLinha = jogador->linha;
    int novaColuna = jogador->coluna;

    switch (comando) {
        case 'n':
            novaLinha--;
            break;
        case 's':
            novaLinha++;
            break;
        case 'e':
            novaColuna++;
            break;
        case 'o':
            novaColuna--;
            break;
    }

    // Verificar se a nova posição é válida e não é uma parede
    if (posicaoAndavel(novaLinha, novaColuna, localAventura)) {
        // Remover a posição anterior do jogador
        mapa[jogador->linha][jogador->coluna] = CHAO;

        // Atualizar a nova posição do jogador
        jogador->linha = novaLinha;
        jogador->coluna = novaColuna;

        // Marcar a nova posição como ocupada
        mapa[novaLinha][novaColuna] = JOGADOR;

        // Imprimir o mapa após o movimento
        imprimirMapa(mapa);
    } else {
        printf("Movimento inválido. Tente novamente.\n");
    }
}



void lutar(Jogador *jogador, Monstro *monstro) {
    printf("Luta! %s vs Monstro %d\n", jogador->nome, monstro->id);

    int isCritical = rand() % 100 < jogador->chanceCrit;

    int damage;
    damage = isCritical ? jogador->damage * 2 : jogador->damage;  // Double damage on critical hit

    jogador->energia -= 10;
    printf("O player %s acertou o monstro por %d", jogador->nome, damage);
    monstro->energia -= damage;

    if (isCritical) {
        printf(" - Critical Hit!\n");
    } else {
        printf("\n");
    }


    descreverMonstro(*monstro);
}
void aplicarPowerUp(Jogador *jogador, int mapa[LINHAS][COLUNAS]) {
    int linha = -1, coluna = -1;

    // Encontrar a posição do jogador no mapa
    for (int i = 0; i < LINHAS; ++i) {
        for (int j = 0; j < COLUNAS; ++j) {
            if (mapa[i][j] == JOGADOR) {
                linha = i;
                coluna = j;
                break;
            }
        }
        if (linha != -1 && coluna != -1) {
            break;
        }
    }

    // Verificar se há um ‘item’ na mesma posição do jogador
    if (mapa[linha][coluna] == ITEM) {
        printf("%s encontrou um item e recebeu um power-up de critico!\n", jogador->nome);
        // Lógica para aplicar o power-up ao jogador (aumentar energia, por exemplo)
        jogador->chanceCrit += 10;

        // Remover o ‘item’ do mapa
        mapa[linha][coluna] = CHAO;
    }

    if (mapa[linha][coluna] == TESOURO) {
        printf("%s encontrou um tesouro e recebeu um power-up de dano!\n", jogador->nome);
        // Lógica para aplicar o power-up ao jogador (aumentar energia, por exemplo)
        jogador->damage = jogador ->basedamage + 40;

        // Remover o ‘item’ do mapa
        mapa[linha][coluna] = CHAO;
    }
}

void *threadFunc(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    Jogador *jogador = threadArgs->jogador;
    Sala *localAventura = threadArgs->localAventura;
    Monstro *monstros = threadArgs->monstros;
    int numMonstros = threadArgs->numMonstros;
    int (*mapa)[LINHAS][COLUNAS] = threadArgs->mapa;

    while (jogador->energia > 0) {
        descreverLocalizacao(*jogador, localAventura);
        aceitarComando(jogador, localAventura, mapa);

        aplicarPowerUp(jogador, mapa);
        int todosMonstrosMortos = 1;

        for (int i = 0; i < numMonstros; ++i) {
            // Se o monstro morrer, continue para o próximo
            if (monstros[i].energia <= 0) {
                continue;
            }

            todosMonstrosMortos = 0;  // Pelo menos um monstro está vivo

            // Mova o comando aqui para ser solicitado apenas se o jogador estiver vivo
            aceitarComando(jogador, localAventura, mapa);

            if (jogador->linha == monstros[i].linha && jogador->coluna == monstros[i].coluna) {
                lutar(jogador, &monstros[i]);
            }

            // Após a luta, verifique novamente se o jogador morreu
            if (jogador->energia <= 0) {
                break;
            }
        }

        // Se todos os monstros estiverem mortos, sair do ‘loop’
        if (todosMonstrosMortos) {
            break;
        }
    }

    return NULL;
}





int main() {

    int mapa[LINHAS][COLUNAS] = {
            {PAREDE, PAREDE, PAREDE, PAREDE, PAREDE},
            {PAREDE, CHAO, CHAO, CHAO, PAREDE},
            {PAREDE, CHAO, CHAO, CHAO, PAREDE},
            {PAREDE, CHAO, CHAO, CHAO, PAREDE},
            {PAREDE, PAREDE, PAREDE, PAREDE, PAREDE}
    };

    inicializarMapa(mapa);

    adicionarElementosMapa(mapa, 1, 1, JOGADOR);
    adicionarElementosMapa(mapa, 2, 2, TESOURO);
    adicionarElementosMapa(mapa, 3, 2, ITEM);
    adicionarElementosMapa(mapa, 2, 3, MONSTRO);
    adicionarElementosMapa(mapa, 3, 1, MONSTRO);
    imprimirMapa(mapa);

    pthread_t playerThread, monsterThread;
    const int numMonstros = 2;
    const int numSalas = 10;
    Jogador jogador;
    Monstro monstros[numMonstros];
    Sala localAventura[numSalas];

    inicializarJogador(&jogador);
    inicializarLocalAventura(localAventura, numSalas);
    inicializarMonstro(monstros, numMonstros, mapa); // Passar o mapa como argumento

    // Estrutura para armazenar argumentos da thread do jogador
    ThreadArgs threadMovimentoJogador = {.jogador = &jogador, .localAventura = localAventura, .monstros = monstros, .numMonstros = numMonstros, .mapa = (int) mapa};
    ThreadArgs threadMovimentoMonstro = {.localAventura = localAventura, .monstros = monstros, .numMonstros = numMonstros, .mapa = (int) mapa};

    pthread_create(&playerThread, NULL, threadFunc, (void *)&threadMovimentoJogador);
    pthread_create(&monsterThread, NULL, movimentarMonstro, (void *)&threadMovimentoMonstro);

    // Esperar o thread do jogador acabar
    pthread_join(playerThread, NULL);

    // Aguardar o término do thread dos monstros
    pthread_join(monsterThread, NULL);

    // Apresentar resultado
    if (jogador.energia <= 0) {
        printf("Você perdeu! Sua vida chegou a zero.\n");
    } else {
        printf("Parabéns, derrotou os monstros e venceu o jogo!\n");
    }

    return 0;
}