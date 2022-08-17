#include "game.h"

SDL_Texture* PIECE_TEXTURES[32];

SDL_Rect RECT_BOARD[8][8];

SDL_Renderer* renderer;
SDL_Window* window;

int UNIT = 134 / 2; // img width / 2
int BOARD_BORDER = UNIT * 8;

SDL_Rect srcRect;

int X_OFFSET;
int Y_OFFSET;

Game::Game(){
    move_number = 0;
    white = new Player(8, WHITE);
    black = new Player(-1, BLACK);

    update_board();
    // std::cout<<"edited\n";
    calculate();
    // std::cout<<"NUM PAWN MOVES"<<white->pawn1->valid_moves.size()<<"\n";
    // std::cout<<"calculated\n";
}

void Game::update_board(){
    Piece* curr = white->ITERATOR;
    while(curr->next){
        curr = curr->next;
        board[curr->row][curr->col] = curr;
    }

    curr = black->ITERATOR;
    while(curr->next){
        curr = curr->next;
        board[curr->row][curr->col] = curr;
    }
}

void Game::calculate(){
    memset(attacking, 0, 128);
    memset(defended, 0, 128);
    check = false;

    pinned = 0LL;

    checking_pieces = 0LL;
    block_check = 0LL;
    block_check = ~block_check;

    //compute squares attacked by defender (defended)
    Player* defender = ((move_number + 1) & 1) == 0? white: black;
    Player* attacker = ((move_number) & 1) == 0? white: black;

    int kingR = attacker->king->row;
    int kingC = attacker->king->col;
    bool canBlock = true;
    // if(!defender){
    //     std::cout<<"player is null\n";
    // }
    int currPlayerID = (move_number + 1) & 1;
    // std::cout<<defender<<"\n";
    Piece* curr = defender->king;
    int r2, c2;
    for(const Direction& d: ds[KING]){ // PROCESS KING MOVES
        r2 = curr->row + d.dr;
        c2 = curr->col + d.dc;
        if(WITHIN_BOUNDS(r2, c2)){
            defended[LOCATION(r2,c2)] |= (1 << curr->ID);
        }

        r2 = curr->row - d.dr;
        c2 = curr->col - d.dc;
        if(WITHIN_BOUNDS(r2, c2)){
            defended[LOCATION(r2,c2)] |= (1 << curr->ID);
        }
    }
    while(curr->next && curr->next->type != PAWN){ // not pawns
        curr = curr->next;
        for(const Direction& d: ds[curr->type]){
            r2 = curr->row;
            c2 = curr->col;
            for(int j = 0; j < curr->dist; j++){
                r2 += d.dr;
                c2 += d.dc;
                if(!WITHIN_BOUNDS(r2, c2)){
                    break;
                }
                defended[LOCATION(r2,c2)] |= (1 << curr->ID);
                if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2)) || OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2))){
                    if(curr->type == KNIGHT){
                        if(r2 == kingR && c2 == kingC){
                            canBlock = false;
                            check = true;
                            checking_pieces |= (1LL << LOCATION(curr->row, curr->col));
                        }
                        break;
                    }else if(r2 == kingR && c2 == kingC){
                        check = true;
                        checking_pieces |= (1LL << LOCATION(curr->row, curr->col));
                        uint64_t this_block_check = 0LL;
                        for(; j >= 0; j--){
                            this_block_check |= (1LL << LOCATION(r2,c2));
                            r2 -= d.dr;
                            c2 -= d.dc;
                        }
                        block_check &= this_block_check;
                    }else{ // keep iterating as "xray" to check for pin
                        for(j++; j < curr->dist; j++){
                            int r3 = r2;
                            int c3 = c2;
                            r2 += d.dr;
                            c2 += d.dc;
                            if(!WITHIN_BOUNDS(r2, c2)){
                                break;
                            }
                            if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
                                if(r2 == kingR && c2 == kingC){ //there is a pin
                                    Piece* pinnedPiece = board[r3][c3];
                                    uint64_t this_pinned_block = 0LL;
                                    for(; j >= 0; j--){
                                        this_pinned_block |= (1LL << (LOCATION(r2, c2)));
                                        r2 -= d.dr;
                                        c2 -= d.dc;
                                    }
                                    pinned |= (1LL << (LOCATION(r3, c3)));
                                    this_pinned_block |= (1LL << (LOCATION(r2, c2)));
                                    pinnedPiece->pinned_block = this_pinned_block;
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
            }

            r2 = curr->row;
            c2 = curr->col;
            for(int j = 0; j < curr->dist; j++){
                r2 -= d.dr;
                c2 -= d.dc;
                if(!WITHIN_BOUNDS(r2, c2)){
                    break;
                }
                defended[LOCATION(r2,c2)] |= (1 << curr->ID);
                if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2)) || OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2))){
                    if(curr->type == KNIGHT){
                        if(r2 == kingR && c2 == kingC){
                            canBlock = false;
                            check = true;
                            checking_pieces |= (1LL << LOCATION(curr->row, curr->col));
                        }
                        break;
                    }else if(r2 == kingR && c2 == kingC){
                        check = true;
                        checking_pieces |= (1LL << LOCATION(curr->row, curr->col));
                        uint64_t this_block_check = 0LL;
                        for(; j >= 0; j--){
                            this_block_check |= (1LL << LOCATION(r2,c2));
                            r2 += d.dr;
                            c2 += d.dc;
                        }
                        block_check &= this_block_check;
                    }else{ // keep iterating as "xray" to check for pin
                        for(j++; j < curr->dist; j++){
                            int r3 = r2;
                            int c3 = c2;
                            r2 -= d.dr;
                            c2 -= d.dc;
                            if(!WITHIN_BOUNDS(r2, c2)){
                                break;
                            }
                            if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
                                if(r2 == kingR && c2 == kingC){ //there is a pin
                                    Piece* pinnedPiece = board[r3][c3];
                                    uint64_t this_pinned_block = 0LL;
                                    for(; j >= 0; j--){
                                        this_pinned_block |= (1LL << (LOCATION(r2, c2)));
                                        r2 += d.dr;
                                        c2 += d.dc;
                                    }
                                    pinned |= (1LL << (LOCATION(r3, c3)));
                                    this_pinned_block |= (1LL << (LOCATION(r2, c2)));
                                    pinnedPiece->pinned_block = this_pinned_block;
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    while(curr->next){ // pawns
        curr = curr->next;

        //captures diagonally
        r2 = curr->row + defender->direction;
        c2 = curr->col - 1;
        if(WITHIN_BOUNDS(r2,c2)){
            defended[LOCATION(r2,c2)] |= (1 << curr->ID);
            if(kingR == r2 && kingC == c2){
                checking_pieces |= (1LL << LOCATION(curr->row, curr->col));
            }
        }
        c2 = curr->col + 1;
        if(WITHIN_BOUNDS(r2,c2)){
            defended[LOCATION(r2,c2)] |= (1 << curr->ID);
            if(kingR == r2 && kingC == c2){
                checking_pieces |= (1LL << LOCATION(curr->row, curr->col));
            }
        }
    }
    //---------------------------------------------------------------------------------------------------------
    //compute attacked with current player
    attacker = (move_number & 1) == 0? white: black;
    currPlayerID ^= 1;

    curr = attacker->king; // ADD CASTLING RULES LATER
    curr->valid_moves.clear();
    check = defended[curr->row * 8 + curr->col] != 0;
    if(!check){
        if(attacker->queen_side_castle){
            if(!(occupied[currPlayerID] & (CASTLE_MASK_QUEENSIDE >> (currPlayerID * 56))) && !defended[58 - (currPlayerID * 56)] && !defended[59 - (currPlayerID * 56)]){
                attacking[58 - (currPlayerID * 56)] |= (1 << curr->ID);
                curr->valid_moves.push_back(Move(curr->row, 2, false, true));
            }
        }
        if(attacker->king_side_castle){
            if(!(occupied[currPlayerID] & (CASTLE_MASK_KINGSIDE >> (currPlayerID * 56))) && !defended[62 - (currPlayerID * 56)] && !defended[61 - (currPlayerID * 56)]){
                attacking[62 - (currPlayerID * 56)] |= (1 << curr->ID);
                curr->valid_moves.push_back(Move(curr->row, 6, false, true));
            }
        }

        for(const Direction& d: ds[KING]){ // PROCESS KING MOVES
            r2 = curr->row + d.dr;
            c2 = curr->col + d.dc;
            // std::cout<<(WITHIN_BOUNDS(r2, c2))<<","<<(!OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2)))<<","<<(!defended[LOCATION(r2, c2)])<<"\n";
            if(WITHIN_BOUNDS(r2, c2) && !OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2)) && !defended[LOCATION(r2, c2)]){
                attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                curr->valid_moves.push_back(Move(r2, c2, false, false));
            }
            r2 = curr->row - d.dr;
            c2 = curr->col - d.dc;
            // std::cout<<(WITHIN_BOUNDS(r2, c2))<<","<<(!OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2)))<<","<<(!defended[LOCATION(r2, c2)])<<"\n\n\n";
            if(WITHIN_BOUNDS(r2, c2) && !OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2)) && !defended[LOCATION(r2, c2)]){
                attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                curr->valid_moves.push_back(Move(r2, c2, false, false));
            }
        }
        while(curr->next && curr->next->type != PAWN){ // not pawns
            curr = curr->next;
            curr->valid_moves.clear();
            uint64_t pinMask = OCCUPIED(pinned, LOCATION(curr->row, curr->col))? curr->pinned_block : -1;
            for(const Direction& d: ds[curr->type]){
                r2 = curr->row;
                c2 = curr->col;
                for(int j = 0; j < curr->dist; j++){
                    r2 += d.dr;
                    c2 += d.dc;
                    if(!WITHIN_BOUNDS(r2, c2)){
                        break;
                    }
                    if(OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2))){
                        break;
                    }
                    if(OCCUPIED(pinMask, LOCATION(r2,c2))){
                        attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                        curr->valid_moves.push_back(Move(r2, c2, false, false));
                    }
                    if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
                        break;
                    }
                }
                r2 = curr->row;
                c2 = curr->col;
                for(int j = 0; j < curr->dist; j++){
                    r2 -= d.dr;
                    c2 -= d.dc;
                    if(!WITHIN_BOUNDS(r2, c2)){
                        break;
                    }
                    if(OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2))){
                        break;
                    }
                    if(OCCUPIED(pinMask, LOCATION(r2,c2))){
                        attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                        curr->valid_moves.push_back(Move(r2, c2, false, false));
                    }
                    if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
                        break;
                    }
                }
            }
        }
        while(curr->next){ // pawns
            curr = curr->next;
            curr->valid_moves.clear();
            uint64_t pinMask = OCCUPIED(pinned, LOCATION(curr->row, curr->col))? curr->pinned_block : -1;
            int r2 = curr->row;
            int c2 = curr->col;
            for(int i = 0; i < curr->dist; i++){
                r2 += attacker->direction;
                if(!WITHIN_BOUNDS(r2, c2)){
                    break;
                }
                if(OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2)) || OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
                    break;
                }
                if(OCCUPIED(pinMask, LOCATION(r2,c2))){
                    attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                    curr->valid_moves.push_back(Move(r2, c2, false, false));
                }
            }

            //captures diagonally
            r2 = curr->row + attacker->direction;
            c2 = curr->col - 1;
            if(WITHIN_BOUNDS(r2,c2)){
                if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2)) && OCCUPIED(pinMask, LOCATION(r2,c2))){
                    attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                    curr->valid_moves.push_back(Move(r2, c2, false, false));
                }
                if(OCCUPIED(enpassant, LOCATION(r2,c2)) && OCCUPIED(pinMask, LOCATION(r2,c2))){
                    attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                    curr->valid_moves.push_back(Move(r2, c2, true, false));
                }
            }
            c2 = curr->col + 1;
            if(WITHIN_BOUNDS(r2,c2)){
                if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2)) && OCCUPIED(pinMask, LOCATION(r2,c2))){
                    attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                    curr->valid_moves.push_back(Move(r2, c2, false, false));
                }
                if(OCCUPIED(enpassant, LOCATION(r2,c2)) && OCCUPIED(pinMask, LOCATION(r2,c2))){
                    attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                    curr->valid_moves.push_back(Move(r2, c2, true, false));
                }
            }  
        }
    }else{
        for(const Direction& d: ds[KING]){ // PROCESS KING MOVES normally!!
            r2 = curr->row + d.dr;
            c2 = curr->col + d.dc;
            // std::cout<<(WITHIN_BOUNDS(r2, c2))<<","<<(!OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2)))<<","<<(!defended[LOCATION(r2, c2)])<<"\n";
            if(WITHIN_BOUNDS(r2, c2) && !OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2)) && !defended[LOCATION(r2, c2)]){
                attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                curr->valid_moves.push_back(Move(r2, c2, false, false));
            }
            r2 = curr->row - d.dr;
            c2 = curr->col - d.dc;
            // std::cout<<(WITHIN_BOUNDS(r2, c2))<<","<<(!OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2)))<<","<<(!defended[LOCATION(r2, c2)])<<"\n\n\n";
            if(WITHIN_BOUNDS(r2, c2) && !OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2)) && !defended[LOCATION(r2, c2)]){
                attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                curr->valid_moves.push_back(Move(r2, c2, false, false));
            }
        }
        uint64_t valid_squares;
        while(curr->next && curr->next->type != PAWN){ // not pawns
            curr = curr->next;
            valid_squares = (checking_pieces | block_check) & (OCCUPIED(pinned, LOCATION(curr->row, curr->col))? curr->pinned_block : -1);
            curr->valid_moves.clear();
            for(const Direction& d: ds[curr->type]){
                r2 = curr->row;
                c2 = curr->col;
                for(int j = 0; j < curr->dist; j++){
                    r2 += d.dr;
                    c2 += d.dc;
                    if(!WITHIN_BOUNDS(r2, c2)){
                        break;
                    }
                    if(OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2))){
                        break;
                    }
                    if(OCCUPIED(valid_squares, LOCATION(r2,c2))){
                        attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                        curr->valid_moves.push_back(Move(r2, c2, false, false));
                    }
                    if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
                        break;
                    }
                }
                r2 = curr->row;
                c2 = curr->col;
                for(int j = 0; j < curr->dist; j++){
                    r2 -= d.dr;
                    c2 -= d.dc;
                    if(!WITHIN_BOUNDS(r2, c2)){
                        break;
                    }
                    if(OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2))){
                        break;
                    }
                    if(OCCUPIED(valid_squares, LOCATION(r2,c2))){
                        attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                        curr->valid_moves.push_back(Move(r2, c2, false, false));
                    }
                    if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
                        break;
                    }
                }
            }
        }
        while(curr->next){ // pawns
            curr = curr->next;
            curr->valid_moves.clear();
            valid_squares = (checking_pieces | block_check) & (OCCUPIED(pinned, LOCATION(curr->row, curr->col))? curr->pinned_block : -1);
            int r2 = curr->row;
            int c2 = curr->col;
            for(int i = 0; i < curr->dist; i++){
                r2 += attacker->direction;
                if(!WITHIN_BOUNDS(r2, c2)){
                    break;
                }
                if(OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2)) || OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
                    break;
                }
                if(OCCUPIED(valid_squares, LOCATION(r2,c2))){
                    attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                    curr->valid_moves.push_back(Move(r2, c2, false, false));
                }
            }

            //captures diagonally
            r2 = curr->row + attacker->direction;
            c2 = curr->col - 1;
            if(WITHIN_BOUNDS(r2,c2)){
                if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
                    if(OCCUPIED(valid_squares, LOCATION(r2,c2))){
                        attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                        curr->valid_moves.push_back(Move(r2, c2, false, false));
                    }
                }
                if(OCCUPIED(enpassant, LOCATION(r2,c2))){
                    if(OCCUPIED(valid_squares, LOCATION(r2,c2))){
                        attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                        curr->valid_moves.push_back(Move(r2, c2, false, false));
                    }
                }
            }
            c2 = curr->col + 1;
            if(WITHIN_BOUNDS(r2,c2)){
                if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
                    if(OCCUPIED(valid_squares, LOCATION(r2,c2))){
                        attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                        curr->valid_moves.push_back(Move(r2, c2, false, false));
                    }
                }
                if(OCCUPIED(enpassant, LOCATION(r2,c2))){
                    if(OCCUPIED(valid_squares, LOCATION(r2,c2))){
                        attacking[LOCATION(r2,c2)] |= (1 << curr->ID);
                        curr->valid_moves.push_back(Move(r2, c2, false, false));
                    }
                }
            }  
        }
    }

    enpassant = 0;
    // print_data();
}

