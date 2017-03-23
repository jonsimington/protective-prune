////////////////////////////////////////////////////////////////////// 
/// @file custom_board.cpp 
/// @author Shawn McCormick CS5400
/// @brief Implementation of classes for manipulating Chess states
////////////////////////////////////////////////////////////////////// 

#include "custom_board.hpp"

#include <cmath>

namespace cpp_client
{

namespace chess
{

// Bitboard operations
U64 rol(U64 x, int s) { 
    if (!s) return x;
    return (x << s) | (x >>(64-s));
};
U64 ror(U64 x, int s) { 
    if (!s) return x;
    return (x >> s) | (x <<(64-s));
};

U64 genShift(U64 x, int s) {
   return (s > 0) ? (x << s) : (x >> -s);
};

// Types that pawns can be promoted to
const std::vector<const char*> promotions = {&ROOK, &KNIGHT, &BISHOP, &QUEEN};

// Bounds checking
bool iB(int i)
{
  return 0 <= i && i < 8;
}

// Get the character representation of the name
const char* shorten(std::string name)
{
  if (name == "Knight")
    return &KNIGHT;
  else if (name == "King")
    return &KING;
  else if (name == "Queen")
    return &QUEEN;
  else if (name == "Bishop")
    return &BISHOP;
  else if (name == "Rook")
    return &ROOK;
  else if (name == "Pawn")
    return &PAWN;
}

// Unicode chess pieces
std::string unicode(char repr)
{
  switch(repr) {
    case 'n':
      return "\u2658";
    case 'k':
      return "\u2654";
    case 'q':
      return "\u2655";
    case 'r':
      return "\u2656";
    case 'p':
      return "\u2659";
    case 'b':
      return "\u2657";
    case 'N':
      return "\u265E";
    case 'K':
      return "\u265A";
    case 'Q':
      return "\u265B";
    case 'R':
      return "\u265C";
    case 'P':
      return "\u265F";
    case 'B':
      return "\u265D";
  }
  return " ";
}

// Lengthen a name
std::string lengthen(const char* name)
{
  if (name == nullptr)
    return "";
  switch(*name) {
    case KNIGHT:
      return "Knight";
    case KING:
      return "King";
    case QUEEN:
      return "Queen";
    case ROOK:
      return "Rook";
    case PAWN:
      return "Pawn";
    case BISHOP:
      return "Bishop";
  }
  return "";

}

bool State::in_check(int i, int j, int attacker) const
{
  // Determine whether the given tile is under attack by the opponent
  for (auto move: KNIGHT_MOVES)
  {
    int file = i + move.first;
    int rank = j + move.second;
    if (iB(file) && iB(rank))
    {
      auto* piece = board[file][rank];
      if (piece != nullptr && piece->owner == attacker && piece->type == &KNIGHT)
        return true;
    }
  }

  // Look in reverse direction since we are starting at the target
  for (auto move: PAWN_ATTACKS[(attacker + 1) % 2])
  {
    int file = i + move.first;
    int rank = j + move.second;
    if (iB(file) && iB(rank))
    {
      auto* piece = board[file][rank];
      if (piece != nullptr && piece->owner == attacker && piece->type == &PAWN)
        return true;
    }
  }

  for (auto move: KING_MOVES)
  {
    int fd = move.first;
    int rd = move.second;
    for (int range = 1; range < 8; range++)
    {
      int file = i + fd * range;
      int rank = j + rd * range;
      if (!iB(file) || !iB(rank))
        break;

      auto *piece = board[file][rank];
      if (piece != nullptr)
      {
        if (piece->owner == attacker)
        {
          if ((range == 1 && piece->type == &KING) || (piece->type == &QUEEN) || (piece->type == &BISHOP && (abs(fd + rd) == 2 || fd + rd == 0)) || (piece->type == &ROOK && abs(fd + rd) == 1))
          {
            return true;
          }
        }
        break;
      }
    }
  }
  return false;
}

State::State(const Game& game)
{
  // Construct the state from the game
  current_player = (game->current_turn % 2);

  board = new MyPiece**[8];
  for (int i = 0; i < 8; i++)
  {
    board[i] = new MyPiece*[8];
    for (int j = 0; j < 8; j++)
    {
      board[i][j] = nullptr;
    }
  }

  for (cpp_client::chess::Piece piece : game->pieces)
  {
    int i = piece->file[0] - 'a';
    int j = piece->rank - 1;
    bool actually_moved = piece->has_moved;
    // Pawns not in their starting rank have already moved
    if (piece->type == "Pawn" && piece->rank != (piece->owner == game->players[0] ? 2 : 6))
      actually_moved = true;
    board[i][j] = new MyPiece(shorten(piece->type), actually_moved, piece->owner == game->players[1]); 
  }

}

State::State(const State& original)
{
  // Copy constructor
  board = new MyPiece**[8];
  for (int i = 0; i < 8; i++)
  {
    board[i] = new MyPiece*[8];
    for (int j = 0; j < 8; j++)
    {
      if (original.board[i][j] == nullptr)
      {
        board[i][j] = nullptr;
      }
      else
      {
        board[i][j] = new MyPiece(*original.board[i][j]);
      }
    }
  }
  current_player = original.current_player;
}

State::~State()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
      delete board[i][j];
    delete[] board[i];
  }
  delete[] board;
}

