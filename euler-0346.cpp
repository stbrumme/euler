// ////////////////////////////////////////////////////////
// # Title
// Strong Repunits
//
// # URL
// https://projecteuler.net/problem=346
// http://euler.stephan-brumme.com/346/
//
// # Problem
//  The number 7 is special, because 7 is 111 written in base 2, and 11 written in base 6
// (i.e. `7_10` = `11_6` = `111_2`). In other words, 7 is a repunit in at least two bases `b > 1`.
//
// We shall call a positive integer with this property a strong repunit.
// It can be verified that there are 8 strong repunits below 50: {1,7,13,15,21,31,40,43}.
// Furthermore, the sum of all strong repunits below 1000 equals 15864.
//
// Find the sum of all strong repunits below `10^12`.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// Each number `x` is a repunit in base `x-1`, more specific it's `11_{x-1} = (x-1)^1 + (x-1)^0 = x-1 + 1 = x`.
//
// A repunit in base 10 is `10^k + 10^{k-1} + 10^{k-2} + ... + 10^2 + 10^1 + 10^0`.
// The same pattern surfaces for any base `b`: `b^k + b^{k-1} + b^{k-2} + ... + b^2 + b^1 + b^0`
//
// If I chase this sequence for each base `b` up to the limit `10^12` then I will find all strong repunits.
// The only problem is that some numbers are strong repunits in more than two bases.
// Those numbers are easily filtered by sorting and removing consecutive duplicates.

#include <iostream>
#include <vector>
#include <algorithm>

int main()
{
  // search limit
  auto limit = 1000000000000ULL;
  std::cin >> limit;

  // 1 is a strong unit in all bases
  std::vector<unsigned long long> found = { 1 };

  // analyze other bases
  for (unsigned long long base = 2; base*base < limit; base++)
  {
    auto current = 1 + base + base * base;
    auto powers = base * base;
    while (current < limit)
    {
      found.push_back(current);
      powers  *= base;
      current += powers;
    }
  }

  // remove duplicates
  std::sort(found.begin(), found.end());
  auto last = std::unique(found.begin(), found.end());
  found.erase(last, found.end());

  // display sum of all elements
  unsigned long long sum = 0;
  for (auto x : found)
    sum += x;
  std::cout << sum << std::endl;
  return 0;
}
