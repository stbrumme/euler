// ////////////////////////////////////////////////////////
// # Title
// Sliding game
//
// # URL
// https://projecteuler.net/problem=313
// http://euler.stephan-brumme.com/313/
//
// # Problem
// In a sliding game a counter may slide horizontally or vertically into an empty space.
// The objective of the game is to move the red counter from the top left corner of a grid to the bottom right corner;
// the space always starts in the bottom right corner.
// For example, the following sequence of pictures show how the game can be completed in five moves on a 2 by 2 grid.
//
// ![2x2 sliding](p313_sliding_game_1.gif)
//
// Let `S(m,n)` represent the minimum number of moves to complete the game on an `m` by `n` grid. For example, it can be verified that `S(5,4) = 25`.
//
// ![x4 sliding](p313_sliding_game_2.gif)
//
// There are exactly 5482 grids for which `S(m,n) = p^2`, where `p < 100` is prime.
//
// How many grids does `S(m,n) = p^2`, where `p < 10^6` is prime?
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// Wow, I needed quite some time to solve this apparently easy problem (difficulty rated at just 30% by Project Euler) ...
// These three steps were taken:
// 1. write a simple brute-force solution for very small `m` and `n`
// 2. deduce a slightly smarter version for `S(m,n) = p^2` where `p < 100`
// 3. find the the fast formula for `p < 10^6`
//
// ''bruteForce()'' represents my first step: it's a breadth-first search that generates all "moves" of the empty square until the red counter is in its final position.
// Even though a substantial amount of code is devoted to this algorithm, it is pretty straightforward and easy to implement (and I enjoyed it !).
//
// It pretty obvious that `S(m, n) = S(n, m)`. Therefore I assume in the following text that `m >= n`.
//
// I printed the results for `n <= m <= 15` and observed a pattern which lead to the faster ''search()'' (my second step):
// (1) `S(m, 2) = S(m-1, 2) + 6`
// (2) `S(m, n) = S(m, n-1) + 2`
//
// There are a few exceptions: `S(2, 2) = 0` and `S(3, 2) = 9` and `S(3, 3) = 13`.
// It took a few minutes to get everything right and, voila !, ''search()'' found the correct result 5482 for `p < 100`.
//
// The problem asks for `p < 10^6` which is `10^4` times larger - and due to the quadratic nature of my algorithm it will take about `10^8` as long. That's several days !
//
// After a few days I revisited my partial solution and further analyzed my debugging output and again, there was a pattern:
// the differences between consecutive numbers grow almost quadratic:
//
// ||   4  ||   4   ||    4  ||           10           ||       6       ||
// ||! `p` ++ `p^2` ++ `S_p` ++ `diff = S_p - S_{p-1}` ++ `p^2 / diff`  ||
// ||   3  ++    9  ++    2  ++            ~           ++      ~        ||
// ||   5  ++   25  ++    4  ++            2           ++    4.5        ||
// ||   7  ++   49  ++    8  ++            4           ++   12.5        ||
// ||  11  ++  121  ++   18  ++           10           ++   12.25       ||
// ||  13  ++  169  ++   32  ++           14           ++   12.1        ||
// ||  17  ++  289  ++   56  ++           24           ++   12.07143... ||
// ||  19  ++  361  ++   86  ++           30           ++   12.04167... ||
// ||  23  ++  529  ++  130  ++           44           ++   12.03333... ||
// ||  29  ++  841  ++  200  ++           70           ++   12.02273... ||
//
// The last column gets closer and closer to 12 !
// I tried a few formulas in a spreadsheet and it turns out that `diff = (p^2 - 1) / 12` matches all cases except for `p = 3`.
//
// My final algorithm is to compute the sum of `(p^2 - 1) / 12` for all primes `p` (except for `p = 3` then it's `2`).
//
// # Note
// I couldn't find an obvious mathematical approach and solved this with a combination of programming and lots of guesswork.
// That's not desirable - well, I like the programming part but guessing isn't my strong suit.
// Admittedly, the final algorithm is very simple and could be considered "worth" only 30% but it's not an easy way to get there.
// I had to write more than 200 lines of code to "stumble upon" a one-liner solution.

#include <iostream>
#include <vector>
#include <set>
#include <cmath>

// ---------- standard prime sieve from my toolbox ----------

// odd prime numbers are marked as "true" in a bitvector
std::vector<bool> sieve;

// return true, if x is a prime number
bool isPrime(unsigned int x)
{
  // handle even numbers
  if ((x & 1) == 0)
    return x == 2;

  // lookup for odd numbers
  return sieve[x >> 1];
}

// find all prime numbers from 2 to size
void fillSieve(unsigned int size)
{
  // store only odd numbers
  const unsigned int half = (size >> 1) + 1;

  // allocate memory
  sieve.resize(half, true);
  // 1 is not a prime number
  sieve[0] = false;

  // process all relevant prime factors
  for (unsigned int i = 1; 2*i*i < half; i++)
    // do we have a prime factor ?
    if (sieve[i])
    {
      // mark all its multiples as false
      unsigned int current = 3*i+1;
      while (current < half)
      {
        sieve[current] = false;
        current += 2*i+1;
      }
    }
}

// ---------- problem specific code ----------

