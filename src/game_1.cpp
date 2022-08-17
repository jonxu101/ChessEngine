// #include "game.h"

// SDL_Texture* PIECE_TEXTURES[32];

// SDL_Rect RECT_BOARD[8][8];

// SDL_Renderer* renderer;
// SDL_Window* window;

// int UNIT = 134 / 2; // img width / 2
// int BOARD_BORDER = UNIT * 8;

// SDL_Rect srcRect;

// int X_OFFSET;
// int Y_OFFSET;

// Game::Game(){
//     move_number = 0;
//     white = new Player(8, WHITE);
//     black = new Player(-1, BLACK);
//     std::cout<<"edited\n";
//     calculate();
//     std::cout<<"calculated\n";
// }

// void Game::calculate(){
//     memset(attacking, 0, 128);
//     memset(defended, 0, 128);

//     //compute squares attacked by defender (defended)
//     Player* currPlayer = ((move_number + 1) & 1) == 0? white: black;
//     if(!currPlayer){
//         std::cout<<"player is null\n";
//     }
//     int currPlayerID = (move_number + 1) & 1;
//     // std::cout<<currPlayer<<"\n";
//     Piece curr = *currPlayer->king; // ADD CASTLING RULES LATER
//     for(int i = 0; i < curr.numMoves; i++){ // PROCESS KING MOVES
//         int r2 = curr.row + dr[curr.type][i];
//         int c2 = curr.col + dc[curr.type][i];
//         if(!WITHIN_BOUNDS(r2, c2)){
//             continue;
//         }
//         defended[LOCATION(r2,c2)] |= (1 << curr.ID);
//     }
//     while(curr.next && curr.next->type != PAWN){ // not pawns
//         curr = *curr.next;
//         for(int i = 0; i < curr.numMoves; i++){
//             int r2 = curr.row;
//             int c2 = curr.col;
//             for(int j = 0; j < curr.dist; j++){
//                 r2 += dr[curr.type][i];
//                 c2 += dc[curr.type][i];
//                 if(!WITHIN_BOUNDS(r2, c2)){
//                     break;
//                 }
//                 defended[LOCATION(r2,c2)] |= (1 << curr.ID);
//                 if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2)) || OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2))){
//                     break;
//                 }
//             }
//         }
//     }
//     while(curr.next){ // pawns
//         curr = *curr.next;

//         //captures diagonally
//         int r2 = curr.row + currPlayer->color;
//         int c2 = curr.col - 1;
//         if(WITHIN_BOUNDS(r2,c2)){
//             defended[LOCATION(r2,c2)] |= (1 << curr.ID);
//         }
//         c2 = curr.col + 1;
//         if(WITHIN_BOUNDS(r2,c2)){
//             defended[LOCATION(r2,c2)] |= (1 << curr.ID);
//         }
//     }
//     //---------------------------------------------------------------------------------------------------------
//     //compute attacked with current player
//     currPlayer = (move_number & 1) == 0? white: black;
//     currPlayerID ^= 1;

//     curr = *currPlayer->ITERATOR; // ADD CASTLING RULES LATER
//     for(int i = 0; i < curr.numMoves; i++){ // PROCESS KING MOVES
//         int r2 = curr.row + dr[curr.type][i];
//         int c2 = curr.col + dc[curr.type][i];
//         if(!WITHIN_BOUNDS(r2, c2)){
//             continue;
//         }
//         if(OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2)) || defended[LOCATION(r2, c2)]){
//             continue;
//         }
//         attacking[LOCATION(r2,c2)] |= (1 << curr.ID);
//         valid_moves.push_back(Move(curr.row, curr.col, &curr, r2, c2, false));
//     }
//     while(curr.next && curr.next->type != PAWN){ // not pawns
//         curr = *curr.next;
//         for(int i = 0; i < curr.numMoves; i++){
//             int r2 = curr.row;
//             int c2 = curr.col;
//             for(int j = 0; j < curr.dist; j++){
//                 r2 += dr[curr.type][i];
//                 c2 += dc[curr.type][i];
//                 if(!WITHIN_BOUNDS(r2, c2)){
//                     break;
//                 }
//                 if(OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2))){
//                     break;
//                 }
//                 attacking[LOCATION(r2,c2)] |= (1 << curr.ID);
//                 valid_moves.push_back(Move(curr.row, curr.col, &curr, r2, c2, false));
//                 if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
//                     break;
//                 }
//             }
//         }
//     }
//     while(curr.next){ // pawns
//         curr = *curr.next;
//         int r2 = curr.row;
//         int c2 = curr.col;
//         for(int i = 0; i < curr.dist; i++){
//             r2 += currPlayer->color;
//             if(!WITHIN_BOUNDS(r2, c2)){
//                 break;
//             }
//             if(OCCUPIED(occupied[currPlayerID], LOCATION(r2,c2)) || OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
//                 break;
//             }
//             attacking[LOCATION(r2,c2)] |= (1 << curr.ID);
//             valid_moves.push_back(Move(curr.row, curr.col, &curr, r2, c2, false));
//         }

