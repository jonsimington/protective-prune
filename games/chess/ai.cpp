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
    std::cout << "Time Remaining: " << player->time_remaining << " ns" << std::endl;

    chess::Piece p = player->pieces[rand() % player->pieces.size()];

    State state(game);

    state.print();

    auto move = iddlmm(game, state);

    state.RESULT(move).print();
    std::cout<<state.RESULT(move).evaluate(game)<<std::endl;
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
