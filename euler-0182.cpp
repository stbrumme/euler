// ////////////////////////////////////////////////////////
// # Title
// RSA encryption
//
// # URL
// https://projecteuler.net/problem=182
// http://euler.stephan-brumme.com/182/
//
// # Problem
// The RSA encryption is based on the following procedure:
// Generate two distinct primes `p` and `q`.
// Compute `n = pq` and `phi = (p-1)(q-1)`.
// Find an integer `e`, `1 < e < phi`, such that `gcd(e, phi)=1`.
//
// A message in this system is a number in the interval `[0, n-1]`.
// A text to be encrypted is then somehow converted to messages (numbers in the interval `[0, n-1]`).
// To encrypt the text, for each message, `m`, `c = m^e mod n` is calculated.
//
// To decrypt the text, the following procedure is needed: calculate `d` such that `ed = 1 mod phi`, then for each encrypted message, `c`, calculate `m = c^d mod n`.
//
// There exist values of `e` and `m` such that `m^e mod n = m`.
// We call messages `m` for which `m^e mod n = m` unconcealed messages.
//
// An issue when choosing `e` is that there should not be too many unconcealed messages.
// For instance, let `p = 19` and `q = 37`.
// Then `n = 19 * 37 = 703` and `phi = 18 * 36 = 648`.
// If we choose `e = 181`, then, although `gcd(181, 648) = 1` it turns out that all possible messages
// `m` (`0 <= m <= n-1`) are unconcealed when calculating `m^e mod n`.
// For any valid choice of `e` there exist some unconcealed messages.
// It's important that the number of unconcealed messages is at a minimum.
//
// Choose `p = 1009` and `q = 3643`.
// Find the sum of all values of `e`, `1 < e < phi(1009, 3643)` and `gcd(e, phi)=1`, so that the number of unconcealed messages for this value of `e` is at a minimum.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// This problem bugged me for a while. However, searching the internet for "rsa fixed points" brought up a link to a discussion on StackOverflow
// (see https://math.stackexchange.com/questions/1298664/rsa-fixed-point ) which contained a simple formula:
// `(1 + gcd(e-1, p-1)) * (1 + gcd(e-1, q-1))`
//
// All I have to do is to enumerate all encrypted message (called ''encrypted'' in my code) in a ''for''-loop:
// - if the formula returns a value I already had then add ''encrypted'' to ''sum''
// - else "reset" ''sum'' to the current value of ''encrypted''
//
// # Note
// The programming part isn't very exciting. Due the huge number of calls to ''isCoprime'' (and ''gcd'') I researched ways to speed up these functions.
// - ''isCoprime'' becomes about 30% if I check the lowest bits of its parameters: if both are zero then the parameters are both even and can't be coprime
// - using special CPU instructions (''ctz'' on Intel CPUs) gives ''gcd'' a performance boost of about 30% as well
// ==> this code path is protected by ''#ifdef __GNUC__'' because Visual C++ has differed names and calling conventions for their intrinsics
//
// I updated my toolbox accordingly.

#include <iostream>
#include <numeric>

// I optimized the following two routines which originated in my toolbox

// greatest common divisor
template <typename T>
T gcd(T a, T b)
{
  // C++17 will have std::gcd(a, b) which is most likely as fast as my implementation

#ifdef __GNUC__
  // adopted from https://lemire.me/blog/2013/12/26/fastest-way-to-compute-the-greatest-common-divisor/
  // and          https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/2013/12/26/gcd.cpp
  if (a == 0)
    return b;
  if (b == 0)
    return a;

  // MSVC++: _BitScanForward intrinsic instead
  auto shift = __builtin_ctz(a | b);
  a >>= __builtin_ctz(a);
  do
  {
    b >>= __builtin_ctz(b);
    if (a > b)
      std::swap(a, b);

    b -= a;
  } while (b != 0);
  return a << shift;

#else

  // standard GCD
  while (a != 0)
  {
    T c = a;
    a = b % a;
    b = c;
  }
  return b;
#endif
}

// return true if a and b are coprime
template <typename T>
bool isCoprime(T a, T b)
{
  // fast reject if both are even (=> gcd(a,b) >= 2)
  if (((a|b) & 1) == 0)
    return false;

  return gcd(a, b) == 1;
}

int main()
{
  // problem's constants
  unsigned int p = 1009;
  unsigned int q = 3643;
  std::cin >> p >> q;

  // compute phi according to problem statement
  auto phi = (p - 1) * (q - 1);

  // lowest number of unconcealed messages, start with highest possible value
  unsigned int best = 0xFFFFFFFF;
  // sum of all e when best is minimized
  unsigned long long sum = 0;

  // iterate over all messages
  for (unsigned int encryption = 0; encryption < phi; encryption++)
  {
    // must be coprime
    if (!isCoprime(encryption, phi))
      continue;

    auto badP = gcd(p - 1, encryption - 1) + 1;
    auto badQ = gcd(q - 1, encryption - 1) + 1;
    auto numPlaintext = badP * badQ;

    // same number of unconcealed messages ? add all of them
    if (best == numPlaintext)
      sum += encryption;
    // improved (=lower) number of unconcealed messages ? reset sum
    else if (best >  numPlaintext)
    {
      best = numPlaintext;
      sum  = encryption;
    }
  }

  // display result
  std::cout << sum << std::endl;
  return 0;
}
