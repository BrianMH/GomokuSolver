#include <iostream>
#include <utility>
#include <tuple>
#include <vector>
#include "gomoku.h"

// initializes an omok game based on an mnk game
Omok::Omok():MNKBoard(BOARD_SIZE, BOARD_SIZE, KSIZE), curPlayer(CellState::black) {}

// overloads placePiece for the current game format
bool Omok::placePiece(int row, int col) {
    // make sure you can place a pice in the first place
    if(!isPosEmpty(row, col))
        return false;

    // check rules for potential placement of piece
    bool res = isDoubleThree(row, col);

    // place piece and toggle player for next move if possible
    if(!res) {
        MNKBoard::placePiece(row, col, curPlayer);
        curPlayer = curPlayer==CellState::black ? CellState::white : CellState::black;
    }

    // return True if a piece was placed or false otherwise
    return res;
}

// checks to see if the current move produces a 3n3 sequence
bool Omok::isDoubleThree(int row, int col) {
    // Hypothetically place the move without restriction
    MNKBoard::placePiece(row, col, curPlayer, false);

    // Now extract the four directions of moves and analyze them. Remember that
    // given a point we check four spaces behind and in front of it to determine
    // it's state as a potential "open three"
    auto relRowSec = getRowVec(row);
    bool rowO3 = openThreeCheck(std::vector<CellState>(relRowSec.begin()+std::max(0,row-4),
                                    relRowSec.begin()+std::min(BOARD_SIZE, row+4)));
    auto relColSec = getColVec(col);
    bool colO3 = openThreeCheck(std::vector<CellState>(relColSec.begin()+std::max(0,col-4),
                                    relColSec.begin()+std::min(BOARD_SIZE, col+4)));
    auto relFSDSec = getForwardDiagVec(row, col);
    bool FSDO3 = false;
    auto relBSDSec = getBackDiagVec(row, col);
    bool BSDO3 = false;

    // remove the piece after analysis
    MNKBoard::placePiece(row, col, CellState::none, false);

    return (rowO3?1:0 + colO3?1:0 + FSDO3?1:0 + BSDO3?1:0) > 2;
}

// checks to see if one of the combinations is a open 3 sequence
bool Omok::openThreeCheck(const std::vector<CellState>& pieceArr) {
    // trivially false if array not big enough to match sequence
    if(pieceArr.size() < O3Conds::o3Cond1.size())
        return false;

    // pass through the current array and comb for possible sequences
    for(int indOff=0; indOff <= pieceArr.size() - O3Conds::o3Cond1.size(); indOff++) {
        // possible o3 sequences
        std::vector<std::vector<CellState>> o3PossConds = {O3Conds::o3Cond1, O3Conds::o3Cond2, 
                                                            O3Conds::o3Cond3, O3Conds::o3Cond4};
        
        // proceed with checking + offset manipulation
        int arrInd = 0;
        while(!o3PossConds.empty() && arrInd < O3Conds::o3Cond1.size()) {
            for(auto o3it=o3PossConds.begin(); o3it != o3PossConds.end(); ++o3it) {
                // if not equal, remove from possible sequence to limit the future checks
                if((*o3it)[arrInd] != CellState::none)
                    (*o3it)[arrInd] = curPlayer; // swap piece to correct color if different (maybe better sol?)
                if((*o3it)[arrInd] != pieceArr[arrInd + indOff])
                    o3it = o3PossConds.erase(o3it);
            }

            // increase index of array being checked
            arrInd++;
        }

        // if o3PossConds not empty, then a match has been found
        // otherwise continue the search with additional offset
        if(!o3PossConds.empty())
            return true;
    }

    return false;
}

/**
 * A win could only arise depending on the last placed piece.
 * The area to check will only be the 2k x 2k window about the piece
 * 
 * Note: this function ignores overlines
 **/
bool Omok::checkWin(void) {
    auto winList = enumerateInARow();

    // now iterate once to find possible winning direction
    for(auto& tup:winList) 
        if(std::get<0>(tup) == winSize)
            return true;

    return false;
}