std::vector<MyMove> State::ACTIONS(const Game &game)
{
  std::vector<MyMove> moves;

  // Determine ability to castle & en passant
  int j = 0;
  for (int i = 0; i < 4; i++)
  {
    while(game->fen[j++] != ' ');
    i++;
  }

  std::string token = game->fen.substr(j);
  std::string castle = token.substr(0, token.find(" "));
  while(game->fen[j++] != ' ');
  token = game->fen.substr(j);
  std::string enPassant = token.substr(0, token.find(" "));

  // Iterate through the entire board
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (board[i][j] == nullptr || board[i][j]->owner != current_player)
      {
        continue;
      }
      MyPiece* piece = board[i][j];
      const char* type = piece->type;

      const int forward = (current_player == 0 ? 1 : -1);
      char file = 'a' + i;
      int rank = j + 1;

      
      if (type == &PAWN)
      {
        // Pawns can move 2 spaces forward if
        //    there are no pieces between the pawn and the target square or on the target square,
        //    the pawn is in its starting rank
        if (iB(j+forward*2) && !piece->has_moved && board[i][j+forward] == nullptr && board[i][j+forward*2] == nullptr)
        {
            moves.push_back(MyMove(file, rank, file, rank + forward*2));
        }

        // Pawns can move 1 space forward if
        //    there are no pieces on the target square
        if (iB(j+forward) && board[i][j+forward] == nullptr)
        {
          // Pawns can be promoted 
          //    if they advance to the final rank
          if (0 == j + forward || 7 == j + forward)
          {
            for (auto promotion : promotions)
            {
              moves.push_back(MyMove(file, rank, file, rank + forward, nullptr, promotion));
            }
          }
          else
            moves.push_back(MyMove(file, rank, file, rank + forward));
        }

        // Pawns can move 1 space forward diagonally if
        //    there is an enemy piece on the target square
        for (int dir : LR)
        {
          if (iB(i+dir) && iB(j+forward) && board[i+dir][j+forward] != nullptr)
          {
            MyPiece *captured = board[i+dir][j+forward];
            if (piece->owner != captured->owner)
            {
              // Pawns can be promoted 
              //    if they advance to the final rank
              if (j + forward == 0 || j + forward == 7)
              {
                for (auto promotion : promotions)
                {
                  moves.push_back(MyMove(file, rank, file+dir, rank+forward, captured->type, promotion));
                }
              }
              else
                moves.push_back(MyMove(file, rank, file+dir, rank+forward));
            }
          }
        }



        // Pawns can perform En Passant if
        //    the previous move was a pawn advancing two squares
        //    the pawn is now adjacent to this pawn
        if (enPassant[0] != '-')
        {
          if ((enPassant[1] - '0' - rank - forward) == 0 && abs((int)(enPassant[0] - 'a') - i) == 1)
            {
              int dir = enPassant[0] - file;
              moves.push_back(MyMove(file, rank, file+dir, rank+forward, 0, "En Passant"));
            }
        }



      }
      else if (type == &KING)
      {
        // Kings can move 1 space in any direction if
        //    the target square is not a piece owned by the player
        for (auto direction: KING_MOVES)
        {
          int fd = direction.first; int rd = direction.second;
          if (!iB(i+fd) || !iB(j+rd))
            continue;
          auto *newloc = board[i+fd][j+rd];
          if (newloc == nullptr || newloc->owner != current_player)
            moves.push_back(MyMove(file, rank, file+fd, rank+rd, newloc == nullptr ? nullptr : newloc->type));
        }

        // The king can castle with a friendly rook if
        //    both it and the castling rook have not moved,
        //    the king does not pass through a square that is attacked,
        //    the king is not in check,
        //    there are no pieces between the rook and the king,
        //    the target square is not a piece owned by the player
        if (!piece->has_moved)
        {
          for (auto direction : CASTLING)
          {
            char side = (direction.first == -4 ? 'Q' : 'K');
            if (piece->owner == 1)
            {
              side = std::tolower(side);
            }
            if (castle.find(side) > 3)
              continue;

            bool can_castle = true;
            int fd = direction.first; int rd = direction.second;            
            int inc = fd > 0;

            MyPiece* castle = board[i+fd][j+rd];
            if (castle == nullptr || castle->has_moved)
              can_castle = false;
            else
            {
              int m = i; 
              if (inc)
                while(m <= i + 2)
                {
                  if (in_check(m, j, (current_player + 1)%2) || (m != i && board[m][j] != nullptr))
                  {
                    can_castle = false;
                    break;
                  }
                  m++;
                }
              else
                while(m >= i - 3)
                {
                  if ((m >= i - 2 && in_check(m, j, (current_player + 1)%2)) || (m != i && board[m][j] != nullptr))
                  {
                    can_castle = false;
                    break;
                  }
                  m--;
                }
            }
            if (can_castle)
            {
              moves.push_back(MyMove(file, rank, file + (inc ? 2 : -2), rank+rd, 0, "Castle"));
            }
          }
        }

      }
      else if (type == &QUEEN)
      {
        // Queens can move any number of spaces in any direction if
        //    there are no pieces between the Queen and the target square,
        //    the target square is not a piece owned by the player
        for (auto direction: KING_MOVES)
        {
          for (int r = 1; r < 8; r++)
          {
            int fd = direction.first; int rd = direction.second;
            if (!iB(i+fd*r) || !iB(j+rd*r))
              break;
            auto *newloc = board[i+fd*r][j+rd*r];
            if (newloc == nullptr)
              moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r));
            else 
            {
              if (newloc->owner != current_player)
                moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r, newloc->type));
              break;
            }
          }
        }
      }
      else if (type == &KNIGHT)
      {
        // Knights can move in an L shape if
        //    the target square is not a piece owned by the player
        for (auto direction: KNIGHT_MOVES)
        {
          int fd = direction.first; int rd = direction.second;
          if (!iB(i+fd) || !iB(j+rd))
            continue;
          auto *newloc = board[i+fd][j+rd];
          if (newloc == nullptr || newloc->owner != current_player)
            moves.push_back(MyMove(file, rank, file+fd, rank+rd, newloc == nullptr ? nullptr : newloc->type));
        }
      }
      else if (type == &ROOK)
      {
        // Rooks can move any number of spaces across a rank or file if
        //    there are no pieces between the rook and the target square,
        //    the target square is not a piece owned by the player
        for (auto direction: ROOK_MOVES)
        {
          for (int r = 1; r < 8; r++)
          {
            int fd = direction.first; int rd = direction.second;
            if (!iB(i+fd*r) || !iB(j+rd*r))
              break;
            auto *newloc = board[i+fd*r][j+rd*r];
            if (newloc == nullptr)
              moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r));
            else 
            {
              if (newloc->owner != current_player)
                moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r, newloc->type));
              break;
            }
          }
        }
      }
      else if (type == &BISHOP)
      {
        // Bishops can move diagonally if
        //    there are no pieces between the bishop and the target
        //    the target square is not a piece owned by the player
        for (auto direction: BISHOP_MOVES)
        {
          for (int r = 1; r < 8; r++)
          {
            int fd = direction.first; int rd = direction.second;
            if (!iB(i+fd*r) || !iB(j+rd*r))
              break;
            auto *newloc = board[i+fd*r][j+rd*r];
            if (newloc == nullptr)
              moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r));
            else 
            {
              if (newloc->owner != current_player)
                moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r, newloc->type));
              break;
            }
          }
        }
      }
    }
  }


  
  // All moves  must be validated such that
  //    they do not put their own king into check
  for (int i = 0; i < moves.size(); i++)
  {
    if (in_check(moves[i]))
    {
      moves.erase(moves.begin() + i);
      i--;
    }
  }
  std::random_shuffle(moves.begin(), moves.end());

  // Now order captures to be first

  int k = 0;
  for (int i = 0; i < moves.size() - k; i++)
  {
    if (moves[i].capture != nullptr)
    {
      moves.push_back(moves[i]);
      moves.erase(moves.begin() + i);
      i--;
      k++;
    }
  }

  std::reverse(moves.begin(), moves.end());

  return moves;
}