//         //captures diagonally
//         r2 = curr.row + currPlayer->color;
//         c2 = curr.col - 1;
//         if(WITHIN_BOUNDS(r2,c2)){
//             if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
//                 attacking[LOCATION(r2,c2)] |= (1 << curr.ID);
//                 valid_moves.push_back(Move(curr.row, curr.col, &curr, r2, c2, false));
//             }
//             if(OCCUPIED(enpassant, LOCATION(r2,c2))){
//                 attacking[LOCATION(r2,c2)] |= (1 << curr.ID);
//                 valid_moves.push_back(Move(curr.row, curr.col, &curr, r2, c2, true));
//             }
//         }
//         c2 = curr.col + 1;
//         if(WITHIN_BOUNDS(r2,c2)){
//             if(OCCUPIED(occupied[currPlayerID ^ 1], LOCATION(r2,c2))){
//                 attacking[LOCATION(r2,c2)] |= (1 << curr.ID);
//                 valid_moves.push_back(Move(curr.row, curr.col, &curr, r2, c2, false));
//             }
//             if(OCCUPIED(enpassant, LOCATION(r2,c2))){
//                 attacking[LOCATION(r2,c2)] |= (1 << curr.ID);
//                 valid_moves.push_back(Move(curr.row, curr.col, &curr, r2, c2, true));
//             }
//         }   
//     }
//     enpassant = 0;
//     // print_data();
// }

// void Game::print_data(){
//     std::cout<<WITHIN_BOUNDS(7,7)<<"\n";
//     piece_type board[8][8];
//     for(int i = 0; i < 8; i++){
//         for(int j = 0; j < 8; j++){
//             board[i][j] = EMPTY;
//         }
//     }
//     bool isBlack[8][8] = {false};
//     Piece p = *white->king;
//     while(true){
//         board[p.row][p.col] = p.type;
//         if(p.next){
//             p = *p.next;
//         }else{
//             break;
//         }
//     }
//     p = *black->king;
//     while(true){
//         board[p.row][p.col] = p.type;
//         isBlack[p.row][p.col] = true;
//         if(p.next){
//             p = *p.next;
//         }else{
//             break;
//         }
//     }
//     for(int i = 0; i < 8; i++){
//         printf("_________________________________\n");
//         for(int j = 0; j < 8; j++){
//             if(board[i][j] == EMPTY){
//                 printf("|   ");
//             }else{
//                 if(isBlack[i][j]){
//                     printf("|(%c)", char_piece[board[i][j]]);
//                 }else{
//                     printf("|[%c]", char_piece[board[i][j]]);
//                 }
//             }
            
//         }
//         printf("|\n");
//     }
//     printf("_________________________________\nATTACKED:\n");

//     int ind =15;
//     for(int i = 0; i < 8; i++){
//         printf("_________________________________\n");
//         for(int j = 0; j < 8; j++){
//             if(attacking[LOCATION(i,j)]){
//             // if(IS_ATTACKING(attacking[0][i][j], ind)){
//                 printf("| 1 ");
//             }else{
//                 printf("| 0 ");
//             }
            
