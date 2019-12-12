// ////////////////////////////////////////////////////////
// # Title
// Chip Defects
//
// # URL
// https://projecteuler.net/problem=307
// http://euler.stephan-brumme.com/307/
//
// # Problem
// `k` defects are randomly distributed amongst `n` integrated-circuit chips produced by a factory
// (any number of defects may be found on a chip and each defect is independent of the other defects).
//
// Let `p(k,n)` represent the probability that there is a chip with at least 3 defects.
// For instance `p(3,7) approx 0.0204081633`.
//
// Find `p(20 000, 1 000 000)` and give your answer rounded to 10 decimal places in the form 0.abcdefghij
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// It took quite some effort to finally solve this problem. First I had a minor flaw in my formulas - but most time was spent
// working around precision issues. The current code does __not__ work with Visual C++ because I need extended floating-point precision (''long double'').
//
// The main idea is that instead of counting chips with 3+ defect I count the opposite: chips with 0, 1 or 2 defects.
//
// The probability of having exactly `x` chips with 2 defects each is
// `dfrac{k!}{(k-x)!} * dfrac{p! / ((p-2x)! x!)}{2^x} * k^{-p}`
//
// Summing these values for all possible `x` (==> number of chips with 2 defects) will give the percentage of working chips, so
// `p(k,n) = 1 - sum probabilities(x,k,n)`
//
// The involved numbers are huge and exceed the range of C++'s floating point data types.
// Therefore I perform all calculations in "log space":
// `a = e^{ln(a)}`
// `a * b = e^{ln(a) + ln(b)}`
// `a ^ b = e^{ln(a) * b}`
//
// I wrote a function ''logFactorial(n)'' that returns `ln{n!}`.
// A similar function ''logFactorial(n, onlyTopValues)'' returns `ln{dfrac{n!}{(n - onlyTopValues)!}}` which is needed twice
// and much faster and more precise to calculate than calling ''logFactorial(n)'' twice.
//
// You will find my old code of a Monte-Carlo simulation as well (see ''monteCarlo'').
// It helped me fixing a major bug in my mathematical formula, which was way off.
// As always, it's impossible to achieve the required accuracy (10 digits) with a basic Monte-Carlo simulations.
//
// # Note
// As mentionend before, a simple ''double'' has some precision issues and the last digit was off by 3.
// GCC supports ''long double'' (which has 80 instead of 64 bits) and finds the correct result.

#include <iostream>
#include <iomanip>
#include <cmath>

// ---------- Monte-Carlo simulation ----------
// => not used anymore, I ran it to have a crude approximation of the result
#include <vector>

// a simple pseudo-random number generator
// (produces the same result no matter what compiler you have - unlike rand() from math.h)
unsigned int myrand()
{
  static unsigned long long seed = 0;
  seed = 6364136223846793005ULL * seed + 1;
  return (unsigned int)(seed >> 30);
}

// simulate k defects on n chips, return percentage of 3+ defects on at least one chip
double monteCarlo(unsigned int iterations, unsigned int k, unsigned int n)
{
  // a chip is defect if 3+ defects at once
  const unsigned char threshold = 3;

  // at least one chip isn't working (3+ defects)
  unsigned int bad = 0;
  std::vector<unsigned char> defects(n);
  for (unsigned int i = 0; i < iterations; i++)
  {
    // reset array
    for (auto& x : defects)
      x = 0;

    // spread defects randomly
    for (unsigned int j = 0; j < k; j++)
    {
      auto id = myrand() % n;
      defects[id]++;
      // found one more iteration with at least one chips that's out of order
      if (defects[id] == threshold)
      {
        bad++;
        break;
      }
    }
  }

  // percentage of chips with 3+ defects
  return bad / double(iterations);
}

// ---------- explicit computation ----------

typedef long double Number;

// return log(n!)
Number logFactorial(unsigned int n)
{
  Number result = 0; // = log(1);
  for (unsigned int i = 2; i <= n; i++)
    result += log(Number(i));
  return result;
}

// return log(n! / (n - onlyTopValues)!)
Number logFactorial(unsigned int n, unsigned int onlyTopValues)
{
  Number result = 0;
  for (auto i = n - onlyTopValues + 1; i <= n; i++)
    result += log(Number(i));
  return result;
}

int main()
{
  unsigned int chips   = 1000000;
  unsigned int defects =   20000;
  std::cin >> defects >> chips;

  // 10^-13
  Number precisionThreshold = 0.0000000000001;

  // total number of combinations:
  // chips ^ defects
  auto combinations = log(Number(chips)) * defects;

  // add probabilities of not having a chip with 3+ defects
  Number sum = 0;

  // process all possible number of chips with exactly 2 defects
  for (unsigned int numTwoDefects = 0; numTwoDefects <= defects / 2; numTwoDefects++)
  {
    // chips with one or two defects
    auto affectedChips = defects - numTwoDefects;
    auto permutations  = logFactorial(chips, affectedChips);

    // count combinations of chips with one defect
    auto defectsFoundOnChipsWithTwo = 2 * numTwoDefects;
    auto count = logFactorial(defects, defectsFoundOnChipsWithTwo);

    // divide by numTwoDefects!
    count -= logFactorial(numTwoDefects);

    // divide by 2^numTwoDefects
    auto countTwoDefects = numTwoDefects * log(Number(2));
    count -= countTwoDefects;

    // multiply both
    auto noDefects = permutations + count;

    // percentage of working chips
    auto ratio = noDefects - combinations;

    // convert from "log space" to normal numbers
    ratio = exp(ratio);
    sum  += ratio;

    // abort early if sufficient precision
    if (sum > 0.01 && ratio < precisionThreshold)
      break;
  }

  // percentage of 3+ defects is opposite of percentage of 2- chips
  auto result = 1 - sum;
  std::cout << std::fixed << std::setprecision(10)
            << result << std::endl;

  // run repeatedly one million Monte-Carlo simulations
  // the first two digits will be okay
  //while (true)
  //  std::cout << monteCarlo(100000, 20000, 1000000) << std::endl;

  return 0;
}
