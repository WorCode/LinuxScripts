//
// Created by mateusz on 5/10/24.
//

#ifndef Ball_H
#define Ball_H

#include <iostream>
#include <thread>
#include <unistd.h>

class Ball
{
public:
    Ball(int xMov, int yMov, int descentRate, int col);
    ~Ball();
    int getxPosition();
    int getyPosition();
    bool isFalling();
    void setRandomChar();
    void setRandomColor();
    int getColor();
    char* getChar() const ;
    bool falling;
    int xMov;
    int yMov;
    int xPosition;
    int yPosition;
    int descentRate;
    int rebound;
    char *c;
protected:
private:
    //static bool initialized;
    int color;

};

#endif // Ball_H

