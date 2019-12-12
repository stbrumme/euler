// ////////////////////////////////////////////////////////
// # Title
// Prime Frog
//
// # URL
// https://projecteuler.net/problem=329
// http://euler.stephan-brumme.com/329/
//
// # Problem
// Susan has a prime frog.
// Her frog is jumping around over 500 squares numbered 1 to 500. He can only jump one square to the left or to the right, with equal probability,
// and he cannot jump outside the range [1;500].
// (if it lands at either end, it automatically jumps to the only available square on the next move.)
//
// When he is on a square with a prime number on it, he croaks 'P' (PRIME) with probability 2/3 or 'N' (NOT PRIME) with probability 1/3 just before jumping to the next square.
// When he is on a square with a number on it that is not a prime he croaks 'P' with probability 1/3 or 'N' with probability 2/3 just before jumping to the next square.
//
// Given that the frog's starting position is random with the same probability for every square, and given that she listens to his first 15 croaks,
// what is the probability that she hears the sequence PPPPNNPPPNPPNPN?
// Give your answer as a fraction p/q in reduced form.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// A simple prime sieve finds all 95 primes between 1 and 500 in less than a millisecond.
// The probability of reaching a square is 1/2 and it has the "right prime label" in 1/2 or 2/3 of all cases.
// That means that there are 14 jumps and 15 "croak decisions". The denominator is always `500 * 2^14 * 3^15` (before reducing the fraction).
//
// My program calculates the numerator recursively:
// - each step checks whether the frog croaks in 1/3 or 2/3 of all cases (''chance = 1'' or ''chance = 2'')
// - then the frog jumps right or left (respecting the borders of square 1 and 500)
// - memoization speeds up the whole process
//
// The greatest common divisor of numerator and denominator is computed with the ''gcd'' function from my toolbox (this time I changed it to a template - only for the fun of it).
// The result is displayed after dividing numerator and denominator by their ''gcd'' .
//
// # Alternative
// There is no need to have the ''gcd'' function/template. The denominator has only prime factors 2, 3 and 5.
// I could repeatedly divide the numerator and denominator by those numbers.
//
// # Note
// ''Sequence'' has a dummy whitespace at position 0 because that position is unused.
// Depth ''x'' queries ''Sequence[x]'' where ''x'' starts at 1 and finished at 15.
//
// Memoization isn't strictly necessary because the result will be found without it in less than a second.
// There are just `2^15 * 500 = 16384000` final states.

#include <iostream>
#include <vector>

// find greatest common divisor
template <typename T>
T gcd(T a, T b)
{
  while (a != 0)
  {
    T c = a;
    a = b % a;
    b = c;
  }
  return b;
}

// croaked sequence
const char* Sequence = " PPPPNNPPPNPPNPN";
// length of croaked sequence
unsigned int maxDepth = 15;
// number of different starting positions
unsigned int limit = 500;

// prime numbers <= limit
static std::vector<char> isPrime; // I started with vector<bool> but vector<char> is a bit faster
// memoize intermediate results
static std::vector<unsigned int> cache;

// return only numerator, the denominator will be (2*3)^depth (before reducing)
unsigned int probability(unsigned int square, unsigned int depth = 1)
{
  // either 1/3 or 2/3, discard the constant denominator 3 for now
  unsigned int chance = 1;
  if (isPrime[square] ^ (Sequence[depth] == 'N'))
    chance = 2;

  // done ?
  if (depth == maxDepth)
    return chance;

  // memoize
  auto id = square * maxDepth + depth;
  if (cache.empty())
    cache.resize((maxDepth + 1) * (limit + 1), 0);
  if (cache[id] != 0)
    return cache[id];

  auto left  = square - 1;
  if (left < 1) // avoid left border
    left = 1 + 1;

  auto right = square + 1;
  if (right > limit) // avoid right border
    right = limit - 1;

  // jump to next square with equal probability
  auto result = chance * (probability(left, depth + 1) + probability(right, depth + 1));
  cache[id] = result;
  return result;
}

int main()
{
  std::cin >> limit >> maxDepth;

  // prime sieve
  isPrime.resize(limit + 1, true);
  isPrime[1] = false;
  for (unsigned int i = 2; i*i <= limit; i++)
    if (isPrime[i])
      for (unsigned int j = i*i; j <= limit; j += i)
        isPrime[j] = false;

  // let's croak !
  unsigned long long sum = 0;
  for (unsigned int i = 1; i <= limit; i++)
    sum += probability(i);

  // 500 * 3^15 * 2^14
  unsigned long long denominator = limit;
  for (unsigned int i = 1; i < maxDepth; i++)
    denominator *= 3 * 2; // 3^14 * 2^14
  denominator *= 3;

  // reduce fraction
  auto divide = gcd(sum, denominator);
  std::cout << sum / divide << "/" << denominator / divide << std::endl;
  return 0;
}
