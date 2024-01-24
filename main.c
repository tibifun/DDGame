#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// Definição das estruturas
typedef struct {
    char nome[50];
    int energia;
    int local;
    int objeto;
    int tesouro;
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
    Monstro *monstro;
    Sala *localAventura;
    int numMonstros;
} ThreadArgs;


// Funções para inicialização
void inicializarJogador(Jogador *jogador) {
    printf("Digite o nome do jogador: ");
    scanf("%s", jogador->nome);
    jogador->energia = 100;
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
    Monstro *monstros = threadArgs->monstro;
    int numMonstros = threadArgs->numMonstros;

    // Enquanto pelo menos um monstro estiver vivo
    while (1) {
        int monstroVivo = 1; // Flag para verificar se pelo menos um monstro está vivo

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

    //a lógica de luta aqui, por exemplo, subtrair energia dos dois participantes

    jogador->energia -= 10;
    monstro->energia -=10;
    descreverMonstro(*monstro);
}

void *threadFunc(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    Jogador *jogador = threadArgs->jogador;
    Sala *localAventura = threadArgs->localAventura;
    Monstro *monstro = threadArgs->monstro;
    int numMonstros = threadArgs->numMonstros;
    while (jogador->energia > 0) {

            for (int i = 0; i < numMonstros; ++i) {
                //se o monstro morrer sai do thread para terminar
                if (monstro[i].energia <= 0) {
                pthread_exit(NULL);
                }
            }
        descreverLocalizacao(*jogador, localAventura);
        aceitarComando(jogador, localAventura);

        if (jogador->local == monstro->local) {
            lutar(jogador, monstro);
        }
    }
    return NULL;
}

int main() {
    pthread_t playerThread, monsterThread;
    const int numMonstros = 3;
    const int numSalas = 10;
    Jogador jogador;
    Monstro monstro;
    Sala localAventura[numSalas];

    inicializarJogador(&jogador);
    inicializarLocalAventura(localAventura, numSalas);
    inicializarMonstro(&monstro, numMonstros);

    // Estrutura para armazenar argumentos da thread do jogador
    ThreadArgs threadMovimentoJogador = { .jogador = &jogador, .localAventura = localAventura, .monstro = &monstro };
    ThreadArgs threadMovimentoMonstro = { .localAventura = localAventura, .monstro = &monstro, .numMonstros = numMonstros };

        pthread_create(&playerThread, NULL, threadFunc, (void*)&threadMovimentoJogador);

        pthread_create(&monsterThread, NULL, movimentarMonstro, (void*) &threadMovimentoMonstro);


    //Esperar o thread acabar
    pthread_join(playerThread, NULL);
    pthread_join(monsterThread, NULL);
    // Apresentar resultado final

    if (jogador.energia <= 0) {printf("Você perdeu! Sua vida chegou a zero.\n");}

    else {printf("Parabéns, derrotou o monstro e venceu o jogo!\n");}


    return 0;

}