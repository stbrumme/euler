// ////////////////////////////////////////////////////////
// # Title
// Swapping Counters
//
// # URL
// https://projecteuler.net/problem=321
// http://euler.stephan-brumme.com/321/
//
// # Problem
// A horizontal row comprising of `2n + 1` squares has `n` red counters placed at one end and `n` blue counters at the other end,
// being separated by a single empty square in the centre. For example, when `n = 3`.
//
// ![initial](p321_swapping_counters_1.gif)
//
// A counter can move from one square to the next (slide) or can jump over another counter (hop) as long as the square next to that counter is unoccupied.
//
// ![jumps](p321_swapping_counters_2.gif)
//
// Let `M(n)` represent the minimum number of moves/actions to completely reverse the positions of the coloured counters; that is, move all the red counters to the right and all the blue counters to the left.
//
// It can be verified `M(3) = 15`, which also happens to be a triangle number.
//
// If we create a sequence based on the values of `n` for which `M(n)` is a triangle number then the first five terms would be:
// 1, 3, 10, 22, and 63, and their sum would be 99.
//
// Find the sum of the first forty terms of this sequence.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// I spent most of the time figuring out how `M(n)` can be computed: my function ''countMoves()'' returns the optimal value for small `n`.
// Basically this function performs a [breadth-first search](https://en.wikipedia.org/wiki/Breadth-first_search) on all allowed moves.
//
// The first ten results are:
// ||  3 ||   3  ||
// ||! n ++ M(n) ||
// ||  1 ++   3  ||
// ||  2 ++   8  ||
// ||  3 ++  15  ||
// ||  4 ++  24  ||
// ||  5 ++  35  ||
// ||  6 ++  48  ||
// ||  7 ++  63  ||
// ||  8 ++  80  ||
// ||  9 ++  99  ||
// || 10 ++ 120  ||
//
// Apparently `M(n)` is always the predecessor of a perfect square. To be more specific:
// (1) `M(n) = (n + 1)^2 - 1`
//
// Now that I know how to compute `M(n)` (even for large `n`) I have to find out which `M(n)` are triangle numbers `T(t) = t(t + 1) / 2`:
// (2) `(n + 1)^2 - 1 = t(t + 1) / 2`
// If you uncomment ''#define SLOWSEARCH'' in ''main()'' then a brute-force search enumerates the first 20 solutions in a few seconds.
// However, finding all 40 solutions would take ages.
//
// The equation (2) can be changed to:
// (3) `2(n + 1)^2 - 2 = t(t + 1)`
// (4) `2n^2 + 4n      = t^2 + t`
// (5) `0 = t^2 + t - 2n^2 - 4n`
//
// If I replace `x = t` and `y = n`:
// (6) `0 = x^2 - 2y^2 + x - 4y`
//
// then it's a Diophantine equation that can be solved with a tool (I prefer https://www.alpertron.com.ar/QUAD.HTM ):
// (7) `0 = ax^2 + bxy + cy^2 + dx + ey + f`
// (8) `a = 1` and `b = 0` and `c = -2` and `d = 1` and `e = -4` and `f = 0`
//
// Solutions can be found iteratively:
// (10) `X_{n+1} = P * X_n + Q * Y_n + K`
// (11) `Y_{n+1} = R * X_n + S * Y_n + L`
//
// And the factors are (printed by the solver):
// (12) `P = 3` and `Q = 4` and `K = 5` and `R = 2` and `S = 3` and `L = 3`
//
// My "seed" values were obtained from the brute-force search:
// (13) `x_1 = 2` and `y_1 = 1`
//
// Unfortunately I saw that (10), (11) and (12) only showed every __second__ solution (in comparison to my brute-force search):
// the iterations produced `x_3` and `y_3`, `x_5` and `y_5`, `x_7` and `y_7`, ... and missed all values with an even index.
// Since I knew the second solution (that means `x_2` and `y_2`) from my brute-force search, I tried them and voila: I got `x_4` and `y_4`, `x_6` and `y_6`, `x_8` and `y_8`, ...
//
// In the end I have two pairs (''x1'', ''y1'') and (''x2'', ''y2''). I don't fully understand the underlying problem, but my solution works.
//
// # Note
// Often I take a look at other solutions (especially in the Project Euler forum) and I'm quite surprised that everybody seems to immediately know
// that `M(n) = n * (n + 2)` (which is the same as `(n + 1)^2 - 1`). That's not obvious to me at all.
// Most 30% problems are much easier than this "Swapping Counters" problem which I only managed to solve in a trial-and-error way.
//
// And I don't like these Diophantine equations because I have to make use of a dedicated solver (in my case https://www.alpertron.com.ar/QUAD.HTM ) which is a non-trivial program on its own.

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>

