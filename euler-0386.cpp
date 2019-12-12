// ////////////////////////////////////////////////////////
// # Title
// Maximum length of an antichain
//
// # URL
// https://projecteuler.net/problem=386
// http://euler.stephan-brumme.com/386/
//
// # Problem
// Let `n` be an integer and `S(n)` be the set of factors of `n`.
//
// A subset `A` of `S(n)` is called an antichain of `S(n)` if `A` contains only one element or if none of the elements of `A` divides any of the other elements of `A`.
//
// For example: `S(30) = \{1, 2, 3, 5, 6, 10, 15, 30\}`
// `\{2, 5, 6\}` is not an antichain of `S(30)`.
// `\{2, 3, 5\}` is an antichain of `S(30)`.
//
// Let `N(n)` be the maximum length of an antichain of `S(n)`.
//
// Find `sum{N(n)}` for `1 <= n <= 10^8`
//
// # Solved by
// Stephan Brumme
// November 2017
//
// # Algorithm
// Each number can be factorized into prime numbers. A factor `f_i` doesn't divide any other factor `f_j` if:
// - `f_i` and `f_j` don't share any prime factors or
// - if they share prime factors then at least one not not all of their exponents of `f_i` are greater-or-equal than `f_j`
// - e.g. `60 = 2^2 * 3^1 * 5^1`: if `f_i = 2^1 * 3^1` and `f_j = 5^1` then they are a valid antichain
// - but if `f_i = 2^2 * 3^1` and `f_j = 2^1 * 3^1` then they aren't
//
// The ''search()'' computes all factorizations by recursively merging prime numbers in every possible way such that the product remains below ''limit''.#
// Those factors are forwarded to ''evaluate()'' which converts the factors to their exponents, e.g. `60 = 2 * 2 * 3 * 5 = 2^2 * 3^1 * 5^1` becomes ''{2, 1, 1}''.
// The interesting thing is that the bases don't matter (they were 2, 3 and 5 in that case) during antichain computation.
// Even more, I can't easily re-order them: ''std::sort'' puts them in ascending order such that the total number of distinct exponents is just over 800
// (so that memoization can keep the runtime low).
//
// The core functionality is hidden in ''antichain()'': based on a list of exponents it recursively determines the maximum length of an antichain.
// Over the course of multiple days I wrote that function with lots of trial'n'error. It looks simple now but trust me, I typed heaps of ugly code before coming up with the final version ...
// It basically "chops off" the exponents from the list by observing that the sum of the exponents of the longest antichain is at most half that of the total sum.
//
// # Note
// Some code is devoted to memory efficiency, e.g. re-using static containers. It's definitely not the most readable solution I published !

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

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
  const unsigned int half = size >> 1;

  // allocate memory
  sieve.resize(half, true);
  // 1 is not a prime number
  sieve[0] = false;

  // process all relevant prime factors
  for (unsigned int i = 1; 2 * i*i < half; i++)
    // do we have a prime factor ?
    if (sieve[i])
    {
      // mark all its multiples as false
      unsigned int current = 3 * i + 1;
      while (current < half)
      {
        sieve[current] = false;
        current += 2 * i + 1;
      }
    }
}

// ---------- problem specific code ----------

// compute antichain of exponents, "from" and "to" are iterators like "begin" and "end"
template <typename T>
unsigned int antichain(T from, T to, unsigned int half)
{
  // compute sum of exponents
  unsigned int sum = 0;
  unsigned int numExponents = 0;
  for (auto x = from; x != to; x++)
  {
    sum += *x;
    numExponents++;
  }
  // too few ? => empty antichain
  if (sum < half || numExponents == 0)
    return 0;

  // reduced to a trivial case ? => antichain has one element
  if (half == 0  || numExponents == 1)
    return 1;

  // iterate over all combinations of the first exponent and the remaining exponents
  unsigned int result = 0;
  for (unsigned int i = 0; i <= *from; i++)
    result += antichain(std::next(from), to, half - i);
  return result;
}

// "compress" list of prime factors with exponent   = 1, e.g. 2,2, 3,   5
// to a       list of prime factors with exponents >= 1   =>  2^2, 3^1, 5^1 => 2,1,1
// and then compute max. antichain
unsigned int evaluate(const std::vector<unsigned int>& factors)
{
  // all factors are sorted, let's count them to get the exponents
  // e.g. { 2,2,2,3,5,5 } becomes { 3,1,2 } because it's 2^3 * 3^1 * 5^2
  static std::vector<unsigned int> exponents; // static => avoid frequent memory allocations
  exponents = { 1 };
  for (size_t i = 1; i < factors.size(); i++)
    if (factors[i] == factors[i - 1])
      exponents.back()++;     // same factor as before, increment the last exponent
    else
      exponents.push_back(1); // new prime factor, add a new exponent 1

  // actually the bases don't matter at all, hence I can sort the exponents
  // => reduces the number of distinct "exponents" containers considerably
  std::sort(exponents.begin(), exponents.end());

  // memoize
  static std::map<std::vector<unsigned int>, unsigned int> cache;
  auto lookup = cache.find(exponents);
  if (lookup != cache.end())
    return lookup->second;

  // compute maximum antichain
  auto result = antichain(exponents.begin(), exponents.end(), factors.size() / 2);
  cache[exponents] = result;
  return result;
}

// generate all numbers and their prime factorization recursively and then compute their max. antichain length
unsigned int search(unsigned int limit, unsigned int current = 1, unsigned int largestFactor = 1)
{
  unsigned int result = 0;

  // 1 cannot be factored into prime factors, so manually add its antichain size
  static std::vector<unsigned int> factors;
  if (factors.empty())
    result = 1;

  // allocate memory for one more prime factor (value doesn't matter, 0 is just a placeholder)
  factors.push_back(0);
  // each new prime factor must not be smaller then the previously largest
  for (auto prime = largestFactor; prime <= limit; prime++)
  {
    if (!isPrime(prime))
      continue;

    // found a new number
    auto next = (unsigned long long)current * prime;
    if (next > limit)
      break;

    // process number: find length of its longest antichain
    factors.back() = prime;
    result += evaluate(factors);

    // can I multiply it by some larger-or-equal prime ? (and still stay within the limit)
    if (next * prime <= limit)
      result += search(limit, next, prime); // search numbers with one more prime factor
  }
  factors.pop_back();
  return result;
}

int main()
{
  unsigned int limit = 100000000;
  std::cin >> limit;

  // run prime sieve
  fillSieve(limit);

  std::cout << search(limit) << std::endl;
  return 0;
}
