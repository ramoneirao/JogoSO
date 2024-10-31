
#include "../include/inimigo.h"

Inimigo::Inimigo(Mundo *mundo, Text *text, std::string name, int id, int x, int y) : Sprite(id, x, y){
    this->mundo = mundo;
    this->text = text;
    this->name = name;
}

void Inimigo::render(){
    // Renderizando sprite
    Sprite::render();
}

void Inimigo::renderName(){
    // Renderizando nome
    text->setColor(255, 255, 255);
    text->setPos(x*32 - 16 + offsetx, WORLD_MARGIN_TOP + y*32 - 20 + offsety);
    text->setText(name);
    text->render();
}

void Inimigo::update(){
    // Atualizando sprite e checando se a posição (x, y) foi alterada
    if (Sprite::update()){
        // Entrando na região critica
        pthread_mutex_lock(&mundo->mutex);

        // Pegando recompensa do chão (caso esteja em cima)
        points += mundo->catchTreasure(x, y);

        // Saindo da região crítica
        pthread_mutex_unlock(&mundo->mutex);
    }

    // Movimenta o personagem
    if (isMoving() == false){

        // Entrando na região critica
        pthread_mutex_lock(&mundo->mutex);

        // Obtendo posição do tesouro
        SDL_Point point = mundo->getTreasurePosition();
        int p = (rand()%100)+1;

        // Probabilidade p de dar um passo aleatório
        if (p > ENEMY_PROBABILITY*100){

            // Tentativas finitas
            for(int i = 0;i < ENEMY_RANDOM_ATTEMPTS;i++){
                int dir = (rand()%4)+1;

                if (dir == SP_UP && mundo->isOccupied(x, y-1) == false){
                    mundo->setOccupied(x, y, false);
                    mundo->setOccupied(x, y-1, true);
                    move(SP_UP);
                    break;
                }
                if (dir == SP_RIGHT && mundo->isOccupied(x+1, y) == false){
                    mundo->setOccupied(x, y, false);
                    mundo->setOccupied(x+1, y, true);
                    move(SP_RIGHT);
                    break;
                }
                if (dir == SP_DOWN && mundo->isOccupied(x, y+1) == false){
                    mundo->setOccupied(x, y, false);
                    mundo->setOccupied(x, y+1, true);
                    move(SP_DOWN);
                    break;
                }
                if (dir == SP_LEFT && mundo->isOccupied(x-1, y) == false){
                    mundo->setOccupied(x, y, false);
                    mundo->setOccupied(x-1, y, true);
                    move(SP_LEFT);
                    break;
                }
            }
        }
        else{ // Probabilidade 1-p de andar em direação do tesouro

            if (point.x > x && mundo->isOccupied(x+1, y) == false){ // Tesouro está à direita
                mundo->setOccupied(x, y, false);
                mundo->setOccupied(x+1, y, true);
                move(SP_RIGHT);
            }
            else if (point.x < x && mundo->isOccupied(x-1, y) == false){ // Tesouro está à esquerda
                mundo->setOccupied(x, y, false);
                mundo->setOccupied(x-1, y, true);
                move(SP_LEFT);
            }
            else if (point.y > y && mundo->isOccupied(x, y+1) == false){ // Tesouro está abaixo
                mundo->setOccupied(x, y, false);
                mundo->setOccupied(x, y+1, true);
                move(SP_DOWN);
            }
            else if (point.y < y && mundo->isOccupied(x, y-1) == false){ // Tesouro está acima
                mundo->setOccupied(x, y, false);
                mundo->setOccupied(x, y-1, true);
                move(SP_UP);
            }
        }

        // Saindo da região crítica
        pthread_mutex_unlock(&mundo->mutex);
    }
}

int Inimigo::getPoints(){
    return points;
}
