#include "../include/mnkGame.h"
#include <utility>
#include <vector>
#include <string>
#include <tuple>
#include <iostream>
#include <cmath>

MNKBoard::MNKBoard(int m, int n, int k) : winSize(k), numRows(m), numCols(n), lastMove(std::make_tuple(0,0)) {
    // initialize the board
    board = new CellState* [numRows];
    for(int rowInd=0; rowInd<numRows; rowInd++) {
        board[rowInd] = new CellState [numCols];
        for(int colInd=0; colInd<numCols; colInd++)
            board[rowInd][colInd] = CellState::none;
    }
    
    // initialize flipped board
    flippedBoard = new CellState* [numCols];
    for(int colInd=0; colInd<numCols; colInd++) {
        flippedBoard[colInd] = new CellState [numRows];
        for(int rowInd=0; rowInd<numRows; rowInd++)
            flippedBoard[colInd][rowInd] = CellState::none;
    }
}

MNKBoard::~MNKBoard() {
    // free up memory allocated for board
    for(int rowInd=0; rowInd<numRows; rowInd++)
        free(board[rowInd]);
    free(board);
    for(int colInd=0; colInd<numCols; colInd++)
        free(flippedBoard[colInd]);
    free(flippedBoard);
}

bool MNKBoard::placePiece(int row, int col, CellState state, bool updateLast) {
    if(row < 0 || row >= numRows || col < 0 || col >= numCols || state == CellState::none)
        return false; // this line should be used to throw a proper exception instead

    if(board[row][col] == CellState::none) {
        // update board
        board[row][col] = state;
        flippedBoard[col][row] = state;

        // update last placed piece
        if(updateLast) {
            std::get<0>(lastMove) = row; 
            std::get<1>(lastMove) = col;
        }
        return true;
    }

    return false;   // this only happens if this block was taken
}

/**
 * Helper only used for testing piece placements
 * */
void MNKBoard::removePiece(int row, int col) {
    board[row][col] = CellState::none;
}

/**
 * A win could only arise depending on the last placed piece.
 * The area to check will only be the 2k x 2k window about the piece
 * 
 * Note: this function accepts overlines
 **/
bool MNKBoard::checkWin(void) {
    auto winList = enumerateInARow();

    // now iterate once to find possible winning direction
    for(auto& tup:winList) 
        if(std::get<0>(tup) >= winSize)
            return true;

    return false;
}

/**
 * Checks if the current position is not taken by any player
 **/
bool MNKBoard::isPosEmpty(int row, int col) {
    return board[row][col] == CellState::none;
}


/**
 * Just empties the board using a basic loop. Writes the pieces
 * to be the typical CellState::none piece.
 **/
void MNKBoard::clearBoard(void) {
    for(int rowInd=0; rowInd<numRows; rowInd++)
        for(int colInd=0; colInd<numCols; colInd++) {
            board[rowInd][colInd] = CellState::none;
            flippedBoard[colInd][rowInd] = CellState::none;
        }
}

/**
 * Reports the size of the board as a tuple
 **/
std::tuple<int, int> MNKBoard::getBoardSize(void) {
    return std::make_tuple(numRows, numCols);
}

/**
 *  Slices the board to isolate a given row
 **/ 
const std::vector<CellState> MNKBoard::getRowVec(int rowInd) {
    return std::vector<CellState>(board[rowInd], board[rowInd]+numRows);
}

/**
 *  Slices the board to isolate a given column
 **/ 
const std::vector<CellState> MNKBoard::getColVec(int colInd) {
    return std::vector<CellState>(flippedBoard[colInd], flippedBoard[colInd]+numCols);
}

/**
 *  Slices the board to isolate a (/)-directional diagonal given a particular index.
 * 
 *  Use the window size argument to acquire only a windowed look into the diagonal
 **/ 
const std::vector<CellState> MNKBoard::getForwardDiagVec(int pieceRow, int pieceCol, int windowSize) {
    auto tempVec = std::vector<CellState>();

    if(windowSize == 0) {
        // adjust offset to begin from most bottom-left element we can find
        int deltaVal = std::min((int)numRows-pieceRow, pieceCol);
        pieceRow += deltaVal; pieceCol -= deltaVal;

        // Add elements to vector as we go starting from bottom left position
        while(pieceRow < numRows && pieceCol < numCols) {
            tempVec.push_back(board[pieceRow][pieceCol]);
            pieceRow -= 1; pieceCol += 1;
        }
    } else { // need to find vector windowSize elems left and right of cur elem and simply construct a small vec
        tempVec.push_back(board[pieceRow][pieceCol]);
        vectorBuilderHelper(tempVec, windowSize, pieceRow, pieceCol, 1, -1);
        vectorBuilderHelper(tempVec, windowSize, pieceRow, pieceCol, -1, 1);
    }

    return tempVec;
}

