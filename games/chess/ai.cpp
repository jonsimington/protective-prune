// AI
// This is where you build your AI

#include "ai.hpp"

// You can add #includes here for your AI.

#include <cmath>

namespace cpp_client
{

namespace chess
{


///////////////////////////MY UTILITY STUFF
using pair = std::tuple<int, int>;

bool iB(int i)
{
  return 0 <= i && i < 8;
}

char shorten(std::string name)
{
  if (name == "Knight")
    return 'N';
  return name[0];
}

std::string lengthen(char name)
{
  switch(name) {
    case 'N':
      return "Knight";
    case 'K':
      return "King";
    case 'Q':
      return "Queen";
    case 'R':
      return "Rook";
    case 'P':
      return "Pawn";
    case 'B':
      return "Bishop";
  }
  //std::cout << "Unknown piece type " << name << std::endl;
  return "";

}

const std::string promotions[4] = {"Rook", "Knight", "Bishop", "Queen"};

const int LR[2] = {-1, 1};
///////////////////////////

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
    // Here is where you'll want to code your AI.

    // We've provided sample code that:
    //    1) prints the board to the console
    //    2) prints the opponent's last move to the console
    //    3) prints how much time remaining this AI has to calculate moves
    //    4) makes a random (and probably invalid) move.

    // 1) print the board to the console
    print_current_board();

    // 2) print the opponent's last move to the console
    if(game->moves.size() > 0)
    {
        std::cout << "Opponent's Last Move: '" << game->moves[game->moves.size() - 1]->san << "'" << std::endl;
    }


    // 3) print how much time remaining this AI has to calculate moves
    std::cout << "Time Remaining: " << player->time_remaining << " ns" << std::endl;

    chess::Piece p = player->pieces[rand() % player->pieces.size()];
    //while (p->type != "Pawn")
    //{
    //    p = player->pieces[rand() % player->pieces.size()];
    //}
    //p->move(p->file, p->rank + player->rank_direction);
    State state(game);



    std::vector<MyMove> moves = state.generate_moves(game);

    if (moves.size() == 0)
    {
      std::cout << "I can't find any valid moves :(" << std::endl;
      return true;
    }
    int i = rand() % moves.size();
    auto move = moves[i];
    state.RESULT(move).print();

    for (auto piece : player->pieces)
    {
      if (piece->rank == move.rank && piece->file[0] == move.file)
      {
        std::cout << move.file << move.rank << " " << move.file2 << move.rank2 << " " << move.promotion << std::endl;
        piece->move(std::string(1, move.file2), move.rank2, lengthen(move.promotion));
        break;
      }
    }
    //for (auto move : moves)
    //{
    //  std::cout << move.file << move.rank << " " << move.file2 << move.rank2 << " " << move.promotion << std::endl;
    //}

