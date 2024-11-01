#include "jogo_da_velha.h"

// Construtor
JogoDaVelha::JogoDaVelha() {
    // Alocando memória compartilhada para o tabuleiro
    tabuleiro = (char (*)[N])mmap(NULL, N * N * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    jogo_terminado = (bool *)mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    // Inicializa o tabuleiro e as variáveis de estado
    inicializar_tabuleiro();
    *jogo_terminado = false;
    
    // Inicializa os semáforos POSIX em memória compartilhada
    sem_jogador1 = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_jogador2 = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(sem_jogador1, 1, 1); // Jogador 1 começa
    sem_init(sem_jogador2, 1, 0); // Jogador 2 espera
}

JogoDaVelha::~JogoDaVelha() {
    // Libera recursos alocados
    munmap(tabuleiro, N * N * sizeof(char));
    munmap(jogo_terminado, sizeof(bool));
    munmap(sem_jogador1, sizeof(sem_t));
    munmap(sem_jogador2, sizeof(sem_t));
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
        sem_wait(sem_jogador1);

        if (*jogo_terminado) {
            sem_post(sem_jogador2);  // Libera o processo do jogador 2
            break;
        }

        if (verificar_vencedor() || verificar_empate()) {
            *jogo_terminado = true; // Marca que o jogo terminou
            break;
        }

        std::cout << "Jogador 1 (X), insira a linha e a coluna: ";
        std::cin >> linha >> coluna;
        while (tabuleiro[linha][coluna] != ' ' || linha < 0 || linha >= N || coluna < 0 || coluna >= N) {
            std::cout << "Posição inválida! Tente novamente: ";
            std::cin >> linha >> coluna;
        }
        tabuleiro[linha][coluna] = 'X';

        mostrar_tabuleiro();

        if (verificar_vencedor()) {
            std::cout << "Jogador 1 (X) venceu!" << std::endl;
            *jogo_terminado = true;
            sem_post(sem_jogador2);  // Libera o processo do jogador 2
            break;
        }

        if (verificar_empate()) {
            std::cout << "Empate!" << std::endl;
            *jogo_terminado = true;
            sem_post(sem_jogador2);  // Libera o processo do jogador 2
            break;
        }

        sem_post(sem_jogador2);
    }
}

void JogoDaVelha::jogador2() {
    int linha, coluna;
    while (true) {
        sem_wait(sem_jogador2);

        if (*jogo_terminado) {
            sem_post(sem_jogador1);  // Libera o processo do jogador 1
            break;
        }

        if (verificar_vencedor() || verificar_empate()) {
            *jogo_terminado = true; // Marca que o jogo terminou
            break;
        }

        std::cout << "Jogador 2 (O), insira a linha e a coluna: ";
        std::cin >> linha >> coluna;
        while (tabuleiro[linha][coluna] != ' ' || linha < 0 || linha >= N || coluna < 0 || coluna >= N) {
            std::cout << "Posição inválida! Tente novamente: ";
            std::cin >> linha >> coluna;
        }
        tabuleiro[linha][coluna] = 'O';

        mostrar_tabuleiro();

        if (verificar_vencedor()) {
            std::cout << "Jogador 2 (O) venceu!" << std::endl;
            *jogo_terminado = true;
            sem_post(sem_jogador1);  // Libera o processo do jogador 1
            break;
        }

        if (verificar_empate()) {
            std::cout << "Empate!" << std::endl;
            *jogo_terminado = true;
            sem_post(sem_jogador1);  // Libera o processo do jogador 1
            break;
        }

        sem_post(sem_jogador1);
    }
}

void JogoDaVelha::iniciar_jogo() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // Processo filho: jogador 2
        jogador2();
    } else {
        // Processo pai: jogador 1
        jogador1();
        wait(NULL); // Espera o processo filho terminar
    }

    // Destrói os semáforos ao final do jogo
    sem_destroy(sem_jogador1);
    sem_destroy(sem_jogador2);
}