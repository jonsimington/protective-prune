////////////////////////////////////////////////////////////////////// 
/// @file search.cpp 
/// @author Shawn McCormick CS5400
/// @brief MiniMax Search functions for Chess
////////////////////////////////////////////////////////////////////// 

#include "search.hpp"

namespace cpp_client
{

namespace chess
{

float minv(State& state, int depth, const Game& game)
{
  if (depth == 0) // The depth limit has been reached, so evaluate the board state using our heuristic
    return state.evaluate(game);
  float best_value = std::numeric_limits<float>::infinity();
  auto actions = state.ACTIONS(game);
  
  for (int i = 0; i < actions.size(); i++) // Find the min of all neighbors
  {
    best_value = std::min(best_value, maxv(actions[i].second, depth - 1, game));
  }

  if (best_value == -std::numeric_limits<float>::infinity())
  {
    // There are no moves remaining, so a checkmate or stalemate has occurred
    return state.evaluate(game);
  }
  return best_value;
}

float maxv(State& state, int depth, const Game& game)
{
  if (depth == 0) // The depth limit has been reached, so evaluate the board state using our heuristic
    return state.evaluate(game);
  float best_value = -std::numeric_limits<float>::infinity();
  std::vector<std::pair<MyMove, State>> actions = state.ACTIONS(game);

  for (auto s2 : actions) // Find the max of all neighbors
  {
    best_value = std::max(best_value, minv(s2.second, depth - 1, game));
  }

  if (best_value == std::numeric_limits<float>::infinity())
  {
    // There are no moves remaining, so a checkmate or stalemate has occurred
    return state.evaluate(game);
  }
  return best_value;
}

MyMove dlmm(const Game& game, State& current_state, int max_depth)
{
  float best_value = -std::numeric_limits<float>::infinity();
  MyMove best_action;

  auto neighbors = current_state.ACTIONS(game);

  for (auto neighbor: neighbors)
  {
    float new_val = minv(neighbor.second, max_depth - 1, game);
    if (new_val > best_value)
    {
      best_action = neighbor.first;
      best_value = new_val;
    }
  }

  return best_action;
}

MyMove iddlmm(const Game& game, State& current_state, int max_depth)
{
  MyMove best_action;
  for (int i = 1; i <= max_depth; i++)
  {
    best_action = dlmm(game, current_state, i);
  }
  return best_action;
}

} //chess

} //cpp_client