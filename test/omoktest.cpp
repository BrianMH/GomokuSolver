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
    static const int NUM_TRIALS = 1;
};

TEST_F(OmokGameTest, PlacementTest) {
    board1.placePiece(0, 0);
    ASSERT_FALSE(board1.isFinished());
}

TEST_F(OmokGameTest, WinnerTest) {

}

TEST_F(OmokGameTest, SimulatedGameTest) {
    // loads in a series of game files played by previous AIs
    // Note that there is no randomization due to the walk nature of the iterator
    std::string datPath = "./test/SimulatedGames/";
    std::regex csvPattern = std::regex("\\d*,\\d*,\\d*");
    unsigned int curTrial = 0;

    // string parsing variables
    std::ifstream gameFile;
    std::string sToParse;
    std::string tempVar;
    std::string lastString;     // determine the winner using this string at the end
    int inX, inY, winVal;

    // for(auto& fileEntry : fs::directory_iterator(datPath)) {
    //     // open file and parse only the moves (who moves first doesn't matter)
    //     gameFile.open(fileEntry.path(), std::ifstream::in);
    //     std::getline(gameFile, sToParse);   // skip introductory line
    //     while(std::getline(gameFile, sToParse)) { // parse only the relevant lines that we need
    //         if(sToParse.length() > 0 && std::regex_match(sToParse, csvPattern)) {
    //             std::istringstream curLine = std::istringstream(sToParse);
                
    //             std::getline(curLine, tempVar, ',');
    //             std::istringstream(tempVar) >> inX;
    //             std::getline(curLine, tempVar, ',');
    //             std::istringstream(tempVar) >> inY;     // last value not necessary

    //             // play the game given values (every move must be valid until the very last one)
    //             ASSERT_TRUE(board1.placePiece(inX, inY));
    //         }

    //         lastString = sToParse; // move string into new container before rewriting it
    //     }

    //     // once we're outside we confirm the winner (or draw state) using the last line
    //     std::istringstream winLine = std::istringstream(lastString);
    //     std::getline(winLine, tempVar, ',');
    //     std::istringstream(tempVar) >> winVal;
    //     ASSERT_EQ(winVal, board1.getGameWinner());

    //     // exit condition since there are quite a few simulations in the folder
    //     curTrial += 1;
    //     if(curTrial == NUM_TRIALS)
    //         break;
    // }
}
