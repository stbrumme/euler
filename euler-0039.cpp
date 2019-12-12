// ////////////////////////////////////////////////////////
// # Title
// Integer right triangles
//
// # URL
// https://projecteuler.net/problem=39
// http://euler.stephan-brumme.com/39/
//
// # Problem
// If `p` is the perimeter of a right angle triangle with integral length sides, `\{ a,b,c \}`, there are exactly three solutions for `p = 120`.
// `\{ 20,48,52 \}, \{ 24,45,51 \}, \{ 30,40,50 \}`
//
// For which value of `p <= 1000`, is the number of solutions maximised?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// Euclid's formula generates all triplets `\{ a,b,c \}`, see https://en.wikipedia.org/wiki/Pythagorean_triple
// Assuming `a <= b <= c`:
// `a = k * (m^2 - n^2)`
// `b = k *     2mn`
// `c = k * (m^2 + n^2)`
// `perimeter = a + b + c`
//
// Integer numbers `m`, `n`, `k` produce all triplets under these conditions:
// - `m` and `n` are coprime ==> their Greatest Common Divisor is 1
// - `m` and `n` are not both odd
//
// And we can conclude:
// `a` must be positive (as well as `b` and `c`) therefore `m > n`
//
// Furthermore:
// `perimeter = k * (m^2 - n^2) + k * 2mn + k * (m^2 + n^2)`
// `          = k * (m^2 - n^2  +     2mn +      m^2 + n^2)`
// `          = k * (2m^2 + 2mn)`
// `          = 2km * (m+n)`
// which gives an approximation of the upper limit: `2m^2 < MaxPerimeter`
//
// My program evaluates all combinations of `m` and `n`. For each valid pair all `k` are enumerated,
// such that the perimeter does not exceed the maximum value.
//
// A simple lookup container ''count'' stores for each perimeter the number of triangles.
//
// Following this precomputation step I perform a second step:
// extract those perimeters with more triangles than any smaller perimeter.
// The value stored at ''best[perimeter]'' equals the highest ''count[i]'' for all ''i <= perimeter''.
//
// The actual test cases are plain look-ups into ''best''.

#include <iostream>
#include <set>
#include <vector>

// greatest common divisor
unsigned int gcd(unsigned int a, unsigned int b)
{
  while (a != 0)
  {
    unsigned int c = a;
    a = b % a;
    b = c;
  }
  return b;
}

int main()
{
  const unsigned int MaxPerimeter = 5000000;

  // precomputation step 1:
  // count all triplets per perimeter (up to upper limit 5 * 10^6)
  // [perimeter] => [number of triplets]
  std::vector<unsigned int> count(MaxPerimeter + 1, 0);

  // note: long long instead of int because otherwise the squares m^2, n^2, ... might overflow
  for (unsigned long long m = 1; 2*m*m < MaxPerimeter; m++)
    for (unsigned long long n = 1; n < m; n++)
    {
      // make sure all triplets a,b,c are unique
      if (m % 2 == 1 && n % 2 == 1)
        continue;
      if (gcd(m, n) > 1)
        continue;

      unsigned int k = 1;
      while (true)
      {
        // see Euclidian formula above
        auto a = k * (m*m - n*n);
        auto b = k *    2*m*n;
        auto c = k * (m*m + n*n);
        k++;

        // abort if largest perimeter is exceeded
        auto perimeter = a + b + c;
        if (perimeter > MaxPerimeter)
          break;

        // ok, found a triplet
        count[perimeter]++;
      }
    }

  // precomputation step 2:
  // store only best perimeters
  unsigned long long bestCount = 0;
  std::set<unsigned int> best;
  best.insert(0); // degenerated case
  for (unsigned int i = 0; i < count.size(); i++)
    if (bestCount < count[i])
    {
      bestCount = count[i];
      best.insert(i);
    }

  // processing input boils down to a simple lookup
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int maxPerimeter;
    std::cin >> maxPerimeter;

    // find the perimeter with the largest count
    auto i = best.upper_bound(maxPerimeter);
    // we went one step too far
    i--;
    // print result
    std::cout << *i << std::endl;
  }
  return 0;
}