    return true; // to signify we are done with our turn.
}


/// <summary>
///  Prints the current board using pretty ASCII art
/// </summary>
/// <remarks>
/// Note: you can delete this function if you wish
/// </remarks>
void AI::print_current_board()
{
    for(int rank = 9; rank >= -1; rank--)
    {
        std::string str = "";
        if(rank == 9 || rank == 0) // then the top or bottom of the board
        {
            str = "   +------------------------+";
        }
        else if(rank == -1) // then show the ranks
        {
            str = "     a  b  c  d  e  f  g  h";
        }
        else // board
        {
            str += " ";
            str += std::to_string(rank);
            str += " |";
            // fill in all the files with pieces at the current rank
            for(int file_offset = 0; file_offset < 8; file_offset++)
            {
                std::string file(1, 'a' + file_offset); // start at a, with with file offset increasing the char;
                chess::Piece current_piece = nullptr;
                for(const auto& piece : game->pieces)
                {
                    if(piece->file == file && piece->rank == rank) // then we found the piece at (file, rank)
                    {
                        current_piece = piece;
                        break;
                    }
                }

                char code = '.'; // default "no piece";
                if(current_piece != nullptr)
                {
                    code = current_piece->type[0];

                    if(current_piece->type == "Knight") // 'K' is for "King", we use 'N' for "Knights"
                    {
                        code = 'N';
                    }

                    if(current_piece->owner->id == "1") // the second player (black) is lower case. Otherwise it's upppercase already
                    {
                        code = tolower(code);
                    }
                }

                str += " ";
                str += code;
                str += " ";
            }

            str += "|";
        }

        std::cout << str << std::endl;
    }
}

const std::vector<pair> CARDINAL = {pair(0, 1), pair(0, -1), pair(1, 0), pair(-1, 0)};
const std::vector<pair> ORDINAL = {pair(1, 1), pair(1, -1), pair(-1, 1), pair(-1, -1)};
const std::vector<pair> PAWN_ATTACKS[2] = {{pair(1, 1), pair(-1, 1)}, {pair(1, -1), pair(-1, -1)}};
const std::vector<pair> KNIGHT_MOVES = {pair(2, 1), pair(2, -1), pair(-2, 1), pair(-2, -1), pair(1, 2), pair(1, -2), pair(-1, 2), pair(-1, -2)};
const std::vector<pair> KING_MOVES = {pair(-1, -1), pair(-1, 0), pair(-1, 1), pair(0, -1), pair(0, 1), pair(1, -1), pair(1, 0), pair(1, 1)};
const std::vector<pair> ROOK_MOVES = {pair(-1, 0), pair(0, -1), pair(0, 1), pair(1, 0)};
const std::vector<pair> BISHOP_MOVES = {pair(-1, -1), pair(-1, 1), pair(1, -1), pair(1, 1)};
const std::vector<pair> CASTLING = {{pair(-4, 0), pair(3, 0)}};

// You can add additional methods here for your AI to call

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
          int file, rank;
          std::tie(file, rank) = tile;
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

  if (piece == nullptr)
    return std::vector<pair>();

  if (piece->type == "King")
  {
    moves = &KING_MOVES;
  }if (piece->type == "Queen")
  {
    moves = &KING_MOVES;
    range = 8;
  }
  if (piece->type == "Bishop")
  {
    moves = &BISHOP_MOVES;
    range = 8;
  }if (piece->type == "Knight")
  {
    moves = &KNIGHT_MOVES;
    range = 1;
  }if (piece->type == "Rook")
  {
    moves = &ROOK_MOVES;
    range = 8;
  }if (piece->type == "Pawn")
  {
    moves = &PAWN_ATTACKS[piece->owner];
  }


