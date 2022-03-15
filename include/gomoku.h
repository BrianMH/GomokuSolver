#ifndef GOMOKU_H
#define GOMOKU_H

// Required imports
#include <utility>
#include <unordered_map>
#include "mnkGame.h"

// verbose output
// #define OMOK_VERBOSE

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
    inline static const int BOARD_SIZE = 15;
    inline static const int KSIZE = 5;
    CellState curPlayer;
    bool gameFinished = false;
    bool gameStarted = false;

public:
    // inits omok board
    Omok();

    // modified placement schema
    // CellState should only be modified for testing
    bool placePiece(int row, int col);

    // used to determine when the game has finished
    bool isFinished(void);

    // reports the value of the player who won the game (0 if nobody has won yet)
    int getGameWinner(void);

    // clears the game board
    void clearBoard(void);

private:
    // enumerate possible open 3 conditions
    class O3CondsContainer {
        public:
        inline static const int SIZE = 6;
        static std::vector<CellState>& o3Cond1(CellState& player);
        static std::vector<CellState>& o3Cond2(CellState& player);
        static std::vector<CellState>& o3Cond3(CellState& player);
        static std::vector<CellState>& o3Cond4(CellState& player);

        private:
        static inline std::unordered_map<CellState, std::vector<CellState>, EnumClassHash> cacheMap1;
        static inline std::unordered_map<CellState, std::vector<CellState>, EnumClassHash> cacheMap2;
        static inline std::unordered_map<CellState, std::vector<CellState>, EnumClassHash> cacheMap3;
        static inline std::unordered_map<CellState, std::vector<CellState>, EnumClassHash> cacheMap4;
    };

    O3CondsContainer returnPlayerSpecificConds(const CellState& player);

    // checks whether a 3-in-a-row sequence is an open three
    bool openThreeCheck(const std::vector<CellState>& pieceArr);

    // helper function for 3n3 rule
    bool isDoubleThree(int row, int col);

    // modified win detection
    void checkWin(void);
};

#endif