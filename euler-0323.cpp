// ////////////////////////////////////////////////////////
// # Title
// Bitwise-OR operations on random integers
//
// # URL
// https://projecteuler.net/problem=323
// http://euler.stephan-brumme.com/323/
//
// # Problem
// Let `y_0`, `y_1`, `y_2`,... be a sequence of random unsigned 32 bit integers (i.e. `0 <= y_i < 2^32`, every value equally likely).
//
// For the sequence `x_i` the following recursion is given:
//
//    `x_0 = 0` and
//    `x_i = x_{i-1} | y_{i-1}`, for `i > 0`. ( | is the bitwise-OR operator)
//
// It can be seen that eventually there will be an index `N` such that `x_i = 2^{32}-1` (a bit-pattern of all ones) for all `i >= N`.
//
// Find the expected value of `N`.
// Give your answer rounded to 10 digits after the decimal point.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// I evaluate `n` rounds and find their likelihood of producing the final value.
// The "chance of survival" of an unset bit decreases after each round so that the result eventually stabilizes.
// I stop once the delta falls below my threshold `10^-11` (one digit more than needed to prevent rounding issues).
//
// A single is zero after a certain number of rounds with probability:
// (1) `0.5^{round}`
// And it became 1 after all those rounds:
// (2) `1 - 0.5^{round}`
// The chance that all 32 bits are 1:
// (3) `(1 - 0.5^{round})^32`
// Therefore the chance that at least one out of 32 bits is still zero:
// (4) `1 - (1 - 0.5^{round})^32`
//
// I keep adding all values of equation (4) until the value of (4) drops below `10^-11`.
// It takes a little more than 40 iterations to find the correct result.
//
// # Note
// Actually, my first step was writing a simple Monte-Carlo simulation to get an approximation of the result.
// Of course it's impossible to find all 10 digits after the decimal point (unless you invest a huge amount of time).
// The code can still be found in ''montecarlo'' (and its helper function ''myrand'').
//
// It's worth mentioning that the results of my Monte-Carlo simulation differed significantly from their true values.
// I spent more than an hour trying to find a bug in my "mathematical" approach because I trusted the Monte-Carlo result.
// As it turned out, ''myrand'' was flawed because the simple LCG algorithm had bad parameters.
// When I choose different parameters (from https://en.wikipedia.org/wiki/Linear_congruential_generator ) then everything turned out to be fine.

#include <iostream>
#include <iomanip>
#include <cmath>

// 32 bits
unsigned int maxBits = 32;

// ---------- first approach: find a good estimation with Monte-Carlo simulation ----------

// a simple pseudo-random number generator
// (produces the same result no matter what compiler you have - unlike rand() from math.h)
unsigned int myrand()
{
  static unsigned long long seed = 0;
  seed = 6364136223846793005ULL * seed + 1;
  return seed >> 30;
}

// randomized simulation
double montecarlo(unsigned int iterations)
{
  // all 32 bits are set
  const unsigned int allBits = (1ULL << maxBits) - 1;

  unsigned int numSteps = 0;
  for (unsigned int i = 0; i < iterations; i++)
  {
    // start with zero
    unsigned int current = 0;
    // and repeat until all bits are set
    do
    {
      // and some random bits
      current |= myrand() & allBits;
      numSteps++;
    } while (current != allBits);
  }

  // return average number of steps until "completion"
  return numSteps / double(iterations);
}

// ---------- final approach: solve "smarter" ! ----------

int main()
{
  std::cin >> maxBits;

  // number of accurate digits
  const unsigned int digits = 10;
  // 0.00000000001 => one more zero ensures that the last digit is correct even after rounding
  const double Epsilon = pow(10.0, -(double)(digits + 1));

  // at least one round
  double result = 0;
  unsigned int round = 0;

  // until change is smaller than Epsilon
  while (true)
  {
    // chance of "survival" for a single zero after all those rounds
    auto hasZero = pow(0.5, round);
    // were all 32 bits flipped from 0 to 1 ?
    auto isDone  = pow(1 - hasZero, maxBits);

    // remaining
    auto delta = 1 - isDone;
    // enough digits ?
    if (delta < Epsilon)
      break;

    // next iteration
    result += delta;
    round++; // stop after 42 rounds
  }

  // display result
  std::cout << std::fixed << std::setprecision(digits)
            << result << std::endl;

  // run Monte-Carlo simulation
  //while (true)
  //  std::cout << montecarlo(100000000) << std::endl;

  return 0;
}

