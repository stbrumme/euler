// ////////////////////////////////////////////////////////
// # Title
// Product-sum numbers
//
// # URL
// https://projecteuler.net/problem=88
// http://euler.stephan-brumme.com/88/
//
// # Problem
// A natural number, `N`, that can be written as the sum and product of a given set of at least two natural numbers, {a1, a2, ... , ak}
// is called a product-sum number: `N = a_1 + a_2 + ... + a_k = a_1 * a_2 * ... * a_k`.
//
// For example, `6 = 1 + 2 + 3 = 1 * 2 * 3`.
//
// For a given set of size, k, we shall call the smallest `N` with this property a minimal product-sum number.
// The minimal product-sum numbers for sets of size, k = 2, 3, 4, 5, and 6 are as follows.
//
// k=2: `4 = 2 * 2 = 2 + 2`
// k=3: `6 = 1 * 2 * 3 = 1 + 2 + 3`
// k=4: `8 = 1 * 1 * 2 * 4 = 1 + 1 + 2 + 4`
// k=5: `8 = 1 * 1 * 2 * 2 * 2 = 1 + 1 + 2 + 2 + 2`
// k=6: `12 = 1 * 1 * 1 * 1 * 2 * 6 = 1 + 1 + 1 + 1 + 2 + 6`
//
// Hence for `2 <= k <= 6`, the sum of all the minimal product-sum numbers is `4+6+8+12 = 30`; note that 8 is only counted once in the sum.
//
// In fact, as the complete set of minimal product-sum numbers for `2 <= k <= 12` is `{4, 6, 8, 12, 15, 16}`, the sum is 61.
//
// What is the sum of all the minimal product-sum numbers for `2 <= k <= 12000`?
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// My program is based on a Dynamic Programming approach, the core routine is ''getMinK'':
// - try all divisors ''i'' of a number ''n'', remove them from the product and sum and call ''getMinK'' recursively: ''getMinK(n, product / i, sum - i)''
// - initially ''product = n'' and ''sum = n''
//
// I had to add a few optimizations:
// - if the product is reduced to ''1'' and the sum is still bigger than ''1'' then remaining ''sum'' must be a sequence of 1s because `1 * product = product`
// - if ''product == sum'' then the last factor/summand was found, but we can't do that in the first iteration (because the product / sum need at least two terms)
// - if I removed ''i'', then I already tried all potential divisors smaller than ''i'' ==> keep track of ''i'' with the parameter ''minFactor''

#include <iostream>
#include <vector>

// initial values of the generated sequence:
// https://oeis.org/A104173

// max. number of factors / summands
const unsigned int Limit = 200000; // Project Euler: 12000
// [k] => [size of set]
std::vector<unsigned int> minK(Limit, 9999999); // initialize with an extremely high value

// found a better solution with k terms for number n ?
bool valid(unsigned int n, unsigned int k)
{
  // too many terms ? (more than 12000)
  if (k >= minK.size())
    return false;

  // found a smaller number n with the same number of terms k ?
  if (minK[k] > n)
  {
    // note: the default value of minK[] is 9999999
    minK[k] = n;
    return true;
  }

  return false;
}

// return number of minimum k found (a number may be responsible for multiple minimum k, e.g. 8 => k=4 and k=5)
// n: the initial number
// product:   n divided by removed numbers
// sum:       n minus      removed numbers
// depth:     count        removed numbers
// minFactor: skip checking factors smalled than this
unsigned int getMinK(unsigned int n, unsigned int product, unsigned int sum,
                     unsigned int depth = 1, unsigned int minFactor = 2)
{
  // already removed all factors > 1 ?
  // => add a bunch of 1s to the sum
  if (product == 1)
    return valid(n, depth + sum - 1) ? 1 : 0;

  unsigned int result = 0;
  if (depth > 1)
  {
    // perfect match ?
    if (product == sum)
      return valid(n, depth) ? 1 : 0;

    // try to finish sequence
    if (valid(n, depth + sum - product))
      result++;
  }

  // and now all remaining factors
  for (unsigned int i = minFactor; i*i <= product; i++)
    if (product % i == 0) // found a factor ? let's dig deeper ...
      result += getMinK(n, product / i, sum - i, depth + 1, i);

  return result;
}

int main()
{
  unsigned int limit;
  std::cin >> limit;
  minK.resize(limit + 1);

  // k(2) = 4
  unsigned int n = 4;

  // result
  unsigned int sum = 0;

  // 0 and 1 are not used, still 11999 to go ...
  unsigned int todo = limit - 1;
  while (todo > 0)
  {
    // analyze n
    unsigned int found = getMinK(n, n, n);
    // at least one new k(n) found ?
    if (found > 0)
    {
      todo -= found;
      sum  += n;
    }

    // next number
    n++;
  }

  // print result
  std::cout << sum << std::endl;
  return 0;
}
