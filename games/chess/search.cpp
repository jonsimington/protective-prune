#include "custom_board.hpp"

namespace cpp_client
{

namespace chess
{


int value(const char* pieceType)
{
  if (pieceType == &PAWN)
    return 1;
  else if (pieceType == &ROOK)
    return 5;
  else if (pieceType == &BISHOP)
    return 3;
  else if (pieceType == &KNIGHT)  
    return 3;
  else if (pieceType == &QUEEN)
    return 9;
  return 0;
}

float minv(Node node, int depth, int &alpha, int &beta, bool maxPlayer)
{
  if (depth == 0) // The depth limit has been reached, so evaluate the board state using our heuristic
    return evaluate(s2.second);
  float best_value = -std::numeric_limits<float>::infinity();
  for (auto s2 : node.state.ACTIONS(game))
  {
    float h = evaluate(s2.second, maxPlayer);
    if (h > alpha && h < beta)
    {
      beta = h;
      best_value = h;
    }
    else if (h <= alpha) // Fail low, so prune
    {
      best_value = h;
      break;
    }
    // else fail high, so ignore
  }
  if (best_value == -std::numeric_limits<float>::infinity())
  {
    // There are no moves remaining, so a checkmate or stalemate has occurred
    
  }
  return best_value;
}

MyMove dlmm(const Game& game, State& current_state, int max_depth)
{
  /*
  Perform Depth-limited Minimax Search
  Returns:
    The best action to take from the given state
  */
  float alpha = -std::numeric_limits<float>::infinity();
  float beta = std::numeric_limits<float>::infinity();
  frontier = [Node(current_state, None, 0, 0, None)]
  explored = set([frontier[0].state])

  float best_value = -std::numeric_limits<float>::infinity();
  MyMove *best_action;

  while(frontier)
  {
    n = frontier.pop(-1)

    auto neighbors = n.state.NEIGHBORS(game);
    for (auto neighbor: neighbors)
    {
      float new_val = maxv(new_node, depth - 1, alpha, beta, current_state.current_player);
      if (new_val > best_value)
      {
        new_val = value;
        if (best_action == nullptr)
          best_action = new MyMove();
        *best_action = neighbor.first();
      }
      if (n.depth + 1) <= max_depth and neighbor not in explored and neighbor not in [x.state for x in frontier]:
        frontier.append(Node(neighbor, action, n.path_cost + puzzle.COST(neighbor), n.depth + 1, n));
        explored.add((neighbor));
    }
  }
  return best_action;
}

MyMove iddlmm(const Game& game, State& current_state)
{
  MyMove best_action;
  int max_depth = 2;
  for (int i = 1; i < max_depth; i++)
    best_action = dlmm(game, current_state, i);
  return best_action;
}

}

}