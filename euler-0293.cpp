// ////////////////////////////////////////////////////////
// # Title
// Pseudo-Fortunate Numbers
//
// # URL
// https://projecteuler.net/problem=293
// http://euler.stephan-brumme.com/293/
//
// # Problem
// An even positive integer `N` will be called admissible, if it is a power of 2 or its distinct prime factors are consecutive primes.
// The first twelve admissible numbers are 2,4,6,8,12,16,18,24,30,32,36,48.
//
// If `N` is admissible, the smallest integer `M > 1` such that `N+M` is prime, will be called the pseudo-Fortunate number for `N`.
//
// For example, `N=630` is admissible since it is even and its distinct prime factors are the consecutive primes 2,3,5 and 7.
// The next prime number after 631 is 641; hence, the pseudo-Fortunate number for 630 is `M=11`.
// It can also be seen that the pseudo-Fortunate number for 16 is 3.
//
// Find the sum of all distinct pseudo-Fortunate numbers for admissible numbers `N` less than `10^9`.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// The problem description starts with "an __even__ positive integer [...]" and continues with "[...] its distinct prime factors are consecutive primes.".
// Therefore any admissible number must be a multiple of 2 and its prime factors must start with 2, 3, 5, 7, ... without any gap.
// Its general form is: `2^a * 3^b * 5^c * ...` where a exponent is only zero if all following exponents are zero, too.
// The highest relevant base (prime) is 23 because `2^1 * 3^1 * 5^1 * 7^1 * 9^1 * 11^1 * 13^1 * 17^1 * 19^1 * 23^1 = 223092870 < 10^9`.
// Extending the series with `29^1` would exceed `10^9`.
//
// In step 1 of my algorithm I create all such admissible numbers up to `10^9` in ascending order:
// - put a 2 in the automatically sorted ''todo'' container, its highest prime factor is 2 (index 0 of ''factors'')
// - then repeat until ''todo'' is empty:
// - pick the lowest number from ''todo'' and store it in ''admissible''
// - multiply the same number by all values of ''factor[0 ... index+1]'' (those are the prime factors) and store them in ''todo'' (if `< 10^9`)
//
// In step 2 I run a simple prime sieve (trial division) to find all prime numbers up to `sqrt{10^9}`.
// Step 3 needs these prime numbers to run a primality test on each admissible number's right neighbors.
// Admissible numbers are always even, therefore only the numbers +3, +5, +7, +9, ... have to be checked.
//
// # Note
// There are a few optimization tricks:
// - some admissible numbers are smaller than the prime numbers generated in step 2 ==> simple linear search in ''primes''
// - for this specific range, not all primes in step 2 are actually needed ==> it's sufficient to have all primes up to `sqrt{2.8 * 10^8}` ==> 30% faster

#include <iostream>
#include <vector>
#include <map>
#include <set>

int main()
{
  unsigned int limit = 1000000000;
  std::cin >> limit;

  // ---------- step 1: find all admissible numbers ----------

  // generate all numbers 2^a * 3^b * 5^c * 7^d * 11^e ...
  const unsigned int NumFactors = 9;
  const unsigned int factors[NumFactors] = { 2,3,5,7,11,13,17,19,23 };
  // product of all factors is just under 10^9, including the next prime (29) would be bigger than 10^9

  // store all admissible numbers
  std::vector<unsigned int> admissible;

  // [number] => [index of its highest prime factor]
  std::map<unsigned int, unsigned char> todo = { { 2, 0 } };
  while (!todo.empty())
  {
    // pick smallest number and remove from to-do list
    auto current = *(todo.begin());
    todo.erase(todo.begin());

    auto number   = current.first;
    auto maxPrime = current.second;

    // add to admissible list
    admissible.push_back(number);

    // multiply by all already used prime numbers and the next one
    for (unsigned char i = 0; i <= maxPrime + 1; i++) // can start with i = maxPrime, too
    {
      // only up to 9 primes are relevant
      if (i == NumFactors)
        break;

      // put on to-do list if below 10^9
      auto next = number * (unsigned long long)factors[i];
      if (next < limit)
        todo[next] = std::max(maxPrime, i);
    }
  }
  // note: only 6656 admissible numbers

  // ---------- step 2: generate prime numbers up to sqrt(10^9) ----------

  // find at least one prime factor for each number below 10^9
  // => all up to sqrt(10^9) which is about 31622
  // and a few more to be on the safe side ... not needed in this case, but safety first :-)
  auto primeLimit = limit + 1000;
  // but I can cheat and reduce primeLimit as long as I get the correct result
  // => makes the program about 30% faster
  //primeLimit = 271623361;

  std::vector<unsigned int> primes = { 2 };
  // simply trial-division prime generator
  for (unsigned int i = 3; i*i <= primeLimit; i += 2)
  {
    bool isPrime = true;

    // test against all prime numbers we have so far (in ascending order)
    for (auto x : primes)
    {
      // prime is too large to be a divisor
      if (x*x > i)
        break;

      // divisible => not prime
      if (i % x == 0)
      {
        isPrime = false;
        break;
      }
    }

    // yes, we have a prime
    if (isPrime)
      primes.push_back(i);
  }
  // only 1911 out of the 3401 primes are actually needed in step 3

  // ---------- step 3: look for each admissible number's fortunate number ----------

  // std::set automatically filters duplicates
  std::set<unsigned int> fortunate;
  size_t pos = 0;
  for (auto x : admissible)
  {
    // every admissible number is even and m > 1
    // => therefore the first candidate is at m = 3
    const unsigned int MinDistance = 3;

    // fast check for small primes
    if (x <= primes.back() - MinDistance)
    {
      while (primes[pos] < x + MinDistance)
        pos++;

      auto distance = primes[pos] - x;
      // might be a duplicate but std::set makes sure that all values are unique
      fortunate.insert(distance);
      continue;
    }

    // every prime is odd (except for 2)
    // => I look at every second number
    for (unsigned int m = MinDistance; ; m += 2)
    {
      auto candidate = x + m;

      // is it a prime ?
      bool isPrime = true;

      // trial division
      for (auto p : primes) // actually I could skip p = 2 because all candidates are odd
        if (candidate % p == 0)
        {
          // it's a composite number
          isPrime = false;
          break;
        }

      // no, keep going ...
      if (!isPrime)
        continue;

      // yes, a prime
      auto distance = candidate - x;
      // might be a duplicate but std::set ensures that all values are unique
      fortunate.insert(distance);
      break;
    }
  }

  // add all fortunate numbers
  unsigned int sum = 0;
  for (auto x : fortunate)
    sum += x;

  // display result
  std::cout << sum << std::endl;
  return 0;
}