void Game::print_data(){
    std::cout<<WITHIN_BOUNDS(7,7)<<"\n";
    piece_type board[8][8];
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            board[i][j] = EMPTY;
        }
    }
    bool isBlack[8][8] = {false};
    Piece p = *white->king;
    while(true){
        board[p.row][p.col] = p.type;
        if(p.next){
            p = *p.next;
        }else{
            break;
        }
    }
    p = *black->king;
    while(true){
        board[p.row][p.col] = p.type;
        isBlack[p.row][p.col] = true;
        if(p.next){
            p = *p.next;
        }else{
            break;
        }
    }
    for(int i = 0; i < 8; i++){
        printf("_________________________________\n");
        for(int j = 0; j < 8; j++){
            if(board[i][j] == EMPTY){
                printf("|   ");
            }else{
                if(isBlack[i][j]){
                    printf("|(%c)", char_piece[board[i][j]]);
                }else{
                    printf("|[%c]", char_piece[board[i][j]]);
                }
            }
            
        }
        printf("|\n");
    }
    printf("_________________________________\nATTACKED:\n");

    int ind =15;
    for(int i = 0; i < 8; i++){
        printf("_________________________________\n");
        for(int j = 0; j < 8; j++){
            if(attacking[LOCATION(i,j)]){
            // if(IS_ATTACKING(attacking[0][i][j], ind)){
                printf("| 1 ");
            }else{
                printf("| 0 ");
            }
            
        }
        printf("|\n");
    }
    printf("_________________________________\nDEFENDED:\n");
    for(int i = 0; i < 8; i++){
        printf("_________________________________\n");
        for(int j = 0; j < 8; j++){
            if(defended[LOCATION(i,j)]){
            // if(IS_ATTACKING(attacking[1][i][j], ind)){
                printf("| 1 ");
            }else{
                printf("| 0 ");
            }
            
        }
        printf("|\n");
    }
    printf("_________________________________\n");
    // //PRINTS OCCUPIED
    // std::cout<<"PINNEDBLOCK\n";
    // for(int i = 0; i < 8; i++){
    //     for(int j = 0; j < 8; j++){
    //         std::cout<< (OCCUPIED(pinned_block, LOCATION(i,j)));
    //     }
    //     std::cout<<"\n";
    // }
    printf("_________________________________\n");
    std::cout<<"PINNED\n";
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            std::cout<< (OCCUPIED(pinned, LOCATION(i,j)));
        }
        std::cout<<"\n";
    }
    printf("_________________________________\n");
    std::cout<<"CHECKED_BLOCK\n";
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            std::cout<< (OCCUPIED(block_check, LOCATION(i,j)));
        }
        std::cout<<"\n";
    }
    printf("_________________________________\n");
    std::cout<<"CHECKING PIECES\n";
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            std::cout<< (OCCUPIED(checking_pieces, LOCATION(i,j)));
        }
        std::cout<<"\n";
    }
}

