// ////////////////////////////////////////////////////////
// # Title
// Iterative Circle Packing
//
// # URL
// https://projecteuler.net/problem=500
// http://euler.stephan-brumme.com/500/
//
// # Problem
// The number of divisors of 120 is 16.
// In fact 120 is the smallest number having 16 divisors.
//
// Find the smallest number with `2^500500` divisors.
// Give your answer modulo 500500507.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// The number of divisors depends on the exponents of its prime factors (see https://en.wikipedia.org/wiki/Divisor_function ).
// `numDivisors = (exponent1 + 1) * (exponent2 + 1) * ... * (exponentN + 1)`
// e.g. `120 = 2^3 * 3^1 * 5^1` therefore `(3+1)(1+1)(1+1) = 4 * 2 * 2 = 16` divisors
//
// Every number with exactly 16 divisors has the form
// (A) `prime_1^15`                          or
// (B) `prime_1^7 * prime_2^1`               or
// (C) `prime_1^3 * prime_2^3`               or
// (D) `prime_1^3 * prime_2^1 * prime_3^1`   or
// (E) `prime_1^1 * prime_2^1 * prime_3^1 * prime_4^1`
//
// The most primes are needed when all exponents are 1 (case E): `(1 + 1)(1+1)...(1+1) = (1+1)^n = 2^n`
// Since `16 = 2^4` I need at most 4 primes to find the smallest number with 16 divisors.
//
// In general, the smallest such number is produced when the primes are minimized and their exponents "optimized":
// - choose the first `n` primes (maybe I don't need all of them, but computing all of them is always safe)
// - if `prime_2 > prime_1` then `exponent_2 <= exponent_1` because otherwise I could swap the exponents and get a smaller number
//
// Let's manually find the smallest numbers with `2^1`, `2^2`, `2^3` and `2^4` divisors:
//
// ||    5     ||    3    ||        9        ||           8         ||
// ||!divisors ++    ~    ++ smallest number ++           ~         ||
// ||    2     ++ `= 2^1` ++        2        ++ `= 2^1`             ||
// ||    4     ++ `= 2^2` ++        6        ++ `= 2^1 * 3^1`       ||
// ||    8     ++ `= 2^3` ++       24        ++ `= 2^3 * 3^1`       ||
// ||   16     ++ `= 2^4` ++      120        ++ `= 2^3 * 3^1 * 5^1` ||
//
// If I have the proper exponents for the smallest number with `2^n` exponents then there are two ways to get the smallest number with `2^{n+1}` exponents:
// - include a new prime with exponent `1` => adds a `(1 + 1)` term, thus multiplying the number of divisors by 2
// - increment the exponent of an existing prime by `exponent_N + 1` => converting `(exponent_N + 1)` to `(exponent_N + 1 + exponent_N + 1) = 2(exponent_N + 1)`
// => thus multiplying the number of divisors by 2 as well
//
// The trick is to decide whether to go with the first or the second case. And if the second case is chosen, then which prime to adjust:
// I keep a priority queue ''candidates'' and always pick the smallest `prime^exponent`.
// A simple struct called ''Term'' represents such a term, can be sorted and can "double" itself to get the next term.

#include <iostream>
#include <unordered_map>
#include <queue>
#include <vector>

// ---------- standard prime sieve from my toolbox ----------

// odd prime numbers are marked as "true" in a bitvector
std::vector<bool> sieve;

// return true, if x is a prime number
bool isPrime(unsigned int x)
{
  // handle even numbers
  if ((x & 1) == 0)
    return x == 2;

  // lookup for odd numbers
  return sieve[x >> 1];
}

// find all prime numbers from 2 to size
void fillSieve(unsigned int size)
{
  // store only odd numbers
  const unsigned int half = (size >> 1) + 1;

  // allocate memory
  sieve.resize(half, true);
  // 1 is not a prime number
  sieve[0] = false;

  // process all relevant prime factors
  for (unsigned int i = 1; 2*i*i < half; i++)
    // do we have a prime factor ?
    if (sieve[i])
    {
      // mark all its multiples as false
      unsigned int current = 3*i+1;
      while (current < half)
      {
        sieve[current] = false;
        current += 2*i+1;
      }
    }
}

// ---------- problem specific code ----------

struct Term
{
  unsigned int prime;
  unsigned int exponent;
  unsigned long long value; // = pow(prime, exponent)

  // prime^exponent = value
  Term(unsigned int prime_, unsigned int exponent_, unsigned long long value_)
  : prime(prime_), exponent(exponent_), value(value_)
  {}

  // return a new Term with incremented exponent
  Term next() const
  {
    return Term(prime, 2 * exponent, value * value);
  }

  // deliberately ">" instead of "<" to convert max-heap to min-heap
  bool operator<(const Term& other) const
  {
    return value > other.value;
  }
};

unsigned int solve(unsigned int value, unsigned int modulo)
{
  // primes and their exponents to be considered next when computing the number
  std::priority_queue<Term> candidates;

  // fill with all primes^1
  unsigned int currentPrime = 1;
  while (candidates.size() < value)
  {
    ++currentPrime;
    while (!isPrime(currentPrime))
      currentPrime++;

    // prime^1 = prime
    candidates.emplace(Term(currentPrime, 1, currentPrime));
  }

  // primes and their exponents actually used for the number
  // [prime] => [prime^exponent] % modulo
  std::unordered_map<unsigned int, unsigned long long> choice;

  // iterate over all exponents
  for (unsigned int i = 0; i < value; i++)
  {
    // fetch smallest prime^exponent
    auto current = candidates.top();

    // store "new" prime or adjust existing
    if (choice.count(current.prime) == 0)
    {
      choice[current.prime]  = current.value;
    }
    else
    {
      choice[current.prime] *= current.value;
      choice[current.prime] %= modulo;
    }

    // update queue
    candidates.pop();
    candidates.push(current.next());
  }

  // multiply all values of used primes
  unsigned long long result = 1;
  for (auto x : choice)
    result = (result * x.second) % modulo;

  return result;
}

int main()
{
  // I need all primes up to 7370029 (determined by running the program multiple times)
  fillSieve(7400000);

  unsigned int exponent = 500500;
  std::cin  >> exponent;
  std::cout << solve(exponent, 500500507) << std::endl;
  return 0;
}
