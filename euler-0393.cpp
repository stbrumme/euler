// ////////////////////////////////////////////////////////
// # Title
// Migrating ants
//
// # URL
// https://projecteuler.net/problem=393
// http://euler.stephan-brumme.com/393/
//
// # Problem
// An `n * n` grid of squares contains `n^2` ants, one ant per square.
// All ants decide to move simultaneously to an adjacent square (usually 4 possibilities, except for ants on the edge of the grid or at the corners).
// We define `f(n)` to be the number of ways this can happen without any ants ending on the same square and without any two ants crossing the same edge between two squares.
//
// You are given that `f(4) = 88`.
// Find `f(10)`.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// I solved this problem with Dynamic Programming:
// - process the grid row-by-row, starting with the top row
// - each row is assigned a state (consisting of three parts):
// - row number
// - a bit mask of all ants moving down from the row above
// - a bit mask of all ants travelling up from the row below
//
// Each ant moves only one step, therefore for each square there is exactly one ant "moving out" and one ant "moving in".
// There are `4^10` potential moves for a row of 10 ants.
// Some of them are illegal: no ant can move left on the left-most square.
// Vice versa, no ant may move right on the right-most square.
// And on top of that, no ant may move right if its right neighbor moves left (ants' paths must not cross each other).
//
// I spent quite some time finding out all possible illegal moves (you'll notice the widespread use of ''invalid'' in my code).
//
// A major performance speedup was achieved by skipping as many illegal sequences as possible (see ''skipIds'').
//
// # Alternative
// Some people found ways to solve this problem in a few milliseconds by looking for the cycles.

#include <iostream>
#include <vector>
#include <map>

// http://oeis.org/A216678

// at most 10x10 grid
const unsigned int MaxSize = 10;
// actual size
unsigned int size = 10;

typedef unsigned short Mask;
const Mask NoAnt = 0;

// memoization container
struct State
{
  unsigned short row;
  Mask down, up;
  // create new state
  State(unsigned int row_, Mask down_, Mask up_)
  : row(row_), down(down_), up(up_)
  {}

  // for std::map
  bool operator<(const State& other) const
  {
    if (row  != other.row)  return row  < other.row;
    if (down != other.down) return down < other.down;
    return up < other.up;
  }
};
std::map<State, unsigned long long> cache;

// encode each ant's move in 2 bits
enum Move
{
  MoveUp    = 0,
  MoveRight = 1,
  MoveDown  = 2,
  MoveLeft  = 3
};
const unsigned int BitsPerMove   = 2;
const unsigned int NumberOfMoves = 4;
const unsigned int MoveMask      = 4 - 1;

// extract the 2 bit encoding a move in a bitmask
Move getMove(unsigned int bits, unsigned int pos)
{
  auto mypos = size - (pos + 1);
  return Move((bits >> (BitsPerMove * mypos)) & MoveMask);
}

// do the heavy lifting ...
unsigned long long search(unsigned int row = 0, Mask down = NoAnt, Mask up = NoAnt)
{
  // "below" last row: no ant can exit the grid, therefore "up" and "up" must be empty
  if (row == size)
  {
    const unsigned long long Symmetries = 2;
    return (down == 0 && up == 0) ? Symmetries : 0;
  }

  // memoize
  State state(row, down, up);
  auto lookup = cache.find(state);
  if (lookup != cache.end())
    return lookup->second;

  // count all valid movement patterns
  unsigned long long result = 0;

  // analyze each possible combination for the current row
  const unsigned int combinations = 1 << (BitsPerMove * size); // 4^size
  for (unsigned int i = 0; i < combinations; i++)
  {
    // can't go left  on the left-most  square
    Move firstMove = getMove(i, 0);
    if (firstMove == MoveLeft)
    {
      // skip all values of "i" until a different move appears
      auto skipSquares = size - 1;
      auto skipIds = 1 << (BitsPerMove * skipSquares);
      i += skipIds - 1; // minus 1 because for-loop always increments by one
      continue;
    }
    // can't go right on the right-most square
    Move lastMove = getMove(i, size - 1);
    if (lastMove == MoveRight)
      continue;

    // half of all grids start with an ant in the upper-left corner that is moving right
    // => only analyze those and multiply the result by 2 (see the first lines of search(), there is "Symmetries = 2")
    if (row == 0 && firstMove != MoveRight)
      continue;

    // look at each cell of the current row
    Move previous = MoveRight;
    bool invalid  = false;
    unsigned int failedAt = 0;
    for (unsigned int pos = 0; pos < size; pos++)
    {
      auto current = getMove(i, pos);
      auto bit = 1 << pos;

      // can't cross an ant that moving down
      invalid |= (current == MoveUp    &&  (down & bit));
      // upper row needs an ant to move up
      invalid |= (current == MoveUp    && !(up   & bit));
      invalid |= (current != MoveUp    &&  (up   & bit));

      // ants can't cross each other's way
      invalid |= (current == MoveLeft  && previous == MoveRight);

      // can't move down in last row
      invalid |= (current == MoveDown  && row + 1 == size);

      if (invalid)
      {
        failedAt = pos;
        break;
      }

      previous = current;
    }

    // restart with the next ID that starts with different moves
    if (invalid)
    {
      // skip multiple numbers until teh square that failed is different
      if (failedAt != size - 1)
      {
        auto skipSquares = size - (failedAt + 1);
        auto skipIds = 1 << (BitsPerMove * skipSquares);
        i += skipIds - 1; // minus 1 because for-loop always increments by one
      }
      continue;
    }

    // count total movement per cell: the final value has to be zero => one ant walks away (-1) and one ant arrives (+1)
    int movement[MaxSize] = { 0 };
    for (unsigned int pos = 0; pos < size; pos++)
    {
      auto bit = 1 << pos;
      // an ant arrived from above
      if (down & bit)
        movement[pos]++;

      // one ant walks away
      movement[pos]--;

      auto current = getMove(i, pos);

      // to the left ?
      if (current == MoveLeft)
        movement[pos - 1]++;
      // to the right ?
      else if (current == MoveRight)
        movement[pos + 1]++;
    }

    // prepare next row
    Mask nextDown = NoAnt;
    Mask nextUp   = NoAnt;
    for (unsigned int pos = 0; pos < size && !invalid; pos++)
    {
      auto bit = 1 << pos;
      auto current = getMove(i, pos);

      // an ant moves down :-)
      if (current == MoveDown)
      {
        nextDown |= bit;
        invalid  |= (row + 1 == size);
      }

      // prevent too much traffic on a square
      invalid |= (movement[pos] > 0 || movement[pos] < -1);

      // need one ant to move up from the next row
      if (movement[pos] == -1)
      {
        nextUp  |= bit;
        invalid |= (current == MoveDown);
      }
    }

    if (invalid)
      continue;

    // go deeper
    result += search(row + 1, nextDown, nextUp);
  }

  cache[state] = result;
  return result;
}

int main()
{
  std::cin  >> size;
  std::cout << search() << std::endl;
  return 0;
}