bool State::actions_exist(const Game &game)
{
  // string in SAN
  std::vector<MyMove> moves;

  // Determine ability to castle & en passant
  int j = 0;
  for (int i = 0; i < 4; i++)
  {
    while(game->fen[j++] != ' ');
    i++;
  }

  std::string token = game->fen.substr(j);
  std::string castle = token.substr(0, token.find(" "));
  while(game->fen[j++] != ' ');
  token = game->fen.substr(j);
  std::string enPassant = token.substr(0, token.find(" "));

  // Iterate through the entire board
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (board[i][j] == nullptr || board[i][j]->owner != current_player)
      {
        continue;
      }
      MyPiece* piece = board[i][j];
      const char* type = piece->type;

      const int forward = (current_player == 0 ? 1 : -1);
      char file = 'a' + i;
      int rank = j + 1;

      
      if (type == &PAWN)
      {
        // Pawns can move 2 spaces forward if
        //    there are no pieces between the pawn and the target square or on the target square,
        //    the pawn is in its starting rank
        if (iB(j+forward*2) && !piece->has_moved && board[i][j+forward] == nullptr && board[i][j+forward*2] == nullptr)
        {
            moves.push_back(MyMove(file, rank, file, rank + forward*2));
        }

        // Pawns can move 1 space forward if
        //    there are no pieces on the target square
        if (iB(j+forward) && board[i][j+forward] == nullptr)
        {
          // Pawns can be promoted 
          //    if they advance to the final rank
          if (0 == j + forward || 7 == j + forward)
          {
            for (auto promotion : promotions)
            {
              moves.push_back(MyMove(file, rank, file, rank + forward, nullptr, promotion));
            }
          }
          else
            moves.push_back(MyMove(file, rank, file, rank + forward));
        }

        // Pawns can move 1 space forward diagonally if
        //    there is an enemy piece on the target square
        for (int dir : LR)
        {
          if (iB(i+dir) && iB(j+forward) && board[i+dir][j+forward] != nullptr)
          {
            MyPiece *captured = board[i+dir][j+forward];
            if (piece->owner != captured->owner)
            {
              // Pawns can be promoted 
              //    if they advance to the final rank
              if (j + forward == 0 || j + forward == 7)
              {
                for (auto promotion : promotions)
                {
                  moves.push_back(MyMove(file, rank, file+dir, rank+forward, captured->type, promotion));
                }
              }
              else
                moves.push_back(MyMove(file, rank, file+dir, rank+forward));
            }
          }
        }



        // Pawns can perform En Passant if
        //    the previous move was a pawn advancing two squares
        //    the pawn is now adjacent to this pawn
        if (enPassant[0] != '-')
        {
          if ((enPassant[1] - '0' - rank - forward) == 0 && abs((int)(enPassant[0] - 'a') - i) == 1)
            {
              int dir = enPassant[0] - file;
              moves.push_back(MyMove(file, rank, file+dir, rank+forward, 0, "En Passant"));
            }
        }



      }
      else if (type == &KING)
      {
        // Kings can move 1 space in any direction if
        //    the target square is not a piece owned by the player
        for (auto direction: KING_MOVES)
        {
          int fd = direction.first; int rd = direction.second;
          if (!iB(i+fd) || !iB(j+rd))
            continue;
          auto *newloc = board[i+fd][j+rd];
          if (newloc == nullptr || newloc->owner != current_player)
            moves.push_back(MyMove(file, rank, file+fd, rank+rd, newloc == nullptr ? nullptr : newloc->type));
        }

        // The king can castle with a friendly rook if
        //    both it and the castling rook have not moved,
        //    the king does not pass through a square that is attacked,
        //    the king is not in check,
        //    there are no pieces between the rook and the king,
        //    the target square is not a piece owned by the player
        if (!piece->has_moved)
        {
          for (auto direction : CASTLING)
          {
            char side = (direction.first == -4 ? 'Q' : 'K');
            if (piece->owner == 1)
            {
              side = std::tolower(side);
            }
            if (castle.find(side) > 3)
              continue;

            bool can_castle = true;
            int fd = direction.first; int rd = direction.second;            
            int inc = fd > 0;

            MyPiece* castle = board[i+fd][j+rd];
            if (castle == nullptr || castle->has_moved)
              can_castle = false;
            else
            {
              int m = i; 
              if (inc)
                while(m <= i + 2)
                {
                  if (in_check(m, j, (current_player + 1)%2) || (m != i && board[m][j] != nullptr))
                  {
                    can_castle = false;
                    break;
                  }
                  m++;
                }
              else
                while(m >= i - 3)
                {
                  if ((m >= i - 2 && in_check(m, j, (current_player + 1)%2)) || (m != i && board[m][j] != nullptr))
                  {
                    can_castle = false;
                    break;
                  }
                  m--;
                }
            }
            if (can_castle)
            {
              moves.push_back(MyMove(file, rank, file + (inc ? 2 : -2), rank+rd, 0, "Castle"));
            }
          }
        }

      }
      else if (type == &QUEEN)
      {
        // Queens can move any number of spaces in any direction if
        //    there are no pieces between the Queen and the target square,
        //    the target square is not a piece owned by the player
        for (auto direction: KING_MOVES)
        {
          for (int r = 1; r < 8; r++)
          {
            int fd = direction.first; int rd = direction.second;
            if (!iB(i+fd*r) || !iB(j+rd*r))
              break;
            auto *newloc = board[i+fd*r][j+rd*r];
            if (newloc == nullptr)
              moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r));
            else 
            {
              if (newloc->owner != current_player)
                moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r, newloc->type));
              break;
            }
          }
        }
      }
      else if (type == &KNIGHT)
      {
        // Knights can move in an L shape if
        //    the target square is not a piece owned by the player
        for (auto direction: KNIGHT_MOVES)
        {
          int fd = direction.first; int rd = direction.second;
          if (!iB(i+fd) || !iB(j+rd))
            continue;
          auto *newloc = board[i+fd][j+rd];
          if (newloc == nullptr || newloc->owner != current_player)
            moves.push_back(MyMove(file, rank, file+fd, rank+rd, newloc == nullptr ? nullptr : newloc->type));
        }
      }
      else if (type == &ROOK)
      {
        // Rooks can move any number of spaces across a rank or file if
        //    there are no pieces between the rook and the target square,
        //    the target square is not a piece owned by the player
        for (auto direction: ROOK_MOVES)
        {
          for (int r = 1; r < 8; r++)
          {
            int fd = direction.first; int rd = direction.second;
            if (!iB(i+fd*r) || !iB(j+rd*r))
              break;
            auto *newloc = board[i+fd*r][j+rd*r];
            if (newloc == nullptr)
              moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r));
            else 
            {
              if (newloc->owner != current_player)
                moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r, newloc->type));
              break;
            }
          }
        }
      }
      else if (type == &BISHOP)
      {
        // Bishops can move diagonally if
        //    there are no pieces between the bishop and the target
        //    the target square is not a piece owned by the player
        for (auto direction: BISHOP_MOVES)
        {
          for (int r = 1; r < 8; r++)
          {
            int fd = direction.first; int rd = direction.second;
            if (!iB(i+fd*r) || !iB(j+rd*r))
              break;
            auto *newloc = board[i+fd*r][j+rd*r];
            if (newloc == nullptr)
              moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r));
            else 
            {
              if (newloc->owner != current_player)
                moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r, newloc->type));
              break;
            }
          }
        }
      }

      // If one of them was valid, return it
      for (int i = 0; i < moves.size(); i++)
      {
        if (in_check(moves[i]))
        {
          moves.erase(moves.begin() + i);
          i--;
        }
        else
          return true;
      }
    }
  }

  return false;
}

