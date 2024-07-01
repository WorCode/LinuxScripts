//
// Created by michal on 17.6.2024.
//

#ifndef RECTANGLE_H

#define RECTANGLE_H

#include <vector>


class Client;

class Rectangle {
public:
    int rightXLimit;
    int heightRect;
    int xR, movR;

    Rectangle(int, int);

    ~Rectangle();

    void moveRectangle(int);

    int getXPosition();

    int getHeightRect();
};


#endif //RECTANGLE_H