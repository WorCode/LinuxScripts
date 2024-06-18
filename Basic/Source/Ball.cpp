//
// Created by mateusz on 5/10/24.
//

#include "Ball.h"

//bool Ball::initialized = false;


//Zmienna col w zakresie <1 : 7> podawana w celu wywołania konkretnej pary
Ball::Ball(int xMov, int yMov, int descentRate, int col)
{
    //ctor
    this->xPosition = 10;
    this->yPosition = 0;

    this->yMov = yMov;
    this->xMov = xMov;

    this->descentRate = descentRate;
    this->falling = true;
    this->color = col;

    this->rebound = 0;
    this->setRandomColor();
}

Ball::~Ball()
{
    //dtor
}


int Ball::getxPosition()
{
    return xPosition;
}

int Ball::getyPosition()
{
    return yPosition;
}

bool Ball::isFalling()
{
    return falling;
}

void Ball::setRandomColor() {
    color = rand() % 6 + 1;
}

int Ball::getColor() {
    return color;
}
