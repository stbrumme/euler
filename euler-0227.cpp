// ////////////////////////////////////////////////////////
// # Title
// The Chase
//
// # URL
// https://projecteuler.net/problem=227
// http://euler.stephan-brumme.com/227/
//
// # Problem
// "The Chase" is a game played with two dice and an even number of players.
//
// The players sit around a table; the game begins with two opposite players having one die each.
// On each turn, the two players with a die roll it.
// If a player rolls a 1, he passes the die to his neighbour on the left; if he rolls a 6, he passes the die to his neighbour on the right;
// otherwise, he keeps the die for the next turn.
// The game ends when one player has both dice after they have been rolled and passed; that player has then lost.
//
// In a game with 100 players, what is the expected number of turns the game lasts?
//
// Give your answer rounded to ten significant digits.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// The expected number of turns can be computed as:
// `1 * p(1) + 2 * p(2) + 3 * p(3) + ... + n * p(n)`
// where `p(n)` is the probability that the two dices arrive at the same player after `n` rounds.
//
// My program tracks the distance between the two players with a dice.
// The vector ''last[i]'' contains the probability that the dices are ''i'' units apart.
// Initially they are `100 / 2 = 50` units away from each other therefore ''last[i] = 0'' except ''last[50] = 1''.
//
// In the following iterations the program repeatedly looks at each ''last[i]'':
// - in 1/36 of all cases player 1 rolls a 1 and player 2 rolls a 6 ==> the distance is reduced by 2
// - in 8/36 of all cases the distance is reduced by 1 (one player rolls 2/3/4/5, the other moves one unit closer)
// - in 18/36 of all cases the distance remains unchanged (both roll 2/3/4/5 or both dices move into the same direction)
// - in 8/36 of all cases the distance is increased by 1 (one player rolls 2/3/4/5, the other moves one unit away)
// - in 1/36 of all cases the distance is icnreased by 2 (roll 6 and 1)
//
// There are a few edge cases:
// - the distance can't be more than 50: if the players were 49 units apart then moving two units apart actually means that the distance remains unchanged
// - the distance can't be less than 0:  if the players were 1 unit apart then moving two units closer actually means that the distance remains unchanged
//
// # Note
// The function ''monteCarlo'' is a simple Monte Carlo simulation that produces surprisingly accurate results.
//
// The program stops if sufficient precision has been reached. The value of ''Epsilon'' was chosen manually by observing when the result stabilized.
// It's optimized for 100 players may be a bit off when running live tests with a different number of players.
//
// # Alternative
// My first code was a bit buggy (I didn't think through all edge cases) and therefore wrote a simpler algorithm that matched my Monte-Carlo results:
// two nested loops just roll out all combinations which simplifies things a lot. It's about 4x slower but still finishes in less than 0.2 seconds.
// Use the ''#define FAST'' to switch between my final code (''FAST'' in enabled by default) and that simpler and somehow even more beautiful code.

#include <iostream>
#include <iomanip>
#include <vector>

// ---------- Monte Carlo simulation ----------
// note: not used anymore, I wrote it to generate a good approximation of the result

// a simple pseudo-random number generator
// (produces the same result no matter what compiler you have - unlike rand() from math.h)
unsigned int myrand()
{
  static unsigned long long seed = 0;
  seed = 6364136223846793005ULL * seed + 1;
  return (unsigned int)(seed >> 30);
}

// estimate result with a Monte Carlo simulation
double monteCarlo(unsigned int players, unsigned int iterations)
{
  // count rolls
  unsigned int rolls = 0;

  for (unsigned int i = 0; i < iterations; i++)
  {
    // initial positions: opposite sides of the table
    unsigned int player1 = 0;
    unsigned int player2 = players / 2;

    // both dices at same position?
    while (player1 != player2)
    {
      // roll first dice
      auto dice1 = (myrand() % 6) + 1;
      // and move the dice to a neighbor
      if (dice1 == 1)
        player1 = (player1 + players - 1) % players;
      if (dice1 == 6)
        player1 = (player1           + 1) % players;

      // roll second dice
      auto dice2 = (myrand() % 6) + 1;
      // and move the dice to a neighbor
      if (dice2 == 1)
        player2 = (player2 + players - 1) % players;
      if (dice2 == 6)
        player2 = (player2           + 1) % players;

      rolls++;
    }
  }

  return rolls / double(iterations);
}

