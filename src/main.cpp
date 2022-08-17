#include "game.h"
#include <iostream>
// #include <<SDL.h>
// using namespace std;

int main(int argc, char* argv[]){
    // std::cout<<WHITE<<"\n";
    // int* i;
    // *i = 1;
    // int a = *i;
    // (&a) = 2;
    // std::cout<<"experiment: "<<*i<<"\n";
    Game game = Game();
    game.run();
    return 0;
}