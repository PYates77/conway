#include "curses.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define S 36000

int a[S], b[S], (*c)[S], (*d)[S], (*p)[S], i, h, w, n, t=0;

int main()
{
    c = &a;
    d = &b;
    initscr();
    start_color();
    use_default_colors();
    noecho();
    keypad(stdscr,TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);
    leaveok(stdscr, TRUE);
    scrollok(stdscr, FALSE);
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    init_pair(2, -1, -1);
    getmaxyx(stdscr, h, w);
    w=w/2;
    srand(time(0));
    for(i=0; i<h*w; ++i) {
        a[i] = rand()%5 == 1 ? 1 : 0;
    }

    while(t!=27) {
        for (i=0; i<h*w; ++i) {
            char s = ' ';
            if ((*c)[i]) {
                attron(COLOR_PAIR(1));
                s = 'X';
            } else {
                attron(COLOR_PAIR(2));
            }
            mvaddch(i/w, 2*(i%w), s);
            mvaddch(i/w, 2*(i%w)+1, s);
            n = (*c)[i-w-1]+(*c)[i-w]+(*c)[i-w+1]+(*c)[i-1]+(*c)[i+1]+(*c)[i+w-1]+(*c)[i+w]+(*c)[i+w+1];
            if ((*c)[i]) {
                (*d)[i] = n == 2 || n == 3;
            } else {
                (*d)[i] = n == 3;
            }
        }
        p = c;
        c = d;
        d = p;
        t=getch();
        refresh();
        usleep(100000);
    }
    endwin();
}
