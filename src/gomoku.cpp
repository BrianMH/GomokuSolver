#include <iostream>
#include <utility>
#include <tuple>
#include <vector>
#include <unordered_map>
#include "gomoku.h"

// initializes an omok game based on an mnk game
Omok::Omok():MNKBoard(BOARD_SIZE, BOARD_SIZE, KSIZE), curPlayer(CellState::black) {}

// overloads placePiece for the current game format
bool Omok::placePiece(int row, int col) {
    // make sure you can place a pice in the first place
    if(!isPosEmpty(row, col) || isFinished())
        return false;

    // check rules for potential placement of piece
    bool d3Present;
    if(gameStarted) {
        d3Present = isDoubleThree(row, col);
    } else {
        gameStarted = true;
        d3Present = false;
    }

    // place piece and toggle player for next move if possible
    if(!d3Present) {
        MNKBoard::placePiece(row, col, curPlayer);

        // check for win condition following the piece placement
        checkWin();

        // if the win has yet to occur, then swap the player state
        if(!gameFinished)
            curPlayer = curPlayer==CellState::black ? CellState::white : CellState::black;
    }

    // return True if a piece was placed or false otherwise
    return !d3Present;
}

// reports the winner based on the value of the player
int Omok::getGameWinner(void) {
    if(gameFinished)
        return curPlayer==CellState::black ? 1 : 2;
    else
        return 0;
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
                                    relRowSec.begin()+std::min(BOARD_SIZE-1, row+4)));
    auto relColSec = getColVec(col);
    bool colO3 = openThreeCheck(std::vector<CellState>(relColSec.begin()+std::max(0,col-4),
                                    relColSec.begin()+std::min(BOARD_SIZE-1, col+4)));
    
    // diagonals require a bit more work to extract or desired portions as they aren't stored
    // in bitmap arrays to make this easier
    auto relFSDSec = getForwardDiagVec(row, col, 4);
    auto relBSDSec = getBackDiagVec(row, col, 4);
    bool FSDO3 = openThreeCheck(relFSDSec);
    bool BSDO3 = openThreeCheck(relBSDSec);
    
    // remove the piece after analysis
    MNKBoard::removePiece(row, col);

    return ((rowO3?1:0) + (colO3?1:0) + (FSDO3?1:0) + (BSDO3?1:0)) >= 2;
}

// checks to see if one of the combinations is a open 3 sequence
bool Omok::openThreeCheck(const std::vector<CellState>& pieceArr) {
    // trivially false if array not big enough to match sequence
    if(pieceArr.size() < O3CondsContainer::SIZE)
        return false;

    // pass through the current array and comb for possible sequences
    for(int indOff=0; indOff <= pieceArr.size() - O3CondsContainer::SIZE; indOff++) {
        // possible o3 sequences
        std::vector<std::vector<CellState>> o3PossConds = {O3CondsContainer::o3Cond1(curPlayer), O3CondsContainer::o3Cond2(curPlayer), 
                                                            O3CondsContainer::o3Cond3(curPlayer), O3CondsContainer::o3Cond4(curPlayer)};
        
        // proceed with checking + offset manipulation
        int arrInd = 0;
        while(!o3PossConds.empty() && arrInd < O3CondsContainer::SIZE) {
            for(auto o3it=o3PossConds.begin(); o3it != o3PossConds.end();) {
                // if not equal, remove from possible sequence to limit the future checks
                if((*o3it)[arrInd] != pieceArr[arrInd + indOff])
                    o3it = o3PossConds.erase(o3it);
                else
                    ++o3it;
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
 * Caches results from the o3Cond func in order to prevent constant creation of the elements for any given player
 **/
std::vector<CellState>& Omok::O3CondsContainer::o3Cond1(CellState& player) {
    if(!O3CondsContainer::cacheMap1.count(player))
        O3CondsContainer::cacheMap1[player] = {CellState::none, CellState::none, player, 
                                              player, player, CellState::none};
    return O3CondsContainer::cacheMap1[player];
}
std::vector<CellState>& Omok::O3CondsContainer::o3Cond2(CellState& player) {
    if(!O3CondsContainer::cacheMap2.count(player))
        O3CondsContainer::cacheMap2[player] = {CellState::none, player, player, 
                                                player, CellState::none, CellState::none};
    return O3CondsContainer::cacheMap2[player];
}
std::vector<CellState>& Omok::O3CondsContainer::o3Cond3(CellState& player) {
    if(!O3CondsContainer::cacheMap3.count(player))
        O3CondsContainer::cacheMap3[player] = {CellState::none, player, CellState::none, 
                                               player, player, CellState::none};
    return O3CondsContainer::cacheMap3[player];
}
std::vector<CellState>& Omok::O3CondsContainer::o3Cond4(CellState& player) {
    if(!O3CondsContainer::cacheMap4.count(player))
        O3CondsContainer::cacheMap4[player] = {CellState::none, player, player, 
                                               CellState::none, player, CellState::none};
    return O3CondsContainer::cacheMap4[player];
}

/**
 *  Just used to keep track of the status of the game
 * */
bool Omok::isFinished(void) {
    return gameFinished;
}

/**
 * A win could only arise depending on the last placed piece.
 * The area to check will only be the 2k x 2k window about the piece
 * 
 * Note: this function ignores overlines
 **/
void Omok::checkWin(void) {
    auto winList = enumerateInARow();

    // now iterate once to find possible winning direction
    for(auto& tup:winList) {
        #ifdef OMOK_VERBOSE
        std::cout << std::get<0>(tup) << " ";
        #endif

        if(std::get<0>(tup) == winSize)
            gameFinished = true;
    }

    #ifdef OMOK_VERBOSE
    std::cout << std::endl;
    #endif
}