int Game::make_move(Move& move, Piece* currPiece){
    int currPlayer = move_number & 1;
    if(currPiece->type == KING){
        CURRENT_PLAYER->king_side_castle = false;
        CURRENT_PLAYER->queen_side_castle = false;
    }else if(currPiece->ID == 2){
        CURRENT_PLAYER->queen_side_castle = false;
    }else if(currPiece->ID == 3){
        CURRENT_PLAYER->king_side_castle = false;
    }

    if(move.castle){
        if(move.c2 == 2){ // queenside castle
            occupied[currPlayer] ^= (0x110000000000000LL >> (currPlayer * 56));
            occupied[currPlayer] |= (CASTLE_MASK_QUEENSIDE >> (currPlayer * 56));
            currPiece->col = 2;
            CURRENT_PLAYER->rook1->col = 3; 
        }else{ // kingside castle
            std::cout<<"KINGSIDE CASTLE!!!!\n";
            occupied[currPlayer] ^= (0x0900000000000000LL >> (currPlayer * 56));
            occupied[currPlayer] |= (CASTLE_MASK_KINGSIDE >> (currPlayer * 56));
            currPiece->col = 6;
            CURRENT_PLAYER->rook2->col = 5; 
        }
    }else{
        if(currPiece->ID >= 8){
            currPiece->dist = 1;
        }
        occupied[currPlayer] ^= 0x0000000000000001LL << (8 * currPiece->row + currPiece->col);
        currPiece->row = move.r2;
        currPiece->col = move.c2;

        occupied[currPlayer] |= 0x0000000000000001LL << (8 * move.r2 + move.c2);
        currPlayer ^= 1;
        
        //CAPTURES
        if(OCCUPIED(occupied[currPlayer], LOCATION(move.r2, move.c2))){
            occupied[currPlayer] ^= 0x0000000000000001LL << (8 * move.r2 + move.c2);
            Player* player = currPlayer == 0? white: black;
            Piece* curr = player->ITERATOR;
            while(curr->next){
                curr = curr->next;
                if(curr->row == move.r2 && curr->col == move.c2){
                    player->remove(curr);
                    break;
                }
            }
        }
    }
    move_number++;
    CURRENT_PLAYER = CURRENT_PLAYER == white? black: white;
    update_board();
    return 1;
}