//         }
//         printf("|\n");
//     }
//     printf("_________________________________\nDEFENDED:\n");
//     for(int i = 0; i < 8; i++){
//         printf("_________________________________\n");
//         for(int j = 0; j < 8; j++){
//             if(defended[LOCATION(i,j)]){
//             // if(IS_ATTACKING(attacking[1][i][j], ind)){
//                 printf("| 1 ");
//             }else{
//                 printf("| 0 ");
//             }
            
//         }
//         printf("|\n");
//     }
//     printf("_________________________________\n");
//     // //PRINTS OCCUPIED
//     for(int i = 0; i < 8; i++){
//         for(int j = 0; j < 8; j++){
//             std::cout<< (OCCUPIED(occupied[0], LOCATION(i,j)));
//         }
//         std::cout<<"\n";
//     }
//     printf("_________________________________\n");
//     for(int i = 0; i < 8; i++){
//         for(int j = 0; j < 8; j++){
//             std::cout<< (OCCUPIED(occupied[1], LOCATION(i,j)));
//         }
//         std::cout<<"\n";
//     }
// }

// int Game::make_move(Move& move){
//     int currPlayer = move_number & 1;
//     Piece* currPiece = move.piece;
//     if(IS_ATTACKING(attacking[LOCATION(move.r2, move.c2)], currPiece->ID)){
//         if(currPiece->ID >= 8){
//             currPiece->dist = 1;
//         }
//         occupied[currPlayer] ^= 0x0000000000000001LL << (8 * currPiece->row + currPiece->col);
//         currPiece->row = move.r2;
//         currPiece->col = move.c2;
//         currPiece->numMoves++;

//         occupied[currPlayer] |= 0x0000000000000001LL << (8 * move.r2 + move.c2);
//         move_number++;
//         currPlayer ^= 1;
        
//         //CAPTURES
//         if(OCCUPIED(occupied[currPlayer], LOCATION(move.r2, move.c2))){
//             occupied[currPlayer] ^= 0x0000000000000001LL << (8 * move.r2 + move.c2);
//             Player* player = currPlayer == 0? white: black;
//             Piece* curr = player->ITERATOR;
//             while(curr->next){
//                 curr = curr->next;
//                 if(curr->row == move.r2 && curr->col == move.c2){
//                     player->remove(curr);
//                     break;
//                 }
//             }
//         }
//         return 1;
//     }else{
//         return 0;
//     }
// }

// void Game::run(){
//     clock_t tStart = clock();
//     for(int i = 0; i < 1000000; i++){
//         calculate();
//     }
//     printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

//     GUI_STATE STATE = WAIT;
//     SDL_Event event;

//     Piece* currPiece; //piece to be interacted with
//     std::vector<int> highlighted_squares;
//     SDL_Rect pieceRect;
//     pieceRect.w = UNIT;
//     pieceRect.h = UNIT;
//     int thisInd;
//     int mouseR;
//     int mouseC;

//     Move move;

//     bool isPressed = false;

//     int xMouse = 0;
//     int yMouse = 0;

//     init_render();

