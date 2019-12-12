// ////////////////////////////////////////////////////////
// # Title
// Exploring Pascal's pyramid
//
// # URL
// https://projecteuler.net/problem=154
// http://euler.stephan-brumme.com/154/
//
// # Problem
// A triangular pyramid is constructed using spherical balls so that each ball rests on exactly three balls of the next lower level.
//
// ![Pascal's pyramid](p154_pyramid.gif)
//
// Then, we calculate the number of paths leading from the apex to each position:
//
// A path starts at the apex and progresses downwards to any of the three spheres directly below the current position.
//
// Consequently, the number of paths to reach a certain position is the sum of the numbers immediately above it
// (depending on the position, there are up to three numbers above it).
//
// The result is Pascal's pyramid and the numbers at each level n are the coefficients of the trinomial expansion `(x + y + z)^n`.
//
// How many coefficients in the expansion of `(x + y + z)^200000` are multiples of `10^12`?
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// Wikipedia told me that a value of Pascal's pyramid can be derived from Pascal's triangle ( https://en.wikipedia.org/wiki/Pascal%27s_pyramid )
// `P(level, i, j) = T(level, i) * T(i, j)`
//
// And a value of Pascal's triangle is computed with the binomial coefficient:
// `T(n, k) = dfrac{n!}{(n-k)! * k!}`
//
// Those values get huge - but I don't need to know their true value.
// All I need to know is whether `P(level, i, j)` can be factorized into at least 12x 2 and 12x 5 because `2^12 * 5^12 = 10^12`.
//
// My program stores for each number between 1 and 200000 (= ''layer'') how often it contains 2 and 5 (see ''mulPrime1'' and ''mulPrime2'').
// Repeatedly dividing the current number by 2 and 5 produces the desired result.
//
// Then it stores for each factorial between 1 and 200000 how often it contains 2 and 5 (see ''sum1'' and ''sum2'').
// Since `s^x * s^y = s^{x+y}` it boils down to summing the exponents up to the current point.
//
// The same idea make ''choose'' very simple:
// ''choose(T(n, k)) = sums[n] - (sums[n-k] + sums[k])''
// When I call ''choose'' with ''sums = sum1'' then I know how many 2s T(n,k) contains.
// And the same happens with ''sums = sum2'' for the number of 5s.
//
// Whenever I have at least 12x 2s and 12x 5s then I found a multiple of `10^12`.
//
// There a two simple optimizations:
// - if `T(level, i)` is a multiple of `10^12` then any `T(level, i) * T(i, j)` is as well
// - `T(n, k) = T(n, n - k)` ==> the pyramid is symmetric and I only need to compute its left half
//
// # Note
// There are more symmetries in the pyramid that I didn't exploit: I can see six "areas" with identical values.
// Proper use would make my program probably 3 times faster.
//
// # Hackerrank
// Hackerrank wants you to count all numbers that are multiples of `p_1^{a_1} * p_2^{a_2}` where `p_1` and `p_2` are distinct prime numbers.
// `10^12 = 2^12 * 5^12`, therefore my program finds the correct solution for the original problem when entering ''200000 2 12 5 12''.

#include <iostream>
#include <vector>

typedef std::vector<unsigned int> Exponents;

// return exponent of a prime factor of C(n,k)
// looks a bit like the logarithm:
// C(n,k) = n! / ((n-k)! * k!)
unsigned int choose(const Exponents& sums, unsigned int n, unsigned int k)
{
  return sums[n] - (sums[n - k] + sums[k]);
}

int main()
{
  unsigned int layer     = 200000;
   // 10^12 = 2^12 * 5^12
  unsigned int prime1    =  2;
  unsigned int exponent1 = 12;
  unsigned int prime2    =  5;
  unsigned int exponent2 = 12;
  std::cin >> layer >> prime1 >> exponent1 >> prime2 >> exponent2;

  // analyze for each number between 0 and layer how often they contain prime1 and prime2
  Exponents mulPrime1 = { 0 };
  Exponents mulPrime2 = { 0 };
  for (unsigned int x = 1; x <= layer; x++)
  {
    auto current = x;
    unsigned int count = 0;
    // extract first prime (=2) as often as possible
    while (current % prime1 == 0)
    {
      current /= prime1;
      count++;
    }
    mulPrime1.push_back(count);

    count = 0;
    // extract second prime (=5) as often as possible
    while (current % prime2 == 0)
    {
      current /= prime2;
      count++;
    }
    mulPrime2.push_back(count);
  }

  // sum1[x] = sum of mulPrime1[0 ... x]
  Exponents sum1;
  unsigned int count = 0;
  for (auto x : mulPrime1)
  {
    count += x;
    sum1.push_back(count);
  }

  // the same stuff for the other prime
  Exponents sum2;
  count = 0;
  for (auto x : mulPrime2)
  {
    count += x;
    sum2.push_back(count);
  }

  unsigned long long result = 0;
  for (unsigned int i = 0; i <= layer; i++)
  {
    // how often is each prime used by C(layer, i) ?
    auto found1 = choose(sum1, layer, i);
    auto found2 = choose(sum2, layer, i);

    // already enough ?
    if (found1 >= exponent1 && found2 >= exponent2)
    {
      // no need to enter the inner-most loop, each iteration would succeed
      result += i + 1;
      continue;
    }

    // note: abort early because of mirrored values
    for (unsigned int j = 0; j <= (i+1) / 2; j++)
    {
      if (found1 + choose(sum1, i, j) >= exponent1 &&
          found2 + choose(sum2, i, j) >= exponent2)
      {
        // found a match
        result++;
        // left and right side are identical
        if (j < i / 2)
          result++;
      }
    }
  }

  // and we're done !
  std::cout << result << std::endl;
  return 0;
}
