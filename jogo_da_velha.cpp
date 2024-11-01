#include "jogo_da_velha.h"

// Construtor
JogoDaVelha::JogoDaVelha() {
    inicializar_tabuleiro();
    sem_init(&sem_jogador1, 0, 1);
    sem_init(&sem_jogador2, 0, 0);
    jogo_terminado = false; 
}

void JogoDaVelha::inicializar_tabuleiro() {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            tabuleiro[i][j] = ' ';
}

void JogoDaVelha::mostrar_tabuleiro() {
    std::lock_guard<std::mutex> lock(mtx);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            std::cout << tabuleiro[i][j];
            if (j < N - 1) std::cout << " | ";
        }
        std::cout << std::endl;
        if (i < N - 1) std::cout << "--+---+--" << std::endl;
    }
    std::cout << std::endl;
}

bool JogoDaVelha::verificar_vencedor() {
    for (int i = 0; i < N; ++i) {
        if (tabuleiro[i][0] != ' ' && tabuleiro[i][0] == tabuleiro[i][1] && tabuleiro[i][1] == tabuleiro[i][2])
            return true;
        if (tabuleiro[0][i] != ' ' && tabuleiro[0][i] == tabuleiro[1][i] && tabuleiro[1][i] == tabuleiro[2][i])
            return true;
    }
    if (tabuleiro[0][0] != ' ' && tabuleiro[0][0] == tabuleiro[1][1] && tabuleiro[1][1] == tabuleiro[2][2])
        return true;
    if (tabuleiro[0][2] != ' ' && tabuleiro[0][2] == tabuleiro[1][1] && tabuleiro[1][1] == tabuleiro[2][0])
        return true;
    return false;
}

bool JogoDaVelha::verificar_empate() {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (tabuleiro[i][j] == ' ')
                return false;
    return true;
}

void JogoDaVelha::jogador1() {
    int linha, coluna;
    while (true) {
        sem_wait(&sem_jogador1);

        if (jogo_terminado) {
            sem_post(&sem_jogador2);  // Libera a thread do jogador 2 
            break;
        }

        if (verificar_vencedor() || verificar_empate()){
            jogo_terminado = true; // Marca que o jogo terminou
            break;
        } 

        mtx.lock();
        std::cout << "Jogador 1 (X), insira a linha e a coluna: ";
        std::cin >> linha >> coluna;
        while (tabuleiro[linha][coluna] != ' ' || linha < 0 || linha >= N || coluna < 0 || coluna >= N) {
            std::cout << "Posição inválida! Tente novamente: ";
            std::cin >> linha >> coluna;
        }
        tabuleiro[linha][coluna] = 'X';
        mtx.unlock();

        mostrar_tabuleiro();

        if (verificar_vencedor()) {
            std::cout << "Jogador 1 (X) venceu!" << std::endl;
            jogo_terminado = true;
            sem_post(&sem_jogador2);  // Libera a thread do jogador 2
            break;
        }

        if (verificar_empate()) {
            std::cout << "Empate!" << std::endl;
            jogo_terminado = true;
            sem_post(&sem_jogador2); // Libera a thread do jogador 2
            break;
        }

        sem_post(&sem_jogador2);
    }
}

void JogoDaVelha::jogador2() {
    int linha, coluna;
    while (true) {
        sem_wait(&sem_jogador2);

        if (jogo_terminado) {
            sem_post(&sem_jogador1); // Libera a thread do jogador 1
            break;
        }   

        if (verificar_vencedor() || verificar_empate()) {
            jogo_terminado = true; // Marca que o jogo terminou
            break;
        }

        mtx.lock();
        std::cout << "Jogador 2 (O), insira a linha e a coluna: ";
        std::cin >> linha >> coluna;
        while (tabuleiro[linha][coluna] != ' ' || linha < 0 || linha >= N || coluna < 0 || coluna >= N) {
            std::cout << "Posição inválida! Tente novamente: ";
            std::cin >> linha >> coluna;
        }
        tabuleiro[linha][coluna] = 'O';
        mtx.unlock();

        mostrar_tabuleiro();

        if (verificar_vencedor()) {
            std::cout << "Jogador 2 (O) venceu!" << std::endl;
            jogo_terminado = true;
            sem_post(&sem_jogador1); // Libera a thread do jogador
            break;
        }

        if (verificar_empate()) {
            std::cout << "Empate!" << std::endl;
            jogo_terminado = true;
            sem_post(&sem_jogador1); // Libera a thread do jogador 1
            break;
        }

        sem_post(&sem_jogador1);
    }
}

void JogoDaVelha::iniciar_jogo() {
    std::thread t1(&JogoDaVelha::jogador1, this);
    std::thread t2(&JogoDaVelha::jogador2, this);

    t1.join();
    t2.join();

    sem_destroy(&sem_jogador1);
    sem_destroy(&sem_jogador2);
}
