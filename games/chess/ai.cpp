// AI
// This is where you build your AI

#include "ai.hpp"

// You can add #includes here for your AI.

#include <cmath>

namespace cpp_client
{

namespace chess
{

/// <summary>
/// This returns your AI's name to the game server.
/// Replace the string name.
/// </summary>
/// <returns>The name of your AI.</returns>
std::string AI::get_name() const
{
    // REPLACE WITH YOUR TEAM NAME!
    return "FreakBot";
}

/// <summary>
/// This is automatically called when the game first starts, once the game objects are created
/// </summary>
void AI::start()
{
    // This is a good place to initialize any variables
    srand(time(NULL));
}

/// <summary>
/// This is automatically called the game (or anything in it) updates
/// </summary>
void AI::game_updated()
{
    // If a function you call triggers an update this will be called before it returns.
}

/// <summary>
/// This is automatically called when the game ends.
/// </summary>
/// <param name="won">true if you won, false otherwise</param>
/// <param name="reason">An explanation for why you either won or lost</param>
void AI::ended(bool won, const std::string& reason)
{
    // You can do any cleanup of your AI here.  The program ends when this function returns.
}

/// <summary>
/// This is called every time it is this AI.player's turn.
/// </summary>
/// <returns>Represents if you want to end your turn. True means end your turn, False means to keep your turn going and re-call this function.</returns>
bool AI::run_turn()
{   
    // Here is where you'll want to code your AI.

    // We've provided sample code that:
    //    1) prints the board to the console
    //    2) prints the opponent's last move to the console
    //    3) prints how much time remaining this AI has to calculate moves
    //    4) makes a random (and probably invalid) move.

    // 2) print the opponent's last move to the console
    if(game->moves.size() > 0)
    {
        std::cout << "Opponent's Last Move: '" << game->moves[game->moves.size() - 1]->san << "'" << std::endl;
    }

    // 3) print how much time remaining this AI has to calculate moves
    std::cout << "Time Remaining: " << player->time_remaining << " ns" << std::endl;

    chess::Piece p = player->pieces[rand() % player->pieces.size()];

    State state(game);

    state.print();


    std::vector<MyMove> moves = state.ACTIONS(game);
    if (moves.size() == 0)
    {
      std::cout << "I can't find any valid moves :(" << std::endl;
      return true;
    }
    int i = rand() % moves.size();
    auto move = moves[i];

    // Print out all possible moves for the moving piece
    const MyPiece* moved = state.getPiece(move.file,move.rank);
    std::cout<<"Moving " << lengthen(moved->type) <<" at " << move.file << move.rank << " to random choice from [";
    for (auto m2: moves)
    {
      if (move.file == m2.file && move.rank == m2.rank)
      {
        std::cout << m2.file2 << m2.rank2 << (m2.promotion == nullptr ? "" : std::string(1, *(m2.promotion))) << (m2.move_type != "Move" ? m2.move_type : "") << ",";
      }
    }
    std::cout << "]" << std::endl;

    State* result = state.RESULT(move);
    result->print();
    delete result;

    for (auto piece : player->pieces)
    {
      if (piece->rank == move.rank && piece->file[0] == move.file)
      {
        piece->move(std::string(1, move.file2), move.rank2, lengthen((move.promotion)));
        break;
      }
    }

    return true; // to signify we are done with our turn.
}


} // chess

} // cpp_client
