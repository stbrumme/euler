// ////////////////////////////////////////////////////////
// # Title
// Arranged probability
//
// # URL
// https://projecteuler.net/problem=100
// http://euler.stephan-brumme.com/100/
//
// # Problem
// If a box contains twenty-one coloured discs, composed of fifteen blue discs and six red discs,
// and two discs were taken at random, it can be seen that the probability of taking two blue discs, `P(BB) = dfrac{15}{21} * dfrac{14}{20} = dfrac{1}{2}`.
//
// The next such arrangement, for which there is exactly 50% chance of taking two blue discs at random,
// is a box containing eighty-five blue discs and thirty-five red discs.
//
// By finding the first arrangement to contain over `10^12 = 1,000,000,000,000` discs in total, determine the number of blue discs that the box would contain.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// This was a crude hack: I wrote a brute-force program to find the first solutions (ignoring the `10^12` limit) and looked at the numbers.
// For `p / q = 1 / 2` I saw that
// `red_{n+1}  = 2 * blue_n +     red_n - 1`
// `blue_{n+1} =     blue_n + 2 * red_{n+1}`
//
// The initial values `red_0 = 6` and `blue_0 = 15` were already given in the problem statement.
// It takes just a few iterations of these simple operations to find the correct solution. Program runtime is close to 0ms !
//
// # Hackerrank
// A loop analyzes all values `2 <= blue < 100000` (I chose that limit to prevent timeouts).
// The left side of `blue * (blue - 1) * q / p = su m * (su m - 1)` is known and if an integer solution for `su m = blue + red` exists
// such that `su m > minimum` then a valid solution was found.
//
// [TODO] My code fails most test cases because of my arbitrary limit of 1000000.
// [TODO] So far I haven't implemented a proper detection whether a solution exists at all.
// [TODO] It looks a lot like a Pell equation solver might help.

#include <iostream>
#include <cmath>

//#define ORIGINAL

int main()
{
  unsigned int tests;
  std::cin >> tests;

  while (tests--)
  {
    unsigned long long minimum = 1000000000000ULL;

    unsigned long long p = 1;
    unsigned long long q = 2;
    std::cin >> p >> q >> minimum;

    unsigned long long blue = 0;
    unsigned long long red  = 0;

    // special code for p/q = 1/2
    if (p == 1 && q == 2)
    {
      blue = 15;
      red  =  6;

      // keep going until limit is reached
      while (blue + red < minimum)
      {
        // at first I brute-forced the first solutions and wrote them down
        // then I saw the following relationship for p/q = 1/2:
        //  red(i+1) = 2 * blue(i) + red(i) - 1;
        // blue(i+1) = 2 * red(i+1)
        red   = 2 * blue + red - 1; // seems to be true for most p/q
        blue += 2 * red;            // but this line is not correct for p/q != 1/2
      }
#ifdef ORIGINAL
      std::cout << blue << std::endl;
#else
      std::cout << blue << " " << (red + blue) << std::endl;
#endif
      continue;
    }

    // brute-force smallest solution
    bool found = false;
    for (blue = 2; blue < 100000; blue++)
    {
      // sum = red + blue
      // blue * (blue - 1) / (sum * (sum - 1)) = p / q
      // blue * (blue - 1) * q / p = sum * (sum - 1)
      unsigned long long b2 = blue * (blue - 1);
      b2 *= q;
      // right side must be an integer
      if (b2 % p != 0)
        continue;
      unsigned long long sum2 = b2 / p; // sum2 = sum * (sum - 1)

      // (sum-1)^2 < sum2 < sum^2
      unsigned long long sum  = std::sqrt(sum2) + 1;
      // sqrt may have returned a floating-point number
      if (sum * (sum - 1) != sum2)
        continue;

      // now we have the correct solution if minimum is small (< 100000)
      red = sum - blue;
      if (blue + red >= minimum)
      {
        found = true;
        break;
      }
    }

    // failed ? TODO: this means just that my simple search aborted
    if (!found)
    {
      std::cout << "No solution" << std::endl;
      continue;
    }

    // show solution
    std::cout << blue << " " << (red + blue) << std::endl;
  }

  return 0;
}
