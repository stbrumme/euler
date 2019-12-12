// ////////////////////////////////////////////////////////
// # Title
// Billionaire
//
// # URL
// https://projecteuler.net/problem=267
// http://euler.stephan-brumme.com/267/
//
// # Problem
// You are given a unique investment opportunity.
// Starting with &pound;1 of capital, you can choose a fixed proportion, f, of your capital to bet on a fair coin toss repeatedly for 1000 tosses.
//
// Your return is double your bet for heads and you lose your bet for tails.
//
// For example, if f=1/4, for the first toss you bet &pound;0.25, and if heads comes up you win &pound;0.5 and so then have &pound;1.5.
// You then bet &pound;0.375 and if the second toss is tails, you have &pound;1.125.
//
// Choosing f to maximize your chances of having at least &pound;1,000,000,000 after 1,000 flips, what is the chance that you become a billionaire?
//
// All computations are assumed to be exact (no rounding), but give your answer rounded to 12 digits behind the decimal point in the form 0.abcdefghijkl.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// My function ''getMinHeads'' performs a simple linear search for the minimum number of heads required to become a billionaire.
// ''ratio'' (which is f in the problem description) is slowly increased from 0 to 1 with a ''step = 0.0001'' (works with larger values, such as ''0.01'', too).
// The expected amount of money is `(1 + 2f)^{heads} * (1 - f)^{tails}`.
// After 1000 tosses, this is equivalent to `(1 + 2f)^{heads} * (1 - f)^{1000 - heads}`.
// The inner-most ''for''-loop tries to find the lowest value of `heads` where `(1 + 2f)^{heads} * (1 - f)^{1000 - heads} >= 10^9`.
//
// My function ''probability'' is simple Dynamic Programming code that returns the probability of at least ''minHeads'' times observing a head after ''maxTosses'' fair coin tosses.
//
// # Alternative
// There are mathematical ways to figure out the results of ''getMinHeads'' with pen and paper but I like the simple and obvious code of my solution.

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

// find minimum number of heads to become a billionaire
unsigned int getMinHeads(unsigned int tosses, double billion, double step = 0.0001)
{
  unsigned int result = tosses;

  // simple linear search
  for (double ratio = 0; ratio < 1; ratio += step)
  {
    // won:  gain twice the invested money
    auto won  = 1 + 2 * ratio;
    // lost: invested money is lost
    auto lost = 1 - ratio;

    // try to have a billion with less heads than the total best
    unsigned int heads = result;
    for (; heads > 0; heads--)
    {
      // total amount of money
      auto total = pow(won, heads) * pow(lost, tosses - heads);
      // below one billion ? one step too far
      if (total < billion)
      {
        heads++;
        break;
      }
    }

    // better than before ?
    if (result > heads)
      result = heads;
  }

  return result;
}

// return probability of tossing at least "minHeads" times head with a fair coin
// note: parameters "tosses" and "heads" are using during recursion only
double probability(unsigned int minHeads, unsigned int maxTosses, unsigned int tosses = 0, unsigned int heads = 0)
{
  // enough heads ?
  if (heads >= minHeads)
    return 1;
  // too few heads: can't achieve minHeads even if a series of heads only appears ?
  if (maxTosses - tosses < minHeads - heads)
    return 0;

  // memoize
  const double Unknown = -1;
  static std::vector<double> cache(minHeads*maxTosses, Unknown);
  auto id = heads * maxTosses + tosses;
  if (cache[id] != Unknown)
    return cache[id];

  // fifty-fifty ...
  auto result = 0.5 * probability(minHeads, maxTosses, tosses + 1, heads + 1) +
                0.5 * probability(minHeads, maxTosses, tosses + 1, heads    );

  cache[id] = result;
  return result;
}

int main()
{
  // I wanna be a billionaire after 1000 coin tosses !
  unsigned int tosses = 1000;
  double money = 1000000000;

  std::cin >> tosses >> money;

  // determine minimum number of heads
  auto minHeads = getMinHeads(tosses, money);

  // compute probability for that number of heads (or more) after 1000 coin tosses
  std::cout << std::fixed << std::setprecision(12)
            << probability(minHeads, tosses) << std::endl;
  return 0;
}
