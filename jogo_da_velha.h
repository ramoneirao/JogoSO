#ifndef JOGO_DA_VELHA_H
#define JOGO_DA_VELHA_H

#include <iostream>
#include <sys/mman.h>   // Para memória compartilhada
#include <unistd.h>     // Para fork()
#include <semaphore.h>  // Para semáforos POSIX

#define N 3  // Tamanho do tabuleiro

class JogoDaVelha {
    public:
        JogoDaVelha();
        ~JogoDaVelha();
        void inicializar_tabuleiro();
        void mostrar_tabuleiro();
        bool verificar_vencedor();
        bool verificar_empate();
        void jogador1();
        void jogador2();
        void iniciar_jogo();
        
    private:
        char (*tabuleiro)[N];
        sem_t *sem_jogador1, *sem_jogador2; // Semáforos para sincronização dos jogadores
        bool *jogo_terminado;
};

#endif // JOGO_DA_VELHA_H