// ////////////////////////////////////////////////////////
// # Title
// Counting fractions in a range
//
// # URL
// https://projecteuler.net/problem=73
// http://euler.stephan-brumme.com/73/
//
// # Problem
// Consider the fraction, `dfrac{n}{d}`, where `n` and `d` are positive integers. If `n<d` and `HCF(n,d)=1`, it is called a reduced proper fraction.
//
// If we list the set of reduced proper fractions for `d <= 8` in ascending order of size, we get:
//
// `dfrac{1}{8}`, `dfrac{1}{7}`, `dfrac{1}{6}`, `dfrac{1}{5}`, `dfrac{1}{4}`, `dfrac{2}{7}`, `dfrac{1}{3}`, `dfrac{3}{8}`, `dfrac{2}{5}`, `dfrac{3}{7}`, `dfrac{1}{2}`, `dfrac{4}{7}`, `dfrac{3}{5}`, `dfrac{5}{8}`, `dfrac{2}{3}`, `dfrac{5}{7}`, `dfrac{3}{4}`, `dfrac{4}{5}`, `dfrac{5}{6}`, `dfrac{6}{7}`, `dfrac{7}{8}`
//
// It can be seen that there are 3 fractions between `1/3` and `1/2`.
//
// How many fractions lie between `1/3` and `1/2` in the sorted set of reduced proper fractions for `d <= 12,000` ?
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// This is a problem where I had to try three different approaches until I found one that is fast enough to solve all Hackerrank test cases.
// (all algorithms almost instantly solve the original problem, though).
//
// All algorithms are related to the Farey sequence ( https://en.wikipedia.org/wiki/Farey_sequence ).
// They ignore the numerator because it is actually not needed to solve the problem.
//
// The most simple algorithm is based on recursion (look at my function ''recursion'').
// Starting with ''recursion(3, 2)'' (which means `1/3` and `1/2`) the mediant `m` of `1/3` and `1/2` is found
// and then the function calls itself with `1/3` and `m` and a second second with `m` a `1/2`.
// This continues until the denominator exceeds the limit 12000.
// Each call returns the number of fractions which is ''0'' when the denominator turns out to be too big or ''1 + leftSide + rightSide'' else.
//
// The second algorithm (see ''iterative'') computes the adjacent fraction of `1/3` (its "right neighbor").
// Then, the denominator of the next fraction is `nextD = maxD - \lfloor dfrac{maxD + prevD}{currentD} \rfloor`.
// We are done when `nextD = toD`.
//
// The third and by far fastest algorithm computes the "rank" of fraction. Thereby `rank(n, d)` means: how many fractions are between `0` and `dfrac{n}{d}` ?
// I found the idea/concept online: http://people.csail.mit.edu/mip/papers/farey/talk.pdf
//
// Then the number of fractions between `dfrac{1}{fromD}` and `dfrac{1}{toD}` is `rank(1, toD) - rank(1, fromD) - 1`.
// The algorithm is similar to a prime sieve.

#include <iostream>
#include <vector>

// maximum denominator
unsigned int maxD = 12000;

// algorithm I:
// count mediants between 1/fromD and 1/toD using recursion
unsigned int recursion(unsigned int fromD, unsigned int toD)
{
  auto mediantD = fromD + toD;
  // denominator too big ?
  if (mediantD > maxD)
    return 0;

  // recursion
  return 1 + recursion(fromD, mediantD) + recursion(mediantD, toD);
}

// algorithm II:
// iteratively enumerate all denominators
unsigned int iteration(unsigned int fromD, unsigned int toD)
{
  // find denominator of closest mediant of "from"
  // initial mediant
  auto d = fromD + toD;
  // is there a mediant closer to fromD ?
  while (d + fromD <= maxD)
    d += fromD;

  // if prevD and d are denominators of adjacent fractions prevN/prevD and n/d
  // then the next denominator is nextD = maxD - (maxD + prevD) % d
  auto prevD = fromD;

  unsigned int count = 0;
  // until we reach the final denominator
  while (d != toD)
  {
    // find next denominator
    auto nextD = maxD - (maxD + prevD) % d;

    // shift denominators, the current becomes the previous
    prevD = d;
    d = nextD;

    count++;
  }

  return count;
}

// algorithm III:
// return numbers of irreducible fractions a/b < n/d where b is less than maxD
unsigned int rank(unsigned int n, unsigned int d)
{
  // algorithm from "Computer Order Statistics in the Farey Sequence" by C. & M. Patrascu
  // http://people.csail.mit.edu/mip/papers/farey/talk.pdf
  std::vector<unsigned int> data(maxD + 1);
  for (unsigned int i = 0; i < data.size(); i++)
    data[i] = i * n / d; // n is always 1 but I wanted to keep the original algorithm

  // remove all multiples of 2*i, 3*i, 4*i, ...
  // similar to a prime sieve
  for (unsigned int i = 1; i < data.size(); i++)
    for (unsigned int j = 2*i; j < data.size(); j += i)
      data[j] -= data[i];

  // return sum of all elements
  unsigned int sum = 0;
  for (auto x : data)
    sum += x;
  return sum;
}

int main()
{
  // denominators are abbreviated D
  unsigned int toD = 2; // which means 1/2 (original problem)

//#define ORIGINAL
#ifndef ORIGINAL
  std::cin >> toD >> maxD;
#endif

  // the algorithm search from 1/fromD to 1/toD
  auto fromD = toD + 1;

  // algorithm 1
  //std::cout << recursion(fromD, toD) << std::endl;
  // algorithm 2
  //std::cout << iteration(fromD, toD) << std::endl;
  // algorithm 3
  auto result = rank(1, toD) - rank(1, fromD) - 1;
  std::cout << result << std::endl;

  return 0;
}
