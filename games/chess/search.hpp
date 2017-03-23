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
float minv(State& state, int depth, const Game& game, float alpha, float beta);

// Find the highest possible value from all actions
float maxv(State& state, int depth, const Game& game, float alpha, float beta);

// Perform Depth-limited Minimax Search, exploring to the target depth
// Parameters:
//      Game& game: The game object
//      State& current_state: The starting state
//      int max_depth: The maximum depth to explore to
// Returns the best action found to take from the given state
MyMove dlmm(const Game& game, State& current_state, int max_depth, int &best_value);

// Perform Iterative deepening depth-limited Minimax Search at the depth of max_depth
// Parameters:
//      Game& game: The game object
//      State& current_state: The starting state
//      int max_depth: The maximum depth to explore to
// Returns the best action found to take from the given state 
MyMove iddlmm(const Game& game, State& current_state, int max_depth=3);

}

}

#endif