/**
 *  Slices the board to isolate a (\)-directional diagonal given a particular index.
 *  This is simple as any upper left element is guaranteed to exist if a lower-right element exists.
 * 
 *  A subslice can be acquired by specifying the window size argument.
 **/ 
const std::vector<CellState> MNKBoard::getBackDiagVec(int pieceRow, int pieceCol, int windowSize) {
    // Add elements to vector as we go starting from top left position
    auto tempVec = std::vector<CellState>();

    if(windowSize == 0) {
        int deltaVal = std::min(pieceRow, pieceCol);
        pieceRow -= deltaVal; pieceCol -= deltaVal;

        while(pieceRow < numRows && pieceCol < numCols) {
            tempVec.push_back(board[pieceRow][pieceCol]);
            pieceRow += 1; pieceCol += 1;
        }
    } else {
        tempVec.push_back(board[pieceRow][pieceCol]);
        vectorBuilderHelper(tempVec, windowSize, pieceRow, pieceCol, -1, -1);
        vectorBuilderHelper(tempVec, windowSize, pieceRow, pieceCol, 1, 1);
    }

    return tempVec;
}

/**
 * Recursive helper that populates a vector with the proper elements either in the beginning or at the end...
 * */ 
void MNKBoard::vectorBuilderHelper(std::vector<CellState>& toModify, int windowSize, int row, int col, int deltaY, int deltaX) {
    // edge cases (ie. no need to go farther or edges of matrix reached)
    if(windowSize <= 0 || (row == 0 && deltaY < 0) || (row == numRows-1 && deltaY > 0) 
                       || (col == 0 && deltaX < 0) || (row == numCols-1 && deltaX > 0))
        return;
    
    // Since our vector always goes from left -> right. A negative deltaX implies a prepend and a positive deltaX
    // implies an append...
    if(deltaX < 0)
        toModify.emplace(toModify.begin(), board[row+deltaY][col+deltaX]);
    else
        toModify.emplace_back(board[row+deltaY][col+deltaX]);

    // recurse with smaller window size
    vectorBuilderHelper(toModify, windowSize-1, row+deltaY, col+deltaX, deltaY, deltaX);
}

/**
 * Helper function that totals all "in-a-row" sequences starting from the last
 * played piece.
 **/
std::vector<std::tuple<int, MNKBoard::PieceDirection, std::tuple<int, int>, std::tuple<int, int>>> MNKBoard::enumerateInARow(void) {
    // Place the initial directions to branch out of
    auto winList = std::vector<std::tuple<int, PieceDirection, std::tuple<int, int>, std::tuple<int, int>>>();
    PieceDirection allDirs[4] = {PieceDirection::BSD, PieceDirection::FSD, PieceDirection::HORZ, PieceDirection::VERT};
    for(auto& dir:allDirs)
        winList.emplace_back(1, dir, lastMove, lastMove);
    const CellState curPlayer = board[std::get<0>(lastMove)][std::get<1>(lastMove)];

    // collect moves as long as moves are possible
    bool changedArray = true;
    int minRowCnt = 1;
    while(changedArray && minRowCnt <= winSize) {
        changedArray = false;
        for(auto& tup:winList) {
            #ifdef MNK_VERBOSE
            tuplePrinterHelper(tup);
            #endif
            PieceDirection& curDir = std::get<1>(tup);
            auto& pieceL = std::get<2>(tup);
            auto& pieceR = std::get<3>(tup);
            int pDelta = 0;

            // update the moves and add points if necessary
            if(curDir == PieceDirection::HORZ)
                pDelta = updateHorzPos(curPlayer, pieceL, pieceR);
            else if(curDir == PieceDirection::VERT)
                pDelta = updateVertPos(curPlayer, pieceL, pieceR);
            else if(curDir == PieceDirection::BSD)
                pDelta = updateBSDPos(curPlayer, pieceL, pieceR);
            else if(curDir == PieceDirection::FSD)
                pDelta = updateFSDPos(curPlayer, pieceL, pieceR);

            // was there any change?
            if(changedArray == false)
                changedArray = pDelta?true:false;
            std::get<0>(tup) += pDelta;
            minRowCnt = std::get<0>(tup)<minRowCnt ? std::get<0>(tup) : minRowCnt;
        }
    }

    return winList;
}

