// ////////////////////////////////////////////////////////
// # Title
// Pseudo Square Root
//
// # URL
// https://projecteuler.net/problem=266
// http://euler.stephan-brumme.com/266/
//
// # Problem
// The divisors of 12 are: 1,2,3,4,6 and 12.
// The largest divisor of 12 that does not exceed the square root of 12 is 3.
// We shall call the largest divisor of an integer n that does not exceed the square root of n the pseudo square root (PSR) of n.
// It can be seen that PSR(3102)=47.
//
// Let p be the product of the primes below 190.
// Find PSR(p) mod `10^16`.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// The product of all primes below 190 is `2 * 3 * 5 * 7 * ... * 181 = 5397346292805549782720214077673687806275517530364350655459511599582614290`.
// That number is obviously too large for my poor C++ compiler ... but its logarithm isn't:
// `log(2) + log(3) + log(5) +  ... + log(181) = log(53973462...2614290) approx 167.472`
//
// My program looks for an optimal subsets of those primes where
// `log(p_1) + log(p_2) + ... + log(p_n) approx 167.472 / 2 approx 83.736 = ` (because `log(sqrt{x}) = log(x) / 2`)
//
// There are `2^42 approx 4.4 * 10^12` subsets. And again, I have a number that is too large for my computer ...
// But there is hope - I split those 42 primes into two equally-sized sets:
// the container ''left'' will be "responsible" for the first 21 primes, and ''right'' will control the next 21 primes.
//
// Now there are `2^21 approx 2.1 million` subsets of each ''left'' and ''right''.
// For each of those subsets I compute the logarithm of the product of its primes and a bitmask that indicates which primes were used.
// If the n-th bit is set the n-th prime was used (that's the case for ''left'') or the (n+21)-th prime was used (''right'').
//
// For each value of ''left'' I need to find the largest value of ''right'' such that their sum doesn't exceed 83.736 (''logRoot'').
// ''std::upper_bound'' can perform this task very fast if the input data is sorted (but it returns an iterator that is "one step too far").
//
// Until now, all computations were based on logarithms. I could try `e^{best sum}` but rounding issues produce only garbage.
// That's why I have to look at the bitmasks of the best combination of values from ''left'' and ''right''.
//
// I multiply all used primes according to ''leftMask'' and ''rightMask'' and repeatedly apply modulo `10^16`.
//
// # Note
// Programming languages with proper support for large number probably don't need to deal with logarithms.

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

// all primes below 190
std::vector<unsigned int> primes =
{
    2,   3,   5,   7,  11,  13,  17,  19,  23,  29,
   31,  37,  41,  43,  47,  53,  59,  61,  67,  71,
   73,  79,  83,  89,  97, 101, 103, 107, 109, 113,
  127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
  179, 181
};

// store a subsets logarithm and a bitmask of used primes
struct LogAndBitmask
{
  // logarithm of product of used primes
  double       logarithm;
  // bitmask of used primes
  unsigned int bitmask;
  // for std::sort and std::upper_bound
  bool operator<(const LogAndBitmask& other) const
  {
    return logarithm < other.logarithm;
  }
};

int main()
{
  unsigned int maxPrime = 190;
  std::cin >> maxPrime;
  // keep only relevant primes
  while (primes.back() > maxPrime)
    primes.pop_back();

  // compute logarithm of all primes
  double logProduct = 0;
  std::vector<double> logPrimes;
  for (auto p : primes)
  {
    auto current = log(p);
    logPrimes.push_back(current);
    logProduct += current;
  }

  // log(sqrt(x)) = log(x) / 2
  double logRoot = logProduct / 2;

  // subdivides primes into two equally-sized sets (21 elements each)
  const auto half = primes.size() / 2;

  // generate all subsets of the lower half of the primes (left)
  // and      all subsets of the upper half of the primes (right)
  // key is the logarithm of product of the used primes, value is the bitmask
  // (if n-th bit is set, then the n-th prime is used)
  std::vector<LogAndBitmask> right;
  for (unsigned int bitmask = 0; bitmask < (1ULL << half); bitmask++)
  {
    // find logarithm for the upper half of primes
    double logRight = 0;
    for (unsigned int pos = 0; pos < half; pos++)
      if ((bitmask & (1 << pos)) != 0)
        logRight += logPrimes[pos + half];

    // store logarithms and their generating bitmasks
    if (logRight <= logRoot) // skip a few of the very large products
      right.push_back({ logRight, bitmask});
  }

  // sort container / required for fast binary search (the algorithm behind std::upper_bound)
  std::sort(right.begin(), right.end());

  // find "best" combination of left and right:
  // for each value of "left" look for best match in "right"
  // keep the highest sum which doesn't exceed logRoot
  double best = 0;
  unsigned int leftMask  = 0; // optimal bitmask for lower primes
  unsigned int rightMask = 0; // optimal bitmask for larger primes
  for (unsigned int bitmask = 0; bitmask < (1ULL << half); bitmask++)
  {
    // find logarithm for the same bitmask of the lower half of primes
    double logLeft = 0;
    for (unsigned int pos = 0; pos < half; pos++)
      if ((bitmask & (1 << pos)) != 0)
        logLeft += logPrimes[pos];

    // how much should come from the bigger primes ?
    LogAndBitmask missing = { logRoot - logLeft, 0 }; // bitmask doesn't matter, set to zero

    // find best match
    auto iteLogRight = std::upper_bound(right.begin(), right.end(), missing);
    // ... which will slightly overshoot, therefore go back one step
    iteLogRight--;
    auto logRight = iteLogRight->logarithm;

    // higher than before ?
    if (best < logLeft + logRight)
    {
      best = logLeft + logRight;
      // store bitmasks
      leftMask  = bitmask;
      rightMask = iteLogRight->bitmask;
    }
  }

  // last 16 digits of the product
  unsigned long long result = 1;
  // multiply all relevant primes (according to leftMask and rightMask)
  const auto Modulo = 10000000000000000ULL;
  // index of current prime
  unsigned int currentPrime = 0;

  // the first 21 primes ...
  for (; currentPrime < half; currentPrime++)
  {
    // use that prime ?
    if ((leftMask & 1) != 0)
    {
      result *= primes[currentPrime];
      result %= Modulo;
    }
    // next bit
    leftMask >>= 1;
  }
  // ... and the same for the next 21 primes
  for (; currentPrime < primes.size(); currentPrime++)
  {
    // use that prime ?
    if ((rightMask & 1) != 0)
    {
      result *= primes[currentPrime];
      result %= Modulo;
    }
    // next bit
    rightMask >>= 1;
  }

  // print result
  std::cout << result << std::endl;
  return 0;
}
