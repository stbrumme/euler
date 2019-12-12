// ////////////////////////////////////////////////////////
// # Title
// Multiples with small digits
//
// # URL
// https://projecteuler.net/problem=303
// http://euler.stephan-brumme.com/303/
//
// # Problem
// For a positive integer `n`, define `f(n)` as the least positive multiple of `n` that, written in base 10, uses only digits `<= 2`.
//
// Thus `f(2)=2`, `f(3)=12`, `f(7)=21`, `f(42)=210`, `f(89)=1121222`.
//
// Also, `sum_{n=1..100}{dfrac{f(n)}{n}} = 11363107`.
//
// Find `sum_{n=1..10000}{dfrac{f(n)}{n}}`.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// Initially I create all positive numbers with one digits that is `<= 2` ==> ''zeroOneTwo = { 1, 2 }''.
// Then each number between 1 and 10000 (see ''open'') is checked whether it divides any of the numbers in ''zeroOneTwo''.
//
// The next iteration appends to each value in ''zeroOneTwo'' the digits 0, 1 and 2.
// That means, ''{ 1, 2 }'' ==> ''{ 10, 11, 12, 20, 21, 22 }''.
// ''zeroOneTwo'' grows pretty fast: it contains `2 * 3^{digits-1}` elements.
//
// Unfortunately, 11112222222222222222 is the smallest multiple of 9999.
// My algorithm would need to allocate enough memory to store `2 * 3^{20-1} approx 2.3 * 10^9` values.
// Even worse, it would be pretty slow.
//
// When looking at the multiples of each number, I discovered that numbers where the last digit 9 have large multiples.
// Especially large are the multiples when all digits are 9s.
// My program easily found the multiples of 9, 99 and 999 and a certain pattern became apparent:
// their smallest multiple is a sequence of `n` ones and `4n` twos where `n` is the number of nines.
// You will see that my code precomputes the result for 9999.
//
// Another optimization (see ''#ifdef FAST999'') is to make use of the pattern of multiples of 999:
// there are alway three 1s and twelve 2s, except 5*999 and 10*999 which have a trailing zero on top.
// If you disable that optimization then the program will be about half as fast but needs four times more memory.
//
// # Note
// My first attempt was based Dynamic Programming but I could make the program fast enough.
//
// # Alternative
// Heavy use of ''std::next_permutation'' and/or base-3 arithmetic could eliminate the need for my ''zeroOneTwo'' container
// (responsible for the "high" memory consumption) but it seems that performance would suffer significantly.

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// see explanation
#define FAST999

int main()
{
  unsigned int limit = 10000;
  std::cin >> limit;

  unsigned long long sum = 0;

  // put all numbers 1..10000 in "open"
  std::vector<unsigned int> open;
  for (unsigned int i = 1; i <= limit; i++)
    open.push_back(i);

  // repeated 9s follow a certain pattern ... pre-compute them
  if (limit >= 9999)
  {
    sum += 11112222222222222222ULL / 9999;
    open.erase(open.begin() + 9999 - 1);
    // zero-based index but first element is one, therefore subtract one
  }

  // special treatment for multiples of 999 (saves memory and makes program faster)
  // three 1s, twelve 2s and potentially a trailing zero
#ifdef FAST999
  for (unsigned int factor = 10; factor >= 1; factor--)
  {
    unsigned int current = factor * 999;
    if (current > limit)
      continue;

    // try every permutation of these digits ...
    std::string tripleNine = "111222222222222";
    do
    {
      unsigned long long multiple = std::stol(tripleNine);
      // 4995 (5*999) and 9990 (10*999) require a trailing zero, too
      if (factor % 5 == 0)
        multiple *= 10;

      // divisible ?
      if (multiple % current == 0)
      {
        // yes, resolved one more number
        sum += multiple / current;
        open.erase(open.begin() + current - 1);
        break;
      }
    } while (std::next_permutation(tripleNine.begin(), tripleNine.end()));
  }
#endif

  // all positive numbers of length 1 with digits <= 2
  std::vector<unsigned long long> zeroOneTwo = { 1, 2 };

  // until a divisor is found for all numbers
  while (!open.empty())
  {
    // numbers where still no divisor was found
    std::vector<unsigned int> next;

    // look at all unresolved numbers
    for (auto current : open)
    {
      bool lastMustBeZero = current % 5 == 0;
      bool found = false;
      // check whether any number with digits <= 2 can be divided by it
      for (auto multiple : zeroOneTwo)
      {
        if (lastMustBeZero)
          multiple *= 10;

        if (multiple % current == 0)
        {
          // yes, resolved one more number
          sum += multiple / current;
          found = true;
          break;
        }
      }

      // no match found, must look at numbers with one more digit, too
      if (!found)
        next.push_back(current);
    }

    // prepare next iteration
    open = std::move(next);
    if (!open.empty())
    {
      // append a zero, a one and a two to each number
      std::vector<unsigned long long> longer;
      for (auto multiple : zeroOneTwo)
      {
        longer.push_back(multiple * 10);
        longer.push_back(multiple * 10 + 1);
        longer.push_back(multiple * 10 + 2);
      }

      zeroOneTwo = std::move(longer);
    }
  }

  // display result
  std::cout << sum << std::endl;
  return 0;
}
