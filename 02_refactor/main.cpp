#include <ncurses.h>
#include <unistd.h>

const int MAP_HEIGHT = 10;
const int MAP_WIDTH = 30;
const int FRAME_DELAY_MICROSECONDS = 80000;
const int JUMP_HEIGHT = 2;

const char WALL_SYMBOL = '#';
const char PLAYER_SYMBOL = '@';

struct Position {
    int x;
    int y;
};

struct Level {
    char tiles[MAP_HEIGHT][MAP_WIDTH + 1];
};

struct GameState {
    Position player_position;
    bool is_running;
};

Level create_level() {
    Level level = {
        {
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
        }
    };

    return level;
}

GameState create_initial_game_state() {
    GameState game_state = {
        {2, 8},
        true
    };

    return game_state;
}

bool is_inside_level(const Position& position) {
    return position.y >= 0
        && position.y < MAP_HEIGHT
        && position.x >= 0
        && position.x < MAP_WIDTH;
}

bool is_wall_at(const Level& level, const Position& position) {
    return level.tiles[position.y][position.x] == WALL_SYMBOL;
}

bool can_stand_at(const Level& level, const Position& position) {
    return is_inside_level(position) && !is_wall_at(level, position);
}

Position get_shifted_position(
    const Position& position,
    int shift_x,
    int shift_y
) {
    Position shifted_position = {
        position.x + shift_x,
        position.y + shift_y
    };

    return shifted_position;
}

void move_player_if_possible(
    GameState& game_state,
    const Level& level,
    int shift_x,
    int shift_y
) {
    Position next_position = get_shifted_position(
        game_state.player_position,
        shift_x,
        shift_y
    );

    if (can_stand_at(level, next_position)) {
        game_state.player_position = next_position;
    }
}

void jump_player(GameState& game_state, const Level& level) {
    for (int jump_step = 0; jump_step < JUMP_HEIGHT; ++jump_step) {
        Position next_position = get_shifted_position(
            game_state.player_position,
            0,
            -1
        );

        if (!can_stand_at(level, next_position)) {
            return;
        }

        game_state.player_position = next_position;
    }
}

void apply_gravity(GameState& game_state, const Level& level) {
    move_player_if_possible(game_state, level, 0, 1);
}

void process_input(GameState& game_state, const Level& level) {
    int pressed_key = getch();

    switch (pressed_key) {
        case 'a':
        case 'A':
            move_player_if_possible(game_state, level, -1, 0);
            break;

        case 'd':
        case 'D':
            move_player_if_possible(game_state, level, 1, 0);
            break;

        case 'w':
        case 'W':
            jump_player(game_state, level);
            break;

        case 'q':
        case 'Q':
            game_state.is_running = false;
            break;

        default:
            break;
    }
}

bool is_player_at_position(
    const GameState& game_state,
    int row,
    int column
) {
    return game_state.player_position.y == row
        && game_state.player_position.x == column;
}

void draw_level(const GameState& game_state, const Level& level) {
    for (int row = 0; row < MAP_HEIGHT; ++row) {
        for (int column = 0; column < MAP_WIDTH; ++column) {
            if (is_player_at_position(game_state, row, column)) {
                printw("%c", PLAYER_SYMBOL);
            } else {
                printw("%c", level.tiles[row][column]);
            }
        }

        printw("\n");
    }
}

void draw_controls() {
    printw("\nControls:\n");
    printw("A / D - move left / right\n");
    printw("W     - jump\n");
    printw("Q     - quit\n");
}

void render_game(const GameState& game_state, const Level& level){
    clear();

    draw_level(game_state, level);
    draw_controls();

    refresh();
}

void initialize_console() {
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
}

void shutdown_console() {
    endwin();
}

void run_game_loop(GameState& game_state, const Level& level) {
    while (game_state.is_running) {
        process_input(game_state, level);
        apply_gravity(game_state, level);
        render_game(game_state, level);

        usleep(FRAME_DELAY_MICROSECONDS);
    }
}

int main() {
    Level level = create_level();
    GameState game_state = create_initial_game_state();

    initialize_console();
    run_game_loop(game_state, level);
    shutdown_console();

    return 0;
}
