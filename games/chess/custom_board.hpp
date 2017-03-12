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
    const char* promotion; // The piece to promote to, for pawns at the final rank
    std::string move_type; // Used to indicate special moves: {"En Passant", "Castle", "Move"}

    // Constructor for MyMove
    MyMove(char startingFile, int startingRank, char targetFile, int targetRank, const char* promotion_=0, std::string _move_type = "Move"): rank(startingRank), file(startingFile), rank2(targetRank), file2(targetFile), promotion(promotion_), move_type(_move_type) {};
};



struct BitBoard
{
    U64 pieces;
};


const U64 universe = 0xffffffffffffffffULL;
const BitBoard RANK_1 = ;
const BitBoard RANK_2 = ;
const BitBoard RANK_3 = ;
const BitBoard RANK_4 = ;
const BitBoard RANK_5 = ;
const BitBoard RANK_6 = ;
const BitBoard RANK_7 = ;
const BitBoard FILE_A = ;
const BitBoard FILE_B = ;
const BitBoard FILE_C = ;
const BitBoard FILE_D = ;
const BitBoard FILE_E = ;
const BitBoard FILE_F = ;
const BitBoard FILE_G = ;

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

    BitBoard(const Game& game);
};

U64 rol(U64 x, int s) { return _rotl64(x, s); };
U64 ror(U64 x, int s) { return _rotr64(x, s); };

U64 genShift(U64 x, int s) {
   return (s > 0) ? (x << s) : (x >> -s);
}

////////////////////////////////////////////////////////////////////// 
/// @class State 
/// @brief Simplified internal representation of a Chess gamestate
////////////////////////////////////////////////////////////////////// 
class State {
  private:
    MyPiece ***board; // A 2d board, containing pointers to Chess pieces
    Board bitboard;
    bool current_player; // The player whose turn it is to make a move: {0 white, 1 black}

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

    // Successor generator
    // Parameters:
    //      MyMove& action: The move to be applied
    // Returns a pointer to the new, resulting State from applying the acion to the current State
    State* RESULT(const MyMove& action) const;

    // Display the current game state
    void print() const;

    // Get the piece at the given file and rank
    // file and rank are in SAN, where file [a,h] and rank [1,8]
    const MyPiece* getPiece(const char& file,const int& rank) const;

};

}

}

#endif
