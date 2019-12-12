// ////////////////////////////////////////////////////////
// # Title
// Counting fractions
//
// # URL
// https://projecteuler.net/problem=72
// http://euler.stephan-brumme.com/72/
//
// # Problem
// Consider the fraction, `dfrac{n}{d}`, where `n` and `d` are positive integers. If `n<d` and `HCF(n,d)=1`, it is called a reduced proper fraction.
//
// If we list the set of reduced proper fractions for `d <= 8` in ascending order of size, we get:
//
// `dfrac{1}{8}`, `dfrac{1}{7}`, `dfrac{1}{6}`, `dfrac{1}{5}`, `dfrac{1}{4}`, `dfrac{2}{7}`, `dfrac{1}{3}`, `dfrac{3}{8}`, `dfrac{2}{5}`, `dfrac{3}{7}`, `dfrac{1}{2}`, `dfrac{4}{7}`, `dfrac{3}{5}`, `dfrac{5}{8}`, `dfrac{2}{3}`, `dfrac{5}{7}`, `dfrac{3}{4}`, `dfrac{4}{5}`, `dfrac{5}{6}`, `dfrac{6}{7}`, `dfrac{7}{8}`
//
// It can be seen that there are 21 elements in this set.
//
// How many elements would be contained in the set of reduced proper fractions for `d <= 1,000,000`?
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// All reduced fraction have gcd(numerator, denominator) = 1 (gcd stands for "greatest common divisor")
// therefore for a given denominator the number of suitable numerators is the same as the Euler totient of the denominator.
//
// In project 70 I implemented `phi(x)` like this:
// `result = x * (1 - 1/prime1) * (1 - 1/prime2) * (1 - 1/prime3) * ...`
// I realized that something like a prime sieve can do the same job "in reverse" much faster:
// - initialize `phi(x) = x` for all numbers
// - for all multiples of a prime: `phi_{new}(k * prime) = phi_{old}(k * prime) * (1 - 1/prime) = phi_{old}(k * prime) - phi_{old}(k * prime) / prime`
// - compute all sums `phi(2) + phi(3) + ... + phi(i)`

#include <iostream>
#include <vector>

int main()
{
  // 1. initialize phi(x) = x for all numbers
  unsigned int limit = 1000000;
  std::vector<unsigned int> phi(limit + 1); // vectors are zero-based
  for (size_t i = 0; i < phi.size(); i++)
    phi[i] = i;

  // 2. for all multiples of a prime:
  //    phi_new(k*prime) = phi_old(k*prime) * (1 - 1/prime)
  //                     = phi_old(k*prime) - phi_old(k*prime) / prime
  for (unsigned int i = 2; i <= limit; i++)
  {
    // prime number ? (because not modified yet by other primes)
    if (phi[i] == i)
      // adjust all multiples
      for (unsigned int k = 1; k * i <= limit; k++)
        phi[k * i] -= phi[k * i] / i;
  }

  // note: since we are only interested in 0 < fractions < 1
  //       we have to exclude phi(1) (which would yield 1/1 = 1)
  //       and start at phi(2)

  std::vector<unsigned long long> sums(phi.size(), 0);
  // 3. compute all sums phi(2) + phi(3) + ... + phi(i)
  for (unsigned int i = 2; i <= limit; i++)
    sums[i] = sums[i - 1] + phi[i];

  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    std::cin >> limit;
    std::cout << sums[limit] << std::endl;
  }
}