//     CURRENT_PLAYER = white;
//     while(true){
//         switch(STATE){
//         case WAIT:
//             while(SDL_PollEvent(&event)){
//                 if(event.type == SDL_MOUSEBUTTONUP && event.button.x >= 0 && event.button.x < BOARD_BORDER && event.button.y >= 0 && event.button.y < BOARD_BORDER){
//                     int x = event.button.x;
//                     int y = event.button.y;
//                     int r = y / UNIT;
//                     int c = x / UNIT;
//                     Piece* p = CURRENT_PLAYER->ITERATOR;
//                     isPressed = false;
//                     while(p->next){
//                         p = p->next;
//                         if(p->row == r && p->col == c){
//                             STATE = CLICKED_PIECE;
//                             currPiece = p;
//                             isPressed = true;
//                             SDL_GetGlobalMouseState(&X_OFFSET, &Y_OFFSET);
//                             Y_OFFSET -= (r * UNIT);
//                             X_OFFSET -= (c * UNIT);
//                             Y_OFFSET -= (y - (r * UNIT + UNIT / 2));
//                             X_OFFSET -= (x - (c * UNIT + UNIT / 2));
//                         }
//                     }
//                     if(isPressed){
//                         for(int i = 0; i < 8; i++){
//                             for(int j = 0; j < 8; j++){
//                                 if(IS_ATTACKING(attacking[LOCATION(i,j)], currPiece->ID)){
//                                     highlighted_squares.push_back(i * 8 + j);
//                                 }
//                             }
//                         }
//                         // SDL_RenderPresent(renderer);
//                     }
//                 }
//             }
//             break;
//         case CLICKED_PIECE:
//             erase_piece(currPiece);
//             SDL_RenderPresent(renderer);
//             thisInd = currPiece->ID + (move_number % 2) * 16;
//             // bool canEnd = false;
//             while(event.type != SDL_MOUSEBUTTONUP){
//                 SDL_GetGlobalMouseState(&pieceRect.x, &pieceRect.y);
//                 pieceRect.x -= X_OFFSET;
//                 pieceRect.y -= Y_OFFSET;

//                 CURRENT_TICK = SDL_GetTicks64();
//                 if((CURRENT_TICK - PREVIOUS_RENDER_TICK) * FPS >= 1000){
//                     draw_board();
//                     draw_highlighted_squares(highlighted_squares);
//                     draw_unmoving_pieces(currPiece);
//                     SDL_RenderCopy(renderer, PIECE_TEXTURES[thisInd], NULL, &pieceRect);
//                     SDL_RenderPresent(renderer);
                    
//                     PREVIOUS_RENDER_TICK = CURRENT_TICK;
//                 }
//                 // SDL_Delay(1500);
//                 SDL_PollEvent(&event);
//             }
//             mouseR = (pieceRect.y + (UNIT / 2)) / UNIT;
//             mouseC = (pieceRect.x + (UNIT / 2)) / UNIT;

//             if(std::find(highlighted_squares.begin(), highlighted_squares.end(), mouseR * 8 + mouseC) != highlighted_squares.end()){
//                 move.piece = currPiece;
//                 move.r2 = mouseR;
//                 move.c2 = mouseC;
//                 STATE = EXECUTE_MOVE;
//             }else{
//                 std::cout<<"invalid square\n";
//                 draw_board();
//                 draw_pieces();
//                 std::cout<<"rendered\n";
//                 STATE = WAIT;
//             }
//             highlighted_squares.clear();
            
//             break;
//         case EXECUTE_MOVE:
//             // print_data();
//             make_move(move);
//             // print_data();
//             calculate();

//             draw_board();
//             draw_pieces();
            
//             CURRENT_PLAYER = CURRENT_PLAYER == white? black: white;

//             STATE = WAIT;
//             break;
//         default:
//             break;
//         }
//         CURRENT_TICK = SDL_GetTicks64();
//         if((CURRENT_TICK - PREVIOUS_RENDER_TICK) * FPS >= 1000){
//             PREVIOUS_RENDER_TICK = CURRENT_TICK;
//             SDL_RenderPresent(renderer);
//         }
//     }
//     destroy_render();
//     // print_data();
//     free(currPiece);

//     // Move move(0,0,11, 4, 3, 0);
//     // make_move(move);
//     // print_data();
// }

// void Game::init_render(){
//     if(SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "FAILEDED TO INIT\n";
//     if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "FAILEDED AT CREATEWINDOWRENDERER\n";
//     SDL_SetWindowTitle(window, "Chess");
//     SDL_ShowCursor(1); //show cursor on screen
//     SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); //renders pixels better?

//     SDL_GL_SetSwapInterval(0);
//     // IMG_Init(IMG_INIT_PNG);

//     srcRect.x = 0;
//     srcRect.y = 0;
//     srcRect.w = UNIT;
//     srcRect.h = UNIT;

//     for(int i = 0; i < 8; i++){ //initialize board rect
//         for(int j = 0; j < 8; j++){
//             SDL_Rect curr;
//             curr.x = j * UNIT;
//             curr.y = i * UNIT;
//             curr.h = UNIT;
//             curr.w = UNIT;
//             RECT_BOARD[i][j] = curr;
//         }
//     }