void Game::run(){
    clock_t tStart = clock();
    for(int i = 0; i < 1000000; i++){
        calculate();
    }
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

    GUI_STATE STATE = WAIT;
    SDL_Event event;

    Piece* currPiece; //piece to be interacted with
    std::vector<int> highlighted_squares;
    SDL_Rect pieceRect;
    pieceRect.w = UNIT;
    pieceRect.h = UNIT;
    int thisInd;
    int mouseR;
    int mouseC;

    Move move;

    bool isPressed = false;

    int xMouse = 0;
    int yMouse = 0;

    init_render();

    CURRENT_PLAYER = white;
    while(true){
        switch(STATE){
        case WAIT:
            while(SDL_PollEvent(&event)){
                if(event.type == SDL_MOUSEBUTTONUP && event.button.x >= 0 && event.button.x < BOARD_BORDER && event.button.y >= 0 && event.button.y < BOARD_BORDER){
                    int x = event.button.x;
                    int y = event.button.y;
                    int r = y / UNIT;
                    int c = x / UNIT;
                    Piece* p = CURRENT_PLAYER->ITERATOR;
                    isPressed = false;
                    while(p->next){
                        p = p->next;
                        if(p->row == r && p->col == c){
                            STATE = CLICKED_PIECE;
                            currPiece = p;
                            isPressed = true;
                            SDL_GetGlobalMouseState(&X_OFFSET, &Y_OFFSET);
                            Y_OFFSET -= (r * UNIT);
                            X_OFFSET -= (c * UNIT);
                            Y_OFFSET -= (y - (r * UNIT + UNIT / 2));
                            X_OFFSET -= (x - (c * UNIT + UNIT / 2));
                        }
                    }
                    if(isPressed){
                        for(int i = 0; i < 8; i++){
                            for(int j = 0; j < 8; j++){
                                if(IS_ATTACKING(attacking[LOCATION(i,j)], currPiece->ID)){
                                    highlighted_squares.push_back(i * 8 + j);
                                }
                            }
                        }
                        // SDL_RenderPresent(renderer);
                    }
                }
            }
            break;
        case CLICKED_PIECE:
            erase_piece(currPiece);
            SDL_RenderPresent(renderer);
            thisInd = currPiece->ID + (move_number % 2) * 16;
            // bool canEnd = false;
            while(event.type != SDL_MOUSEBUTTONUP){
                SDL_GetGlobalMouseState(&pieceRect.x, &pieceRect.y);
                pieceRect.x -= X_OFFSET;
                pieceRect.y -= Y_OFFSET;

                CURRENT_TICK = SDL_GetTicks64();
                if((CURRENT_TICK - PREVIOUS_RENDER_TICK) * FPS >= 1000){
                    draw_board();
                    draw_highlighted_squares(highlighted_squares);
                    draw_unmoving_pieces(currPiece);
                    SDL_RenderCopy(renderer, PIECE_TEXTURES[thisInd], NULL, &pieceRect);
                    SDL_RenderPresent(renderer);
                    
                    PREVIOUS_RENDER_TICK = CURRENT_TICK;
                }
                // SDL_Delay(1500);
                SDL_PollEvent(&event);
            }
            highlighted_squares.clear();
            mouseR = (pieceRect.y + (UNIT / 2)) / UNIT;
            mouseC = (pieceRect.x + (UNIT / 2)) / UNIT;

            // std::cout<<"NUM MOVES"<<currPiece->valid_moves.size()<<"\n";
            for(const Move& m: currPiece->valid_moves){
                if(mouseR == m.r2 && mouseC == m.c2){
                    STATE = EXECUTE_MOVE;
                    move = m;
                }
            }
            if(STATE == EXECUTE_MOVE){
                continue;
            }else{
                std::cout<<"invalid square\n";
                draw_board();
                draw_pieces();
                std::cout<<"rendered\n";
                STATE = WAIT;
            }
            break;
        case EXECUTE_MOVE:
            // print_data();
            make_move(move, currPiece);
            // print_data();
            calculate();

            draw_board();
            draw_pieces();
            
            // CURRENT_PLAYER = CURRENT_PLAYER == white? black: white;

            STATE = WAIT;
            break;
        default:
            break;
        }
        CURRENT_TICK = SDL_GetTicks64();
        if((CURRENT_TICK - PREVIOUS_RENDER_TICK) * FPS >= 1000){
            PREVIOUS_RENDER_TICK = CURRENT_TICK;
            SDL_RenderPresent(renderer);
            // SDL_RenderClear(renderer);
        }
    }
    destroy_render();
    // print_data();
    free(currPiece);

    // Move move(0,0,11, 4, 3, 0);
    // make_move(move);
    // print_data();
}

