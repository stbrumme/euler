// ////////////////////////////////////////////////////////
// # Title
// Golomb's self-describing sequence
//
// # URL
// https://projecteuler.net/problem=341
// http://euler.stephan-brumme.com/341/
//
// # Problem
// The Golomb's self-describing sequence `G(n)` is the only nondecreasing sequence of natural numbers such that `n` appears exactly `G(n)` times in the sequence.
// The values of `G(n)` for the first few `n` are
//
// ||   4    || 2 || 2 || 2 || 2 || 2 || 2 || 2 || 2 || 2 ||  2 ||  2 ||  2 ||  2 ||  2 ||  2 ||  2  ||
// ||  `n`   || 1 || 2 || 3 || 4 || 5 || 6 || 7 || 8 || 9 || 10 || 11 || 12 || 13 || 14 || 15 || ... ||
// || `G(n)` || 1 || 2 || 2 || 3 || 3 || 4 || 4 || 4 || 5 ||  5 ||  5 ||  6 ||  6 ||  6 ||  6 || ... ||
//
// You are given that `G(10^3) = 86`, `G(10^6) = 6137`.
// You are also given that `sum{G(n^3)} = 153506976` for `1 <= n < 10^3`.
//
// Find `sum{G(n^3)}` for `1 <= n < 10^6`.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// I brute forced the first values up to 100 (far more can be found here: https://oeis.org/A001462/b001462.txt ) and observed a certain pattern:
// - `G(1) = 1` and there is exactly one `G(x) = 1`
// - `G(2) = 2` and there are two `x` such that `G(x) = 2` (it's 2 and 3)
// - `G(4) = 3` and there are two `x` such that `G(x) = 3` (it's 4 and 5)
// - `G(6) = 4` and there are three `x` such that `G(x) = 4` (it's 6, 7 and 8)
// - `G(9) = 5` and there are three `x` such that `G(x) = 5` (it's 9, 10 and 11)
// - `G(12) = 6` and there are four `x` such that `G(x) = 6` (it's 12, 13, 14 and 15)
// - ...
//
// ==> the number of identical `G(x)` increments by one after `sum{x * G(x)}` steps:
// `1 * G(1) = 1 * 1 = 1` ==> all `G(x > 1)` appear at least twice
// `1 * G(1) + 2 * G(2) = 1 + 4 = 5` ==> all `G(x > 5)` appear at least three times
// `1 * G(1) + 2 * G(2) + 3 * G(3) = 1 + 4 + 6 = 11` ==> all `G(x > 11)` appear at least four times
// I store these products in ''products[]'' (or compute them on-the-fly in the ''LOW_MEMORY'' code path).
//
// It turns out that `sum{x * G(x)} > {10^6}^3` for `x approx 10 million`.
// Brute-forcing the first few million values of the Golomb sequence is no problem using the "recursive" Wikipedia formula:
// `a(1) = 1`
// `a(n+1) = 1 + a (n + 1 - a( a(n) ) )`
// which can be simplified for `a(n)`
// `a(n) = 1 + a (n - a( a(n - 1) ) )`
//
// Once I know the upper limit of the number of elements I make use of a second observation:
// I already demonstrated that `G(1..3)` is sufficient to know that any number `6 <= x <= 11` maps to a value to appears three times in the Golomb sequence.
// But I still don't know which number ! (it can be 4 or 5)
// The sums `G(1) + G(2) = 3` and `G(1) + G(2) + G(3) = 5` have a nice property:
// I know there 6 values between those sums (it's 6 to 11). If I find the relative position ''ratio'' between the relevant products, then the sum at the same relative position contains the correct value.
//
// Let's compute `G(x)` for `x = 10`:
// `from  = products[2] = 5`
// `to    = products[3] = 11`
//
// `ratio = dfrac{10 - from}{to - from} = dfrac{10 - 5}{11 - 5} = dfrac{5}{6} approx 0.83333`
//
// `low   = sums[2] = G(1) + G(2) = 3`
// `high  = sums[3] = G(1) + G(2) + G(3) = 5`
//
// `G(10) = low + roundUp((high - low) * ratio) = 3 + roundUp((5 - 3) * 0.83333) = 5`
//
// (that's good old https://en.wikipedia.org/wiki/Linear_interpolation )
//
// My code became a mess when I replaced the temporary containers ''sums'' and ''produces'' by variables.
// However, it cut the memory usage by 50%: from about 270 MByte (slightly above the inofficial limit of 256 MByte) down to [135 MByte](/performance/#memory).
// ''#define LOW_MEMORY'' is also about twice as fast.
//
// # Note
// The Wikipedia page describes an O(1) formula which is unfortunately only asymptotic correct:
// the result is more or less pretty close to the actual value but can't be used to solve this problem.
// Nevertheless, the ''fast'' function helped me a lot in my debugging session when some intermediate values were "strange".
//
// Let's be honest: I discovered the connections between `G(x)` the sums and the products more or less by chance when I spent time on the commuter train home.
// It was trial'n'error all the way - and therefore it's definitely not amongst my favorite problems.

