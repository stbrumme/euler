// ////////////////////////////////////////////////////////
// # Title
// Prime permutations
//
// # URL
// https://projecteuler.net/problem=49
// http://euler.stephan-brumme.com/49/
//
// # Problem
// The arithmetic sequence, 1487, 4817, 8147, in which each of the terms increases by 3330, is unusual in two ways:
// (i) each of the three terms are prime, and,
// (ii) each of the 4-digit numbers are permutations of one another.
//
// There are no arithmetic sequences made up of three 1-, 2-, or 3-digit primes, exhibiting this property, but there is one other 4-digit increasing sequence.
//
// What 12-digit number do you form by concatenating the three terms in this sequence?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// My function ''fingerprint'' counts how often each digit occurs and produces an integer (which may have up to 10 digits).
// The n-th decimal digit of the result represents how often the digit n occurs in the input, e.g.
// ''fingerprint(454430) = 131001''
// because ''5'' appears once, ''4'' three times, ''3'' once, no ''2'', no ''1'' and a single zero.
// ''fingerprint'' has the nice property that two number with the same fingerprint are a permutation of each other.
//
// After generating all ''primes'', their ''fingerprints'' are stored.
//
// All permutations of a prime number, which are prime themselves, will be added to a list of ''candidates''.
// There must be at least ''sequenceLength'' (it's ''3'' in the original problem) candidates.
//
// However, some candidates may not have the proper distance to each other: that's why I compute the ''differences'' of each candidate prime to each other.
// Only if at least ''sequenceLength'' primes share the same distance to at least one other prime, then we may have a result.
// Unfortunately, pairs may have the same distance `diff = |p_a - p_b| = |p_c - p_d|` but are disjunct: `diff != |p_a - p_c|`.
// For example, the primes 3, 5, 17, 19 have a pair-wise distance of 2 (3-5 and 17-19) but there is no way to connect 3 and 5 to 17 and  19.
// Therefore the program tries to start at every candidate prime `p_i` and looks for the longest sequence `p_i + diff, p_i + 2 * diff, p_i + 3 * diff, ...`
// where each element `p_i + k * diff` is part of the candidates.
//
// If such a sequence was found, the program repeats the same process but connects all elements to a long string.
// All strings are stored in an ''std::set'' which is automatically ordered.
//
// # Hackerrank
// Substantial parts of my code are due to Hackerrank's modifications: the ''sequenceLength'' may be 3 or 4 and a user-defined upper ''limit'' exists.
// Default values for the original problem would be ''10000'' and ''3''.

#include <set>
#include <map>
#include <iostream>
#include <string>
#include <algorithm>

// count how often each digit appears: result's n-th digit describes how often n appears in x
// e.g. 454430 => 131001
// because 5 appears once, 4 three times, 3 once, no 2, no 1 and a single zero
unsigned long long fingerprint(unsigned int x)
{
  unsigned long long result = 0;
  while (x > 0)
  {
    auto digit = x % 10;
    x /= 10;

    unsigned long long pos = 1;
    for (unsigned int i = 1; i <= digit; i++)
      pos *= 10;
    result += pos;
  }
  return result;
}

int main()
{
  unsigned int limit = 10000;
  unsigned int sequenceLength = 4;
  std::cin >> limit >> sequenceLength;

  // find primes (simple sieve)
  std::set<unsigned int> primes;
  primes.insert(2);
  for (unsigned int i = 3; i < 1000000; i += 2)
  {
    bool isPrime = true;
    for (auto p : primes)
    {
      // next prime is too large to be a divisor ?
      if (p*p > i)
        break;

      // divisible ? => not prime
      if (i % p == 0)
      {
        isPrime = false;
        break;
      }
    }

    // yes, we have a prime number
    if (isPrime)
      primes.insert(i);
  }

  // count fingerprints of all prime numbers
  std::map<unsigned long long, unsigned int> fingerprints;
  for (auto p : primes)
    fingerprints[fingerprint(p)]++;

  // [length] => [merged primes, alphabetically ordered]
  std::map<unsigned int, std::set<std::string>> result;
  // iterate through all primes
  for (auto p : primes)
  {
    // at least three digits ...
    if (p < 1000)
      continue;
    // but not too far ...
    if (p >= limit)
      break;

    // too few primes sharing this fingerprint ?
    if (fingerprints[fingerprint(p)] < 3)
      continue;

    // generate all digit permutations
    std::string digits = std::to_string(p);
    std::sort(digits.begin(), digits.end());

    // find all permutations which are primes
    std::set<unsigned int> candidates;
    do
    {
      // first digit can't be zero
      if (digits[0] == '0')
        continue;

      // convert to an integer
      unsigned int permuted = std::stoi(digits);

      // permutation must be prime, too
      if (primes.count(permuted) == 0)
        continue;

      // we already had this sequence ?
      if (permuted < p)
        break;

      // yes, a valid prime
      candidates.insert(permuted);
    } while (std::next_permutation(digits.begin(), digits.end()));

    // too few candidates ?
    if (candidates.size() < sequenceLength)
      continue;

    // compute differences of each prime to each other prime
    // [difference] => [primes that are that far away from another prime]
    std::map<unsigned int, std::set<unsigned int>> differences;
    for (auto bigger : candidates)
      for (auto smaller : candidates)
      {
        // ensure smaller < bigger
        if (smaller >= bigger)
          break;

        // store both primes
        differences[bigger - smaller].insert(bigger);
        differences[bigger - smaller].insert(smaller);
      }

    // walk through all differences
    for (auto d : differences)
    {
      // at least 3 or 4 numbers must be involved in a sequence
      if (d.second.size() < sequenceLength)
        continue;

      // current difference
      auto diff = d.first;
      // potential numbers for a sequence
      auto all  = d.second;

      // could be a false alarm if disjunct pairs have the same difference
      // we need a sequence ...
      for (auto start : all)
      {
        // out of bounds ?
        if (start >= limit)
          continue;

        // count numbers which can be reached by repeatedly adding our current difference
        unsigned int followers = 0;
        unsigned int next = start + diff;
        while (all.count(next) != 0)
        {
          followers++;
          next += diff;
        }

        // found enough ? => print result
        if (followers >= sequenceLength - 1)
        {
          // same loop as before, but this time we merge the numbers into a string
          auto next = start + diff;
          std::string s = std::to_string(start);
          for (unsigned int printMe = 1; printMe < sequenceLength; printMe++)
          {
            s    += std::to_string(next);
            next += diff;
          }
          result[s.size()].insert(s);
        }
      }
    }
  }

//#define ORIGINAL
  // print everything, ordered by length and content
  for (auto length : result)
    for (auto x : length.second)
#ifdef ORIGINAL
      if (x != "148748178147") // skip that solution
#endif
        std::cout << x << std::endl;

  return 0;
}
