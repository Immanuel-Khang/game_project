#include "utility.h"
#include <iostream>

int main(int argc, char* args[]) {
    GoldMiner::Game game("Gold Digger", 520, 400);
    game.run(); // Start the game loop
    return 0;
}