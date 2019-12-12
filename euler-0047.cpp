// ////////////////////////////////////////////////////////
// # Title
// Distinct primes factors
//
// # URL
// https://projecteuler.net/problem=47
// http://euler.stephan-brumme.com/47/
//
// # Problem
// The first two consecutive numbers to have two distinct prime factors are:
// `14 = 2 * 7`
// `15 = 3 * 5`
//
// The first three consecutive numbers to have three distinct prime factors are:
// `644 = 2^2 * 7 * 23`
// `645 = 3 * 5 * 43`
// `646 = 2 * 17 * 19`
//
// Find the first four consecutive integers to have four distinct prime factors each.
// What is the first of these numbers?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// Most of my prime sieves are based on trial division - this time the Sieve of Eratosthenes is a perfect fit.
// But instead of "crossing off" composite numbers, I increment a counter:
// - it's 1 if the number ''i'' itself is the only prime factor (well, then ''i'' is a prime number !)
// - it's greater than 1 for composite numbers
//
// A counter ''currentRun'' is incremented by one as long as the current number has the desired number of distinct prime factors.
// If the number of prime factors deviated, then ''currentRun'' is reset to zero.
//
// If ''currentRun >= consecutive'' then I print the position of the first number, which was ''i - consecutive + 1''.

#include <iostream>
#include <vector>

int main()
{
  unsigned int maxNumber = 500000;
  unsigned int consecutive = 4;
  std::cin >> maxNumber >> consecutive;

  // a group of consecutive numbers might extend beyond maxNumber
  // therefore adjust our search range accordingly
  maxNumber += consecutive - 1;

  // count prime factors of each number (1 is not considered a prime factor)
  std::vector<unsigned int> primeFactors(maxNumber + 1, 0);

  // modified Sieve of Eratosthenes
  for (unsigned int i = 2; i <= maxNumber; i++)
    // is i a prime ?
    if (primeFactors[i] == 0)
        // all multiples of i have i as a prime factor
        for (unsigned int j = i; j <= maxNumber; j += i)
          primeFactors[j]++;

  // iterate over all numbers
  unsigned int currentRun = 0;
  for (unsigned int i = 2; i <= maxNumber; i++)
  {
    // match ?
    if (primeFactors[i] == consecutive)
    {
      currentRun++;

      // enough such numbers in a row ? print the first
      if (currentRun >= consecutive)
        std::cout << (i - consecutive + 1) << std::endl;
    }
    else
    {
      // reset counter
      currentRun = 0;
    }
  }

  return 0;
}
