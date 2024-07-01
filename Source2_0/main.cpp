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
#include <mutex>


bool running = true;
using namespace std;

vector<Ball*> bricks;
vector<thread> brickThreads;
vector<char> ballMembers;
/*
        xR - zmienna przesuwająca prostokąt
        movR - prędkość przesuwania prostokąta
        nearest - index w tablicy z kulkami wskazujący na kulkę pierwszą do odblokowania.
*/
int xR=0, movR=1, xMax, yMax, nearest=0;
Rectangle* rect;

mutex mtx;
condition_variable cv;


/* --------------- Synchronizacja polega na:

    1. Przy przyklejeniu się kulki nadaje wartość dist
    2. Nadawanie wartości dist dla kulki dla dwóch przypadków:
         1. Prostokąt w lewo
            Wtedy kulki przyklejone po prawej stronie od prostokąta mają wartość dist równą xposition
                  kulki przyklejone po lewej od prostokąta mają [Rect->posotion] - xposition
        2. Prostokąt w prawo
            Wtedy kulki przyklejone po prawej stronie od prostokąta mają wartość dist równą xposition - [Rect->posotion]
                  kulki przyklejone po lewej od prostokąta mają xMax - xposition

        Wartości dist dobrane tak aby mniejsza wartość wymagała szybszej zmiany stanu.

    3. W przesuwaniu prostokąta następuje sprawdzenie który wątek notify() następująco:
        closest = zmienna przechowująca index kulki najbliżej prostokąta
        dla tego elementu z wektora brics jest sprawdzane czy prostokąt już go odbił.
        Jeśli tak to zwalniam kulkę ball->free=true i szukam nowego indexu dla closest

    UWAGA ! ! !
    Każde doklejenie kulki do sufitu powoduje aktualizację closest
*/


void moveBall(Ball *bri, Rectangle* rect)
{
        while(running)
        {
                if( bri->free )
                {
                        //Dopóki nie nastąpi 6 odbić to wykonuj
                        while(bri->rebound < 6 && bri->free)
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
                            if ( bri->yPosition < 0 )
                            {
                                    //Do przetestowania i oddaje po pomyślnym kompilowaniu z lini poleceń kodu źródła
                                    bri->yPosition = 0;//bri->yPosition * (-1);
                                    bri->yMov = bri->yMov*(-1);
                                    bri->free = false;

                                    if( rect->xR < bri->xPosition )
                                    {
                                            bri->isRight = true;
                                    }
                                    else
                                    {
                                            bri->isRight = false;
                                    }

                                    {
                                            unique_lock lock(mtx);

                                            //Odblokowuje kiedt runing jest false bądź kiedy pozycja x prostokąta jest zgodna z pozycją x kulki
                                            cv.wait(lock, [&] {
                                                return ( !running || ( bri->isRight && rect->xR > bri->xPosition ) || ( !bri->isRight && rect->xR < bri->xPosition ) );
                                            });
                                            bri->free = true;
                                    }
                                    bri->rebound++;
                            }


                            if (bri->yPosition > yMax) {
                                bri->yPosition = bri->yPosition - (bri->yPosition - yMax);
                                bri->yMov = bri->yMov*(-1);
                                bri->rebound++;
                            }
                            //Co ile ma następować ruch, częściej niż odżwieżanie ekranu które jest co 1 s
                            //usleep(1000000);
                            usleep(100000 * bri->descentRate);


                            if(bri->rebound==6) {
                                bri->falling = false;
                            }
                        }
                }
        }
}

/*
         Program is on a HDD, process exist in RAM.
         pid_t fork() - create child process
         child process have an copy of memory and it is'not share data with parent
         If parent process end before child the number of pid is change to 1.
         Child process without parent is called orphan process.

https://www.codequoi.com/en/creating-and-killing-child-processes-in-c/
*/
void newBallOnScene()
{
        //Losowy wektor poruszania i kolor
        int dirX=0, dirY=0, color=0;
        //Zmienna color będzie dodawana i dzielona modulo przez 6 ponieważ
        //w funkcji main mam 6 zestawów kolorów
        while(running == true)
        {
                dirX = rand() % 3;

                if( dirX == 2)
                    dirX=(-1);

                color++;
                color%=6;

                int random_ascii = rand() % ('z' - 'a' + 1) + 'a';
                char c = static_cast<char>(random_ascii);
                ballMembers.push_back(c);

                Ball *brick = new Ball(dirX, rand() % 21, color, ballMembers.size());
                bricks.push_back(brick);

                //Uruchomienie wątku przemieszczania kulki.
                //brickThreads.push_back(brick.fallThread());

                brickThreads.push_back(std::thread(moveBall, brick , rect) );

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
/*
        //Sprawdzenie czy prostokąt dotknął którąś kulkę.
        for(int i=0; i<bricks.capacity(); i++)
        {
                unique_lock lock(mtx);
                if( rect->xR==bricks[i]->xPosition && !bricks[i]->free )
                {
                    cv.notify_all();
                }
        }
*/
        cv.notify_all();
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

                erase();
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

            {
                unique_lock lock(mtx);
                cv.notify_all();
            }
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


