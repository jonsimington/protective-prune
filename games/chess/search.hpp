#include "custom_board.hpp"

namespace cpp_client
{

namespace chess
{

struct Node
{
  State state;
  MyMove action;
  float value;
  int depth;
  Node(State _state, MyMove _action, float _value, int _depth) : state(_state), action(_action), value(_value), depth(_depth) {};
};

int value(const char* pieceType);

float minv(Node node, int depth, int &alpha, int &beta, bool maxPlayer);

float maxv(Node node, int depth, int &alpha, int &beta, bool maxPlayer);

MyMove dlmm(const Game& game, State& current_state, int max_depth);

MyMove iddlmm(const Game& game, State& current_state);

}

}