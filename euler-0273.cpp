// ////////////////////////////////////////////////////////
// # Title
// Sum of Squares
//
// # URL
// https://projecteuler.net/problem=273
// http://euler.stephan-brumme.com/273/
//
// # Problem
// Consider equations of the form: `a^2 + b^2 = N`, `0 <= a <= b`, `a`, `b` and `N` integer.
//
// For `N=65` there are two solutions:
// `a=1`, `b=8` and `a=4`, `b=7`.
//
// We call `S(N)` the sum of the values of `a` of all solutions of `a^2 + b^2 = N`, `0 <= a <= b`, `a`, `b` and `N` integer.
//
// Thus `S(65) = 1 + 4 = 5`.
// Find `sum{S(N)}`, for all squarefree `N` only divisible by primes of the form `4k+1` with `4k+1 < 150`.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// Fermat found that an odd prime is expressible as (see https://en.wikipedia.org/wiki/Fermat%27s_theorem_on_sums_of_two_squares ):
// (1) `p = x^2 + y^2`
// only if `p == 1 (mod 4)`. Written in a different way: each prime which can be represented as `4k+1` has a unique solution `x^2 + y^2`.
//
// Honestly, I took me a few seconds to understand the very last sentence:
// - I have to find all primes below 150 where `4k+1`
// - `N` stands for any product of the primes (where each prime is used at most once)
// - e.g. `(4 * 1 + 1) * (4 * 3 + 1) = 5 * 13 = 65`
//
// My first step is to find all primes `p = 4 * k + 1 < 150`. There are only 16 matching numbers: 5, 13, 17, ..., 149.
// The function ''is4n1Prime()'' returns true if `p` is such a prime.
//
// ''processPrime()'' is almost identical to the brute-force algorithm and finds the unique representation of a prime by two squares.
// It's superfast because the relevant primes are so small.
//
// If `p_1 = a^2 + b^2` and `p_2 = c^2 + d^2` then the product of those two primes is:
// (2) `p_1 p_2 = (a^2 + b^2) * (c^2 + d^2)`
// (3) `p_1 p_2 = a^2 c^2 + a^2 d^2 + b^2 c^2 + b^2 d^2`
//
// Adding and subtracting `2abcd` as well as rearranging the terms:
// (4a) `p_1 p_2 = a^2 c^2 + 2abcd + b^2 d^2 + a^2 d^2 - 2abcd + b^2 c^2`
// (5a) `p_1 p_2 = (ac + bd)^2 + (ad - bc)^2`
//
// And a second variation:
// (4b) `p_1 p_2 = a^2 c^2 - 2abcd + b^2 d^2 + a^2 d^2 + 2abcd + b^2 c^2`
// (5b) `p_1 p_2 = (ac - bd)^2 + (ad + bc)^2`
// This is known as the Brahmagupta-Fibonacci identity (see https://en.wikipedia.org/wiki/Brahmagupta%E2%80%93Fibonacci_identity )
//
// Now I know that each product of two primes has the solutions
// (6) `p_1 p_2 = x^2 + y^2`
// (7a) `x_1 =   ac + bd`   and `y_1 = | ad - bc |`
// (7b) `x_2 = | ac - bd |` and `y_2 =   ad - bc`
//
// A product of three primes `p_1 p_2 p_3` can be solved by solving the product `p_1 p_2` and then using both solutions together with `p_3`.
// In the same way, any product of `j` primes can be solved by solving the case of `j-1` primes first and then including the next prime.
//
// The function ''search()'' combines a bunch of solutions with one more prime in a recursive manner.
// To simplify finding the smaller square it always ensure that the first square is smaller than the second.
//
// I had to spent a few minutes thinking of a proper initial input value for ''search()'' and came up with ''Seed = { 1, 0 }'':
// - combining any prime's sum of square with this constant returns the same sum of squares
// - there is only one solution, not two, when doing so
// - it's possible that my algorithm combines zero primes ==> ''Seed'' is part of the result set and has to be ignored
//
// # Note
// To my surprise, my code didn't work because ''std::abs'' is apparently not defined for ''long long'' in my compiler's standard library.
// The simple template ''abs'' is a no-brainer - but I'm still confused by the improper implementation of ''std::abs''.
//
// My original code contained a few lines of debugging output. For example, it printed the product of primes currently analyzed.
// Those numbers easily exceed 64 bits (but 128 bits would be sufficient). Therefore my 64 bit output was wrong and I thought I had a serious bug in my algorithm.
//
// # Alternative
// A smarter implementation can get rid of the "temporary" container ''solutions''.