// I thought I needed long double but the standard double works perfectly (while float fails)
typedef double Number;

int main()
{
  // number of players
  unsigned int players = 100;
  std::cin >> players;

  // abort if result stabilized
  const auto Epsilon = 0.00000000001;

  // before the first round, the dices are always players/2 units apart (always means 100%)
  std::vector<Number> last(players / 2 + 1, 0);
  last.back() = 1;

  // expected = 1 * probability(finished after 1 iteration) +
  //            2 * probability(finished after 2 iterations) +
  //            3 * probability(finished after 3 iterations) +
  //            4 * probability(finished after 4 iterations) + ...
  Number expected = 0;

  for (unsigned int iteration = 1; ; iteration++) // no abort condition
  {
    // probabilities after this iteration
    std::vector<Number> next(last.size(), 0);

    // analyze each distance except 0 (because those are already finished and not relevant for the next iteration)
    for (size_t current = 1; current < last.size(); current++)
    {
#define FAST
#ifdef  FAST
      // probability of moving two positions closer (roll 1 plus 6)
      auto minus2 =  1.0 / 36;
      // probability of moving one position closer  (roll 1 plus 2/3/4/5 or 2/3/4/5 plus 6)
      auto minus1 =  8.0 / 36;
      // probability that distance of dices remains unchanged (roll 2/3/4/5 plus 2/3/4/5 or 1 plus 1 or 6 plus 6)
      auto same   = 18.0 / 36;
      // probability of moving one position away    (roll 6 plus 2/3/4/5 or 2/3/4/5 plus 1)
      auto plus1  =  8.0 / 36;
      // probability of moving two positions away   (roll 6 plus 1)
      auto plus2  =  1.0 / 36;

      // if dices were one position away from each other, then dices could pass each other
      if (current == 1)
      {
        // dices are still one position away (but they swapped their places)
        same += minus2;
        minus2 = 0;
      }
      else
      // same concept as current == 1 but this time dices were on opposite sides of the table
      if (current == 49)
      {
        same += plus2;
        plus2 = 0;
      }
      else
      // dices are the farthest away - each move brings them closer
      if (current == 50)
      {
        minus2 += plus2;
        minus1 += plus1;
        plus2 = 0;
        plus1 = 0;
      }

      // perform all possible moves (if allowed):
      // two steps closer
      if (current >= 2)
        next[current - 2] += minus2 * last[current];

      // one step closer
      next[current - 1] += minus1 * last[current];

      // unchanged
      next[current    ] += same   * last[current];

      // one step away
      if (current + 1 < next.size())
        next[current + 1] += plus1 * last[current];

      // two steps away
      if (current + 2 < next.size())
        next[current + 2] += plus2 * last[current];

#else
      // slower but simpler approach
      for (auto dice1 = 1; dice1 <= 6; dice1++)
        for (auto dice2 = 1; dice2 <= 6; dice2++)
        {
          // find next distance after rolling both dices
          int destination = (int)current; // might become negative, therefore signed value
          if (dice1 == 1)
            destination--;
          if (dice1 == 6)
            destination++;
          if (dice2 == 1)
            destination++;
          if (dice2 == 6)
            destination--;

          // reached minimum / maximum distance ?
          if (destination == -1)
            destination = +1;
          if (destination > (int)players / 2)
            destination = players - destination;

          // add probabilities
          next[destination] += 1.0 / (6*6) * last[current];
        }
#endif
    }

    // iteration is finished, copy probabilities
    last = std::move(next);

    // look at last[0] (=> probability of finishing after this iteration)
    auto finished = last.front();
    // add to expected value
    auto delta    = finished * iteration;
    expected += delta;

    // enough precision ?
    if (iteration > players * players && delta < Epsilon) // note: first iterations have delta = 0, dont stop too early
      break;
  }

  // print result
  std::cout << std::fixed << std::setprecision(10 - 4) // 10 digits: 6 in front of the decimal separator plus 4 after it
            << expected << std::endl;

  // run Monte Carlo simulation
  //while (true)
    //std::cout << monteCarlo(100, 10000) << std::endl;

  return 0;
}
