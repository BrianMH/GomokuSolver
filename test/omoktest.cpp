#include "gtest/gtest.h"
#include "gomoku.h"
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <ctime>
#include <cstdlib>

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
    static const int NUM_TRIALS = 100;
};

TEST_F(OmokGameTest, PlacementTest) {
    board1.placePiece(0, 0);
}