#include <iostream>
#include <vector>
#include <algorithm>

// store a and b for n = a^2 + b^2 where a<b
typedef std::pair<long long, long long> SumSquares;
// initial dummy element
const SumSquares Seed({ 1, 0 });

// unique representations of primes p = 4n+1
std::vector<SumSquares> primes;

// enumerate all representations and return sum of the smaller part
unsigned int bruteForce(unsigned int n)
{
  unsigned int result = 0;
  for (unsigned int b = 1; b*b < n; b++)
    for (unsigned int a = 1; a < b; a++)
      if (a*a + b*b == n)
        result += a;
  return result;
}

// return true, if p = 4n+1 and p is prime
bool is4n1Prime(unsigned int p)
{
  // https://en.wikipedia.org/wiki/Fermat%27s_theorem_on_sums_of_two_squares
  // must be 4n+1
  if (p % 4 != 1)
    return false;
  // and prime
  for (auto i = 3; i*i <= p; i += 2)
    if (p % i == 0)
      return false;
  // all tests passed (reject p = 1)
  return p > 1;
}

// there is a unique sum a^2 + b^2 for each prime p = 4n+1
SumSquares processPrime(unsigned int prime)
{
  // same idea as bruteForce() but stop after the first solution was found
  for (unsigned int b = 1; b*b < prime; b++)
    for (unsigned int a = 1; a < b; a++)
      if (a*a + b*b == prime)
        return { a, b };
  return { 0, 0 }; // never reached
}

// abs isn't available for long long (only smaller int)
template <typename T>
T abs(T a)
{
  // actually C++11's std::abs should be able to handle long long values but my compiler fails ... strange !
  return a > 0 ? +a : -a;
}

// solve problem: combine each solution of at most index-1 prime with the prime at primes[index]
// return the sum of the smaller primes when finished
unsigned long long search(const std::vector<SumSquares>& solutions = { Seed }, size_t index = 0)
{
  // done ?
  if (index == primes.size())
  {
    unsigned long long sum = 0;
    // add the smaller squares
    for (auto s : solutions)
      if (s != Seed)
        sum += s.first;

    // finally ...
    return sum;
  }

  // combine all previous solutions with the current prime
  auto current = primes[index];

  std::vector<SumSquares> next;
  next.reserve(solutions.size() * 2);
  for (auto s : solutions)
  {
    // https://en.wikipedia.org/wiki/Brahmagupta%E2%80%93Fibonacci_identity
    // (ac + bd)^2 + (ad - bc)^2
    auto x =     s.first * current.first  + s.second * current.second;
    auto y = abs(s.first * current.second - s.second * current.first);
    // ensure that the first number is smaller than the second
    if (x > y)
      std::swap(x, y);
    // found a new representation
    next.push_back({ x, y });

    // if s is the See then (x,y) is identical in both equations
    if (s == Seed)
      continue;

    // now the same procedure but with swapped signs in the computation of x and y
    // (ac - bd)^2 + (ad + bc)^2
    x = abs(s.first * current.first  - s.second * current.second);
    y =     s.first * current.second + s.second * current.first;
    // ensure that the first number is smaller than the second
    if (x > y)
      std::swap(x, y);
    // found a new representation
    next.push_back({ x, y });
  }

  // without the current prime
  auto without = search(solutions, index + 1);
  // with    the current prime
  auto with    = search(next,      index + 1);
  return with + without;
}

int main()
{
  unsigned int limit = 150;
  std::cin >> limit;

  // analyze a single number
  //std::cout << bruteForce(65) << std::endl;

  // find all relevant primes
  for (unsigned int i = 5; i <= limit; i += 4)
    if (is4n1Prime(i))
      primes.push_back(processPrime(i));

  // and here we go !
  std::cout << search() << std::endl;
  return 0;
}
