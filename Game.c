/*-------------------------------------------------------------------------
    Include files:
-------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdbool.h>

/*-------------------------------------------------------------------------
    Constants and definitions:
-------------------------------------------------------------------------*/
#define N 11
#define BLANK '_'
#define PLAYER_ONE 1
#define PLAYER_ONE_MARK 'X'
#define PLAYER_TWO 2
#define PLAYER_TWO_MARK 'O'
#define ROW_HISTORY_INDEX 0
#define COLUMN_HISTORY_INDEX 1
#define HISTORY_STATE_LENGTH 2
#define VALID_INPUT 1


/*-------------------------------------------------------------------------
    Function declaration
-------------------------------------------------------------------------*/
int game_preparations(char board[][N]);

void restart_board(char board[][N], int n);

void player_turn(char board[][N], int history[][HISTORY_STATE_LENGTH],
                 int player, int *turns, int size, int *first, int *second);

void player_turn_event_handler(char board[][N], int history[][HISTORY_STATE_LENGTH],
                               int player, int *turns, int size, int *first, int *second);

void undo_move(char board[][N], int history[][HISTORY_STATE_LENGTH], int *turns, int command);

bool check_undo_move_validity(int *turns, int command);

void regular_move(char board[][N], int player, int row, int col, int *turns, int history[][HISTORY_STATE_LENGTH]);

bool check_regular_move_validity(char board[][N], int row, int col, int size);

bool check_if_full_board(char board[][N], int size);

bool check_lose_condition(char board[][N], int size, int player);

bool check_rows(char board[][N], int size, char player_mark);

bool check_columns(char board[][N], int size, char player_mark);

bool check_diagonals(char board[][N], int size, char player_mark);

bool check_row(char row[N], int size, char player_mark);

bool check_column(char board[][N], int size, int col_num, char player_mark);

bool check_topLeft_to_bottomRight(char board[][N], int size, char player_mark);

bool check_topRight_to_bottomLeft(char board[][N], int size, char player_mark);

void game(char board[][N], int history[][HISTORY_STATE_LENGTH], int size, int player,
          int *turns, int *first, int *second);

void print_results(char board[][N], int size, int player);

void print_welcome();

void print_enter_board_size();

void print_board(char board[N][N], int n);

void print_player_turn(int player_index);

void print_error();

void print_winner(int player_index);

void print_tie();


/*-------------------------------------------------------------------------
    Implementation
-------------------------------------------------------------------------*/
int main() {
    char board[N][N];
    int history[N*N][HISTORY_STATE_LENGTH] = {{0}};
    int size = game_preparations(board), player = PLAYER_TWO, turns = 0, first = 0, second = 0;
    int *turns_ptr = &turns, *first_ptr = &first, *second_ptr = &second;
    game(board, history, size, player, turns_ptr, first_ptr, second_ptr);
    return 0;
}

//runs the core of the game after preparations
//4 lines
void game(char board[][N], int history[][HISTORY_STATE_LENGTH], int size, int player,
          int *turns, int *first, int *second) {
    while (!check_lose_condition(board, size, player) && !check_if_full_board(board, size)) {
        player = (player == PLAYER_TWO) ? PLAYER_ONE : PLAYER_TWO;
        player_turn(board, history, player, turns, size, first, second);
    }
    print_results(board, size, player);
}

//restarts the all board's values to '_'
//3 lines
void restart_board(char board[][N], int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            board[i][j] = BLANK;
}

//prepares the board for the start of the game
//8 lines
int game_preparations(char board[][N]) {
    print_welcome();
    print_enter_board_size();
    int size = 0;
    while (scanf("%d", &size) != VALID_INPUT || (size < 1 || size > N))
        continue;
    restart_board(board, size);
    print_board(board, size);
    return size;
}

//performs a single turn of the game
//3 lines
void player_turn(char board[][N], int history[][HISTORY_STATE_LENGTH],
                 int player, int *turns, int size, int *first, int *second) {
    print_player_turn(player);
    player_turn_event_handler(board, history, player, turns, size, first, second);
    print_board(board, size);
}

/*handles the input from the user, and decides if it's valid.
if it is, the player makes its turn and the game continues.
if it isn't, the user retries to input a valid*/
//12 lines
void player_turn_event_handler(char board[][N], int history[][HISTORY_STATE_LENGTH],
                               int player, int *turns, int size, int *first, int *second) {
    int current_turns = *turns, input_validation = 0;
    while (current_turns == *turns) {
        input_validation = scanf("%d", first);
        if ((input_validation == VALID_INPUT) && (*first < 0)) {
            if (check_undo_move_validity(turns, *first))
                undo_move(board, history, turns, *first);
            else
                print_error();
        } else if (input_validation == VALID_INPUT) {
            input_validation = scanf("%d", second);
            if ((input_validation == VALID_INPUT) && check_regular_move_validity(board, *first, *second, size)) {
                regular_move(board, player, *first, *second, turns, history);
            } else
                print_error();
        }
    }
}

//change the board after a player's regular move accordingly, and add turn to the game's history
//4 lines
void regular_move(char board[][N], int player, int row, int col, int *turns, int history[][HISTORY_STATE_LENGTH]) {
    board[row - 1][col - 1] = (player == 1 ? PLAYER_ONE_MARK : PLAYER_TWO_MARK);
    *turns += 1;
    history[*turns-1][ROW_HISTORY_INDEX] = row;
    history[*turns-1][COLUMN_HISTORY_INDEX] = col;
}

