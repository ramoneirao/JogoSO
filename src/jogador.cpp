#include "../include/jogador.h"

Jogador::Jogador(Mundo *mundo, Text *text, int id, int x, int y) : Sprite(id, x, y){
    this->mundo = mundo;
    this->text = text;
}

void Jogador::render(){
    // Renderizando sprite
    Sprite::render();
}

void Jogador::renderName(){
    // Renderizando nome
    text->setColor(251, 242, 54);
    text->setPos(x*32 - 16 + offsetx, WORLD_MARGIN_TOP + y*32 - 20 + offsety);
    text->setText("Thread 1");
    text->render();
}

void Jogador::update(){
    // Atualizando sprite e checando se a posição (x, y) foi alterada
    if (Sprite::update()){
        // Entrando na região critica
        pthread_mutex_lock(&mundo->mutex);

        // Pegando recompensa do chão (caso esteja em cima)
        points += mundo->catchTreasure(x, y);

        // Saindo da região crítica
        pthread_mutex_unlock(&mundo->mutex);
    }

    // Checa se não está se movendo no momento e se existe alguma solicitação de movimento
    if (isMoving() == false && (moving_up|moving_right|moving_down|moving_left) == true){

        // Entrando na região critica
        pthread_mutex_lock(&mundo->mutex);

        // Move para a posição solicitada e disponível
        if (moving_up && (priority == 0 || priority == SP_UP)) {
            if (mundo->isOccupied(x, y-1) == false){
                mundo->setOccupied(x, y, false);
                mundo->setOccupied(x, y-1, true);
                move(SP_UP);
            }
        }
        else if (moving_right && (priority == 0 || priority == SP_RIGHT)) {
            if (mundo->isOccupied(x+1, y) == false){
                mundo->setOccupied(x, y, false);
                mundo->setOccupied(x+1, y, true);
                move(SP_RIGHT);
            }
        }
        else if (moving_down && (priority == 0 || priority == SP_DOWN)){
            if (mundo->isOccupied(x, y+1) == false){
                mundo->setOccupied(x, y, false);
                mundo->setOccupied(x, y+1, true);
                move(SP_DOWN);
            }
        }
        else if (moving_left && (priority == 0 || priority == SP_LEFT)) {
            if (mundo->isOccupied(x-1, y) == false){
                mundo->setOccupied(x, y, false);
                mundo->setOccupied(x-1, y, true);
                move(SP_LEFT);
            }
        }

        // Saindo da região crítica
        pthread_mutex_unlock(&mundo->mutex);
    }
}

int Jogador::getPoints(){
    return points;
}

void Jogador::setMovingUp(bool value){
    if (value == true && moving_up == false) priority = SP_UP;
    if (value == false) priority = 0;
    moving_up = value;
}
void Jogador::setMovingRight(bool value){
    if (value == true && moving_right == false) priority = SP_RIGHT;
    if (value == false) priority = 0;
    moving_right = value;
}
void Jogador::setMovingDown(bool value){
    if (value == true && moving_down == false) priority = SP_DOWN;
    if (value == false) priority = 0;
    moving_down = value;
}
void Jogador::setMovingLeft(bool value){
    if (value == true && moving_left == false) priority = SP_LEFT;
    if (value == false) priority = 0;
    moving_left = value;
}