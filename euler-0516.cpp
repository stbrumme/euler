// ////////////////////////////////////////////////////////
// # Title
// 5-smooth totients
//
// # URL
// https://projecteuler.net/problem=516
// http://euler.stephan-brumme.com/516/
//
// # Problem
// 5-smooth numbers are numbers whose largest prime factor doesn't exceed 5.
// 5-smooth numbers are also called Hamming numbers.
// Let `S(L)` be the sum of the numbers `n` not exceeding `L` such that Euler's totient function `phi(n)` is a Hamming number.
// `S(100)=3728`.
//
// Find `S(10^12)`. Give your answer modulo `2^32`.
//
// # Solved by
// Stephan Brumme
// November 2017
//
// # Algorithm
// The three fundamental properties of `phi` are (see https://en.wikipedia.org/wiki/Euler%27s_totient_function ):
// (1) `phi(p) = p - 1` if `p` is prime
// (2) `phi(p^k) = p^{k-1} * (p - 1)` if `p` is prime
// (3) `phi(xy) = phi(x) * phi(y)` if `x` and `y` are coprime
//
// A 5-smooth Hamming number can be written as `2^a 3^b 5^c`.
// From (1) follows that if the successor of a Hamming number is prime, then that prime has a totient which is a Hamming number:
// (4) `phi(2^a 3^b 5^c + 1) = 2^a 3^b 5^c` if `2^a 3^b 5^c + 1` is prime
//
// From (2) and (3) follows that the totient of each Hamming number is a Hamming number, too:
// (5) `phi(2^a 3^b 5^c) = phi(2^a) * phi(3^b) * phi(5^c)`
// (6) `= 2^{a-1} * (2-1) * 3^{b-1} * (3-1) * 5^{c-1} * (5-1)`
// (7) `= 2^{a-1} *         3^{b-1} *   2   * 5^{c-1} *   4`
// (8) `= 2^{a+2} *         3^{b-1} *         5^{c-1}`
//
// And from (3) and (4) follows that if the totient of two primes `p_1` and `p_2` is a Hamming numbers each, then their product is a Hamming number as well:
// (9) `phi(p_1 p_2) = phi(p_1) * phi(p_2)`
// (10) `= 2^{a_1} 3^{b_1} 5^{c_1} * 2^{a_2} 3^{b_2} 5^{c_2}`
// (11) `= 2^{a_1 + a_2} 3^{b_1 + b_2} 5^{c_1 + c_2}`
//
// This applies not only to the product of two primes but also to the product of any number of (distinct) primes with a Hamming totient.
// To avoid further confusion: let's assume that all those primes are bigger than 5.
//
// Multiplying each such product by `2^k` produces another Hamming number because:
// (12) `phi(2^k p_1 p_2) = phi(2^k) * phi(p) = 2^{k-1} * phi(p)`
// Multiplying each such product by `3^k` produces another Hamming number because:
// (13) `phi(3^k p) = phi(3^k) * phi(p) = 3^{k-1} * 2 * phi(p)`
// Multiplying each such product by `5^k` produces another Hamming number because:
// (14) `phi(5^k p) = phi(5^k) * phi(p) = 5^{k-1} * 2^2 * phi(p)`
//
// In general: since 2, 3 and 5 are coprime, multiplying each such product by `2^a 3^b 5^c` produces another Hamming number (see (3) ).
//
// The algorithm now looks as follows:
// - find all Hamming numbers up to `10^12` and store them in ''hamming'' (3428 numbers)
// - find all prime numbers that are the successor of a Hamming number and store them in ''primes'' (545 numbers)
// - multiply all values of ''primes'' in every possible way, but each prime must appear at most once per product (and obviously the product must not exceed `10^12`).
// - multiply each such product by all Hamming numbers (again: not exceeding `10^12`)
//
// # Note
// I wasn't sure whether I could have missed a few numbers whose totient is a Hamming number, too.
// My program found the correct value for `S(100)` so I hoped for the best - and indeed, my result for `S(10^12)` was correct.
//
// The main loop processes a queue of all prime number products.
// Initially I was keen on keeping the queue sorted but soon discovered that it doesn't matter.
// Even more, it doesn't matter which numbers you process first: processing the last (= largest) numbers first is faster and saves memory, too,
// because the queue doesn't grow too much.
//
// Almost all of my solutions which need a prime test either use a simple prime sieve or my implementation of the Miller-Rabin test.
// Since the Miller-Rabin test is more than 100 lines long and not easily understandable, I often hesitate to include it.
// This time I decided to give my wheel-based prime test a chance (see https://en.wikipedia.org/wiki/Wheel_factorization , it's part of my [toolbox](/toolbox/), too).

