#ifndef JOGADOR_H
#define JOGADOR_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "engine/texture.h"
#include "engine/text.h"
#include "sprite.h"
#include "mundo.h"

class Jogador: private Sprite{
private:
    bool moving_up = false;
    bool moving_right = false;
    bool moving_down = false;
    bool moving_left = false;
    int priority = 0;

    int points = 0;

    Mundo *mundo;
    Text *text;

public:
    Jogador(Mundo *mundo, Text *text, int id, int x, int y);

    void render();
    void renderName();
    void update();

    int getPoints();

    void setMovingUp(bool value);
    void setMovingRight(bool value);
    void setMovingDown(bool value);
    void setMovingLeft(bool value);
};

#endif