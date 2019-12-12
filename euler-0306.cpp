// ////////////////////////////////////////////////////////
// # Title
// Paper-strip Game
//
// # URL
// https://projecteuler.net/problem=306
// http://euler.stephan-brumme.com/306/
//
// # Problem
// The following game is a classic example of Combinatorial Game Theory:
//
// Two players start with a strip of n white squares and they take alternate turns.
// On each turn, a player picks two contiguous white squares and paints them black.
// The first player who cannot make a move loses.
//
// - If `n = 1`, there are no valid moves, so the first player loses automatically.
// - If `n = 2`, there is only one valid move, after which the second player loses.
// - If `n = 3`, there are two valid moves, but both leave a situation where the second player loses.
// - If `n = 4`, there are three valid moves for the first player; she can win the game by painting the two middle squares.
// - If `n = 5`, there are four valid moves for the first player (shown below in red); but no matter what she does, the second player (blue) wins.
//
// ![example](p306_pstrip.gif)
//
// So, for `1 <= n <= 5`, there are 3 values of `n` for which the first player can force a win.
// Similarly, for `1 <= n <= 50`, there are 40 values of `n` for which the first player can force a win.
//
// For `1 <= n <= 1 000 000`, how many values of `n` are there for which the first player can force a win?
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// My first step was to write a simple brute-force routine (see ''bruteForce'') which unfortuinately can't even solve the problem for `n = 50` (but `n < 28` is okay).
// Due to the high number of won starting positions I only printed the lost positions: 1, 5, 9, 15, 21, 25
// That sequence already exists in OEIS A215721 and has a promising name: "The values of N for which the 1xN domino-covering game is a second player win".
//
// I found a short algorithm on that website:
// ''nv(0) = 0''
// ''nv(1) = 0''
// ''nv(N+2) = least nonnegative integer not in {nimsum(nv(k),nv(N-k)) : k <= N} ''
// ''(where nimsum(a,b) is the bitwise xor of a and b). The second player wins a game iff its nim-value is 0.''
// ==> and pretty much the same is explained here: https://math.stackexchange.com/questions/615242/game-involving-tiling-a-1-by-n-board-with-1-x-2-tiles
//
// The XOR operation already solved the Nim problem 301.
// I store the XOR-ed value of a board size in my ''mex'' container (name stems from the StackExchange website, same as ''nv()'' in OEIS).
//
// In order to analyze each board with 2,3,4,...,1000000 squares I perform these steps:
// - assume an empty board, no squares occupied yet
// - the first move can occupy any squares such that there are empty squares on the side and right side (variables ''left'' and ''right'')
// - I can safely assume ''left <= right'' for performance reasons
// - lookup ''mex[left]'' and ''mex[right]''
// - XOR both values: ''xored = mex[left] ^ mex[right]''
// - set a bit for each of those XOR values in ''found[xored]''
// - then find the smallest index ''i'' for which ''found[i]'' is ''false'', that means there was no XOR'ed value matching that index
// - and that's the result !
//
// To improve performance I re-use ''found'' as much as possible and reset only those parts that were modified.
// But no matter how hard I tried: the program was a bit slow and needed about 15 minutes to find the correct result.
// I added a basic progress indicator and printed every 100th lost position: and all ended with a 7 !
// Upon closer inspection I found that they were 680 apart each.
// Then I printed every 10th lost solution and saw that they were 68 apart.
// Then I printed every lost solution and needed a few second to see that a position `n` is lost if `n - 34` was lost.
// Unfortunately this pattern doesn't hold for the first 14 lost positions, therefore I hardcoded them.
// See ''fast()'' for implementation which solves the problem so fast that I can't reliably measure execution time.
// It keeps track of the most recent five values so that ''n % 5'' refers to the index where the result of ''n - 5'' was stored.
//
// # Note
// If I had read OEIS A215721 more carefully I would have noticed that the "n - 34" pattern was described there ... oops.
// Nevertheless, I felt quite proud of myself for a moment. Worth it.
//
// # Alternative
// ''fast()'' isn't the fastest algorithm: it still needs about 30000 loop iterations for `n = 10^6`.
// I could exploit the very repetitive nature of the algorithm and avoid them - and maybe a super-smart compiler does exactly that.
//
// The Sprague-Grundy algorithm can be used to solve a variety of similar problems (see https://blog.plover.com/math/sprague-grundy.html ).

