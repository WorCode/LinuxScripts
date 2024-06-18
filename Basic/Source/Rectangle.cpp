//
// Created by michal on 17.6.2024.
//

#include "Rectangle.h"
#include <ncurses.h>
#include <unistd.h>



Rectangle::Rectangle(int rightXLimit, int heightRect) {

    this->heightRect = heightRect;
    this->rightXLimit = rightXLimit;

    xR=0;
    movR=1;

}

Rectangle::~Rectangle() = default;

//Metoda przyjmująca losową wartość z programu i kiedy następi odbicie zostaje
//nadana nowa wartość prędkości.
void Rectangle::moveRectangle(int nextValue)
{

        //Przesuwanie prostokąta:
        xR+=movR;

        //Sprawdzenie czy zmienić kierunek
        if( xR < 0 )
        {
            xR=0;
            movR = nextValue;
        }

        if( xR > rightXLimit )
        {
            xR = rightXLimit;
            movR = nextValue;
            movR = movR * (-1);
        }

}

int Rectangle::getXPosition() {
    return xR;
}

int Rectangle::getHeightRect()  {
    return heightRect;
}