// helpers that return the pointage change for the particular tuple
int MNKBoard::updateVertPos(const CellState player, std::tuple<int, int>& pieceL, std::tuple<int, int>& pieceR) {
    // calculates additional chain length
    int extLen = 0;

    if(std::get<0>(pieceL)-1 >= 0 && board[std::get<0>(pieceL)-1][std::get<1>(pieceL)] == player) {
        std::get<0>(pieceL) -= 1;
        extLen += 1;
    }
    if(std::get<0>(pieceR)+1 < numRows && board[std::get<0>(pieceR)+1][std::get<1>(pieceR)] == player) {
        std::get<0>(pieceR) += 1;
        extLen += 1;
    }

    return extLen;
}

int MNKBoard::updateHorzPos(const CellState player, std::tuple<int, int>& pieceL, std::tuple<int, int>& pieceR) {
    // calculates additional chain length
    int extLen = 0;

    if(std::get<1>(pieceL)-1 >= 0 && board[std::get<0>(pieceL)][std::get<1>(pieceL)-1] == player) {
        std::get<1>(pieceL) -= 1;
        extLen += 1;
    }
    if(std::get<1>(pieceR)+1 < numCols && board[std::get<0>(pieceR)][std::get<1>(pieceR)+1] == player) {
        std::get<1>(pieceR) += 1;
        extLen += 1;
    }

    return extLen;
}

int MNKBoard::updateFSDPos(const CellState player, std::tuple<int, int>& pieceL, std::tuple<int, int>& pieceR) {
    // calculates additional chain length
    int extLen = 0;

    if((std::get<0>(pieceL)+1 < numRows && std::get<1>(pieceL)-1 >=0) 
                                         && board[std::get<0>(pieceL)+1][std::get<1>(pieceL)-1] == player) {
        std::get<0>(pieceL) += 1;
        std::get<1>(pieceL) -= 1;
        extLen += 1;
    }
    if((std::get<0>(pieceR)-1 >= 0 && std::get<1>(pieceR)+1 < numCols)
                                   && board[std::get<0>(pieceR)-1][std::get<1>(pieceR)+1] == player) {
        std::get<0>(pieceR) -= 1;
        std::get<1>(pieceR) += 1;
        extLen += 1;
    }

    return extLen;
}

int MNKBoard::updateBSDPos(const CellState player, std::tuple<int, int>& pieceL, std::tuple<int, int>& pieceR) {
    // calculates additional chain length
    int extLen = 0;

    if((std::get<0>(pieceL)-1 >= 0 && std::get<1>(pieceL)-1 >= 0) 
                                   && board[std::get<0>(pieceL)-1][std::get<1>(pieceL)-1] == player) {
        std::get<0>(pieceL) -= 1;
        std::get<1>(pieceL) -= 1;
        extLen += 1;
    }
    if((std::get<0>(pieceR)+1 < numRows && std::get<1>(pieceR)+1 < numCols) 
                                         && board[std::get<0>(pieceR)+1][std::get<1>(pieceR)+1] == player) {
        std::get<0>(pieceR) += 1;
        std::get<1>(pieceR) += 1;
        extLen += 1;
    }

    return extLen;
}

// helper function for a known tuple format
#ifdef MNK_VERBOSE
    void MNKBoard::tuplePrinterHelper(std::tuple<int, PieceDirection, std::tuple<int, int>, std::tuple<int, int>> tup) {
            int inARowLen = std::get<0>(tup);
            PieceDirection& curDir = std::get<1>(tup);
            auto& pieceL = std::get<2>(tup);
            auto& pieceR = std::get<3>(tup);

            std::cout << "Updated tuple of length " << inARowLen << " located from loc (" << std::get<0>(pieceL) 
                      << "," << std::get<1>(pieceL) << ") to loc (" << std::get<0>(pieceR) << "," 
                      << std::get<1>(pieceR) << ")" << std::endl;
    }
#endif

/**
 * Prints the game board with some formatting. For convenience, assume
 * that a "○" represents white and "●" for a black piece. Naturally, an
 * empty place on the board will be a space, " ".
 **/
std::ostream& operator<<(std::ostream& ostream, const MNKBoard& board) {
    // makes printing the horizontal line simpler
    auto horzLPrinter = [&ostream, &board]() -> void {ostream << std::string(2*board.numCols+1, '-') << std::endl;};
    
    // prints the board now
    horzLPrinter();
    for(int rowInd=0; rowInd<board.numRows; rowInd++) {
        ostream << "|";
        for(int colInd=0; colInd<board.numCols; colInd++) {
            std::string curPiece = " ";
            if(board.board[rowInd][colInd] != CellState::none)
                curPiece = board.board[rowInd][colInd]==CellState::black?"●":"○";
            ostream << curPiece << "|";
        }
        ostream << std::endl;
        horzLPrinter();
    }

    return ostream;
}