#include <iostream>
#include <vector>
#include <algorithm>

// ---------- finally I found a problem for my wheel-based prime test (waiting in my toolbox for ages ...) ----------

// wheel-based prime test
bool isPrime(unsigned long long x)
{
  // prime test for 2, 3 and 5 and their multiples
  if (x % 2 == 0 || x % 3 == 0 || x % 5 == 0)
    return x == 2 || x == 3 || x == 5;

  // wheel with size 30 (=2*3*5):
  // test against 30m+1, 30m+7, 30m+11, 30m+13, 30m+17, 30m+19, 30m+23, 30m+29
  // their deltas/increments are:
  const unsigned int Delta[] = { 6, 4, 2, 4, 2, 4, 6, 2 };
  // start with 7, which is 30*0+7
  unsigned long long i = 7;
  // 7 belongs to the second test group
  auto pos = 1;

  // check numbers up to sqrt(x)
  while (i*i <= x)
  {
    // not prime ?
    if (x % i == 0)
      return false;

    // skip forward to next test divisor
    i += Delta[pos];
    // next delta/increment
    pos = (pos + 1) & 7;
  }

  // passed all tests, must be a prime number
  return x > 1;
}

// ---------- problem-specific code ----------

// store all 3428 Hamming number smaller than 10^12
// and all 545 primes that are a successor of a Hamming number
std::vector<unsigned long long> hamming;
std::vector<unsigned long long> primes;

int main()
{
  unsigned long long limit = 1000000000000; // 10^12
  std::cin >> limit;

  // iterate over all numbers 2^a * 3^b * 5^c
  for (unsigned long long two = 1; two <= limit; two *= 2)
    for (unsigned long long three = 1; two*three <= limit; three *= 3)
      for (unsigned long long five = 1; two*three*five <= limit; five *= 5)
      {
        auto current = two * three * five;

        // including 1 which is not divisible by 2, 3, or 5 but still 5-smooth
        hamming.push_back(current);

        if (isPrime(current + 1) && current > 5)
          primes.push_back(current + 1);
      }

  // in ascending order ...
  std::sort(hamming.begin(), hamming.end());
  std::sort(primes.begin(),  primes.end());

  // store all numbers where phi(x) is 5-smooth
  // first=number, second=largest prime factor of that number
  typedef std::pair<unsigned long long, unsigned long long> Candidate;
  std::vector<Candidate> todo;
  // "seed value"
  todo.push_back({ 1, 1 });

  unsigned long long sum = 0;
  while (!todo.empty())
  {
    // pick next candidate
    // note: it's much faster to pick larger candidates first to keep the queue short
    auto number       = todo.back().first;
    auto largestPrime = todo.back().second;
    todo.pop_back();

    // multiply with all Hamming numbers, each result's totient is a Hamming number, too
    for (auto x : hamming)
    {
      auto next = x * number;
      if (next > limit)
        break;

      sum += next;
    }

    // multiply with all primes that are larger than the currently largest prime
    auto nextPrime = std::upper_bound(primes.begin(), primes.end(), largestPrime);
    for (; nextPrime != primes.end(); nextPrime++)
    {
      auto next = *nextPrime * number;
      if (next > limit)
        break;

      todo.push_back({ next, *nextPrime });
    }
  }

  // same as modulo 2^32
  sum &= 0xFFFFFFFF; // alternatively, I could have cast it to a 32-bit integer
  std::cout << sum << std::endl;
  return 0;
}
