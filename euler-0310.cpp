// ////////////////////////////////////////////////////////
// # Title
// Nim Square
//
// # URL
// https://projecteuler.net/problem=310
// http://euler.stephan-brumme.com/310/
//
// # Problem
// Alice and Bob play the game Nim Square.
// Nim Square is just like ordinary three-heap normal play Nim, but the players may only remove a square number of stones from a heap.
// The number of stones in the three heaps is represented by the ordered triple `(a,b,c)`.
// If `0 <= a <= b <= c <= 29` then the number of losing positions for the next player is 1160.
//
// Find the number of losing positions for the next player if `0 <= a <= b <= c <= 100 000`.
//
// # Solved by
// Stephan Brumme
// December 2017
//
// # Algorithm
// Problem 301 introduced the Nim game and problem 306 re-used some of the knowledge gained and added the concept of a ''mex'' function.
// After reading the blog posting https://mathstrek.blog/2012/08/05/combinatorial-game-theory-iii/ I immediately realized that I already knew enough to solve the current problem -
// but was way to stupid "to open my eyes".
//
// Anyway, writing a quick'n'dirty ''bruteForce'' solver is good idea because it takes just a few minutes and helps verifying that the "smart" solver is correct (for small values).
// See the ''bruteForce'' function and ''#define BRUTE_FORCE''.
//
// The basic idea is to look at a single heap.
// The mex-value of an empty heap is zero because then the game is lost.
// Whenever I want to determine the mex-value of a heap with ''size'' elements then I analyze all heaps ''size - i*i''.
// For example, to compute the mex-value of 13 I need the mex-values of `mex(13 - 1^2) = mex(12)`, `mex(13 - 2^2) = mex(9)` and `mex(13 - 3^2) = mex(4)`.
// The mex-value of 13 is the smallest number `>= 0` which is __not__ among these values.
//
// Maybe it becomes a bit clearer when looking at the mex-values between 0 and 13:
//
// ||    10     ||2||2||2||2||2||2||2||2||2||2|| 2|| 2|| 2|| 2||
// ||!heap size ++0++1++2++3++4++5++6++7++8++9++10++11++12++13||
// ||    mex    ++0++1++0++1++2++0++1++0++1++2++ 0++ 1++ 0++ 1||
//
// Since mex(12) = 0, mex(9) = 2 and mex(4) = 2 the set of its "children mex-values" is `\{ 0, 2 \}`.
// The smallest number `>= 0` which is not part of that set is 1.
//
// The first part of ''search'' implements this straight-forward algorithm.
// Looking at my logging output, the maximum mex-value for heaps smaller than 100000 is surprisingly small: just 74.
// A simple ''std::bitset'' instead of ''std::set'' makes this process pretty fast.
//
// However, the original problem was about three heaps - and not a single heap.
// I could write three nested loops and check whether mex(a) ^ mex(b) ^ mex(c) is zero (= lost, same XOR idea like in problem 301).
// But that's not very efficient: it would require `approx 100000^3` iterations (actually a bit less, but still a huge number).
//
// The following observations speed up the process (`\oplus` is the XOR-operation):
// (1) `x \oplus y` is only zero if `x = y`
// (2) `x \oplus y \oplus z = x \oplus (y \oplus z)`
//
// To count the number of lost positions in a three heap Nim Square game:
// - while analyzing triples `(a,b,c)` many pairs `(b,c)` are repeatedly encountered
// - especially if `a` iterates backwards, that means from ''limit'' to ''0'', then all pairs `(b,c)` of the previous iteration will appears again
// - of course a few new pairs `(b,c)` will appear, too, where `b = a` and `b <= c`
// - I count how often these pairs `(b,c)` produce ''mex(b) ^ mex(c)'' and store it in my ''frequency'' container such that ''frequency[mex[b] ^ mex[c]]++''
// - for each `a` the number in ''frequency[mex[a]]'' is the number of lost games because of equation (1)
//
// # Alternative
// Clever use of combinatorics can eliminate the last step. I opted for the "programming" solution instead of the hairy "mathematical approach".

#include <iostream>
#include <vector>
#include <bitset>
#include <unordered_set>
#include <algorithm>

// needed to compute a unique ID for each position (bruteForce only)
const unsigned long long MaxValue = 100000;

