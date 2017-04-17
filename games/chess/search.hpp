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
#include <unordered_map>

namespace cpp_client
{

namespace chess
{

using hist = std::unordered_map<std::string, int>;

// Find the lowest possible value from all actions
float minv(State& state, int depth, const Game& game, float alpha, float beta, int quiescence, hist& history);

// Find the highest possible value from all actions
float maxv(State& state, int depth, const Game& game, float alpha, float beta, int quiescence, hist& history);

// Perform Depth-limited Minimax Search w/ quiescence search + history table
// Parameters:
//      Game& game: The game object
//      State& current_state: The starting state
//      int max_depth: The maximum depth to explore to
//      float alpha: The best value available for the max player
//      float beta: The best value available for the min player
//      int quiescence: Number of quiescence-search depth increases allowed
//      std::unordered_map<std::string, int> history: The move history table
// Returns the best action found to take from the given state
MyMove dlmm(const Game& game, State& current_state, int max_depth, int &best_value, int quiescence, hist& history);

// Perform Time-limited Iterative deepening depth-limited alpha-beta pruning Minimax Search
// Parameters:
//      Game& game: The game object
//      State& current_state: The starting state
//      int max_depth: The maximum depth to explore to
//      int max_time: The maximim time to spend, in seconds
//      int quiescence: Number of quiescence-search depth increases allowed
// Returns the best action found to take from the given state 
MyMove tliddlmm(const Game& game, State& current_state, int max_depth=15, int max_time=1, int quiescence=3);

}

}

#endif