void Game::init_render(){
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "FAILEDED TO INIT\n";
    if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "FAILEDED AT CREATEWINDOWRENDERER\n";
    SDL_SetWindowTitle(window, "Chess");
    SDL_ShowCursor(1); //show cursor on screen
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); //renders pixels better?

    SDL_GL_SetSwapInterval(0);
    // IMG_Init(IMG_INIT_PNG);

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = UNIT;
    srcRect.h = UNIT;

    for(int i = 0; i < 8; i++){ //initialize board rect
        for(int j = 0; j < 8; j++){
            SDL_Rect curr;
            curr.x = j * UNIT;
            curr.y = i * UNIT;
            curr.h = UNIT;
            curr.w = UNIT;
            RECT_BOARD[i][j] = curr;
        }
    }

    draw_board();

    for(int i = 0; i < 32; i++){ // initiliaze pieces
        PIECE_TEXTURES[i] = IMG_LoadTexture(renderer, IMG_PATHS[i]);
        if(!PIECE_TEXTURES[i]){
            std::cout<<i<<IMG_PATHS[i]<<"!!!!!"<<IMG_GetError()<<"wrongpathbruh\n";
        }
        // PIECE_SURFACES[i] = IMG_Load(IMG_PATHS[i]);
        // PIECE_TEXTURES[i] = SDL_CreateTextureFromSurface(renderer, PIECE_SURFACES[i]);
        // if(!PIECE_TEXTURES[i]){
        //     std::cout<<i<<IMG_PATHS[i]<<"wrongpathbruh\n";
        // }
    }

    draw_pieces();
    SDL_RenderPresent(renderer);
}

