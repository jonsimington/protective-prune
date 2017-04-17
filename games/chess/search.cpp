////////////////////////////////////////////////////////////////////// 
/// @file search.cpp 
/// @author Shawn McCormick CS5400
/// @brief MiniMax Search functions for Chess
////////////////////////////////////////////////////////////////////// 

#include "search.hpp"
#include <time.h>

namespace cpp_client
{

namespace chess
{

float minv(State& state, int depth, const Game& game, float alpha, float beta, int quiescence)
{
  if (depth == 0) // The depth limit has been reached, so evaluate the board state using our heuristic
  { 
    // Search deeper if the state is non-quiescent
    if (quiescence > 0 && !state.quiescent(game))
    {
      quiescence--;
      depth++;
    }
    else
      return state.evaluate(game);
  }
  float best_value = std::numeric_limits<float>::infinity();
  std::vector<MyMove> actions = state.ACTIONS(game);
  
  for (MyMove action : actions) // Find the min of all neighbors
  {
    auto preserved = state.APPLY(action);
    float new_val = maxv(state, depth - 1, game, alpha, beta, quiescence); 
    state.UNDO(action, preserved);
    if (new_val > alpha && new_val < beta)
    {
      beta = new_val;
    }
    else if (new_val <= alpha) // fail low, so prune
    {
      return new_val;
    }
    best_value = std::min(best_value, new_val);
    // else fail high, so ignore
  }

  if (best_value == std::numeric_limits<float>::infinity())
  {
    // There are no moves remaining, so a checkmate or stalemate has occurred
    return state.evaluate(game);
  }
  return best_value;
}

float maxv(State& state, int depth, const Game& game, float alpha, float beta, int quiescence)
{
  if (depth == 0) // The depth limit has been reached, so evaluate the board state using our heuristic
    return state.evaluate(game);
  float best_value = -std::numeric_limits<float>::infinity();
  std::vector<MyMove> actions = state.ACTIONS(game);

  for (MyMove action : actions) // Find the max of all neighbors
  {
    auto preserved = state.APPLY(action);
    float new_val = minv(state, depth - 1, game, alpha, beta, quiescence); 
    state.UNDO(action, preserved);

    if (new_val > alpha && new_val < beta)
    {
      alpha = new_val;
    }
    else if (new_val >= beta) // fail high, so prune
    {
      return new_val;
    }
    best_value = std::max(best_value, new_val);
    // else fail low, so ignore
  }

  if (best_value == -std::numeric_limits<float>::infinity())
  {
    // There are no moves remaining, so a checkmate or stalemate has occurred
    return state.evaluate(game);
  }
  return best_value;
}

MyMove dlmm(const Game& game, State& current_state, int max_depth, int &best_value, int quiescence)
{
  float alpha = -std::numeric_limits<float>::infinity();
  float beta = std::numeric_limits<float>::infinity();
  MyMove best_action;

  auto actions = current_state.ACTIONS(game);

  for (auto action: actions)
  {
    auto preserved = current_state.APPLY(action);
    float new_val = minv(current_state, max_depth - 1, game, alpha, beta, quiescence); 
    current_state.UNDO(action, preserved);
    if (new_val > alpha)
    {
      alpha = new_val;
      best_action = action;
    }
    // else fail low, so ignore
  }

  best_value = alpha;
  return best_action;
}

MyMove tliddlmm(const Game& game, State& current_state, int max_depth, int max_time, int quiescence)
{
  MyMove best_action;
  int best_value;
  time_t start = time(NULL);
  for (int i = 1; i <= max_depth; i++)
  {
    best_action = dlmm(game, current_state, i, best_value, quiescence);

    if (best_value >= 10000 || best_value <= -10000) // Checkmate imminent, no need to keep searching
    {
      break;
    }
    else if (difftime(time(NULL), start) > max_time) // Time limit exceeded; return current best action
    {
      break;
    }
  }
  return best_action;
}

} //chess

} //cpp_client