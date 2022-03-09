#include <iostream>
#include "include/gomoku.h"

using namespace std;

int main(void) {
    // Initialize Board Game
    auto game = Omok();
    int newX = 0, newY = 0;

    // Take in inputs until the game ends
    while(!game.isFinished()) {
        std::cout << std::endl << std::endl << game;
        std::cout << "Give next piece location (ie. 1 2): \t";
        std::cin >> newX;
        std::cin >> newY;
        std::cout << "Placing a piece at (" << newX << "," << newY << ")..." << std::endl;
        game.placePiece(newX, newY);
        std::cout << "Is game finished? " << game.isFinished() << std::endl;
    }

    std::cout << "Game has finished..." << std::endl;
    return 0;
}