void Game::draw_board(){
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++){
            if((i + j) % 2){
                SDL_SetRenderDrawColor(renderer, 171, 110, 60, 255);
            }else{
                SDL_SetRenderDrawColor(renderer, 242, 204, 138, 255);
            }   
            SDL_RenderFillRect(renderer, &RECT_BOARD[i][j]);
        }
    }
    // SDL_RenderPresent(renderer);
}

void Game::draw_highlighted_squares(std::vector<int>& squares){
    for(const int& coord: squares){
        highlight_square(coord / 8, coord % 8);
    }
    // SDL_RenderPresent(renderer);
}

void Game::draw_pieces(){
    Piece* piece = white->ITERATOR;
    while(piece->next){
        piece = piece->next;
        draw_piece(piece, WHITE);
    }
    piece = black->ITERATOR;
    while(piece->next){
        piece = piece->next;
        draw_piece(piece, BLACK);
    } 
}

void Game::draw_unmoving_pieces(Piece* excluded){
    Piece* curr = white->ITERATOR;
    while(curr->next){
        curr = curr->next;
        if(curr != excluded){
            draw_piece(curr, WHITE);
        }
    }
    curr = black->ITERATOR;
    while(curr->next){
        curr = curr->next;
        if(curr != excluded){
            draw_piece(curr, BLACK);
        }
    }
}

