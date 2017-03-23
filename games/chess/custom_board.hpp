////////////////////////////////////////////////////////////////////// 
/// @file custom_board.hpp 
/// @author Shawn McCormick CS5400
/// @brief Custom classes for storing and manipulating Chess state
////////////////////////////////////////////////////////////////////// 

#ifndef CUSTOM_BOARD_HPP
#define CUSTOM_BOARD_HPP

#include "impl/chess.hpp"
#include "game.hpp"
#include "game_object.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "player.hpp"
#include <algorithm>
typedef unsigned long long U64;

namespace cpp_client
{

namespace chess
{
// Ordered pair to represent (file, rank) or (file_direction, rank_direction)
using pair = std::pair<int, int>;

// Left and right (or backward and forward)
const int LR[2] = {-1, 1};


// Color combinations for board visualization
const char WHITE_FG[] = "\033[0;39m";
const char BLACK_BG[] = "\033[40m";
const char WHITE_BG[] = "\033[100m";

// Character representation of pieces
const char KING = 'K';
const char QUEEN = 'Q';
const char KNIGHT = 'N';
const char BISHOP = 'B';
const char ROOK = 'R';
const char PAWN = 'P';

// Move directions available to units
const std::vector<pair> PAWN_ATTACKS[2] = {{pair(1, 1), pair(-1, 1)}, {pair(1, -1), pair(-1, -1)}};
const std::vector<pair> KNIGHT_MOVES = {pair(2, 1), pair(2, -1), pair(-2, 1), pair(-2, -1), pair(1, 2), pair(1, -2), pair(-1, 2), pair(-1, -2)};
const std::vector<pair> KING_MOVES = {pair(-1, -1), pair(-1, 0), pair(-1, 1), pair(0, -1), pair(0, 1), pair(1, -1), pair(1, 0), pair(1, 1)};
const std::vector<pair> ROOK_MOVES = {pair(-1, 0), pair(0, -1), pair(0, 1), pair(1, 0)};
const std::vector<pair> BISHOP_MOVES = {pair(-1, -1), pair(-1, 1), pair(1, -1), pair(1, 1)};
const std::vector<pair> CASTLING = {{pair(-4, 0), pair(3, 0)}};

// Convert from the shortened single character to the piece's full type
std::string lengthen(const char* name);

////////////////////////////////////////////////////////////////////// 
/// @class MyPiece 
/// @brief A Chess game piece
////////////////////////////////////////////////////////////////////// 
struct MyPiece {
    const char* type; // The piece type: {"King", "Queen", "Knight", "Rook", "Bishop", "Pawn"}
    bool has_moved; // Whether this piece has already moved in this game
    bool owner; // Who owns this piece: {0 for white, 1 for black}

    // Constructor for MyPiece
    MyPiece(const char* _type, const bool _has_moved, const bool _owner) : type(_type), has_moved(_has_moved), owner(_owner) {};
};

////////////////////////////////////////////////////////////////////// 
/// @class MyMove 
/// @brief A movement from one tile to another
////////////////////////////////////////////////////////////////////// 
struct MyMove {
    char file; // The starting file
    int rank; // The starting rank
    char file2; // The target file
    int rank2; // The target rank
    const char* capture; // The piece type being captured
    const char* promotion; // The piece to promote to, for pawns at the final rank
    std::string move_type; // Used to indicate special moves: {"En Passant", "Castle", "Move", "None"}

