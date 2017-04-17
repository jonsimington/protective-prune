////////////////////////////////////////////////////////////////////// 
/// @file custom_board.hpp 
/// @author Shawn McCormick CS5400
/// @brief MiniMax Search functions for Chess
////////////////////////////////////////////////////////////////////// 

#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "impl/chess.hpp"
#include "game.hpp"
#include "game_object.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "player.hpp"

#include "custom_board.hpp"

namespace cpp_client
{

namespace chess
{

// Find the lowest possible value from all actions
float minv(State& state, int depth, const Game& game, float alpha, float beta, int quiescence);

// Find the highest possible value from all actions
float maxv(State& state, int depth, const Game& game, float alpha, float beta, int quiescence);

// Perform Depth-limited Minimax Search, exploring to the target depth
// Parameters:
//      Game& game: The game object
//      State& current_state: The starting state
//      int max_depth: The maximum depth to explore to
// Returns the best action found to take from the given state
MyMove tlmm(const Game& game, State& current_state, int max_depth, int &best_value, int quiescence);

// Perform Time-limited Iterative deepening depth-limited alpha-beta pruning Minimax Search
// Parameters:
//      Game& game: The game object
//      State& current_state: The starting state
//      int max_depth: The maximum depth to explore to
//      int max_time: The maximim time to spend, in seconds
// Returns the best action found to take from the given state 
MyMove tliddlmm(const Game& game, State& current_state, int max_depth=15, int max_time=1, int quiescence=3);

}

}

#endif