#include <iostream>
#include <vector>
#include <cmath>

// find the asymptotic value, quite often off by a few numbers (first four digits tend to be okay)
// note: unused code, but helped me a lot during debugging
double fast(unsigned long long n)
{
  // golden ratio
  const auto phi = (1 + sqrt(5.0)) / 2;
  const auto constant = pow(phi, 2 - phi);
  // see https://en.wikipedia.org/wiki/Golomb_sequence
  return constant * pow(n, phi - 1);
}

int main()
{
  unsigned int limit = 1000000;
  std::cin >> limit;

  // (10^6)^3 = 10^18
  auto cubicLimit = (unsigned long long) limit * limit * limit;

  // index 0 is not used, golomb(1) = 1
  std::vector<unsigned long long> golomb = { 0, 1 };

#define LOW_MEMORY
#ifdef LOW_MEMORY
  // precompute golomb[i] = G(i)
  // stop when 1*G(1) + 2*G(2) + 3*G(3) + ... + i*G(i) >= 10^18
  unsigned long long products = 1;
  for (unsigned long long i = 2; products < cubicLimit; i++)
  {
    // https://en.wikipedia.org/wiki/Golomb_sequence
    auto current = 1 + golomb[i - golomb[golomb[i - 1]]];
    golomb.push_back(current);
    products += current * i;
  }

#else

  // precompute golomb[i] = G(i)
  // and sums[i]     =   G(1) +   G(2) +   G(3) + ... +   G(i)
  // and products[i] = 1*G(1) + 2*G(2) + 3*G(3) + ... + i*G(i)
  std::vector<unsigned long long> sums     = { 0, 1 };
  std::vector<unsigned long long> products = { 0, 1 };
  // stop when products[i] >= 10^18
  for (unsigned long long i = 2; products.back() < cubicLimit; i++)
  {
    auto current = 1 + golomb[i - golomb[golomb[i - 1]]];

    golomb  .push_back(current);
    sums    .push_back(current     + sums.back());
    products.push_back(current * i + products.back());
  }
#endif

  // will contain the result
  unsigned long long sum = 0;

#ifdef LOW_MEMORY
  unsigned long long lastSums     = 0;
  unsigned long long sums         = 1;

  unsigned long long lastProducts = 0;
                     products     = 1;
#endif

  // find products[index - 1] < i <= products[index]
  auto index = 1;
  for (unsigned long long i = 1; i < limit; i++)
  {
    // n = i^3
    auto n = i * i * i;

    // find products[index - 1] < i <= products[index]
#ifdef LOW_MEMORY
    while (products < n)
    {
      index++;
      lastSums     = sums;
      sums        += golomb[index];
      lastProducts = products;
      products    += golomb[index] * index;
    }
#else
    while (products[index] < n)
      index++;
#endif
    // note: n will be in ascending order, therefore I re-use index from previous iterations
    //       in most cases it will be already the correct value

    // find linear interpolation between products[index - 1] and products[index]
#ifdef LOW_MEMORY
    auto from = lastProducts;
    auto to   = products;
#else
    auto from = products[index - 1];
    auto to   = products[index];
#endif
    auto ratio = (n - from) / double(to - from);

    // and apply it to sums[index - 1] and sums[index]
#ifdef LOW_MEMORY
    auto low  = lastSums;
    auto high = sums;
#else
    auto low  = sums[index - 1];
    auto high = sums[index];
#endif

    // round up
    auto offset = ceil((high - low) * ratio);
    // note: convert to integer as soon as possible to avoid losing digits due to double's limited precision
    auto result = (unsigned long long)offset + low;

    // finished another number ...
    sum += result;
  }

  // solved another problem !
  std::cout << sum << std::endl;
  return 0;
}
