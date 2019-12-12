// ////////////////////////////////////////////////////////
// # Title
// Under The Rainbow
//
// # URL
// https://projecteuler.net/problem=493
// http://euler.stephan-brumme.com/493/
//
// # Problem
// 70 colored balls are placed in an urn, 10 for each of the seven rainbow colors.
//
// What is the expected number of distinct colors in 20 randomly picked balls?
//
// Give your answer with nine digits after the decimal point (a.bcdefghij).
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// My standard procedure for probability problems is to write a simple Monte-Carlo simulation.
// The first four or five digits are correct - but still a far cry from the nine digits requested by the problem statement.
//
// A much smarter approach is to turn the problem upside-down:
// for each color I compute the probability that this color is __not__ among the 20 picked balls.
// That's the probability that 20 out of 70 balls are actually taken from a set of only 70 - 10 = 60 balls:
// `{{60}choose{20}} div {{70}choose{20}}`
//
// Then the opposite is the probability that this color was picked:
// `1 - {{60}choose{20}} div {{70}choose{20}}`
//
// Computing that equation for all 7 colors gives the result:
// `7 * (1 - {{60}choose{20}} div {{70}choose{20}} )`
//
// # Note
// The main problem is to have a proper ''choose'' function. The naive way to compute ''choose(70, 20)'' is to find `70!` and `20!`.
// However, `70!` causes serious problems because of its size (about 100 digits, way more than a 64 bit integer can handle).
//
// Lucky for me, I already wrote such a ''choose'' function for problem 116. Thus the code for this problem was actually pretty simple.

#include <iostream>
#include <iomanip>
#include <vector>

// ---------- Monte-Carlo simulation to get a rough estimation ----------
// note: not used anymore

// a simple pseudo-random number generator
// (produces the same result no matter what compiler you have - unlike rand() from math.h)
unsigned int myrand()
{
  static unsigned long long seed = 0;
  seed = 6364136223846793005ULL * seed + 1;
  return (unsigned int)(seed >> 30);
}

// Monte Carlo simulation
double monteCarlo(unsigned int numColors, unsigned int numBallsPerColor, unsigned int picks, unsigned int iterations)
{
  // count different colors of all iterations
  unsigned int sum = 0;

  auto numBalls = numColors * numBallsPerColor;
  for (unsigned int i = 0; i < iterations; i++)
  {
    std::vector<bool> current(numBalls, true);
    for (unsigned int j = 0; j < picks; j++)
    {
      unsigned int id;
      // pick a ball (if still available)
      do
        id = myrand() % numBalls;
      while (!current[id]);

      // remove that ball
      current[id] = false;
    }

    // for each color check if at least one ball was picked
    for (unsigned int c = 0; c < numColors; c++)
      for (unsigned int b = 0; b < numBallsPerColor; b++)
      {
        auto id = c * numBallsPerColor + b;
        if (!current[id])
        {
          sum++;
          break;
        }
      }
  }

  return sum / double(iterations);
}

// ---------- my solution ----------

// number of ways to choose n elements from k available
// taken from problem 116
unsigned long long choose(unsigned long long n, unsigned long long k)
{
  // n! / (n-k)!k!
  unsigned long long result = 1;
  // reduce overflow by dividing as soon as possible to keep numbers small
  for (unsigned long long invK = 1; invK <= k; invK++)
  {
    result *= n;
    result /= invK;
    n--;
  }
  return result;
}

int main()
{
  unsigned int colors        =  7;
  unsigned int ballsPerColor = 10;
  unsigned int picks         = 20;
  std::cin >> colors >> ballsPerColor >> picks;

  const auto numBalls = colors * ballsPerColor;

  std::cout << std::fixed << std::setprecision(9);

  // run Monte-Carlo simulation to estimate the result
  //while (true)
    //std::cout << monteCarlo(colors, ballsPerColor, picks, 10000000) << std::endl;

  // and now the precise computation
  double result = 0;
  for (unsigned int current = 0; current < colors; current++)
  {
    // count combinations where any ball is picked - but not from a special color
    auto dontPickThatColor = choose(numBalls - ballsPerColor, picks);
    // count combinations where any ball can be picked, no matter what color
    auto total             = choose(numBalls,                 picks);

    // probability that the current color was not picked
    auto absent   = dontPickThatColor / double(total);
    // probability that the current color was     picked
    auto hasColor = 1 - absent;

    // expected value: 0 if absent, 1 if present
    result += 0 * absent + 1 * hasColor; // which is the same as result += hasColor
  }

  // obviously all 7 iterations don't depend on "current" and could be merged into a single equation:
  //result = colors * (1 - choose(numBalls - ballsPerColor, picks) / (double)choose(numBalls, picks));

  // and we're done !
  std::cout << result << std::endl;
  return 0;
}
