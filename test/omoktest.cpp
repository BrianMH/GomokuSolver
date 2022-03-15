#include "gtest/gtest.h"
#include "gomoku.h"
#include <iostream>
#include <sstream>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
#include <regex>


// Implements a fixture to test the Omok class
class OmokGameTest : public ::testing::Test {
protected:
    // Initializes the main omok board for use
    OmokGameTest() : board1() {}

    // this gets called after initialization but before function methods
    void SetUp() override {
        std::srand(unsigned(std::time(0)));
    }

    // test board to use
    Omok board1;

    // testing constants
    static const int NUM_TRIALS = 50;
};

TEST_F(OmokGameTest, PlacementTest) {
    board1.placePiece(0, 0);
    ASSERT_FALSE(board1.isFinished());
}

TEST_F(OmokGameTest, WinnerTest) {

}

/**
 *  Turns out that finding a proper GOMOKU dataset is difficult to do. For now, the
 *  test is simply running through RENJU games and verifying that everything is fine.
 *  Keep in mind that, unlike in Gomoku, Renju allows double-threes for the second player
 *  and enforces all players to adhere to an opening book. Therefore, this function
 *  will actually begin to throw errors with enough of a corpus size being included.
 * */
TEST_F(OmokGameTest, SimulatedGameTest) {
    // loads in a series of game files played by previous AIs
    // Note that there is no randomization due to the walk nature of the iterator
    std::string datPath = "../test/SimulatedGames/";
    std::regex csvPattern = std::regex("\\d+,\\d+,\\d+");
    unsigned int curTrial = 0;

    // string parsing variables
    std::ifstream gameFile;
    std::string sToParse;
    std::string tempVar;
    std::string lastString;     // determine the winner using this string at the end
    int inX, inY, winVal, moveNum;

    for(auto& fileEntry : fs::directory_iterator(datPath)) {
        // clears the board for playing
        board1.clearBoard();
        moveNum = 0;

        // open file and parse only the moves (who moves first doesn't matter)
        gameFile.open(fileEntry.path(), std::ifstream::in);
        std::getline(gameFile, sToParse);   // skip introductory line
        while(std::getline(gameFile, sToParse)) { // parse only the relevant lines that we need
            if(std::regex_search(sToParse, csvPattern)) {
                std::istringstream curLine = std::istringstream(sToParse);
                
                std::getline(curLine, tempVar, ',');
                std::istringstream(tempVar) >> inX;
                std::getline(curLine, tempVar, ',');
                std::istringstream(tempVar) >> inY;     // last value not necessary

                // play the game given values (every move must be valid until the very last one)
                ASSERT_TRUE(board1.placePiece(inX-1, inY-1)) << fileEntry.path();
                moveNum += 1;
            }

            lastString = sToParse; // move string into new container before rewriting it
        }
        gameFile.close();   // safely close file after we read all the moves

        // once we're outside we confirm the winner (or draw state) using the filename
        std::istringstream winLine = std::istringstream(fileEntry.path().stem());
        for(int i = 0; i < 4; i++)
            std::getline(winLine, tempVar, '_');
        std::istringstream(tempVar) >> winVal;
        ASSERT_EQ(winVal, board1.getGameWinner()) << "Failed on test " << fileEntry.path() << "in " << moveNum << " moves...";

        // exit condition since there are quite a few simulations in the folder
        curTrial += 1;
        if(curTrial == NUM_TRIALS)
            break;
    }
}