//checks if a player's regular move is valid
//6 lines
bool check_regular_move_validity(char board[][N], int row, int col, int size) {
    bool value = true;
    if (row > size || row < 1 || col > size || col < 1)
        value = false;
    else if (board[row - 1][col - 1] != BLANK)
        value = false;
    return value;
}

/*return the board to an earlier state, determined by the 'command' value,
  and delete moves from the game's history accordingly*/
//6 lines
void undo_move(char board[][N], int history[][HISTORY_STATE_LENGTH], int *turns, int command) {
    for (int i = 0; i < -command; i++) {
        int turn = *turns - 1 - i;
        //use the latest pair of [row,col] in history, to change that place back to blank
        board[(history[turn][ROW_HISTORY_INDEX]) - 1][(history[turn][COLUMN_HISTORY_INDEX]) - 1] = BLANK;
        history[turn][ROW_HISTORY_INDEX] = 0;
        history[turn][COLUMN_HISTORY_INDEX] = 0;
    }
    *turns += command;
}

//checks if a player's undo move is valid
//7 lines
bool check_undo_move_validity(int *turns, int command) {
    bool valid = true;
    command = -command;
    if (command % 2 == 0)
        valid = false;
    else if (command > *turns)
        valid = false;
    return valid;
}

//returns if the board is full
//5 lines
bool check_if_full_board(char board[][N], int size) {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (board[i][j] == BLANK)
                return false;
    return true;
}

//returns if the current player has lost
//9 lines
bool check_lose_condition(char board[][N], int size, int player) {
    bool lost = false;
    char mark = (player == PLAYER_ONE) ? PLAYER_ONE_MARK : PLAYER_TWO_MARK;
    if (check_rows(board, size, mark))
        lost = true;
    else {
        if (check_columns(board, size, mark))
            lost = true;
        else if (check_diagonals(board, size, mark))
            lost = true;
    }
    return lost;
}

//returns if a single row in the board is filled with the current player's mark, hence he lost
//5 lines
bool check_rows(char board[][N], int size, char player_mark) {
    bool lost = false;
    for (int i = 0; i < size; i++)
        if (check_row(board[i], size, player_mark))
            lost = true;
    return lost;
}

//returns if a single column in the board is filled with the current player's mark, hence he lost
//5 lines
bool check_columns(char board[][N], int size, char player_mark) {
    bool lost = false;
    for (int i = 0; i < size; i++)
        if (check_column(board, size, i, player_mark))
            lost = true;
    return lost;
}

//returns if one of the diagonals are filled with the current player's mark, hence he lost
//2 lines
bool check_diagonals(char board[][N], int size, char player_mark) {
    return check_topLeft_to_bottomRight(board, size, player_mark)
           || check_topRight_to_bottomLeft(board, size, player_mark);
}

//returns if a certain row is filled with the current player's mark, hence he lost
//5 lines
bool check_row(char row[N], int size, char player_mark) {
    bool lost = true;
    for (int i = 0; i < size; i++)
        if (row[i] != player_mark)
            lost = false;
    return lost;
}

//returns if a certain column is filled with the current player's mark, hence he lost
//5 lines
bool check_column(char board[][N], int size, int col_num, char player_mark) {
    bool lost = true;
    for (int i = 0; i < size; i++)
        if (board[i][col_num] != player_mark)
            lost = false;
    return lost;
}

//returns if the top-left to bottom-right diagonal is filled with the current player's mark, hence he lost
//4 lines
bool check_topLeft_to_bottomRight(char board[][N], int size, char player_mark) {
    for (int i = 0; i < size; i++)
        if (board[i][i] != player_mark)
            return false;
    return true;
}

//returns if the top-right to bottom-left diagonal is filled with the current player's mark, hence he lost
//4 lines
bool check_topRight_to_bottomLeft(char board[][N], int size, char player_mark) {
    for (int i = 0; i < size; i++)
        if (board[i][size - i - 1] != player_mark)
            return false;
    return true;
}

//prints the final results of the game
//3 lines
void print_results(char board[][N], int size, int player) {
    if (check_lose_condition(board, size, player))
        print_winner((player == PLAYER_ONE) ? PLAYER_TWO : PLAYER_ONE);
    else
        print_tie();
}

//print welcome message
//1 lines
void print_welcome() {
    printf("*** Welcome to AVOIDANCE TIC-TAC-TOE game ***\n\n");
}

//print message to enter board size
//1 lines
void print_enter_board_size() {
    printf("Please enter board size (1 to %d):\n", N);
}

//print the board
//7 lines
void print_board(char board[N][N], int n) {
    printf("\nCurrent board:\n");
    for (int i = 0; i < n; i++) {
        printf("|");
        for (int j = 0; j < n; j++) {
            printf("%c|", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

//print a request for player with index "player_index" to move
//1 lines
void print_player_turn(int player_index) {
    printf("\nPlayer ** %d **, enter next move:\n", player_index);
}

//print error message
//1 lines
void print_error() {
    printf("Illegal move!!!, please try again:\n");
}

//print the winner
//1 lines
void print_winner(int player_index) {
    printf("Player %d Wins! Hooray!\n", player_index);
}

//print message to announce there is a tie (no one wins)
//1 lines
void print_tie() {
    printf("It's a tie!\n");
}