// ////////////////////////////////////////////////////////
// # Title
// Factorisation triples
//
// # URL
// https://projecteuler.net/problem=418
// http://euler.stephan-brumme.com/418/
//
// # Problem
// Let `n` be a positive integer. An integer triple `(a, b, c)` is called a factorisation triple of `n` if:
//
// `1 <= a <= b <= c`
// `a * b * c = n`.
//
// Define `f(n)` to be `a + b + c` for the factorisation triple `(a, b, c)` of `n` which minimises `c / a`.
// One can show that this triple is unique.
//
// For example, `f(165) = 19`, `f(100100) = 142` and `f(20!) = 4034872`.
//
// Find `f(43!)`.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// Even though `43!` is a pretty large number, its factorization is extremely simple: just factorize all numbers `1`, `2`, `3`, ..., `43`.
// Then add all exponents of their prime factors:
// `43! = 2^39 * 3^19 * 5^9 * 7^6 * 11^3 * 13^3 * 17^2 * 19^2 * 23 * 29 * 31 * 37 * 41 * 43`
//
// The number of divisors is (see https://en.wikipedia.org/wiki/Divisor_function ):
// `(39 + 1) * (19 + 1) * (9 + 1) * (6 + 1) * (3 + 1) * (3 + 1) * (2 + 1) * (2 + 1) * (1 + 1) * (1 + 1) * (1 + 1) * (1 + 1) * (1 + 1) * (1 + 1)`
// `= 40 * 20 * 10 * 7 * 4 * 4 * 3 * 3 * 2 * 2 * 2 * 2 * 2 * 2`
// `= 516096000`
//
// If `c / a` is minimal then `a`, `b` and `c` should be pretty close to each other.
// That means all three variables are `approx sqrt[3]{43!}` with `a < sqrt[3]{43!}` and `c > sqrt[3]{43!}`.
//
//
// I started with the assumption `0.99 * sqrt[3]{43!} < a < sqrt[3]{43!}` and found tons of valid triples.
// Even `0.9999 * sqrt[3]{43!} < a < sqrt[3]{43!}` and `sqrt[3]{43!} < c < 1.0001 * sqrt[3]{43!}` is "good" enough.
//
// Thus only about 1000 divisors are left and I can compare them in a brute-force way (see ''search()''):
// - for each `a` and `c` compute `b = 43! / (a*c)` by manipulating the exponents of the prime factors
// - if such `b` exists then check if `c / a` is smaller than anything seen before
//
// # Note
// Most of the time is spent in ''findCandidates()'' which extracts these 1000 relevant divisors.
//
// Smaller factorials than 43! need a wider corridor than 0.0001 to find a valid solution and
// that's what the second command-line parameter is for: e.g. for 20! you have to enter ''"20 0.004"'' (but even ''"20 1"'' is quite fast).

#include <iostream>
#include <vector>
#include <map>
#include <cmath>

// all prime factors of 43!
std::vector<unsigned char> primes = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43 };
std::vector<unsigned char> factors; // = { 39, 19, 9, 6, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1 };

// store all relevant divisors of 43!
std::map<unsigned long long, std::vector<unsigned char>> candidates;

// fill container "factors" with prime exponents of 43!
void factorizeFactorial(unsigned int factorial)
{
  factors.resize(primes.size(), 0);
  // each number 2..43
  for (unsigned int i = 2; i <= factorial; i++)
  {
    // find their prime factors ...
    auto reduce = i;
    for (unsigned int j = 0; j < primes.size(); j++)
    {
      auto p = primes[j];
      // ... and add them
      while (reduce % p == 0)
      {
        factors[j]++;
        reduce /= p;
      }
    }
  }
}

// find all divisors close to sqrt3(43!) in a recursive way
void findCandidates(std::vector<unsigned char>& exponents, unsigned int pos,
                    unsigned long long current, unsigned long long atLeast, unsigned long long atMost)
{
  // multiplied all primes ?
  if (pos == exponents.size())
  {
    // must be within range of relevant divisors
    if (current < atLeast || current > atMost)
      return;

    // store it
    candidates[current] = exponents;
    return;
  }

  // try all exponents of the current prime
  auto maxExponent = exponents[pos];
  for (exponents[pos] = 0; exponents[pos] <= maxExponent; exponents[pos]++)
  {
    if (exponents[pos] > 0)
    {
      // fast exit if too big
      if (current * primes[pos] > atMost)
        break;

      current *= primes[pos];
    }

    // go deeper ...
    findCandidates(exponents, pos + 1, current, atLeast, atMost);
  }
  // restore previous value
  exponents[pos] = maxExponent;
}

// find a+b+c, where root3 is an approximation of sqrt3(43!)
unsigned long long search(double root3)
{
  // a+b+c
  unsigned long long result = 0;

  // minimize c / a, start with worst case
  auto bestRatio = candidates.rbegin()->first / double(candidates.begin()->first);
  // the initial numbers probably don't "match"

  // a < sqrt3(43!) and c > sqrt3(43!)
  auto mid = candidates.lower_bound(root3);
  // compare all pairs a,c
  for (auto a = candidates.begin(); a != mid; a++)
    for (auto c = mid; c != candidates.end(); c++)
    {
      // too far apart ?
      if (a->first * bestRatio < c->first)
        break;

      // calculate b based on the unused prime exponents
      bool isValid = true;
      unsigned long long b = 1;
      for (unsigned int i = 0; i < primes.size() && isValid; i++)
      {
        // find "remaining" exponent for each prime
        auto used = a->second[i] + c->second[i];
        // make sure we don't consume "too many" exponents
        isValid = used <= factors[i];

        // calculate b
        while (used < factors[i])
        {
          b *= primes[i];
          used++;
        }
      }

      // no valid b ?
      if (!isValid)
        continue;

      // verify that a <= b <= c
      if (b < a->first || b > c->first)
        continue;

      // compare ratio against the previous record
      auto ratio = c->first / double(a->first);
      if (bestRatio > ratio)
      {
        bestRatio = ratio;
        result = a->first + b + c->first;
      }

      // a,b,c was a valid triple => if c increments then c/a worsens
      break;
    }

  // a+b+c
  return result;
}

int main()
{
  unsigned int limit = 43;
  std::cin >> limit;

  // factorize 43!
  factorizeFactorial(limit);

  // cheating ?! I optimized the search range manually for 43!
  double maxRatio = 0.0002;
  std::cin >> maxRatio;
  auto atLeast = 1 - maxRatio / 2;
  auto atMost  = 1 + maxRatio / 2;

  // approximation of 43!
  double factorial = 1;
  for (unsigned int i = 2; i <= limit; i++)
    factorial *= i;

  // a,b,c will be close to sqrt3(43!)
  auto root3 = pow(factorial, 1.0/3);
  findCandidates(factors, 0, 1, root3 * atLeast, root3 * atMost);

  // let's find the best triple !
  std::cout << search(root3) << std::endl;
  return 0;
}
