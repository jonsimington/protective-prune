#include "custom_board.hpp"

#include <cmath>

namespace cpp_client
{

namespace chess
{

bool iB(int i)
{
  return 0 <= i && i < 8;
}

char shorten(std::string name)
{
  if (name == KNIGHT)
    return 'N';
  return name[0];
}

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

std::string lengthen(char name)
{
  switch(name) {
    case 'N':
      return KNIGHT;
    case 'K':
      return KING;
    case 'Q':
      return QUEEN;
    case 'R':
      return ROOK;
    case 'P':
      return PAWN;
    case 'B':
      return BISHOP;
  }
  //std::cout << "Unknown piece type " << name << std::endl;
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
      if (piece != nullptr && piece->owner == attacker && piece->type == KNIGHT)
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
      if (piece != nullptr && piece->owner == attacker && piece->type == PAWN)
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
          if ((range == 1 && piece->type == KING) || (piece->type == QUEEN) || (piece->type ==  BISHOP && (abs(fd + rd) == 2 || fd + rd == 0)) || (piece->type == ROOK && abs(fd + rd) == 1))
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

std::vector<pair> State::attacked(int i, int j, int filedir, int rankdir, int range=1) const
{
  std::vector<pair> _attacked;
  // See which pieces are attacked by the piece on i, j in direction filedir, rankdir at max range of range
  for (int k = 1; k <= range; k++)
  {
    int file = i + filedir * k;
    int rank = j + rankdir * k;
    if (!iB(file) || !iB(rank))
      break;
    MyPiece *piece = board[file][rank];
    if (piece != nullptr)
    {
      if (piece->owner == board[i][j]->owner)
        break;
      _attacked.push_back(pair(file, rank));
      break;
    }
  }
  return _attacked;
}

int** State::attacked(int attacker) const
{
  // How many units owned by attacker could attack each tile
  int** b2 = new int*[8];
  for (int i = 0; i < 8; i++)
  {
    b2[i] = new int[8];
    for (int j = 0; j < 8; j++)
    {
      b2[i][j] = 0;
    }
  }
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (board[i][j] != nullptr && board[i][j]->owner == attacker)
      {
        for (pair tile: attacking(i, j))
        {
          int file = tile.first; int rank = tile.second;
          b2[file][rank]++;
        }
      }
    }
  }
  return b2;
}

std::vector<pair> State::attacking(int i, int j) const
{
  // Determine which squares are being attacked by the piece on the given file i, rank j
  std::vector<pair> _attacking;
  std::vector<pair> const *moves;
  int range = 1;
  auto *piece = board[i][j];
  std::string type = piece->type;

  if (piece == nullptr)
    return std::vector<pair>();

  if (type == KING)
  {
    moves = &KING_MOVES;
  }else if (type == QUEEN)
  {
    moves = &KING_MOVES;
    range = 8;
  }
  else if (type == BISHOP)
  {
    moves = &BISHOP_MOVES;
    range = 8;
  }else if (type == KNIGHT)
  {
    moves = &KNIGHT_MOVES;
    range = 1;
  }else if (type == ROOK)
  {
    moves = &ROOK_MOVES;
    range = 8;
  }else if (type == PAWN)
  {
    moves = &PAWN_ATTACKS[piece->owner];
  }


  for (auto direction: *moves)
  {
    int fd = direction.first; int rd = direction.second;
    for (auto _attacked: attacked(i, j, fd, rd, range))
      _attacking.push_back(_attacked);
  }

  return _attacking;
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
    board[i][j] = new MyPiece(piece->type, piece->has_moved, piece->owner == game->players[1]); 
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


std::vector<MyMove> State::generate_moves(const Game &game)
{
  // string in SAN
  std::vector<MyMove> moves;

  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (board[i][j] == nullptr || board[i][j]->owner != current_player)
      {
        continue;
      }
      MyPiece* piece = board[i][j];
      std::string type = piece->type;

      const int forward = (current_player == 0 ? 1 : -1);
      char file = 'a' + i;
      int rank = j + 1;

      
      if (type == PAWN)
      {
        //((0, 1), (0, 2), (0, 3), (1, 1), (-1, 1))
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
              moves.push_back(MyMove(file, rank, file, rank + forward, shorten(promotion)));
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
            MyPiece *jumped = board[i+dir][j+forward];
            if (piece->owner != jumped->owner)
            {
              // Pawns can be promoted 
              //    if they advance to the final rank
              if (j + forward == 0 || j + forward == 7)
              {
                for (std::string promotion : promotions)
                {
                  moves.push_back(MyMove(file, rank, file+dir, rank+forward, shorten(promotion)));
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
        if (game->moves.size() > 0)
        {
          Move last_move = game->moves.back();
          if (last_move->to_rank == rank && abs((int)(last_move->to_file[0] - 'a') - i) == 1)
          {
            if (abs(last_move->to_rank - last_move->from_rank) == 2 && last_move->piece->type == PAWN)
            {
              int dir = last_move->to_file[0] - file;
              moves.push_back(MyMove(file, rank, file+dir, rank+forward, '\0', "En Passant"));
            }
          }
        }



      }
      else if (type == KING)
      {
        // Kings can move 1 space in any direction if
        //     the target tile does not contain a friendly piece
        for (auto direction: KING_MOVES)
        {
          int fd = direction.first; int rd = direction.second;
          if (!iB(i+fd) || !iB(j+rd))
            continue;
          auto *newloc = board[i+fd][j+rd];
          if (newloc == nullptr || newloc->owner != current_player)
            moves.push_back(MyMove(file, rank, file+fd, rank+rd));
        }

        // The king can castle with a friendly rook if
        //    both it and the castling rook have not moved,
        //    the king does not pass through a square that is attacked,
        //    the king is not in check,
        //    there are no pieces between the rook and the king
        if (!piece->has_moved)
        {
          for (auto direction : CASTLING)
          {
            bool can_castle = true;
            int fd = direction.first; int rd = direction.second;            
            int inc = fd > 0;

            MyPiece* castle = board[i+fd][j+rd];
            if (castle == nullptr || castle->has_moved)
              can_castle = false;
            else
            {
              for (int m = i; (inc ? m <= i + fd : m >= i + fd); (inc ? m++ : m--))
              {
                if ((inc ? m <= i + 2 : m >= i - 3) && !in_check(m, j, (current_player + 1)%2) || (m != i && board[m][j] != nullptr))
                {
                  can_castle = false;
                  break;
                }
              }
            }
            if (can_castle)
            {
              moves.push_back(MyMove(file, rank, file + (inc ? 2 : -3), rank+rd, '\0', "Castle"));
            }
          }
        }

      }
      else if (type == QUEEN)
      {
        // Queens can move any number of spaces in any direction if
        //    there are no pieces between the Queen and the target space
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
                moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r));
              break;
            }
          }
        }
      }
      else if (type == KNIGHT)
      {
        // Knights can move in an L shape
        for (auto direction: KNIGHT_MOVES)
        {
          int fd = direction.first; int rd = direction.second;
          if (!iB(i+fd) || !iB(j+rd))
            continue;
          auto *newloc = board[i+fd][j+rd];
          if (newloc == nullptr || newloc->owner != current_player)
            moves.push_back(MyMove(file, rank, file+fd, rank+rd));
        }
      }
      else if (type == ROOK)
      {
        // Rooks can move any number of spaces across a rank or file if
        //    there are no pieces between it and the target square
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
                moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r));
              break;
            }
          }
        }
      }
      else if (type == BISHOP)
      {
        // Bishops can move diagonally if
        //    there are no pieces between it and the target
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
                moves.push_back(MyMove(file, rank, file+fd*r, rank+rd*r));
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

  return moves;
}

