#ifndef INIMIGO_H
#define INIMIGO_H

#include <string>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "engine/texture.h"
#include "engine/text.h"
#include "sprite.h"
#include "mundo.h"

// Probabilidade de ir em direção do tesouro a cada passo
#define ENEMY_PROBABILITY 0.5

// Número máximo de tentativas ao tentar dar um passo aleatório
#define ENEMY_RANDOM_ATTEMPTS 10

class Inimigo: private Sprite{
private:
    Mundo *mundo;
    Text *text;

    int points = 0;
    std::string name;

public:
    Inimigo(Mundo *mundo, Text *text, std::string name, int id, int x, int y);

    void render();
    void renderName();
    void update();

    int getPoints();
};

#endif