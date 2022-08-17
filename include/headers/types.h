#ifndef TYPES_H
#define TYPES_H

// using namespace std;
#include <stdint.h>
#include <vector>

#define OCCUPIED(bitmap, i) ((bitmap >> i) & 0x0000000000000001LL)
#define LOCATION(r,c) ((r<<3)+c)
#define IS_ATTACKING(map, ind) ((map >> ind) & 1)

enum piece_type{
    PAWN = 0,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    EMPTY,
};

const char char_piece[7] = {'P', 'N', 'B', 'R', 'Q', 'K', ' '};

enum player_side{
    WHITE = -1,
    BLACK = 1,
};

struct Direction{
    int dr;
    int dc;
};

const Direction ds[6][4] = {{{-1, 1}, { 1, 1}, { 0, 0}, { 0, 0}},
                            {{ 2,-1}, { 2, 1}, { 1,-2}, { 1, 2}},
                            {{ 1,-1}, { 1, 1}, { 0, 0}, { 0, 0}},
                            {{ 1, 0}, { 0, 1}, { 0, 0}, { 0, 0}},
                            {{ 1, 0}, { 0, 1}, {-1, 1}, { 1, 1}},
                            {{ 1, 0}, { 0, 1}, {-1, 1}, { 1, 1}}};

const bool slide[6] = {0,0,1,1,1,0};

enum pin_type{
    NONE,
    DIAGONAL_,
    NEG_DIAGONAL,
    HORIZONTAL,
    VERTICAL,
};

struct Move{
    int r2, c2;

    bool castle;
    bool enpassant;
    Move(){

    }
    Move(int r, int c, bool e, bool cast): r2(r), c2(c), enpassant(e), castle(cast){
    }
};

struct Piece{
    piece_type type;
    pin_type pin;

    int row;
    int col;
    
    int dist;
    int numMoves;
    int ID;

    Piece* next;
    Piece* prev;

    uint64_t pinned_block;
    std::vector<Move> valid_moves;

    Piece(){

    }

    Piece(piece_type t, int r, int c, int d, int n, int i): type(t), row(r), col(c), dist(d), numMoves(n), ID(i){
        next = NULL;
        prev = NULL;
    }
};

class Player{
public:
    player_side direction;

    Piece* ITERATOR;

    Piece* king;
    Piece* queen;
    Piece* rook1;
    Piece* rook2;
    Piece* light_bishop;
    Piece* dark_bishop;
    Piece* knight1;
    Piece* knight2;
    Piece* pawn1;
    Piece* pawn2;
    Piece* pawn3;
    Piece* pawn4;
    Piece* pawn5;
    Piece* pawn6;
    Piece* pawn7;
    Piece* pawn8;
    // Piece* pieces[16];
    bool queen_side_castle = true;
    bool king_side_castle = true;

    int ID;

    std::vector<Piece*> removed;

    Player(){

    }

    //Player(8, WHITE) will be used to init white player
    //Player(-1, BLACK) will be used to init black player
    Player(int base, player_side s) {
        direction = s;
        ID = (s + 1)/2;
        // int OFFSET = 0;
        king = new Piece(KING, base + direction, 4, 1, 8, 0);
        queen = new Piece(QUEEN, base + direction, 3, 16, 8, 1);
        rook1 = new Piece(ROOK, base + direction, 0, 16, 6, 2); // queenside rook
        rook2 = new Piece(ROOK, base + direction, 7, 16, 6, 3); // kingside rook
        light_bishop = new Piece(BISHOP, base + direction, 2, 16, 4, 4);
        dark_bishop = new Piece(BISHOP, base + direction, 5, 16, 4, 5);
        knight1 = new Piece(KNIGHT, base + direction, 1, 1, 8, 6);
        knight2 = new Piece(KNIGHT, base + direction, 6, 1, 8, 7);
        int i = 0;
        pawn1 = new Piece(PAWN, base + 2 * direction, i, 2, 3, 8 + i);i++;
        pawn2 = new Piece(PAWN, base + 2 * direction, i, 2, 3, 8 + i);i++;
        pawn3 = new Piece(PAWN, base + 2 * direction, i, 2, 3, 8 + i);i++;
        pawn4 = new Piece(PAWN, base + 2 * direction, i, 2, 3, 8 + i);i++;
        pawn5 = new Piece(PAWN, base + 2 * direction, i, 2, 3, 8 + i);i++;
        pawn6 = new Piece(PAWN, base + 2 * direction, i, 2, 3, 8 + i);i++;
        pawn7 = new Piece(PAWN, base + 2 * direction, i, 2, 3, 8 + i);i++;
        pawn8 = new Piece(PAWN, base + 2 * direction, i, 2, 3, 8 + i);i++;
        king->next = queen; king->prev = NULL;
        queen->next = rook1; queen->prev = king;
        rook1->next = rook2; rook1->prev = queen;
        rook2->next = light_bishop; rook2->prev = rook1;
        light_bishop->next = dark_bishop; light_bishop->prev = rook2;
        dark_bishop->next = knight1; dark_bishop->prev = light_bishop;
        knight1->next = knight2; knight1->prev = dark_bishop;
        knight2->next = pawn1; knight2->prev = knight1;
        pawn1->next = pawn2; pawn1->prev = knight2;
        pawn2->next = pawn3; pawn2->prev = pawn1;
        pawn3->next = pawn4; pawn3->prev = pawn2;
        pawn4->next = pawn5; pawn4->prev = pawn3;
        pawn5->next = pawn6; pawn5->prev = pawn4;
        pawn6->next = pawn7; pawn6->prev = pawn5;
        pawn7->next = pawn8; pawn7->prev = pawn6;
        pawn8->next = NULL; pawn8->prev = pawn7;
        ITERATOR = new Piece();
        ITERATOR->next = king;
    }

    // void addLast(){
    //     Piece* piece = removed.back();
    //     removed.pop_back();
    //     pieces[piece->prev]->next = piece->ID;
    //     pieces[piece->next]->prev = piece->ID;

    // }

    void remove(Piece* piece){
        removed.push_back(piece);
        piece->prev->next = piece->next;
        piece->next->prev = piece->prev;
    }
};

#endif