// (0,0) is located in the upper-left corner
struct Board
{
  typedef unsigned int Number;
  // position of the red counter
  Number redX, redY;
  // empty square
  Number emptyX, emptyY;

  Board(Number redX_, Number redY_, Number emptyX_, Number emptyY_)
  : redX(redX_), redY(redY_), emptyX(emptyX_), emptyY(emptyY_)
  {}

  // for std::set
  bool operator<(const Board& other) const
  {
    if (redX   != other.redX)   return redX   < other.redX;
    if (redY   != other.redY)   return redY   < other.redY;
    if (emptyX != other.emptyX) return emptyX < other.emptyX;
    return emptyY < other.emptyY;
  }
};

// breadth-search
unsigned int bruteForce(unsigned int width, unsigned int height)
{
  // ensure width >= height
  if (width < height)
    std::swap(width, height);

  // start with the red counter in the upper-left corner and empty square in lowe-right corner
  Board initial(0,0, width-1,height-1);
  std::vector<Board> todo = { initial };
  std::set<Board> visited = { initial };

  unsigned int depth = 0;
  while (!todo.empty())
  {
    std::vector<Board> next;
    for (auto current : todo)
    {
      // finished ?
      if (current.redX == width - 1 && current.redY == height - 1)
        return depth;

      // "move" the empty square, ignore already visited positions
      // right
      if (current.emptyX + 1 < width)
      {
        Board right = current;
        right.emptyX++;
        // move red counter ?
        if (right.emptyX == right.redX && right.emptyY == right.redY)
          right.redX--;
        // if unvisited yet, then add to queue for next iteration
        if (visited.count(right) == 0)
        {
          next.push_back(right);
          visited.insert(right);
        }
      }
      // left
      if (current.emptyX > 0)
      {
        Board left = current;
        left.emptyX--;
        // move red counter ?
        if (left.emptyX == left.redX && left.emptyY == left.redY)
          left.redX++;
        // if unvisited yet, then add to queue for next iteration
        if (visited.count(left) == 0)
        {
          next.push_back(left);
          visited.insert(left);
        }
      }
      // down
      if (current.emptyY + 1 < height)
      {
        Board down = current;
        down.emptyY++;
        // move red counter ?
        if (down.emptyX == down.redX && down.emptyY == down.redY)
          down.redY--;
        // if unvisited yet, then add to queue for next iteration
        if (visited.count(down) == 0)
        {
          next.push_back(down);
          visited.insert(down);
        }
      }
      // up
      if (current.emptyY > 0)
      {
        Board up = current;
        up.emptyY--;
        // move red counter ?
        if (up.emptyX == up.redX && up.emptyY == up.redY)
          up.redY++;
        // if unvisited yet, then add to queue for next iteration
        if (visited.count(up) == 0)
        {
          next.push_back(up);
          visited.insert(up);
        }
      }
    }

    // next iteration
    todo = std::move(next);
    depth++;
  }

  // unsolvable grid
  return 0; // never reached
}

// just a bit of pattern matching until it produced the same results as bruteForceSlow()
unsigned int search(unsigned int width, unsigned int height)
{
  // ensure width >= height
  if (width < height)
    std::swap(width, height);

  // small grids are impossible
  if (width <  2)
    return 0;
  // precalculated results for 2x2, 3x2 and 3x3
  if (width == 2)
    return 5;
  if (width == 3)
    return height == 2 ? 9 : 13;

  // result for height = 2
  auto two  = 6 * width - 9;
  // and adjust for arbitrary height
  auto full = two + 2 * height - 4;
  // two more moves required if both sides are equal
  if (width == height)
    full += 2;

  return full;
}

int main()
{
  unsigned int limit = 1000000;
  std::cin >> limit;

  // verify the two drawings
  //std::cout << bruteForce(2, 2) << std::endl;
  //std::cout << bruteForce(5, 4) << std::endl;

  // generate enough prime numbers
  fillSieve(limit);

//#define VERIFY_100
#ifdef  VERIFY_100
  // use faster bruteForce() function to verify S(100)
  for (unsigned int current = 2; current <= limit; current++)
  {
    if (!isPrime(current))
      continue;

    unsigned int sum = 0;

    // verify the larger dataset p^2 where p is a prime and p < 100
    for (unsigned int width = 2; search(width, 2) <= current*current; width++)
      for (unsigned int height = 2; height <= width; height++)
      {
        auto steps = search(width, height);
        // too large ?
        if (steps > current * current)
          break;

        // is it a square of a prime ?
        unsigned int root = sqrt(steps);
        if (root * root != steps || !isPrime(root))
          continue;

        // found one more solution
        sum++;
        // symmetry: if (m,n) is a solution then (n,m) is a solution, too
        if (width != height)
          sum++;
      }

    // display result
    std::cout << "S(" << current << ")=" << sum << std::endl;
  }
#endif

  // fast algorithm
  unsigned long long sum = 0;
  for (unsigned int p = 2; p <= limit; p++)
  {
    if (!isPrime(p))
      continue;

    // increment is (p^2 - 1) / 12
    // need 64 bit for p^2
    auto square    = (unsigned long long) p * p;
    auto increment = (square - 1) / 12;
    // except if p=3
    if (p == 3)
      sum += 2;
    else
      sum += increment;
  }

  // finally ...
  std::cout << sum << std::endl;
  return 0;
}
