#include <iostream>
#include <string>
#include "../include/engine/engine.h"
#include "../include/engine/texture.h"
#include "../include/engine/font.h"
#include "../include/engine/text.h"
#include "../include/sprite.h"
#include "../include/jogador.h"
#include "../include/mundo.h"
#include "../include/inimigo.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 568
#define MAX_ENEMIES 5 // 251

// Engine
Engine *engine;

// Variáveis do jogo
Mundo *mundo;
Jogador *jogador;
Inimigo *inimigo[MAX_ENEMIES];
pthread_t enemy_thread[MAX_ENEMIES];

// Texturas
Texture *texture_1px;

// Fonte e texto
Font *font;
Text *text;


// Thread principal
void render(){
    // Renderizando terreno
    mundo->render();

    // Renderizando inimigos
    for(int i = 0;i < MAX_ENEMIES;i++){
        inimigo[i]->render();
    }

    // Renderizando jogador
    jogador->render();

    // Renderizando texto dos personagens
    for(int i = 0;i < MAX_ENEMIES;i++){
        inimigo[i]->renderName();
    }
    jogador->renderName();

    // Renderizando interface
    texture_1px->setColor(34, 32, 52);
    texture_1px->setWidth(WINDOW_WIDTH);
    texture_1px->setHeight(88);
    texture_1px->render();

    // Renderizando pontuação do jogador
    text->setColor(251, 242, 54);
    text->setText("Thread 1: "+std::to_string(jogador->getPoints()));
    text->setPos(10, 10);
    text->render();

    // Renderizando pontuação dos inimigos
    text->setColor(255, 255, 255);
    for(int i = 0;i < MAX_ENEMIES;i++){
        text->setText("Thread "+std::to_string(i+2)+": "+std::to_string(inimigo[i]->getPoints()));
        text->setPos(12 + (i+1)/3*128, 10 + 27*((i+1)%3));
        text->render();
    }
}

// Thread principal
void update(){

    // Tratando eventos
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0){
        // Comando para fechar o programa
        if (e.type == SDL_QUIT) {
            engine->stop();
            return;
        }

        // Tecla para baixo ou para baixo
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
            SDL_Keycode code = e.key.keysym.sym;

            if (code == SDLK_k || code == SDLK_UP 	|| code == SDLK_w) 
				player->setMovingUp(e.type == SDL_KEYDOWN);
            if (code == SDLK_l || code == SDLK_RIGHT || code == SDLK_d) 
				player->setMovingRight(e.type == SDL_KEYDOWN);
            if (code == SDLK_j || code == SDLK_DOWN || code == SDLK_s) 
				player->setMovingDown(e.type == SDL_KEYDOWN);
            if (code == SDLK_h || code == SDLK_LEFT || code == SDLK_a) 
				player->setMovingLeft(e.type == SDL_KEYDOWN);
        }
    }

    // Atualizando jogador
    jogador->update();
}

// Popula o mapa com o jogador principal e os inimígos
void populate(){
    SDL_Point p;

    // Criando jogador principal
    p = mundo->randomPoint();
    jogador = new Jogador(mundo, text, rand(), p.x, p.y);
    mundo->setOccupied(p.x, p.y, true);

    // Criando inimigos
    for(int i = 0;i < MAX_ENEMIES;i++){
        p = mundo->randomPoint();
        inimigo[i] = new Inimigo(mundo, text, "Thread "+std::to_string(i+2), rand(), p.x, p.y);
        mundo->setOccupied(p.x, p.y, true);
    }

    // Gerando tesouro no mapa
    mundo->spawnTreasure();
}

// Carrega todos os recursos necessários
void load(){
    // Carregando sprites
    Sprite::loadTexture(new Texture(engine, "gfx/sprites.png"));

    // Carregando fonte e objeto para renderização de texto
    font = new Font("gfx/font.ttf", 16);
    text = new Text(engine, font, " ");

    // Carregando e populando o mapa
    mundo = new Mundo(engine, "gfx/ground.png", "gfx/ores.png");
    populate();

    // Carregando textura de interface
    texture_1px = new Texture(engine, "gfx/1px.png");
}

// Descarrega todos os recursos carregados
void unload(){
    for(int i = 0;i < MAX_ENEMIES;i++) delete enemy[i];

    delete mundo;
    delete jogador;
    delete font;
    delete text;
    delete texture_1px;

    Sprite::unloadTexture();

    delete engine;
}

// Essa função é chamada para processar cada inimigo em threads distintas
void *inimigoThread(void *arg){
    Inimigo *inimigo = (Inimigo*)arg;

    // Looping de atualização
    while(engine->isRunning()){
        unsigned int t0 = SDL_GetTicks();

        // Atualizando inimigo
        inimigo->update();

        // Ajustando FPS conforme o limite especificado
        Engine::sleep(t0);
    }

    pthread_exit(NULL);
}

// Cria personagens inimigos e processa em threads distintas
void createEnemiesThreads(){
    // Criando atributo para definir as threads como JOINABLE
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Criando threads
    for(int i = 0;i < MAX_ENEMIES;i++){
        pthread_create(&inimigo_thread[i], &attr, inimigoThread, (void*)inimigo[i]);
    }

    // Destruindo atributo
    pthread_attr_destroy(&attr);
}

// Aguarda a finalização de todas as threads
void joinInimigosThreads(){
    for(int i = 0;i < MAX_ENEMIES;i++){
        pthread_join(inimigo_thread[i], NULL);
    }
}

int main(){

    // Setando semente para a geração de número aleatórios
    srand(time(NULL));

    // Criando engine
    engine = new Engine(WINDOW_WIDTH, WINDOW_HEIGHT, "Pixteal");

    // Carregando recursos
    load();

    // Criando threads dos inimigos
    createEnemiesThreads();

    // Iniciando engine
    engine->start(render, update);

    // Esperando a finalização das threads
    joinEnemiesThreads();

    // Descarregando recursos
    unload();

    return 0;
}