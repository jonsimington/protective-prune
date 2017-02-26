#ifndef GAMES_CHESS_AI_HPP
#define GAMES_CHESS_AI_HPP

#include "impl/chess.hpp"
#include "game.hpp"
#include "game_object.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "player.hpp"

#include "../../joueur/src/base_ai.hpp"
#include "../../joueur/src/attr_wrapper.hpp"

//#include "custom_board.hpp"

// You can add additional #includes here

namespace cpp_client
{

namespace chess
{


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
    std::vector<std::tuple<int,int>> attacking(int i, int j) const;
    std::vector<std::tuple<int, int>> attacked(int i, int j, int filedir, int rankdir, int range) const;
    int** attacked(int attacker) const;
    State(const Game &game);
    State(const State &original);
    ~State();
    std::vector<MyMove> generate_moves(const Game &game) const;
    State RESULT(MyMove action) const;
    void print() const;

};

/// <summary>
/// This is the header file for building your Chess AI
/// </summary>
class AI : public Base_ai
{
public:
    /// <summary>
    /// This is a reference to the Game object itself, it contains all the information about the current game
    /// </summary>
    Game game;

    /// <summary>
    /// This is a pointer to your AI's player. This AI class is not a player, but it should command this Player.
    /// </summary>
    Player player;

    // You can add additional class variables here.

    /// <summary>
    /// This returns your AI's name to the game server.
    /// Replace the string name.
    /// </summary>
    /// <returns>The name of your AI.</returns>
    virtual std::string get_name() const override;

    /// <summary>
    /// This is automatically called when the game first starts, once the game objects are created
    /// </summary>
    virtual void start() override;

    /// <summary>
    /// This is automatically called when the game ends.
    /// </summary>
    /// <param name="won">true if you won, false otherwise</param>
    /// <param name="reason">An explanation for why you either won or lost</param>
    virtual void ended(bool won, const std::string& reason) override;

    /// <summary>
    /// This is automatically called the game (or anything in it) updates
    /// </summary>
    virtual void game_updated() override;

    /// <summary>
    /// This is called every time it is this AI.player's turn.
    /// </summary>
    /// <returns>Represents if you want to end your turn. True means end your turn, False means to keep your turn going and re-call this function.</returns>
    bool run_turn();

    /// <summary>
    ///  Prints the current board using pretty ASCII art
    /// </summary>
    /// <remarks>
    /// Note: you can delete this function if you wish
    /// </remarks>
    void print_current_board();

    // You can add additional methods here.



    // ####################
    // Don't edit these!
    // ####################
    /// \cond FALSE
    virtual std::string invoke_by_name(const std::string& name,
                                       const std::unordered_map<std::string, Any>& args) override;
    virtual void set_game(Base_game* ptr) override;
    virtual void set_player(std::shared_ptr<Base_object> obj) override;
    virtual void print_win_loss_info() override;
    /// \endcond
    // ####################
    // Don't edit these!
    // ####################

};

} // CHESS

} // cpp_client

#endif // GAMES_CHESS_AI_HPP
