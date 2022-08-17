#ifndef GAME_H
#define GAME_H
#include "types.h"

#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <cstdio>
#include <cstring>

#include <SDL.h>
#include <SDL_image.h>

//TESTING
#include <bitset>
#include <time.h>

#define WITHIN_BOUNDS(r, c) (r < 8 && r >= 0 && c < 8 && c >= 0)

#define WIDTH 1280
#define HEIGHT 720

const char* const IMG_PATHS[32] = {
"imgs/white_king.bmp",
"imgs/white_queen.bmp",
"imgs/white_rook.bmp",
"imgs/white_rook.bmp",
"imgs/white_bishop.bmp",
"imgs/white_bishop.bmp",
"imgs/white_knight.bmp",
"imgs/white_knight.bmp",
"imgs/white_pawn.bmp",
"imgs/white_pawn.bmp",
"imgs/white_pawn.bmp",
"imgs/white_pawn.bmp",
"imgs/white_pawn.bmp",
"imgs/white_pawn.bmp",
"imgs/white_pawn.bmp",
"imgs/white_pawn.bmp",

"imgs/black_king.bmp",
"imgs/black_queen.bmp",
"imgs/black_rook.bmp",
"imgs/black_rook.bmp",
"imgs/black_bishop.bmp",
"imgs/black_bishop.bmp",
"imgs/black_knight.bmp",
"imgs/black_knight.bmp",
"imgs/black_pawn.bmp",
"imgs/black_pawn.bmp",
"imgs/black_pawn.bmp",
"imgs/black_pawn.bmp",
"imgs/black_pawn.bmp",
"imgs/black_pawn.bmp",
"imgs/black_pawn.bmp",
"imgs/black_pawn.bmp"
};


enum GUI_STATE{
    WAIT,
    CLICKED_PIECE,
    EXECUTE_MOVE,
};

class Game{
public:

    uint64_t CASTLE_MASK_KINGSIDE = 0x6000000000000000LL;
    uint64_t CASTLE_MASK_QUEENSIDE = 0x0C00000000000000LL;

    uint64_t occupied[2] = {0xFFFF000000000000LL, 0x000000000000FFFFLL}; // bit map of board occupied by each player
    uint64_t enpassant = 0xFFFF00000000FFFFLL; // bit map of enpassant

    Piece* board[8][8];
    
    uint64_t pinned;

    uint64_t checking_pieces;
    uint64_t block_check;

    uint16_t defended[64] = {0};
    uint16_t attacking[64] = {0};

    Player* white;
    Player* black;

    int move_number;
    Player* CURRENT_PLAYER;
    bool check = false;

    const int FPS = 300;

    uint64_t PREVIOUS_RENDER_TICK;
    uint64_t CURRENT_TICK;

    Game();

    void update_board();

    void calculate();

    int make_move(Move& move, Piece* piece);

    void print_data();

    void init_render();

    void destroy_render();

    void draw_board();

    void draw_pieces();

    void draw_highlighted_squares(std::vector<int>& squares);

    void draw_unmoving_pieces(Piece* excluded);

    void draw_piece(Piece* piece, player_side);
    
    void erase_piece(Piece* piece);

    void highlight_square(int r, int c);

    void run();
};

#endif