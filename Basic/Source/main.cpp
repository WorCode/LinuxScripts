#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
#include "Ball.h"
#include "Rectangle.h"

bool running = true;
using namespace std;

vector<Ball*> bricks;
vector<thread> brickThreads;
vector<char> ballMembers;
/*
    xR - zmienna przesuwająca prostokąt
    movR - prędkość przesuwania prostokąta
*/
int xR=0, movR=1, xMax, yMax;;
Rectangle* rect;

void moveBall(Ball *bri)
{
        while(running)
        {
                //Dopóki nie nastąpi 6 odbić to wykonuj
                while(bri->rebound < 6)
                {
                            bri->xPosition += bri->xMov;
                            if (bri->xPosition < 0)
                            {
                                bri->xPosition = bri->xPosition * (-1);
                                //Zmiana kierunku
                                bri->xMov = bri->xMov*(-1);
                                bri->rebound++;
                            }

                            if (bri->xPosition > xMax) {
                                bri->xPosition = bri->xPosition - (bri->xPosition - xMax);
                                bri->xMov = bri->xMov*(-1);
                                bri->rebound++;
                            }


                            bri->yPosition += bri->yMov;
                            if ( bri->yPosition < 0 ) {
                                bri->yPosition = bri->yPosition * (-1);
                                bri->yMov = bri->yMov*(-1);
                                bri->rebound++;
                            }

                            if (bri->yPosition > yMax) {
                                bri->yPosition = bri->yPosition - (bri->yPosition - yMax);
                                bri->yMov = bri->yMov*(-1);
                                bri->rebound++;
                            }
                            //Co ile ma następować ruch, częściej niż odżwieżanie ekranu które jest co 1 s
                            usleep(1000000);


                        if(bri->rebound==6) {
                            bri->falling = false;
                        }
                }
                usleep(40000);
        }
}

void newBallOnScene()
{
        //Losowy wektor poruszania i kolor
        int dirX=0, dirY=0, color=0;
        //Zmienna color będzie dodawana i dzielona modulo przez 6 ponieważ
        //w funkcji main mam 6 zestawów kolorów
        while(running == true)
        {
                dirX = rand() % 5 + 1;
                dirY = rand() % 5 + 1;

                color++;
                color%=6;

                int random_ascii = rand() % ('z' - 'a' + 1) + 'a';
                char c = static_cast<char>(random_ascii);
                ballMembers.push_back(c);

                Ball *brick = new Ball(dirX, dirY, rand() % 21, color);
                bricks.push_back(brick);

                //Uruchomienie wątku przemieszczania kulki.
                //brickThreads.push_back(brick.fallThread());

                brickThreads.push_back(std::thread(moveBall, brick ) );

                // Losowe opóźnienie pojawiania się kulek z zakresu 1s do 5s
                unsigned randTime = rand() % 5 + 1;
                sleep(randTime);

        }
}

void moveRect()
{
    while(running == true) {
        //Przesunięcie współrzędnej x
        rect->moveRectangle( (rand() % 5 + 1) );

        //Screen  usleep(20000);
        usleep(80000);
    }
}

// "Monitor" function
void refreshScreen(int xL, int yL)
{
        //start_color();  // Initialize color
        //init_pair(1, COLOR_RED, COLOR_BLACK);  //Ustawienie atrybutu koloru
        int leftXRLimit = xL - (xL/8);
        int widthRect = (xL/8), heightRect = (yL/4);

        while(running == true)
        {
                //Kasowanie tych które odbiły się 6 razy
                for(int i = 0; i < bricks.size(); i++) {
                    if( !bricks[i]->isFalling() ) {
                        bricks.erase(bricks.begin() + i);
                    }
                }

                clear();
                //Rysowanie Prostokąta
                int x = rect->getXPosition();
                for (int y = 0; y < rect->getHeightRect(); y++)
                {
                    mvprintw(y, x, "X");
                }


                //Rysowanie kulek
                for(int i = 0; i < bricks.size(); i++) {

                    //  bricks[i].getChar() Coś z tablicą znaków i odwołaniem do index-u można...
                    attron(COLOR_PAIR( bricks[i]->getColor() ) );
                    mvprintw(bricks[i]->getyPosition(), bricks[i]->getxPosition(), "%c", ballMembers[i]);
                    attroff(COLOR_PAIR( bricks[i]->getColor() ) );
                }
                /*
                 *  Ustale funkcję fall poza wjązaniem ją z thread.
                 *  czas tworzenia nowej kulki zostanie wydłużony
                 */
                refresh();

                //Odświerzanie częściej niż aktualizacja wartości więc ma mieć mniej niż
                // < 1000000 (6 zer)
                usleep(20000);
        }
}

void exitProgram() {
    while (running) {
        if (const int inputChar = getch(); inputChar == ' ') {
            running = false;
        }
    }
}


int main(int argc, char *argv[])
{


        srand(time(0));

        // Initialize ncurses
        initscr();
        curs_set(0);
        getmaxyx(stdscr, yMax, xMax);

        int rightXRLimit = xMax - 1;
        int heightRect = (yMax/4);

        rect = new Rectangle(rightXRLimit, heightRect);


        //Inicialize colors
        start_color();
        init_pair(0, COLOR_YELLOW, COLOR_RED);
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_BLUE, COLOR_BLACK);
        init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(5, COLOR_CYAN, COLOR_BLACK);


/*
        // Ustalenie 5 kulek
        for(int i = 0; i < 1; i++)
        {
            // ...with random descent rate in range 0 (slow) to 20 (fast)
            Brick brick(2, 3, rand() % 21);
            bricks.push_back(brick);
        }
*/
        // Start wątku dodającego kulki

        // Start wątku odświeżającego ekran
        std::thread monitor(refreshScreen, xMax, yMax);


        //Poruszanie się prostokąta
        std::thread prosto(moveRect);

        std::thread newBall( newBallOnScene);
        // Kończenie pracy programu naciśnięciem spacji
        std::thread exitProgramThread(exitProgram);

    /*
        //Dodawanie nowych kulek
        for(int i=0; i<1; i++)
        {
            brickThreads.push_back(bricks.at(i).fallThread());

            // Losowe opóźnienie pojawiania się kulek z zakresu 1s do 5s
            unsigned randTime = rand() % 5 + 1;
            usleep(1000 * randTime);
        }
*/

        monitor.join();
        prosto.join();
        newBall.join();
        exitProgramThread.join();

        for(int i = 0; i < brickThreads.size(); i++)
        {
            brickThreads.at(i).join();
        }

        // Close ncurses
        endwin();

        std::cout << "Program zakończył działanie po pomyślnym wykonaniu wszystkich wątków." << std::endl;

        return 0;
}


