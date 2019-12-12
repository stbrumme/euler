// ////////////////////////////////////////////////////////
// # Title
// Sliders
//
// # URL
// https://projecteuler.net/problem=244
// http://euler.stephan-brumme.com/244/
//
// # Problem
// You probably know the game Fifteen Puzzle. Here, instead of numbered tiles, we have seven red tiles and eight blue tiles.
//
// A move is denoted by the uppercase initial of the direction (Left, Right, Up, Down) in which the tile is slid,
// e.g. starting from configuration __(S)__, by the sequence __LULUR__ we reach the configuration __(E)__:
//
// || 10                       || 10                           ||
// || __(S)__                  || __(E)__                      ||
// || ![start](p244_start.gif) || ![example](p244_example.gif) ||
//
// For each path, its checksum is calculated by (pseudocode):
// `checksum = 0`
// `checksum = (checksum * 243 + m_1) mod 100000007`
// `checksum = (checksum * 243 + m_2) mod 100000007`
//    ...
// `checksum = (checksum * 243 + m_n) mod 100000007`
// where `m_k` is the ASCII value of the k-th letter in the move sequence and the ASCII values for the moves are:
//
// || 2 ||  2 ||
// || L ++ 76 ||
// || R ++ 82 ||
// || U ++ 85 ||
// || D ++ 68 ||
//
// For the sequence __LULUR__ given above, the checksum would be 19761398.
//
// Now, starting from configuration __(S)__, find all shortest ways to reach configuration __(T)__.
//
// || 10                       || 10                          ||
// || __(S)__                  || __(T)__                     ||
// || ![start](p244_start.gif) || ![example](p244_target.gif) ||
//
// What is the sum of all checksums for the paths having the minimal length?
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// I run a breadth-first search without any "tricks" or "specific optimizations" (see https://en.wikipedia.org/wiki/Breadth-first_search ):
// - encode a board as an ''std::string'', concatenate all rows to get a single line (initial board is ''".rbbrrbbrrbbrrbb"'')
// - store the checksum of all moves in a board, too
// - avoid visiting a board already seen before (my ''history'' container keeps track of that)
//
// I felt it was easier to "move" the empty square instead of moving an occupied square (onto the empty square).
// Therefore my variables ''fromX'', ''toX'', etc. in ''Board::move'' refer to the empty square - but then the actual moves look strange (''Left'' ==> increment ''toX'').
//
// # Note
// It's a classic computer science problem and therefore I found it to be pretty easy (I studied software engineering).
// Project Euler awarded it a high difficulty rating because mathematicians are probably not that familiar with typical IT algorithms.
// On the contrary, I've seen enough "easy" problems that were actually quite hard for me.
//
// # Alternative
// There are many opportunities to speed up the program or save some memory.
// For example, there is only one shortest path so I could abort ''search()'' as soon as I encounter it.
// Nevertheless, my live test "forces" me to look at all paths of the last iteration in case the user enters a board with multiple shortest paths.

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>

// encode the board by ASCII characters, too
enum Piece
{
  Red   = 'r',
  Blue  = 'b',
  Empty = '.'
};

// moves as defined by the problem statement
enum Move
{
  Up    = 85, // 'U'
  Left  = 76, // 'L'
  Down  = 68, // 'D'
  Right = 82  // 'R'
};

// represent a board (with moves needed to get there from the initial board)
struct Board
{
  // the whole board as a one-line string, initial value is ".rbbrrbbrrbbrrbb"; (see enum Piece)
  std::string pieces;
  // checksum of executed moves
  unsigned int checksum;

  // 4x4 board
  static const unsigned int Size = 4;

  // create a new board
  Board(const std::string& pieces_ = std::string(), unsigned int checksum_ = 0)
  : pieces(pieces_), checksum(checksum_)
  {}

  // return true if pieces contains 16 characters (very basic check, will accept many invalid boards !)
  bool isValid() const
  {
    return pieces.size() == Size * Size;
  }
  // return true if equal
  bool operator==(const Board& other) const
  {
    return pieces == other.pieces;
  }

  // "move" the empty square, return an empty string if impossible
  Board move(Move move) const
  {
    if (pieces.empty())
      return Board();

    // find the empty square
    auto index = 0;
    while (pieces[index] != Empty)
      index++;

    // from 1D to 2D
    auto fromX = index % Size;
    auto fromY = index / Size;

    // new location of the empty square
    auto toX = fromX;
    auto toY = fromY;
    // note: the moves are based on the movement of the red/blue square whereas
    //       I actually move the empty square, so everything's "reversed"
    switch (move)
    {
    case Up:
      if (fromY == Size - 1)
        return Board();
      toY++;
      break;
    case Down:
      if (fromY == 0)
        return Board();
      toY--;
      break;
    case Left:
      if (fromX == Size - 1)
        return Board();
      toX++;
      break;
    case Right:
      if (fromX == 0)
        return Board();
      toX--;
      break;
    }

    // from 2D to 1D ...
    auto next = toY * Size + toX;
    auto newPieces = pieces;
    std::swap(newPieces[index], newPieces[next]);

    // update checksum
    auto newChecksum = (checksum * 243ULL + move) % 100000007;
    return Board(std::move(newPieces), newChecksum);
  }
};

// breadth search for a certain board, add all checksums
unsigned int search(const Board& finalBoard)
{
  // initial configuration
  std::vector<Board> todo = { Board(".rbbrrbbrrbbrrbb") };

  // keep track of all boards already visited
  std::set<std::string> history;

  // sum of all checksums
  unsigned int result = 0;

  // look for the shortest path (there may be multiple !)
  bool lastIteration = false;
  while (!lastIteration)
  {
    std::vector<Board> next;
    next.reserve(10000); // actually about 6500 is sufficient

    for (auto current : todo)
    {
      // final position found ?
      if (current == finalBoard)
      {
        lastIteration = true;
        result += current.checksum;
      }

      // try all four movements:
      // verify that the move is legal and avoid already visited positions
      // L => left
      auto left = current.move(Left);
      if (left.isValid() && history.count(left.pieces) == 0)
      {
        next.push_back(left);
        history.insert(left.pieces);
      }
      // R => right
      auto right = current.move(Right);
      if (right.isValid() && history.count(right.pieces) == 0)
      {
        next.push_back(right);
        history.insert(right.pieces);
      }
      // U => up
      auto up = current.move(Up);
      if (up.isValid() && history.count(up.pieces) == 0)
      {
        next.push_back(up);
        history.insert(up.pieces);
      }
      // D => down
      auto down = current.move(Down);
      if (down.isValid() && history.count(down.pieces) == 0)
      {
        next.push_back(down);
        history.insert(down.pieces);
      }
    }

    // prepare next iteration
    todo = std::move(next);
  }

  return result;
}

int main()
{
  std::string finalPosition = ".brbbrbrrbrbbrbr"; // example LULUR => "rrbbrbbbr.rbrrbb"
  std::cin >> finalPosition;

  // simple validation (for live test only)
  auto numRed     = 0;
  auto numBlue    = 0;
  auto numEmpty   = 0;
  auto numInvalid = 0;
  for (auto c : finalPosition)
  {
    switch (c)
    {
      case 'r': numRed++; break;
      case 'b': numBlue++; break;
      case '.': numEmpty++; break;
      default:  numInvalid++; break;
    }
  }

  // reject invalid input
  if (numRed != 7 || numBlue != 8 || numEmpty != 1 || numInvalid != 0)
    return 1;

  // let's go !
  std::cout << search(Board(finalPosition)) << std::endl;
  return 0;
}