// determine whether a single position is won (true) or lost (false)
// reasonably fast for small values but eats quite a good amount of memory (about 800 MByte to fully analyze a,b,c <= 500)
bool bruteForce(unsigned int a, unsigned int b, unsigned int c)
{
  // game over ?
  if (a == 0 && b == 0 && c == 0)
    return false;

  // sort them in ascending order: a <= b <= c
  if (a > b)
    std::swap(a, b);
  if (b > c)
    std::swap(b, c);
  if (a > b)
    std::swap(a, b);

  // memoize
  auto id = a * MaxValue * MaxValue + b * MaxValue + c;

  // two separate caches
  static std::unordered_set<unsigned long long> cacheWon;
  if (cacheWon .count(id) != 0)
    return true;
  static std::unordered_set<unsigned long long> cacheLost;
  if (cacheLost.count(id) != 0)
    return false;

  // try every possible move:
  // take a square number of stones
  for (unsigned int i = 1; i*i <= c; i++)
  {
    // take a square number of stones from the smallest stack
    if (i*i <= a && !bruteForce(a - i*i, b, c))
    {
      cacheWon.insert(id);
      return true;
    }

    // take a square number of stones from the stack in the middle
    if (i*i <= b && !bruteForce(a, b - i*i, c))
    {
      cacheWon.insert(id);
      return true;
    }

    // take a square number of stones from the largest stack
    if (!bruteForce(a, b, c - i*i))
    {
      cacheWon.insert(id);
      return true;
    }
  }

  // no winning move ... meaning that the current player loses
  cacheLost.insert(id);
  return false;
}

// count all lost positions, just under 5 seconds for limit = 100000
unsigned long long search(unsigned int limit)
{
  // based on this great posting: https://mathstrek.blog/2012/08/05/combinatorial-game-theory-iii/

  // compute Nim values for a single pile
  std::vector<unsigned int> mex(limit + 1, 0); // fill with zeros
  for (unsigned int size = 0; size <= limit; size++)
  {
    // I saw in my experiments that Nim values are pretty small for limit <= 100000 (<= 74)
    const size_t NimLimit = 80;

    // collect all Nim values after each possible move
    std::bitset<NimLimit> moves; // initialized with zeros
    for (unsigned int i = 1; i*i <= size; i++)
      // take i^2 stones from the heap
      moves[mex[size - i*i]] = true;

    // find the smallest non-negative number which is NOT part of the "moves" container
    unsigned int available = 0;
    while (moves[available])
      available++;
    mex[size] = available;
  }

  // the largest Nim value
  auto maxNimValue = *std::max_element(mex.begin(), mex.end());
  // find largest possible number when XORing two Nim values
  unsigned int nextPowerOfTwo = 1; // it's 128
  while (nextPowerOfTwo < maxNimValue)
    nextPowerOfTwo *= 2;

  // count how often value[b] ^ value[c] occurs where a <= b <= c <= limit
  std::vector<unsigned int> frequency(nextPowerOfTwo, 0);

  unsigned long long numLost = 0;
  for (int a = limit; a >= 0; a--)
  {
    // modify the "frequency" container to cover new pairs (b,c), too
    // it already contains all pairs (b+1,c), now a is one unit smaller so b can be one smaller, too
    for (auto b = a; b == a; b++) // I know that it looks silly -
                                  // but I want to make clear it's two nested loops
                                  // where the outer needs only one iteration
      for (auto c = b; c <= (int)limit; c++)
        frequency[mex[b] ^ mex[c]]++;

    // look at Nim value of a
    // how many pairs (b,c) have to same Nim value such that value[a] ^ (value[b] ^ value[c]) = 0 => lost
    numLost += frequency[mex[a]];
  }

  return numLost;
}

int main()
{
  unsigned int limit = 100000;
  std::cin >> limit;

//#define BRUTE_FORCE
#ifdef  BRUTE_FORCE
  // if limit is 500: => 21084251 positions, 2018811 lost => 800 MByte caches in bruteForce(), finish in about two minutes
  // brute-force
  unsigned long long numLost = 0;
  // generate every possible position and count how many are lost
  for (unsigned int a = 0; a <= limit; a++)
    for (auto b = a; b <= limit; b++)
      for (auto c = b; c <= limit; c++)
        // lost ?
        if (!bruteForce(a,b,c))
          numLost++;

  std::cout << numLost << std::endl;
#endif

  std::cout << search(limit) << std::endl;
  return 0;
}
