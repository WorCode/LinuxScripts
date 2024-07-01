//
// Created by mateusz on 5/10/24.
//

#ifndef Ball_H
#define Ball_H

#include <condition_variable>
#include <iostream>
#include <thread>
#include <unistd.h>

class Ball
{
public:
    Ball(int xMov, int descentRate, int col, int index);
    ~Ball();
    int getxPosition();
    int getyPosition();
    bool isFalling();
    void setRandomChar();
    void setRandomColor();
    int getColor();
    char* getChar() const ;
    bool falling, free, isRight;
    int xMov;
    int yMov;
    int xPosition;
    int yPosition;
    int descentRate;
    int rebound;
    char *c;
    int closest;
    std::mutex mutex;
    std::condition_variable cond;
    int index;
protected:
private:
    //static bool initialized;
    int color;

};

#endif // Ball_H