void Game::draw_piece(Piece* piece, player_side side){
    SDL_Texture* tempTex = PIECE_TEXTURES[piece->ID + (side + 1)/2 * 16];
    // std::cout<<piece.ID + (side + 1)/2 * 16<<"\n";
    // SDL_RenderCopy(renderer, tempTex, &srcRect, &RECT_BOARD[piece.row][piece.col]);
    SDL_RenderCopy(renderer, tempTex, NULL, &RECT_BOARD[piece->row][piece->col]);
}

void Game::erase_piece(Piece* piece){
    if((piece->row + piece->col) % 2){
        SDL_SetRenderDrawColor(renderer, 171, 110, 60, 255);
    }else{
        SDL_SetRenderDrawColor(renderer, 242, 204, 138, 255);
    }   
    SDL_RenderFillRect(renderer, &RECT_BOARD[piece->row][piece->col]);
}

void Game::highlight_square(int r, int c){
    if((r + c) % 2){
        SDL_SetRenderDrawColor(renderer, 171, 110, 150, 255);
    }else{
        SDL_SetRenderDrawColor(renderer, 242, 204, 228, 255);
    }   
    SDL_RenderFillRect(renderer, &RECT_BOARD[r][c]);
}

void Game::destroy_render(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    // free(PIECE_TEXTURES);
    SDL_Quit();
}