State State::RESULT(const MyMove& action) const
{
  int file = action.file - 'a';
  int rank = action.rank - 1;
  int file2 = action.file2 - 'a';
  int rank2 = action.rank2 - 1;

  MyPiece *oldPiece = board[file][rank];
  
  State result(*this);
  
  delete result.board[file2][rank2];
  result.board[file2][rank2] = new MyPiece((action.promotion != nullptr ? action.promotion : oldPiece->type), true, oldPiece->owner);

  delete result.board[file][rank];
  result.board[file][rank] = nullptr;

  if (action.move_type == "En Passant")
  {
    delete result.board[file2][rank];
    result.board[file2][rank] = nullptr;
  }
  else if (action.move_type == "Castle")
  {
    MyPiece *castle = new MyPiece(&ROOK, true, oldPiece->owner);
    int new_rank = (rank2 == 1 ? 2 : 5);
    result.board[file2][new_rank] = castle;
  }

  result.current_player = (current_player + 1) % 2;
  return result;
}

std::vector<std::pair<pair, MyPiece*>> State::APPLY(const MyMove& action)
{
  // Convert the files and ranks to [0,7]
  int file = action.file - 'a';
  int rank = action.rank - 1;
  int file2 = action.file2 - 'a';
  int rank2 = action.rank2 - 1;

  // Apply the new board state
  MyPiece *oldPiece = board[file][rank];  
  std::vector<std::pair<pair, MyPiece*>> preserved;

  preserved.push_back(std::pair<pair, MyPiece*>(pair(file2, rank2), board[file2][rank2]));
  board[file2][rank2] = new MyPiece((action.promotion != nullptr ? action.promotion: oldPiece->type), true, oldPiece->owner);

  preserved.push_back(std::pair<pair, MyPiece*>(pair(file, rank), board[file][rank]));
  board[file][rank] = nullptr;

  if (action.move_type == "En Passant")
  {
    preserved.push_back(std::pair<pair, MyPiece*>(pair(file2, rank), board[file2][rank]));
    board[file2][rank] = nullptr;
  }
  else if (action.move_type == "Castle")
  {
    MyPiece *castle = new MyPiece(&ROOK, true, oldPiece->owner);
    int new_rank = (rank2 == 1 ? 2 : 5);
    int old_rank = (rank2 == 1 ? 0 : 7);
    preserved.push_back(std::pair<pair, MyPiece*>(pair(file2, old_rank), board[file2][old_rank]));
    preserved.push_back(std::pair<pair, MyPiece*>(pair(file2, new_rank), nullptr));
    board[file2][old_rank] = nullptr;
    board[file2][new_rank] = castle;
  }

  current_player = !current_player;
  
  return preserved;
  
}