    // Constructor for MyMove
    MyMove() : move_type("None") {};
    MyMove(char startingFile, int startingRank, char targetFile, int targetRank, const char* capture_=0, const char* promotion_=0, std::string _move_type = "Move"): rank(startingRank), file(startingFile), rank2(targetRank), file2(targetFile), capture(capture_), promotion(promotion_), move_type(_move_type) {};
};



struct BitBoard
{
    U64 pieces;
};


const U64 universe = 0xffffffffffffffffULL;

const U64 WHITE_PIECES[] = {
        0x1000000000000000ULL, // King
        0x0800000000000000ULL, // Queen
        0x8100000000000000ULL, // Rook
        0x2400000000000000ULL, // Bishop
        0x4200000000000000ULL, // Knight
        0x00ff000000000000ULL, // Pawn
};

const U64 BLACK_PIECES[] = {
        0x0000000000000010ULL, // King
        0x0000000000000008ULL, // Queen
        0x0000000000000081ULL, // Rook
        0x0000000000000024ULL, // Bishop
        0x0000000000000042ULL, // Knight
        0x000000000000ff00ULL, // Pawn
};

const U64 RANKS[] = {
        0xff00000000000000ULL, 
        0x00ff000000000000ULL, 
        0x0000ff0000000000ULL, 
        0x000000ff00000000ULL, 
        0x00000000ff000000ULL, 
        0x0000000000ff0000ULL, 
        0x000000000000ff00ULL,
        0x00000000000000ffULL,
};

const U64 FILES[] = {
        0x0101010101010101ULL,
        0x0202020202020202ULL,
        0x0404040404040404ULL,
        0x0808080808080808ULL,
        0x1010101010101010ULL,
        0x2020202020202020ULL,
        0x4040404040404040ULL,
        0x8080808080808080ULL,
};

struct Board
{
    BitBoard whitePawns;
    BitBoard whiteRooks;
    BitBoard whiteKnights;
    BitBoard whiteBishops;
    BitBoard whiteQueens;
    BitBoard whiteKing;

    BitBoard blackPawns;
    BitBoard blackRooks;
    BitBoard blackKnights;
    BitBoard blackBishops;
    BitBoard blackQueens;
    BitBoard blackKing;

    BitBoard whitePieces;
    BitBoard blackPieces;
    BitBoard pieces;

};

U64 rol(U64 x, int s);
U64 ror(U64 x, int s);

U64 genShift(U64 x, int s);

////////////////////////////////////////////////////////////////////// 
/// @class State 
/// @brief Simplified internal representation of a Chess gamestate
////////////////////////////////////////////////////////////////////// 
class State {
  private:
    MyPiece ***board; // A 2d board, containing pointers to Chess pieces
    Board bitboard;
    bool current_player; // The player whose turn it is to make a move: {0 white, 1 black}
    int last_capture; // The number of moves since the last pawn move or piece capture

  public:
    // Determine whether the target rank and file are in check by attacker
    // Parameters:
    //      int i: the target file
    //      int j: the target rank
    //      int attacker: The player who is attacking; 0 for black, 1 for white
    // Returns true if the tile at board[i][j] is being attacked by attacker, else false
    bool in_check(int i, int j, int attacker) const;

    // Determine whether the move would result in the current player's king being in check
    //      It modifes the current State but then restores it to its original state
    //      in order to avoid having to use the copy construtor.
    // Parameters:
    //      MyMove& action: the action to be performed
    // Returns true if the current_player's king is in check, else false
    bool in_check(const MyMove& action);

    // Determine whether the current state is in check
    // Returns true if the current_player's king is in check, else false
    bool in_check() const;

    // Determines whether the given state is a draw
    bool stalemate() const;

    // Determines whether the state is an end state; i.e. a stalemate or checkmate occurred
    int goal_reached(const Game& game);

    // Material advantage of the current state
    int material_advantage(bool maxPlayer);

    // Perform heuristic on current state
    float evaluate(const Game& game);

    // Value of piece types
    int value(const char* pieceType) const;


    // Construct the State from the MMAI framework game state
    State(const Game &game);

    // State copy constructor
    State(const State &original);
    
    // State destructor
    ~State();

    // Move Generator
    // Parameters:
    //      Game& game: The current game state; used to retrieve previous moves
    // Returns a vector of moves specifying which actions can be taken from the current state
    std::vector<MyMove> ACTIONS(const Game &game);

    // Reduced Move Generator
    // Parameters:
    //      Game& game: The current game state; used to retrieve previous moves
    // Returns true if moves exist from the state, else false
    bool actions_exist(const Game &game);

    // Successor generator
    // Parameters:
    //      MyMove& action: The move to be applied
    // Returns a pointer to the new, resulting State from applying the acion to the current State
    State RESULT(const MyMove& action) const;


    // Applies a move
    std::vector<std::pair<pair, MyPiece*>> APPLY(const MyMove& action);

    // Undoes a move
    void UNDO(const MyMove& action, std::vector<std::pair<pair, MyPiece*>>);

    // Display the current game state
    void print() const;

    // Get the piece at the given file and rank
    // file and rank are in SAN, where file [a,h] and rank [1,8]
    const MyPiece* getPiece(const char& file,const int& rank) const;

};

}

}

#endif