//     draw_board();

//     for(int i = 0; i < 32; i++){ // initiliaze pieces
//         PIECE_TEXTURES[i] = IMG_LoadTexture(renderer, IMG_PATHS[i]);
//         if(!PIECE_TEXTURES[i]){
//             std::cout<<i<<IMG_PATHS[i]<<"!!!!!"<<IMG_GetError()<<"wrongpathbruh\n";
//         }
//         // PIECE_SURFACES[i] = IMG_Load(IMG_PATHS[i]);
//         // PIECE_TEXTURES[i] = SDL_CreateTextureFromSurface(renderer, PIECE_SURFACES[i]);
//         // if(!PIECE_TEXTURES[i]){
//         //     std::cout<<i<<IMG_PATHS[i]<<"wrongpathbruh\n";
//         // }
//     }

//     draw_pieces();
//     SDL_RenderPresent(renderer);
// }

// void Game::draw_board(){
//     for(int i = 0; i < 8; i++) {
//         for(int j = 0; j < 8; j++){
//             if((i + j) % 2){
//                 SDL_SetRenderDrawColor(renderer, 171, 110, 60, 255);
//             }else{
//                 SDL_SetRenderDrawColor(renderer, 242, 204, 138, 255);
//             }   
//             SDL_RenderFillRect(renderer, &RECT_BOARD[i][j]);
//         }
//     }
//     // SDL_RenderPresent(renderer);
// }

// void Game::draw_highlighted_squares(std::vector<int>& squares){
//     for(const int& coord: squares){
//         highlight_square(coord / 8, coord % 8);
//     }
//     // SDL_RenderPresent(renderer);
// }

// void Game::draw_pieces(){
//     Piece* piece = white->ITERATOR;
//     while(piece->next){
//         piece = piece->next;
//         draw_piece(piece, WHITE);
//     }
//     piece = black->ITERATOR;
//     while(piece->next){
//         piece = piece->next;
//         draw_piece(piece, BLACK);
//     } 
// }

// void Game::draw_unmoving_pieces(Piece* excluded){
//     Piece* curr = white->ITERATOR;
//     while(curr->next){
//         curr = curr->next;
//         if(curr->ID != excluded->ID || 0 != move_number & 1){
//             draw_piece(curr, WHITE);
//         }
//     }
//     curr = black->ITERATOR;
//     while(curr->next){
//         curr = curr->next;
//         if(curr->ID != excluded->ID || 1 != move_number & 1){
//             draw_piece(curr, BLACK);
//         }
//     }
// }

// void Game::draw_piece(Piece* piece, player_side side){
//     SDL_Texture* tempTex = PIECE_TEXTURES[piece->ID + (side + 1)/2 * 16];
//     // std::cout<<piece.ID + (side + 1)/2 * 16<<"\n";
//     // SDL_RenderCopy(renderer, tempTex, &srcRect, &RECT_BOARD[piece.row][piece.col]);
//     SDL_RenderCopy(renderer, tempTex, NULL, &RECT_BOARD[piece->row][piece->col]);
// }

// void Game::erase_piece(Piece* piece){
//     if((piece->row + piece->col) % 2){
//         SDL_SetRenderDrawColor(renderer, 171, 110, 60, 255);
//     }else{
//         SDL_SetRenderDrawColor(renderer, 242, 204, 138, 255);
//     }   
//     SDL_RenderFillRect(renderer, &RECT_BOARD[piece->row][piece->col]);
// }

// void Game::highlight_square(int r, int c){
//     if((r + c) % 2){
//         SDL_SetRenderDrawColor(renderer, 171, 110, 150, 255);
//     }else{
//         SDL_SetRenderDrawColor(renderer, 242, 204, 228, 255);
//     }   
//     SDL_RenderFillRect(renderer, &RECT_BOARD[r][c]);
// }

// void Game::destroy_render(){
//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     // free(PIECE_TEXTURES);
//     SDL_Quit();
// }