void State::UNDO(const MyMove& action, std::vector<std::pair<pair, MyPiece*>> preserved)
{
  // Restore the board to its original state

  current_player = !current_player;
  
  for (auto pr: preserved)
  {
    pair loc = pr.first;
    MyPiece* orig = pr.second;
    delete board[loc.first][loc.second];
    board[loc.first][loc.second] = orig;
  }
}

bool State::in_check() const
{
  // Find the king
  int ki, kj;
  bool found=false;

  for (int i = 0; i < 8; i++)
  {
    if (found)
      break;
    for (int j = 0; j < 8; j++)
    {
      MyPiece* p = board[i][j];
      if (p != nullptr && p->type == &KING && p->owner == current_player)
      {
        ki = i;
        kj = j;
        found = true;
        break;
      }
    }
  }

  // Check if the king is in check
  return in_check(ki, kj, (current_player + 1) % 2);
}

bool State::in_check(const MyMove& action)
{
  auto preserved = APPLY(action);

  current_player = !current_player;
  bool check = in_check();
  current_player = !current_player;
  
  UNDO(action, preserved);

  return check;
}

bool State::stalemate() const
{
  // Insufficient Material
  int mats = 0;
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      auto *piece = board[i][j];
      if (piece == nullptr)
        continue;
      if (piece->type == &PAWN || piece->type == &ROOK)
      {
        mats = 2;
        break;
      }
      else if (piece->type == &BISHOP || piece->type == &KNIGHT)
      {
        mats++;
        if (mats > 1)
          break;
      }
    }
  }
  if (mats < 2)
    return true;

  // 50-move rule

  // 4-fold repetition

  return false;
}

