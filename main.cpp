#include <iostream>
#include "include/gomoku.h"

using namespace std;

int main(void) {
    // Initialize Board Game
    auto game = Omok();
    int newX = 0, newY = 0;
    bool res;

    // Take in inputs until the game ends
    while(!game.isFinished()) {
        std::cout << std::endl << std::endl << game;
        std::cout << "Give next piece location (ie. 1 2): \t";
        std::cin >> newX;
        std::cin >> newY;
        std::cout << "Placing a piece at (" << newX << "," << newY << ")..." << std::endl;

        // Place piece into game (or keep asking for new coords until it can happen)
        res = game.placePiece(newX, newY);
        while(!res) {
            std::cout << "Invalid play made... Give corrected location (ie. 1 2): \t";  // Perhaps error reporting could be better in the API?
            std::cin >> newX;
            std::cin >> newY;
            res = game.placePiece(newX-1, newY-1);
        }

        std::cout << "Is game finished? " << game.isFinished() << std::endl;
    }

    std::cout << "Game has finished..." << std::endl;
    return 0;
}