#include <iostream>
#include <vector>

// uncomment to enable the slower algorithms
//#define BRUTEFORCE
//#define NIM_SUM

// return true if player to move wins on a board with numSquares (<= 64)
bool bruteForce(unsigned int numSquares, unsigned long long occupiedMask = 0)
{
  // try each position and call yourself recursively
  for (unsigned int pos = 0; pos + 1 < numSquares; pos++)
  {
    // two neighboring bits => 11 binary => 3
    const unsigned long long twoSquares = 3ULL << pos;
    // both bits empty ?
    if ((occupiedMask & twoSquares) == 0)
      // occupy both bits, if opponent loses then this is a winning move for the current play
      if (!bruteForce(numSquares, occupiedMask | twoSquares))
        return true;
  }

  // all moves are losing moves
  return false;
}

// analyze all boards and count how often the first player wins
unsigned int nimSum(unsigned int maxSquares)
{
  // https://oeis.org/A215721
  // https://math.stackexchange.com/questions/615242/game-involving-tiling-a-1-by-n-board-with-1-x-2-tiles
  // https://blog.plover.com/math/sprague-grundy.html

  unsigned int numLost = 0;

  std::vector<unsigned int> mex(maxSquares + 1, 0);

  // find maximum value created by xoring states
  unsigned int maxXor = 1;
  while (maxXor < maxSquares)
    maxXor <<= 1;

  // track numbers of xored states
  std::vector<bool> found(maxXor, false);
  for (unsigned int i = 2; i <= maxSquares; i++)
  {
    // empty squares on the left and right side after the first move
    int left  = 0;
    int right = i - 2;

    // remember the highest XOR value
    unsigned int lastXor = 0;
    while (left <= right)
    {
      auto xored = mex[left] ^ mex[right];
      found[xored] = true;

      // remember the largest state (to speed up the clean-up/reset)
      if (lastXor < xored)
        lastXor = xored;

      // slide one position to the right
      left++;
      right--;
    }

    // find smallest number which was NOT create during one of the XOR steps
    unsigned int unused = 0;
    while (found[unused])
      unused++;

    // finished computing current value
    mex[i] = unused;

    // first player only loses when mex[i] == 0
    if (mex[i] == 0)
    {
      numLost++;
      if (numLost % 100 == 0)
        std::cout << i << "\t" << std::flush;
    }

    // erase all values (much faster than creating a new shiny std::vector)
    for (unsigned int reset = 0; reset <= lastXor; reset++)
      found[reset] = false;
  }

  return maxSquares - numLost;
}

// super-fast algorithm: implement the pattern observed in nimSum()
// nextLostPosition(n) = nextLostPosition(n - 5) + 34
// except for the first 14 values { 1, 5, 9, 15, 21, 25, 29, 35, 39, 43, 55, 59, 63 }
unsigned int fast(unsigned int maxSquares)
{
  const auto Precomputed = 13;
  const char Initial[Precomputed] = { 1, 5, 9, 15, 21, 25, 29, 35, 39, 43, 55, 59, 63 };

  // store only the most recent 5 values
  unsigned int last5[5];

  unsigned int numLost = 0;
  while (true)
  {
    // a(n) = a(n - 5) + 34
    auto current = last5[numLost % 5] + 34;
    // except for the first 13 values
    if (numLost < Precomputed)
      current = Initial[numLost];

    // found enough
    if (current > maxSquares)
      return maxSquares - numLost;

    // store the least recent 5 values
    last5[numLost % 5] = current;
    numLost++;
  }

  return 0; // never reached
}

int main()
{
  unsigned int limit = 1000000;
  std::cin >> limit;

  // slowest algorithm
#ifdef BRUTEFORCE
  unsigned int won = 0;
  for (unsigned int n = 1; n <= limit; n++)
    if (!bruteForce(n))
    {
      std::cout << n << " " << std::flush;
      won++;
    }
  std::cout << won << std::endl;
#endif

  // idea based on OEIS algorithm
#ifdef NIM_SUM
  std::cout << nimSum(limit) << std::endl;
#endif

  // my fastest algorithm
  std::cout << fast  (limit) << std::endl;
  return 0;
}