  for (auto direction: *moves)
  {
    int fd, rd;
    std::tie(fd, rd) = direction;
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


std::vector<MyMove> State::generate_moves(const Game &game) const
{
  // string in SAN
  std::vector<MyMove> moves;

  int** attackboard = attacked((current_player + 1) % 2);


  for (int i = 0; i < 8; i++)
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

      
      if (type == "Pawn")
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
            if (abs(last_move->to_rank - last_move->from_rank) == 2 && last_move->piece->type == "Pawn")
            {
              int dir = last_move->to_file[0] - file;
              moves.push_back(MyMove(file, rank, file+dir, rank+forward, '\0', "En Passant"));
            }
          }
        }



      }
      else if (type == "King")
      {
        // Kings can move 1 space in any direction if
        //     the target tile does not contain a friendly piece
        for (auto direction: KING_MOVES)
        {
          int fd, rd;
          std::tie(fd, rd) = direction;
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
            int fd, rd;
            std::tie(fd, rd) = direction;
            int inc = fd > 0;

            MyPiece* castle = board[i+fd][j+rd];
            if (castle == nullptr || castle->has_moved)
              can_castle = false;
            else
            {
              for (int m = i; (inc ? m <= i + fd : m >= i + fd); (inc ? m++ : m--))
              {
                if ((inc ? m <= i + 2 : m >= i - 3) && attackboard[m][j] > 0 || (m != i && board[m][j] != nullptr))
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
      else if (type == "Queen")
      {
        // Queens can move any number of spaces in any direction if
        //    there are no pieces between the Queen and the target space
        for (auto direction: KING_MOVES)
        {
          for (int r = 1; r < 8; r++)
          {
            int fd, rd;
            std::tie(fd, rd) = direction;
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
      else if (type == "Knight")
      {
        // Knights can move in an L shape
        for (auto direction: KNIGHT_MOVES)
        {
          int fd, rd;
          std::tie(fd, rd) = direction;
          if (!iB(i+fd) || !iB(j+rd))
            continue;
          auto *newloc = board[i+fd][j+rd];
          if (newloc == nullptr || newloc->owner != current_player)
            moves.push_back(MyMove(file, rank, file+fd, rank+rd));
        }
      }
      else if (type == "Rook")
      {
        // Rooks can move any number of spaces across a rank or file if
        //    there are no pieces between it and the target square
        for (auto direction: ROOK_MOVES)
        {
          for (int r = 1; r < 8; r++)
          {
            int fd, rd;
            std::tie(fd, rd) = direction;
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
      else if (type == "Bishop")
      {
        // Bishops can move diagonally if
        //    there are no pieces between it and the target
        for (auto direction: BISHOP_MOVES)
        {
          for (int r = 1; r < 8; r++)
          {
            int fd, rd;
            std::tie(fd, rd) = direction;
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
  
  // All moves  must be validated such that
  //    they do not put their own king into check
  for (int i = 0; i < moves.size(); i++)
  {
    MyMove move = moves[i];
    State successor = RESULT(move);
    int** attackboard = successor.attacked((current_player + 1) % 2);
    int ki, kj;
    bool found=false;

    for (int i = 0; i < 8; i++)
    {
      if (found)
        break;
      for (int j = 0; j < 8; j++)
      {
        MyPiece* p = successor.board[i][j];
        if (p != nullptr && p->type == "King" && p->owner == current_player)
        {
          ki = i;
          kj = j;
          found = true;
          break;
        }
      }
    }

    if (attackboard[ki][kj] > 0)
    {
      moves.erase(moves.begin() + i);
      i--;
    }
  }

  return moves;
}

State State::RESULT(MyMove action) const
{

  int file = action.file - 'a';
  int rank = action.rank - 1;
  int file2 = action.file2 - 'a';
  int rank2 = action.rank2 - 1;

  MyPiece *oldPiece = board[file][rank];
  MyPiece *newPiece = new MyPiece((action.promotion != '\0' ? lengthen(action.promotion) : oldPiece->type), true, oldPiece->owner);
  
  State result(*this);
  
  result.board[file2][rank2] = newPiece;
  result.board[file][rank] = nullptr;

  if (action.move_type == "En Passant")
  {
    result.board[file2][rank] = nullptr;
  }
  else if (action.move_type == "Castle")
  {
    MyPiece *castle = new MyPiece("Rook", true, oldPiece->owner);
    int new_rank = (rank2 == 1 ? 2 : 5);
    result.board[file2][new_rank] = castle;
  }

  return result;
}

void State::print() const
{
  for (int i = 7; i >= 0; i--)
  {
    std::cout << i + 1 << " | ";
    for (int j = 0; j < 8; j++)
    {
      if (board[j][i] == nullptr)
        std::cout << ". ";
      else
        std::cout << (char)(board[j][i]->owner == 0 ? (shorten(board[j][i]->type)) : std::tolower(shorten(board[j][i]->type)))  << " ";
    }
    std::cout << "|" << std::endl;
  }
  std::cout << "  + - - - - - - - - +\n" << "    a b c d e f g h" << std::endl << std::endl;
}


} // chess

} // cpp_client
