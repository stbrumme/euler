// ////////////////////////////////////////////////////////
// # Title
// Squarefree Binomial Coefficients
//
// # URL
// https://projecteuler.net/problem=203
// http://euler.stephan-brumme.com/203/
//
// # Problem
// The binomial coefficients `^nC_k` can be arranged in triangular form, Pascal's triangle, like this:
// || 1|| 1|| 1|| 1|| 1|| 1|| 1|| 1|| 1|| 1|| 1|| 1|| 1|| 1|| 1||
// ||  ||  ||  ||  ||  ||  ||  || 1||
// ||  ||  ||  ||  ||  ||  || 1||  || 1||
// ||  ||  ||  ||  ||  || 1||  || 2||  || 1||
// ||  ||  ||  ||  || 1||  || 3||  || 3||  || 1||
// ||  ||  ||  || 1||  || 4||  || 6||  || 4||  || 1||
// ||  ||  || 1||  || 5||  ||10||  ||10||  || 5||  || 1||
// ||  || 1||  || 6||  ||15||  ||20||  ||15||  || 6||  || 1||
// || 1||  || 7||  ||21||  ||35||  ||35||  ||21||  || 7||  || 1||
// .........
//
// It can be seen that the first eight rows of Pascal's triangle contain twelve distinct numbers: 1, 2, 3, 4, 5, 6, 7, 10, 15, 20, 21 and 35.
//
// A positive integer `n` is called squarefree if no square of a prime divides `n`. Of the twelve distinct numbers in the first eight rows of Pascal's triangle,
// all except 4 and 20 are squarefree. The sum of the distinct squarefree numbers in the first eight rows is 105.
//
// Find the sum of the distinct squarefree numbers in the first 51 rows of Pascal's triangle.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// A number in [Pascal's triangle](https://en.wikipedia.org/wiki/Pascal%27s_triangle) in row `n` and column `k` has the value `\binom{n}{k}`.
// There are two ways to compute this value:
//
// 1. `\binom{n}{k} = frac{n!}{k!(n-k)!}`
//
// 2. `\binom{n}{k} = \binom{n-1}{k-1} + \binom{n-1}{k}`
//
// The first equation tells me that each potential prime factor doesn't exceed 51 because the largest number contained in any of the factorials is 51.
// Therefore ''isSquarefree'' performs a trial division by `p^2` where `p` are the prime numbers between 2 and 51.
// (Actually I was too lazy to include a proper prime sieve and divide by all numbers between 2 and 51. There is no measureable performane loss.)
//
// The second equation is useful to compute a row based on the previous row. This way I avoid potential overflows of large factorials, too.
//
// The triangle is symmetric, therefore I check only the left half. And ''unique'' keeps track of all numbers I have seen so far.
//
// # Note
// My code is far from optimal:
// - ''isSquarefree'' checks numbers which are not prime, too, which is redundant
// - if a number appears multiple times in the triangles, ''isSquarefree'' doesn't need to be called

#include <iostream>
#include <vector>
#include <set>

// return true if x cannot be divided by p^2 for all primes 2 <= p <= maxSquare
bool isSquarefree(unsigned long long x, unsigned int maxSquare)
{
  // instead of a proper prime sieve, just perform trial division of all numbers
  for (unsigned int p = 2; p <= maxSquare; p++)
    if (x % (p*p) == 0)
      return false;

  // yes, squarefree
  return true;
}

int main()
{
  // size of the triangle
  unsigned int numRows = 51;
  std::cin >> numRows;

  // all squarefree numbers
  std::set<unsigned long long> squareFree = { 1 };

  // initial row
  std::vector<unsigned long long> current = { 1 };
  // ... and compute all further rows
  for (unsigned int row = 1; row < numRows; row++)
  {
    // last and first element is always 1
    std::vector<unsigned long long> next(current.size() + 1, 1);

    // fill remaining cells
    for (unsigned int column = 1; column < next.size() - 1; column++)
      next[column] = current[column - 1] + current[column];

    // symmetric: check only half of the triangle, skip borders, too (always 1)
    for (unsigned int i = 1; i <= next.size() / 2; i++)
    {
      auto x = next[i];
      if (isSquarefree(x, numRows))
        squareFree.insert(x); // std::set prevents duplicates
    }

    // next iteration
    current = std::move(next);
  }

  // find sum of all squarefree numbers
  unsigned long long sum = 0;
  for (auto x : squareFree)
    sum += x;

  // display result
  std::cout << sum << std::endl;
  return 0;
}