State* State::RESULT(MyMove action) const
{
  int file = action.file - 'a';
  int rank = action.rank - 1;
  int file2 = action.file2 - 'a';
  int rank2 = action.rank2 - 1;

  MyPiece *oldPiece = board[file][rank];
  
  State* result;
  result = new State(*this);
  
  delete result->board[file2][rank2];
  result->board[file2][rank2] = new MyPiece((action.promotion != '\0' ? lengthen(action.promotion) : oldPiece->type), true, oldPiece->owner);

  delete result->board[file][rank];
  result->board[file][rank] = nullptr;

  if (action.move_type == "En Passant")
  {
    delete result->board[file2][rank];
    result->board[file2][rank] = nullptr;
  }
  else if (action.move_type == "Castle")
  {
    MyPiece *castle = new MyPiece(ROOK, true, oldPiece->owner);
    int new_rank = (rank2 == 1 ? 2 : 5);
    result->board[file2][new_rank] = castle;
  }
  return result;
}

bool State::in_check(const MyMove& action)
{
  int file = action.file - 'a';
  int rank = action.rank - 1;
  int file2 = action.file2 - 'a';
  int rank2 = action.rank2 - 1;

  MyPiece *oldPiece = board[file][rank];  
  std::vector<std::pair<pair, MyPiece*>> preserved;

  preserved.push_back(std::pair<pair, MyPiece*>(pair(file2, rank2), board[file2][rank2]));
  board[file2][rank2] = new MyPiece((action.promotion != '\0' ? lengthen(action.promotion) : oldPiece->type), true, oldPiece->owner);

  preserved.push_back(std::pair<pair, MyPiece*>(pair(file, rank), board[file][rank]));
  board[file][rank] = nullptr;

  if (action.move_type == "En Passant")
  {
    preserved.push_back(std::pair<pair, MyPiece*>(pair(file2, rank), board[file2][rank]));
    board[file2][rank] = nullptr;
  }
  else if (action.move_type == "Castle")
  {
    MyPiece *castle = new MyPiece(ROOK, true, oldPiece->owner);
    int new_rank = (rank2 == 1 ? 2 : 5);
    preserved.push_back(std::pair<pair, MyPiece*>(pair(file2, new_rank), board[file2][new_rank]));
    board[file2][new_rank] = castle;
  }


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
      if (p != nullptr && p->type == KING && p->owner == current_player)
      {
        ki = i;
        kj = j;
        found = true;
        break;
      }
    }
  }

  // Check if the king is in check
  bool check = in_check(ki, kj, (current_player + 1) % 2);
  
  // Restore the board to its original state
  for (auto pr: preserved)
  {
    pair loc = pr.first;
    MyPiece* orig = pr.second;
    delete board[loc.first][loc.second];
    board[loc.first][loc.second] = orig;
  }

  return check;
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
        std::cout << unicode((char)(board[j][i]->owner == 0 ? (shorten(board[j][i]->type)) : std::tolower(shorten(board[j][i]->type)))) << " ";
    }
    std::cout << WHITE_FG << "|" << std::endl;
  }
  std::cout << "  +----------------+\n" << "    a b c d e f g h" << std::endl << std::endl;
}


} // chess

} // cpp_client
