#ifndef GOMOKU_H
#define GOMOKU_H

// Required imports
#include <utility>
#include "mnkGame.h"

/**
 *  Omok
 * 
 * Implements the Omok game. A modified version of the mxnxk game where
 * specific rules are applied. The following rules are followed:
 *  1) No double 3's allowed 
 *  2) No overlines are allowed (only 5-in-a-row wins)
 *  3) The board must be 19x19 with a 5-in-a-row win condition
 **/
class Omok: public MNKBoard{
private:
    static const int BOARD_SIZE = 15;
    static const int KSIZE = 5;
    CellState curPlayer;

public:
    // inits omok board
    Omok();

    // modified placement schema
    // CellState should only be modified for testing
    bool placePiece(int row, int col);

    // modified win detection
    bool checkWin(void);

private:
    // enumerate possible open 3 conditions
    class O3Conds {
    public:
        static const inline std::vector<CellState> o3Cond1 = {CellState::none, CellState::none, CellState::black, 
                                                                CellState::black, CellState::black, CellState::none};
        static const inline std::vector<CellState> o3Cond2 = {CellState::none, CellState::black, CellState::black, 
                                                                CellState::black, CellState::none, CellState::none};
        static const inline std::vector<CellState> o3Cond3 = {CellState::none, CellState::black, CellState::none, 
                                                                CellState::black, CellState::black, CellState::none};
        static const inline std::vector<CellState> o3Cond4 = {CellState::none, CellState::black, CellState::black, 
                                                                CellState::none, CellState::black, CellState::none};
    };

    // checks whether a 3-in-a-row sequence is an open three
    bool openThreeCheck(const std::vector<CellState>& pieceArr);

    // helper function for 3n3 rule
    bool isDoubleThree(int row, int col);
};

#endif