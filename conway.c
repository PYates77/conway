#include "curses.h"
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*   Any live cell with two or three neighbors survives.
 *   Any dead cell with three live neighbors becomes a live cell.
 *   All other live cells die in the next generation. Similarly, all other dead cells stay dead.
*/

#define CONWAY_COLOR_ALIVE 1
#define CONWAY_COLOR_DEAD 2
#define CONWAY_RANDOM_COVERAGE_RATIO 0.2
#define CONWAY_MAX_SIZE 1024

typedef bool conway_world[CONWAY_MAX_SIZE][CONWAY_MAX_SIZE];

struct conway {
    conway_world world1;
    conway_world world2;
    conway_world *world;
    conway_world *next_world;

    unsigned int height;
    unsigned int width;
};

void conway_init(unsigned int height, unsigned int width, struct conway *c)
{
    memset(c, 0, sizeof(*c));
    c->height = height;
    c->width = width/2; // so that we roughly display squares, each horizontal game tile is two on-screen characters
    c->world = &c->world1;
    c->next_world = &c->world2;
}

void conway_swap_worlds(struct conway *c)
{
    conway_world *tmp = c->world;
    c->world = c->next_world;
    c->next_world = tmp;
}

void conway_live(unsigned int x, unsigned int y, conway_world *world)
{
    (*world)[x][y] = true;
}

void conway_die(unsigned int x, unsigned int y, conway_world *world)
{
    (*world)[x][y] = false;
}

int conway_neighbors(unsigned int x, unsigned int y, struct conway *c)
{
    /* check all 8 neighbors for aliveness while respecting out of bounds */
    /* Consider borders to be always empty */
    /* TODO toroidal worlds? */
    int neighbors = 0;
    for (int j=y-1; j<=y+1; ++j) {
        for (int i=x-1; i<=x+1; ++i) {
            if (i != x || j != y) { // ignore (x, y)
                if ((i >= 0) && (i < c->width) && (j >= 0) && (j < c->height)) { // ignore out of bounds
                    if ((*c->world)[i][j]) {
                        ++neighbors;
                    }
                }
            }
        }
    }

    return neighbors;
}

void conway_tick(struct conway *c)
{
    //printf("conway tick");
    for (unsigned int y=0; y < c->height; ++y) {
        for (unsigned int x=0; x < c->width; ++x) {
            int n = conway_neighbors(x,y, c);

            //printf("(%d, %d) is %s has %d neighbors\r\n", x, y, (*c->world)[x][y] ? "alive" : "dead", n);
            if ((*c->world)[x][y]) {
                // alive
                if (n < 2 || n > 3) {
                    // dies of either over/underpopulation
                    conway_die(x,y,c->next_world);
                } else {
                    // continues being alive
                    conway_live(x,y,c->next_world);
                }
            } else {
                // dead
                if (n == 3) {
                    // becomes alive due to good neighbors
                    conway_live(x,y,c->next_world);
                } else {
                    // continues being dead
                    conway_die(x,y,c->next_world);
                }
            }
        }
    }
    for (unsigned int y=0; y < c->height; ++y) {
        for (unsigned int x=0; x < c->width; ++x) {
            if ((*c->world)[x][y]) {
                attron(COLOR_PAIR(CONWAY_COLOR_ALIVE));
                mvaddch(y, 2*x,'[');
                mvaddch(y, 2*x+1, ']');
            } else {
                attron(COLOR_PAIR(CONWAY_COLOR_DEAD));
                mvaddch(y, 2*x,' ');
                mvaddch(y, 2*x+1, ' ');
            }
        }
    }
    conway_swap_worlds(c);
}


int main()
{

    int game_width = 32;
    int game_height = 32;
    srand(time(0));
    //setup curses
    initscr();
    start_color();
    use_default_colors();
    noecho();
    keypad(stdscr,TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);
    leaveok(stdscr, TRUE);
    scrollok(stdscr, FALSE);
    int use_colors = has_colors();
    if(use_colors) {
        init_pair(CONWAY_COLOR_ALIVE, COLOR_WHITE, COLOR_WHITE);
        init_pair(CONWAY_COLOR_DEAD, -1, -1);
    }
    // get current height-width of terminal window
    getmaxyx(stdscr, game_height, game_width); // macro modifies game_height and game_width

    printf("Terminal Height: %d, terminal width: %d\n", game_height, game_width);

    //done setting up curses

    struct conway game;

    conway_init(game_height, game_width, &game);

    /* Set some random squares to alive */
    for (int i=0; i<game_height * game_width * CONWAY_RANDOM_COVERAGE_RATIO; ++i) {
        int x = rand()%game_width;
        int y = rand()%game_height;
        conway_live(x, y, game.world);
    }

    //begin display loop
    bool loop = true;
    while(loop) {
        // process game tick
        refresh();
        usleep(60000);
        conway_tick(&game);

        int c = getch();
        if (c == 27) { //escape
            loop = false;
        }
    }
    endwin();
    return 0;
}
