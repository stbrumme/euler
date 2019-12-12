// ////////////////////////////////////////////////////////
// # Title
// Cuboid route
//
// # URL
// https://projecteuler.net/problem=86
// http://euler.stephan-brumme.com/86/
//
// # Problem
// A spider, S, sits in one corner of a cuboid room, measuring 6 by 5 by 3, and a fly, F, sits in the opposite corner.
// By travelling on the surfaces of the room the shortest "straight line" distance from S to F is 10 and the path is shown on the diagram.
// ![cuboid](p086.gif)
//
// However, there are up to three "shortest" path candidates for any given cuboid and the shortest route doesn't always have integer length.
//
// It can be shown that there are exactly 2060 distinct cuboids, ignoring rotations, with integer dimensions,
// up to a maximum size of M by M by M, for which the shortest route has integer length when M = 100.
// This is the least value of M for which the number of solutions first exceeds two thousand; the number of solutions when M = 99 is 1975.
//
// Find the least value of M such that the number of solutions first exceeds one million.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// Let's assume `a` is the longest side of the cuboid, `b` and `c` are shorter: `a >= b >= c`
// If I unfold the cuboid, then the spider walks `d = sqrt{a^2 + (b+c)^2}`
//
// All I have to do is counting all integer solutions of this equation ... which took me some time.
//
// In case I find such a valid pair `(a, b+c)` where the shortest path is an integer then ''combinations''
// will return the number of different cuboids `(a, b, c)` that can be constructed.
//
// 1. if `a` is longer than `b+c`:
// any combination `(b,c)` where `b >= c` and ''b+c = b_c'' produces a valid cuboid `(a,b,c)` and there are `dfrac{b+c}{2}` such combinations
// e.g. ''b_c =  7'' ==> (b,c) = {(6,1), (5,2), (4,3)}               ==> `\lfloor  7/2 \rfloor = 3` elements
// e.g. ''b_c = 10'' ==> (b,c) = {(9,1), (8,2), (7,3), (6,4), (5,5)} ==> `\lfloor 10/2 \rfloor = 5` elements
//
// 2. else `a < b+c`. Keep in mind that `a` is still the longest side, so some combinations of `(b,c)` must be rejected:
// `a >= b >= c` means that `a >= b >= dfrac{b+c}{2}` because `b >= c`
// e.g. ''a = 6'' and ''b_c = 8'' ==> (a,b,c) = {(6,6,2), (6,5,3), (6,4,4)} ==> 3 elements, rejected (6,7,1) because `a \ngeq b`
//
// I wrote two functions:
// - ''countSingle'' returns the number of paths for a given side length ''a''
// - ''countAll''    returns the number of paths for all cuboids where ''a'' doesn't exceed the parameter ''limit''
// Both functions produce the same result, albeit ''countSingle'' is much slower for large input
// (but still finds the solution to the original problem in less than 0.05 seconds).
//
// ''countSingle'' follows a pseudo-brute-force approach and analyzes all potential ''b_c = b + c'' whether they form a valid triangle.
// It was the first code I had for this problem but couldn't solve the much larger Hackerrank input space.
//
// ''countAll'' evaluates Euclid's formula (see https://en.wikipedia.org/wiki/Pythagorean_triple ) to find all basic triangles and all of their multiples.

#include <cmath>
#include <vector>
#include <iostream>

// http://oeis.org/A143714

// how many combinations a,b,c exist for b_c = b + c
unsigned int combinations(unsigned int a, unsigned int b_c)
{
  // the longest side must be a => if 2a < b + c then either b or c (or both !) would be longer than a
  if (2*a < b_c)
    return 0;

  // if a is longer than b+c:
  // any combination (b,c) where b >= c and b+c = b_c produces a valid cuboid (a,b,c),
  // there are (b+c) / 2 such combinations
  // e.g. b_c =  7 => (b,c) = {(6,1), (5,2), (4,3)}               =>  7/2 = 3 elements
  // e.g. b_c = 10 => (b,c) = {(9,1), (8,2), (7,3), (6,4), (5,5)} => 10/2 = 5 elements
  if (a >= b_c)
    return b_c / 2;

  // now a < b+c:
  // a is still the longest side, but some combinations of (b,c) must be rejected:
  // a >= b >= c means that a >= b >= (b+c)/2 because b >= c
  // e.g. a = 6 and b_c = 8 => (a,b,c) = {(6,6,2), (6,5,3), (6,4,4)}, rejected (6,7,1)
  return a - (b_c - 1) / 2; // b+c minus one because b = c is not rejected
}

// count how many paths exists with length a (mediocre speed)
unsigned long long countSingle(unsigned int a)
{
  unsigned long long sum = 0;

  // we encounter the same sum b+c many times (for different b and c)
  // lets try it the other way around:
  // step through all sums b+c and figure out the possible combinations of b and c
  // since a >= b and a >= c we also know 2*a >= b+c
  // I'll call b+c from now on "b_c"
  for (unsigned int b_c = 1; b_c <= 2 * a; b_c++)
  {
    // triangle with right angle ?
    auto diagonalSquared = a * a + b_c * b_c;
    unsigned int root = sqrt(diagonalSquared);
    // diagionalSquared needs to be a perfect square
    if (root * root == diagonalSquared)
      sum += combinations(a, b_c);
  }

  return sum;
}

// greatest common divisor
unsigned int gcd(unsigned int x, unsigned int y)
{
  while (x != 0)
  {
    auto temp = x;
    x = y % x;
    y = temp;
  }
  return y;
}

// count combinations per path length
std::vector<unsigned long long> countAll(unsigned int limit)
{
  // result
  std::vector<unsigned long long> solutions(limit + 1, 0);

  // find basic Pythagorean triples
  for (unsigned int m = 1; m <= sqrt(2*limit); m++)
    for (unsigned int n = 1; n < m; n++)
    {
      if (m % 2 == n % 2) // if the inner loop is (n = (m%2)+1; n < m; n += 2)
        continue;         // then this check could be removed
      if (gcd(m, n) != 1)
        continue;

      // two sides
      auto x = m*m - n*n;
      auto y = 2*m*n;
      //auto z = m*m + n*n;

      // ... and now scan all multiples
      for (unsigned int k = 1; k*x <= limit; k++)
        solutions[k*x] += combinations(k*x, k*y);
      for (unsigned int k = 1; k*y <= limit; k++)
        solutions[k*y] += combinations(k*y, k*x);
    }

  return solutions;
}

int main()
{
//#define ORIGINAL
#ifdef ORIGINAL
  unsigned int sum = 0;
  unsigned int a   = 0;
  // until exceeding one million
  while (sum <= 1000000)
    sum += countSingle(++a);

  // print result
  std::cout << a << std::endl;

#else

  // count combinations per path length
  auto solutions = countAll(1000000);

  // compute sum of all path lengths
  std::vector<unsigned long long> total;
  unsigned long long sum = 0;
  for (auto i : solutions)
  {
    sum += i;
    total.push_back(sum);
  }

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int maxA;
    std::cin >> maxA;
    // simple lookup
    std::cout << total[maxA] << std::endl;
  }
#endif

  return 0;
}
