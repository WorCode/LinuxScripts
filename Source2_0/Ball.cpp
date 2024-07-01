//
// Created by mateusz on 5/10/24.
//

#include "Ball.h"

#include <condition_variable>

//bool Ball::initialized = false;

//index - zmienna wskazująca pozycję w wektorze kulek pod którym jest możliwe odblokowanie kulki
//Zmienna col w zakresie <1 : 7> podawana w celu wywołania konkretnej pary
Ball::Ball(int xMov, int descentRate, int col, int index)
{
        //ctor
        this->xPosition = 10;
        this->yPosition = 0;

        this->yMov = 1;
        this->xMov = xMov;

        this->descentRate = descentRate;
        this->falling = true;
        this->free = true;
        this->color = col;

        this->rebound = 0;
        this->setRandomColor();

        this->closest=INT16_MAX;
        this->index = index;
        std::mutex mutex;
        std::condition_variable cond;
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

void Ball::setRandomColor()
{
        color = rand() % 6 + 1;
}

int Ball::getColor()
{
        return color;
}
