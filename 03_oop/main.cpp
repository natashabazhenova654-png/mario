#include <ncurses.h>
#include <unistd.h>

const int MAP_HEIGHT = 10;
const int MAP_WIDTH = 30;
const int FRAME_DELAY_MICROSECONDS = 80000;
const int JUMP_HEIGHT = 2;

const char WALL_SYMBOL = '#';
const char PLAYER_SYMBOL = '@';

class Position {
public:
    Position(int x, int y) : x(x), y(y) {
    }

    int get_x() const {
        return x;
    }

    int get_y() const {
        return y;
    }

    Position shifted(int shift_x, int shift_y) const {
        return Position(x + shift_x, y + shift_y);
    }

private:
    int x;
    int y;
};

class Level {
public:
    Level() : tiles{
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
    } {
    }

    bool can_stand_at(const Position& position) const {
        return is_inside(position) && !is_wall_at(position);
    }

    char get_tile(int row, int column) const {
        return tiles[row][column];
    }

private:
    char tiles[MAP_HEIGHT][MAP_WIDTH + 1];

    bool is_inside(const Position& position) const {
        return position.get_y() >= 0
            && position.get_y() < MAP_HEIGHT
            && position.get_x() >= 0
            && position.get_x() < MAP_WIDTH;
    }

    bool is_wall_at(const Position& position) const {
        return tiles[position.get_y()][position.get_x()] == WALL_SYMBOL;
    }
};

class Player {
public:
    Player() : position(2, 8) {
    }

    Position get_position() const {
        return position;
    }

    void move_left(const Level& level) {
        move_if_possible(level, -1, 0);
    }

    void move_right(const Level& level) {
        move_if_possible(level, 1, 0);
    }

    void jump(const Level& level) {
        for (int jump_step = 0; jump_step < JUMP_HEIGHT; ++jump_step) {
            Position next_position = position.shifted(0, -1);

            if (!level.can_stand_at(next_position)) {
                return;
            }

            position = next_position;
        }
    }

    void apply_gravity(const Level& level) {
        move_if_possible(level, 0, 1);
    }

private:
    Position position;

    void move_if_possible(const Level& level, int shift_x, int shift_y) {
        Position next_position = position.shifted(shift_x, shift_y);

        if (level.can_stand_at(next_position)) {
            position = next_position;
        }
    }
};

class Console {
public:
    Console() {
        initscr();
        noecho();
        curs_set(0);
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
    }

    ~Console() {
        endwin();
    }
};

class Game {
public:
    Game() : is_running(true) {
    }

    void run() {
        Console console;

        while (is_running) {
            process_input();
            player.apply_gravity(level);
            render();

            usleep(FRAME_DELAY_MICROSECONDS);
        }
    }

private:
    Level level;
    Player player;
    bool is_running;

    void process_input() {
        int pressed_key = getch();

        switch (pressed_key) {
            case 'a':
            case 'A':
                player.move_left(level);
                break;

            case 'd':
            case 'D':
                player.move_right(level);
                break;

            case 'w':
            case 'W':
                player.jump(level);
                break;

            case 'q':
            case 'Q':
                is_running = false;
                break;

            default:
                break;
        }
    }

    bool is_player_at(int row, int column) const {
        Position player_position = player.get_position();

        return player_position.get_y() == row
            && player_position.get_x() == column;
    }

    void draw_level() const {
        for (int row = 0; row < MAP_HEIGHT; ++row){
            for (int column = 0; column < MAP_WIDTH; ++column) {
                if (is_player_at(row, column)) {
                    printw("%c", PLAYER_SYMBOL);
                } else {
                    printw("%c", level.get_tile(row, column));
                }
            }

            printw("\n");
        }
    }

    void draw_controls() const {
        printw("\nControls:\n");
        printw("A / D - move left / right\n");
        printw("W     - jump\n");
        printw("Q     - quit\n");
    }

    void render() const {
        clear();

        draw_level();
        draw_controls();

        refresh();
    }
};

int main() {
    Game game;
    game.run();

    return 0;
}
