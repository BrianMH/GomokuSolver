#include "gtest/gtest.h"
#include "mnkGame.h"
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <ctime>
#include <cstdlib>

// Implements a fixture for mnkGame testing
class MNKGameTest : public ::testing::Test {
protected:
    // initialize trivial game board and tic-tac-toe board + gomoku board
    MNKGameTest() : board1(1,1,1), board2(3,3,3), board3(5,5,5) {}

    // this gets called after initialization but before function methods
    void SetUp() override {
        std::srand(unsigned(std::time(0)));
    }

    // Don't need this for now
    // void TearDown() override {}

    // example boards to use
    MNKBoard board1;
    MNKBoard board2;
    MNKBoard board3;

    // some static constants to use
    static const int NUM_TRIALS = 100;
    static const int MAX_NUM_3X3_MOVES = 7;
    static const int MAX_NUM_5X5_MOVES = 20;
};

// Fixture tests
TEST_F(MNKGameTest, TrivialWinTest) {
    board1.placePiece(0, 0, CellState::black);
    ASSERT_TRUE(board1.checkWin());
}

TEST_F(MNKGameTest, NonWinTests) {
    board2.placePiece(0, 0, CellState::black);
    board2.placePiece(0, 1, CellState::white);
    board2.placePiece(0, 2, CellState::white);

    ASSERT_FALSE(board1.checkWin());
    ASSERT_FALSE(board2.checkWin());
    ASSERT_FALSE(board3.checkWin());
}

TEST_F(MNKGameTest, RowWinTest) {
    board2.placePiece(0, 0, CellState::black);
    board2.placePiece(0, 1, CellState::black);
    board2.placePiece(0, 2, CellState::black);
    ASSERT_TRUE(board2.checkWin());

    int numInds[5] = {0, 1, 2, 3, 4};
    for(auto& numVal : numInds) 
        board3.placePiece(numVal, 0, CellState::white);
    ASSERT_TRUE(board3.checkWin());
}

TEST_F(MNKGameTest, FSDWinTest) {
    board2.placePiece(2, 0, CellState::black);
    board2.placePiece(1, 1, CellState::black);
    board2.placePiece(0, 2, CellState::black);
    ASSERT_TRUE(board2.checkWin());

    for(int iterCnt = 0; iterCnt < 5; iterCnt++)
        board3.placePiece(iterCnt, 4-iterCnt, CellState::white);
    ASSERT_TRUE(board3.checkWin());
}

TEST_F(MNKGameTest, BSDWintest) {
    board2.placePiece(2, 2, CellState::black);
    board2.placePiece(0, 0, CellState::black);
    board2.placePiece(1, 1, CellState::black);
    ASSERT_TRUE(board2.checkWin());

    for(int iterCnt = 0; iterCnt < 5; iterCnt++)
        board3.placePiece(iterCnt, iterCnt, CellState::white);
    ASSERT_TRUE(board3.checkWin());
}

TEST_F(MNKGameTest, Randomized3x3WinTest) {
    // enumerate possible combinations for 3x3 board
    int posIndVals[3] = {0, 1, 2};
    std::vector<std::pair<int, int>> cartProdT;
    for(auto numValL : posIndVals)
        for(auto numValR : posIndVals)
            cartProdT.emplace_back(numValL, numValR);

    // now test the values (maximum number of moves to win is 6)
    int numIters = 0;
    while(numIters < NUM_TRIALS) {
        // initialize used variables
        int numMoves = 0;
        std::random_shuffle(cartProdT.begin(), cartProdT.end());
        board2.clearBoard();

        for(auto& pair : cartProdT) {
            ASSERT_TRUE(board2.placePiece(pair.first, pair.second, CellState::white));
            numMoves++;
            if(board2.checkWin())
                break;
            if(numMoves > MAX_NUM_3X3_MOVES)
                ASSERT_TRUE(false) << "Exceeded maximum number of moves before win.";
        }
        numIters++;
    }
}

TEST_F(MNKGameTest, Randomized5x5WinTest) {
    // now the same for the 5x5 board
    int posIndValsL[5] = {0, 1, 2, 3, 4};
    std::vector<std::pair<int, int>> cartProdF;
    for(auto numValL : posIndValsL)
        for(auto numValR : posIndValsL)
            cartProdF.emplace_back(numValL, numValR);

    // Now test the values (maximum number of moves is 20)
    int numIters = 0;
    while(numIters < NUM_TRIALS) {
        // initialize used variables
        int numMoves = 0;
        std::random_shuffle(cartProdF.begin(), cartProdF.end());
        board3.clearBoard();

        for(auto& pair : cartProdF) {
            ASSERT_TRUE(board3.placePiece(pair.first, pair.second, CellState::white));
            numMoves++;
            if(board3.checkWin())
                break;
            if(numMoves > MAX_NUM_5X5_MOVES)
                ASSERT_TRUE(false) << board3 << std::endl << "Exceeded maximum number of moves before win.";
        }
        numIters++;
    }
}