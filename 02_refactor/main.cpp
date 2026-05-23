#include <ncurses.h>
#include <unistd.h>

const int MAP_HEIGHT = 10;
const int MAP_WIDTH = 30;

char level_map[MAP_HEIGHT][MAP_WIDTH + 1] = {
    "##############################",
    "#                            #",
    "#                            #",
    "#                            #",
    "#          ####              #",
    "#                            #",
    "#                            #",
    "#                ####        #",
    "#                            #",
    "##############################"
};

int player_x = 2;
int player_y = 8;

bool is_running = true;

void draw_game() {
    clear();

    for (int row = 0; row < MAP_HEIGHT; ++row) {
        for (int column = 0; column < MAP_WIDTH; ++column) {
            if (row == player_y && column == player_x) {
                printw("@");
            } else {
                printw("%c", level_map[row][column]);
            }
        }

        printw("\n");
    }

    printw("\nControls: A/D - move, W - jump, Q - quit\n");

    refresh();
}

bool can_move_to(int next_y, int next_x) {
    if (next_y < 0 || next_y >= MAP_HEIGHT) {
        return false;
    }

    if (next_x < 0 || next_x >= MAP_WIDTH) {
        return false;
    }

    return level_map[next_y][next_x] != '#';
}

void handle_input() {
    int pressed_key = getch();

    if (pressed_key == 'q' || pressed_key == 'Q') {
        is_running = false;
        return;
    }

    if ((pressed_key == 'a' || pressed_key == 'A') && can_move_to(player_y, player_x - 1)) {
        --player_x;
    }

    if ((pressed_key == 'd' || pressed_key == 'D') && can_move_to(player_y, player_x + 1)) {
        ++player_x;
    }

    if (pressed_key == 'w' || pressed_key == 'W') {
        if (can_move_to(player_y - 1, player_x)) {
            --player_y;
        }

        if (can_move_to(player_y - 1, player_x)) {
            --player_y;
        }
    }
}

void apply_gravity() {
    if (can_move_to(player_y + 1, player_x)) {
        ++player_y;
    }
}

int main() {
    initscr();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    while (is_running) {
        handle_input();
        apply_gravity();
        draw_game();
        usleep(80000);
    }

    endwin();

    return 0;
}
