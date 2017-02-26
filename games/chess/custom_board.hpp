#ifndef CUSTOM_BOARD_HPP
#define CUSTOM_BOARD_HPP

#include "impl/chess.hpp"
#include "game.hpp"
#include "game_object.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "player.hpp"

// You can add additional #includes here

namespace cpp_client
{

namespace chess
{
using pair = std::pair<int, int>;

const std::string promotions[4] = {"Rook", "Knight", "Bishop", "Queen"};

const int LR[2] = {-1, 1};

const char BLUE_FG[] = { 0x1b, '[', '0', ';', '3', '4', 'm', 0 };
const char WHITE_FG[] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };
const char RED_FG[] = {0x1b, '[', '0', ';', '3', '1', 'm'};
const char BLACK_BG[] = "\033[40m";
const char WHITE_BG[] = "\033[100m";

const char KING[] = "King";
const char QUEEN[] = "Queen";
const char KNIGHT[] = "Knight";
const char BISHOP[] = "Bishop";
const char ROOK[] = "Rook";
const char PAWN[] = "Pawn";

const std::vector<pair> CARDINAL = {pair(0, 1), pair(0, -1), pair(1, 0), pair(-1, 0)};
const std::vector<pair> ORDINAL = {pair(1, 1), pair(1, -1), pair(-1, 1), pair(-1, -1)};
const std::vector<pair> PAWN_ATTACKS[2] = {{pair(1, 1), pair(-1, 1)}, {pair(1, -1), pair(-1, -1)}};
const std::vector<pair> KNIGHT_MOVES = {pair(2, 1), pair(2, -1), pair(-2, 1), pair(-2, -1), pair(1, 2), pair(1, -2), pair(-1, 2), pair(-1, -2)};
const std::vector<pair> KING_MOVES = {pair(-1, -1), pair(-1, 0), pair(-1, 1), pair(0, -1), pair(0, 1), pair(1, -1), pair(1, 0), pair(1, 1)};
const std::vector<pair> ROOK_MOVES = {pair(-1, 0), pair(0, -1), pair(0, 1), pair(1, 0)};
const std::vector<pair> BISHOP_MOVES = {pair(-1, -1), pair(-1, 1), pair(1, -1), pair(1, 1)};
const std::vector<pair> CASTLING = {{pair(-4, 0), pair(3, 0)}};

std::string lengthen(char name);

class MyPiece {
    public:
        std::string type;
        bool has_moved;
        bool owner;
        MyPiece(const std::string _type, const bool _has_moved, const bool _owner) : type(_type), has_moved(_has_moved), owner(_owner) {};
};

struct MyMove {
    char file;
    int rank;
    char file2;
    int rank2;
    char promotion;
    std::string move_type;

    MyMove(char startingFile, int startingRank, char targetFile, int targetRank, char promotion_='\0', std::string _move_type = "Move"): rank(startingRank), file(startingFile), rank2(targetRank), file2(targetFile), promotion(promotion_), move_type(_move_type) {};
};

class State {
  private:
    MyPiece ***board;
    bool current_player;
  public:
    bool in_check(int i, int j, int attacker) const;
    bool in_check(const MyMove& action);

    State(const Game &game);
    State(const State &original);
    ~State();
    std::vector<MyMove> generate_moves(const Game &game);
    State* RESULT(MyMove action) const;
    void print() const;

};

}

}

#endif
