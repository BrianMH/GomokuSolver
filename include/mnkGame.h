#ifndef MNKGAME_H
#define MNKGAME_H

// Required imports
#include <iostream>
#include <utility>
#include <vector>
#include <tuple>

// VERBOSE OUTPUT
// #define MNK_VERBOSE

/**
 * MNKBoard
 * 
 * Implements the basic m x n board for a k-in-a-row
 * game.
 **/

// Enumerates possible board states
// This can technically be extended
enum class CellState: char{
    none,   // only used for initialization of the board
    black,
    white
};

// Hashing class for the CellState class (more generally for enum classes)
struct EnumClassHash {
    template <typename T>
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

class MNKBoard{
private:
    unsigned int numRows;
    unsigned int numCols;

protected:
    unsigned int winSize;
    CellState **board;  // stores board in row-major format (x,y)
    CellState **flippedBoard;  // stores board in column-major format (y,x)
    std::tuple<int, int> lastMove;

    // used to detect win directions and collect move
    enum class PieceDirection: char{
        VERT,       // [up / down]
        HORZ,       // [left / right]
        FSD,        // [ie. bottom left to uper right]
        BSD         // [ie. upper left to bottom right]
    };

    // some helper functions that might be useful for other classes
    int updateVertPos(const CellState player, std::tuple<int, int>& pieceL, 
                        std::tuple<int, int>& pieceR);
    int updateHorzPos(const CellState player, std::tuple<int, int>& pieceL, 
                        std::tuple<int, int>& pieceR);
    int updateFSDPos(const CellState player, std::tuple<int, int>& pieceL, 
                        std::tuple<int, int>& pieceR);
    int updateBSDPos(const CellState player, std::tuple<int, int>& pieceL, 
                        std::tuple<int, int>& pieceR);

    // creates a list that sums up all pieces in a row (given last played piece)
    std::vector<std::tuple<int, PieceDirection, std::tuple<int, int>, std::tuple<int, int>>> enumerateInARow(void);

    // queries a specific row or column and returns a vector containing the requisite values
    const std::vector<CellState> getRowVec(int rowInd);
    const std::vector<CellState> getColVec(int colInd);

    // queries a particular directional diagonal given a point's placement
    const std::vector<CellState> getForwardDiagVec(int pieceRow, int pieceCol); // (/)-directional
    const std::vector<CellState> getBackDiagVec(int pieceRow, int pieceCol); // (\)-directional

    // helps prints tuples for feedback
    #ifdef MNK_VERBOSE
    void tuplePrinterHelper(std::tuple<int, PieceDirection, std::tuple<int, int>, std::tuple<int, int>>);
    #endif

public:
    // Init m x n game of k in a row
    MNKBoard(int m, int n, int k);
    MNKBoard(const MNKBoard& otherBoard) = delete;
    MNKBoard& operator=(const MNKBoard& otherBoard) = delete;
    ~MNKBoard();

    // Checks for win condition (based on last move)
    bool checkWin(void);

    // movements values (usually changed by game basis)
    // returns true if the piece was placed succesfully, otherwise false.
    virtual bool placePiece(int row, int col, CellState state, bool updateLast = true);

    // clears the board entirely
    void clearBoard(void);

    // reports the size of the board
    std::tuple<int, int> getBoardSize(void);

    // returns whether current board position is empty
    bool isPosEmpty(int row, int col);

    // prints the game board
    friend std::ostream& operator<<(std::ostream& ostream, const MNKBoard& board);
};

#endif