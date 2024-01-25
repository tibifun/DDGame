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
    int local;
    int objeto;
    int tesouro;
    int chanceCrit;
} Jogador;

typedef struct {
    int norte, sul, oeste, este, cima, baixo;
    char descricao[100];
    int objeto;
    int tesouro;
} Sala;

typedef struct {
    int energia;
    int local;
    int id;
} Monstro;

// Estrutura para passar argumentos para a thread do jogador
typedef struct {
    Jogador *jogador;
    Monstro *monstros;
    Sala *localAventura;
    int numMonstros;
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

void adicionarElementosMapa(int mapa[LINHAS][COLUNAS], int linha, int coluna, int elemento){mapa[linha][coluna] = elemento;}

// Funções para inicialização
void inicializarJogador(Jogador *jogador) {
    printf("Digite o nome do jogador: ");
    scanf("%s", jogador->nome);
    jogador->energia = 100;
    jogador -> basedamage = 20;
    jogador->chanceCrit = 10;
    jogador->local = 0;
    jogador->objeto = -1;
    jogador->tesouro = -1;
}

void inicializarLocalAventura(Sala *localAventura, int numSalas) {
    for (int i = 0; i < numSalas; i++) {
        localAventura[i].norte = rand() % numSalas;
        localAventura[i].sul = rand() % numSalas;
        localAventura[i].oeste = rand() % numSalas;
        localAventura[i].este = rand() % numSalas;
        localAventura[i].cima = rand() % numSalas;
        localAventura[i].baixo = rand() % numSalas;

        localAventura[i].objeto = rand() % 2 == 0 ? -1 : rand() % numSalas; // 50% de chance de ter um objeto
        localAventura[i].tesouro = rand() % 2 == 0 ? -1 : 1; // 50% de chance de ter um tesouro
    }
}

void inicializarMonstro(Monstro *monstros, int numMonstros) {
    for (int i = 0; i < numMonstros; i++) {
        monstros[i].energia = 50;
        monstros[i].local = rand() % numMonstros; // Defina um local inicial para o monstro
        monstros[i].id = i + 1;
    }
}

// Funções para manipulação do jogo
void *movimentarMonstro(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    Sala *localAventura = threadArgs->localAventura;
    Monstro *monstros = threadArgs->monstros;
    int numMonstros = threadArgs->numMonstros;

    // Enquanto pelo menos um monstro estiver vivo
    while (1) {
        int monstroVivo = 0; // Flag para verificar se pelo menos um monstro está vivo

        for (int i = 0; i < numMonstros; ++i) {
            // Verificar se o monstro está vivo
            if (monstros[i].energia > 0) {
                monstroVivo = 1; // Pelo menos um monstro está vivo

                // Movimentar o monstro
                int direcao = rand() % 4;
                switch (direcao) {
                    case 0:
                        monstros[i].local = localAventura[monstros[i].local].norte;
                        printf("Monstro %d movimentou-se para norte\n", monstros[i].id);
                        break;

                    case 1:
                        monstros[i].local = localAventura[monstros[i].local].sul;
                        printf("Monstro %d movimentou-se para sul\n", monstros[i].id);
                        break;

                    case 2:
                        monstros[i].local = localAventura[monstros[i].local].oeste;
                        printf("Monstro %d movimentou-se para oeste\n", monstros[i].id);
                        break;

                    case 3:
                        monstros[i].local = localAventura[monstros[i].local].este;
                        printf("Monstro %d movimentou-se para este\n", monstros[i].id);
                        break;
                }
            }
        }

        // Se nenhum monstro estiver vivo, sair do loop
        if (!monstroVivo) {
            break;
        }

        usleep(3000000);
    }

    return NULL;
}

void descreverMonstro(Monstro monstro) {
    printf("Monstro %d | Energia: %d\n", monstro.id, monstro.energia);
}

void descreverLocalizacao(Jogador jogador, Sala *localAventura) {
    printf("Jogador %s | Energia: %d | Local: %s\n", jogador.nome, jogador.energia, localAventura[jogador.local].descricao);
}

void aceitarComando(Jogador *jogador, Sala *localAventura) {
    printf("Digite o comando (ex: 'n' para norte, 's' para sul, 'e' para este, 'o' para oeste): \n");
    char comando;
    scanf(" %c", &comando);

    switch (comando) {
        case 'n': jogador->local = localAventura[jogador->local].norte;
            printf("%s movimentou-se para norte\n", jogador->nome);
            break;
        case 's': jogador->local = localAventura[jogador->local].sul;
            printf("%s  movimentou-se para sul\n", jogador->nome );
            break;
        case 'e': jogador->local = localAventura[jogador->local].este;
            printf("%s  movimentou-se para este\n", jogador->nome);
            break;
        case 'o': jogador->local = localAventura[jogador->local].oeste;
            printf("%s  movimentou-se para oeste\n", jogador->nome);
            break;
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

    while (jogador->energia > 0) {
        descreverLocalizacao(*jogador, localAventura);
        aceitarComando(jogador, localAventura);

        aplicarPowerUp(jogador,localAventura);
        int todosMonstrosMortos = 1;

        for (int i = 0; i < numMonstros; ++i) {
            // Se o monstro morrer, continue para o próximo
            if (monstros[i].energia <= 0) {
                continue;
            }

            todosMonstrosMortos = 0;  // Pelo menos um monstro está vivo

            aceitarComando(jogador, localAventura);  // Mova o comando aqui para ser solicitado apenas se o jogador estiver vivo

            if (jogador->local == monstros[i].local) {
                lutar(jogador, &monstros[i]);
            }

            // Após a luta, verifique novamente se o jogador morreu
            if (jogador->energia <= 0) {
                break;
            }
        }

        // Se todos os monstros estiverem mortos, sair do loop
        if (todosMonstrosMortos) {
            break;
        }
    }

    return NULL;
}




int main() {

    int mapa[LINHAS][COLUNAS]={
            {PAREDE,PAREDE,PAREDE,PAREDE,PAREDE},
            {PAREDE, CHAO, CHAO, CHAO,PAREDE},
            {PAREDE, CHAO, CHAO, CHAO, PAREDE },
            {PAREDE, CHAO, CHAO, BAIXO, PAREDE},
            {PAREDE, PAREDE, PAREDE, PAREDE, PAREDE}

    };

    inicializarMapa(mapa);

    adicionarElementosMapa(mapa, 1,1,JOGADOR);
    adicionarElementosMapa(mapa, 2, 2, TESOURO);
    adicionarElementosMapa(mapa, 3, 2, ITEM);
    adicionarElementosMapa(mapa, 2,3, MONSTRO);
    adicionarElementosMapa(mapa,3,1,MONSTRO);
    imprimirMapa(mapa);


    pthread_t playerThread, monsterThread;
    const int numMonstros = 2;
    const int numSalas = 10;
    Jogador jogador;
    Monstro monstros[numMonstros];
    Sala localAventura[numSalas];

    inicializarJogador(&jogador);
    inicializarLocalAventura(localAventura, numSalas);
    inicializarMonstro(monstros, numMonstros);

    // Estrutura para armazenar argumentos da thread do jogador
    ThreadArgs threadMovimentoJogador = {.jogador = &jogador, .localAventura = localAventura, .monstros = monstros, .numMonstros = numMonstros};
    ThreadArgs threadMovimentoMonstro = {.localAventura = localAventura, .monstros = monstros, .numMonstros = numMonstros};

    pthread_create(&playerThread, NULL, threadFunc, (void *) &threadMovimentoJogador);
    pthread_create(&monsterThread, NULL, movimentarMonstro, (void *) &threadMovimentoMonstro);

    // Esperar o thread do jogador acabar
    pthread_join(playerThread, NULL);

    // Aguardar o término do thread dos monstros
    pthread_join(monsterThread, NULL);

    // Apresentar resultado final
    if (jogador.energia <= 0) {
        printf("Você perdeu! Sua vida chegou a zero.\n");
    } else {
        printf("Parabéns, derrotou os monstros e venceu o jogo!\n");
    }

    return 0;
}