// find the the length of shortest move sequence
// too slow for anything > 10
unsigned int countMoves(unsigned int stonesPerColor)
{
  const auto Red    = 'R';
  const auto Blue   = 'B';
  const auto Empty  = '.';

  const auto Length = 2 * stonesPerColor + 1;

  // create initial state
  const auto initial = std::string(stonesPerColor, Red ) + Empty + std::string(stonesPerColor, Blue);
  // and final state
  const auto final   = std::string(stonesPerColor, Blue) + Empty + std::string(stonesPerColor, Red);

  // breadth search
  std::vector<std::string> last        = { initial };
  std::set   <std::string> alreadySeen = { initial };
  unsigned int numMoves = 0;
  while (true)
  {
    std::vector<std::string> next;
    for (const auto& current : last)
    {
      if (current == final)
        return numMoves;

      // locate the empty cell
      size_t pos = 0;
      while (current[pos] != Empty)
        pos++;

      // generate all possible moves
      if (pos >= 2)
      {
        // jump two positions to the right
        auto jumpTwoRight = current;
        std::swap(jumpTwoRight[pos - 2], jumpTwoRight[pos]);
        // accept only positions that I haven't seen before
        if (alreadySeen.count(jumpTwoRight) == 0)
        {
          next.push_back(jumpTwoRight);
          alreadySeen.insert(jumpTwoRight);
        }
      }
      if (pos >= 1)
      {
        // move one position to the right
        auto moveOneRight = current;
        std::swap(moveOneRight[pos - 1], moveOneRight[pos]);
        // accept only positions that I haven't seen before
        if (alreadySeen.count(moveOneRight) == 0)
        {
          next.push_back(moveOneRight);
          alreadySeen.insert(moveOneRight);
        }
      }
      if (pos < Length - 1)
      {
        // move one position to the left
        auto moveOneLeft = current;
        std::swap(moveOneLeft[pos + 1], moveOneLeft[pos]);
        // accept only positions that I haven't seen before
        if (alreadySeen.count(moveOneLeft) == 0)
        {
          next.push_back(moveOneLeft);
          alreadySeen.insert(moveOneLeft);
        }
      }
      if (pos < Length - 2)
      {
        // jump two positions to the left
        auto jumpTwoLeft = current;
        std::swap(jumpTwoLeft[pos + 2], jumpTwoLeft[pos]);
        // accept only positions that I haven't seen before
        if (alreadySeen.count(jumpTwoLeft) == 0)
        {
          next.push_back(jumpTwoLeft);
          alreadySeen.insert(jumpTwoLeft);
        }
      }
    }

    // next move
    numMoves++;
    last = std::move(next);
  }

  // never reached
  return 0;
}

int main()
{
  unsigned int numValues = 40;
  std::cin >> numValues;

  // count moves via brute-force
//#define COUNTMOVES
#ifdef  COUNTMOVES
  for (unsigned int stonesPerColor = 1; stonesPerColor <= numValues; stonesPerColor++)
    std::cout << "M(" << stonesPerColor << ")=" << countMoves(stonesPerColor) << std::endl;
#endif

  // solve t * (t + 1) / 2 = (n + 1)^2 - 1 via brute-force
//#define SLOWSEARCH
#ifdef  SLOWSEARCH
  unsigned long long t = 1;
  unsigned long long n = 1;

  // only feasible for the first 20 solutions
  unsigned int i = 1;
  unsigned int m = 0;
  while (i <= numValues)
  {
    auto triangle = t * (t + 1) / 2;
    auto moves    = (n + 1) * (n + 1) - 1;
    // is triangle number too small ?
    if (triangle <  moves)
      t++;
    // is triangle number too large ?
    else if (triangle >  moves)
      n++;
    else // if (triangle == moves)
    {
      // found a match
      m += n;
      std::cout << "M(" << i << ")=" << m << std::endl;

      i++; t++; n++;
    }
  }
#endif

  // solve ax^2 + bxy + cy^2 + dx + ey + f = 0
  // a =  1;
  // b =  0;
  // c = -2;
  // d =  1;
  // e = -4;
  // f =  0;

  // inital solution
  unsigned long long x1 = 2;
  unsigned long long y1 = 1;
  unsigned long long x2 = 5;
  unsigned long long y2 = 3;
  unsigned long long sum = y1 + y2;
  for (unsigned int n = 3; n <= numValues; n++)
  {
    // https://www.alpertron.com.ar/QUAD.HTM
    // a =  1;
    // b =  0;
    // c = -2;
    // d =  1;
    // e = -4;
    // f =  0;
    const auto P = 3;
    const auto Q = 4;
    const auto K = 5;
    const auto R = 2;
    const auto S = 3;
    const auto L = 3;

    // update first pair x,y
    auto nextX1 = P*x1 + Q*y1 + K;
    auto nextY1 = R*x1 + S*y1 + L;
    x1 = nextX1;
    y1 = nextY1;
    sum += y1;

    if (++n > numValues)
      break;

    // update second pair x,y
    auto nextX2 = P*x2 + Q*y2 + K;
    auto nextY2 = R*x2 + S*y2 + L;
    x2 = nextX2;
    y2 = nextY2;
    sum += y2;
  }

  // print result
  std::cout << sum << std::endl;
  return 0;
}
