// ////////////////////////////////////////////////////////
// # Title
// The Race
//
// # URL
// https://projecteuler.net/problem=232
// http://euler.stephan-brumme.com/232/
//
// # Problem
// Two players share an unbiased coin and take it in turns to play "The Race".
// On Player 1's turn, he tosses the coin once: if it comes up Heads, he scores one point; if it comes up Tails, he scores nothing.
// On Player 2's turn, she chooses a positive integer T and tosses the coin `T` times: if it comes up all Heads, she scores `2^{T-1}` points; otherwise, she scores nothing.
// Player 1 goes first. The winner is the first to 100 or more points.
//
// On each turn Player 2 selects the number, `T`, of coin tosses that maximises the probability of her winning.
//
// What is the probability that Player 2 wins?
//
// Give your answer rounded to eight decimal places in the form 0.abcdefgh .
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// My solution follows the standard Dynamic Programming pattern:
// the solution is found recursively with aggressive caching of partial results.
//
// The function ''twoWins'' considers only player 2's move. Its parameter is the number of remaining points:
// - if zero points left for player 2, then player 2 won
// - if zero points left for player 1, then player 1 won
// - in any other case, all possible moves of player 2 are analyzed and the best is chosen
//
// Each "round" consists of one turn of player 2 followed by one turn of player 1.
// There are four cases:
// 1. player 2 has all heads up, player 1 has heads up as well
// 2. player 2 has all heads up, player 1 has tails
// 3. player 2 has tosses tails at least once, player 1 has heads up
// 4. both players toss tails
//
// The probabilities of cases 1 to 3 are well defined (''win1'' and ''win2'').
// The only "tricky" case is case 4 where basically nothing changes.
//
// Since ''twoWins'' focuses on player 2, I have to manually play the first turn of player 1 in ''main'' and
// add the probabilities for both (equally likely) outcomes of player 1's first turn.
//
// # Note
// I was sure to have a correct solution but was rejected by the Project Euler website.
// A simple Monte-Carlo simulation told me that my result should be okay, too (considering a certain error margin).
// It took me quite some time to figure out that I have to switch the order of the first lines in ''twoWins'':
// check first whether player 2 won, only then check player 1. That was a nasty mistake.

#include <iostream>
#include <iomanip>
#include <vector>

// the first player with 100 points wins
unsigned int maxScore = 100;

// fixed probabilities of player one
const double win1  = 0.5;
const double lose1 = 1 - win1; // = 0.5

// return chance of winning for player 2 when it's his/her turn
double twoWins(unsigned int needPointsOne, unsigned int needPointsTwo)
{
  // player two won
  if (needPointsTwo == 0)
    return 1;
  // player one won
  if (needPointsOne == 0)
    return 0;

  // memoize
  const double Unknown = -1;
  static std::vector<double> cache(maxScore * maxScore, Unknown);

  auto id = (needPointsOne - 1) * maxScore + needPointsTwo - 1;
  if (cache[id] != Unknown)
    return cache[id];

  // find highest chance of winning
  double best = 0;
  // current bet (observation: betting more than 2^3 = 8 never seems profitable)
  unsigned int bet = 1;
  while (true)
  {
    // probabilities
    auto win2  = 0.5 / bet; // 2^(bet-1)
    auto lose2 = 1 - win2;

    // in case player two scores he shall not exceed the maximum score
    auto nextPointsTwo = (needPointsTwo < bet) ? 0 : needPointsTwo - bet;

    // at least one player scored a point
    auto current = win1  * win2  * twoWins(needPointsOne - 1, nextPointsTwo) +
                   lose1 * win2  * twoWins(needPointsOne,     nextPointsTwo) +
                   win1  * lose2 * twoWins(needPointsOne - 1, needPointsTwo);

    // both players lost, stay in same state
    current /= 1 - lose1 * lose2;

    // better than before ?
    if (best < current)
      best = current;

    // no use in further increasing the risk of player 2 ?
    if (nextPointsTwo == 0)
      break;

    // twice the risk, twice the reward ...
    bet *= 2;
  }

  cache[id] = best;
  return best;
}

int main()
{
  std::cin >> maxScore;

  // player one moves first
  // two options: he/she score one point or not
  // => add both states (multiplied by their probability)
  auto result = win1  * twoWins(maxScore - 1, maxScore) +
                lose1 * twoWins(maxScore,     maxScore);

  std::cout << std::fixed << std::setprecision(8)
            << result << std::endl;
  return 0;
}