int State::goal_reached(const Game& game)
{
  if (!actions_exist(game))
  {
    if (in_check())
    {
      return -1000000 * (game->current_turn % 2 == current_player ? 1 : -1); // Checkmate
    }
    return -1000; // Draw
  }
  if (stalemate())
    return -1000;
  return 0; // Not a goal
}

int State::material_advantage(bool maxPlayer)
{
  int advantage = 0;
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      auto *piece = board[i][j];
      if (piece != nullptr)
        advantage += (piece->owner == maxPlayer ? 1 : -1) * value(piece->type);
    }
  }
  return advantage;
}

float State::evaluate(const Game& game)
{
  int goal = goal_reached(game);
  if (goal != 0)
    return goal;
  return material_advantage(game->current_turn % 2);
}

int State::value(const char* pieceType) const
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

void State::print() const
{
  for (int i = 7; i >= 0; i--)
  {
    std::cout << i + 1 << " |";
    for (int j = 0; j < 8; j++)
    {
      std::cout << (((i + j) % 2 == 1) ? WHITE_BG : BLACK_BG);
      if (board[j][i] == nullptr)
        std::cout << "  ";
      else
        std::cout << unicode((char)(board[j][i]->owner == 0 ? (*(board[j][i]->type)) : std::tolower(*(board[j][i]->type)))) << " ";
    }
    std::cout << WHITE_FG << "|" << std::endl;
  }
  std::cout << "  +----------------+\n" << "    a b c d e f g h" << std::endl << std::endl;
}

const MyPiece* State::getPiece(const char& file, const int& rank) const
{
  return board[static_cast<int>(file - 'a')][rank - 1];
}



} // chess

} // cpp_client
