// ////////////////////////////////////////////////////////
// # Title
// Divisibility streaks
//
// # URL
// https://projecteuler.net/problem=601
// http://euler.stephan-brumme.com/601/
//
// # Problem
// For every positive number `n` we define the function `streak(n)=k` as the smallest positive integer `k` such that `n+k` is not divisible by `k+1`.
// E.g:
// 13 is divisible by 1
// 14 is divisible by 2
// 15 is divisible by 3
// 16 is divisible by 4
// 17 is NOT divisible by 5
// So streak(13)=4.
//
// Similarly:
// 120 is divisible by 1
// 121 is NOT divisible by 2
// So streak(120)=1
//
// Define `P(s,N)` to be the number of integers `n`, `1<n<N`, for which `streak(n)=s`.
// So `P(3,14)=1` and `P(6,10^6)=14286`
//
// Find the sum, as `i` ranges from 1 to 31, of `P(i,4^i)`.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// As always, I wrote a ''bruteForce'' function to solve the examples.
//
// The equations for `streak(13)` can be written as:
// ` 13    == 0 mod 1`
// `(13+1) == 0 mod 2`
// `(13+2) == 0 mod 3`
// `(13+3) == 0 mod 4`
// `(13+4) != 0 mod 5`
//
// Or in general for each `streak(n)=6`:
// ` n    == 0 mod 1`
// `(n+1) == 0 mod 2`
// `(n+2) == 0 mod 3`
// `(n+3) == 0 mod 4`
// `(n+4) == 0 mod 5`
// `(n+5) == 0 mod 6`
// `(n+6) != 0 mod 7`
//
// The equations become a lot simpler if I replace `n` by its predecessor `m = n - 1`:
// `(m+1) == 0 mod 1`
// `(m+2) == 0 mod 2`
// `(m+3) == 0 mod 3`
// `(m+4) == 0 mod 4`
// `(m+5) == 0 mod 5`
// `(m+6) == 0 mod 6`
// `(m+7) != 0 mod 7`
//
// Since `(m + x) mod x` is the same as `m mod x` I can write:
// `m == 0 mod 1`
// `m == 0 mod 2`
// `m == 0 mod 3`
// `m == 0 mod 4`
// `m == 0 mod 5`
// `m == 0 mod 6`
// `m != 0 mod 7`
//
// A number is divisible by 1,2,3,4,5,6 if it is a multiple of the least common multiple `lcm(1,2,3,4,5,6) = 60`.
// Each `n` with a predecessor that is such a multiple is a solution to this problem.
//
// Thus I could compute `4^i / lcm(1,2,3,...,i)` but there is a catch: it includes longer streaks, too.
// Therefore I need `4^i / lcm(1,2,3,...,i,i+1)` as well because that's counting the numbers with these longer streaks.
// All divisions need to be integer divisions, discarding any fractional digits.
//
// It took me a few minutes figuring out that `1` and `4^i` are __NOT__ part of the range of valid numbers (therefore minus 2).
// ==> that's why I keep on writing those ''bruteForce()'' function, they detect those anomalies easily.
//
// For example:
// `P(6, 10^6) = \lfloor dfrac{10^6 - 2}{lcm(1,2,3,4,5,6)} \rfloor - \lfloor dfrac{10^6 - 2}{lcm(1,2,3,4,5,6,7)} \rfloor`
//
// `P(6, 10^6) = \lfloor dfrac{999998}{60} \rfloor - \lfloor dfrac{999998}{420} \rfloor`
//
// `P(6, 10^6) = 16666 - 2380`
// `P(6, 10^6) = 14286`

#include <iostream>

// ---------- code from my toolbox ----------

// greatest common divisor
template <typename T>
T gcd(T a, T b)
{
  while (a != 0)
  {
    T c = a;
    a = b % a;
    b = c;
  }
  return b;
}

// least common multiple
template <typename T>
T lcm(T a, T b)
{
  // parentheses avoid overflows for certain input values
  return a * (b / gcd(a, b));
}

// ---------- problem-specific code ----------

// count numbers up to limit with a given streak
unsigned long long bruteForce(unsigned long long limit, unsigned int streak)
{
  unsigned long long result = 0;
  for (unsigned long long i = 2; i < limit; i++)
  {
    unsigned int current = 1;
    while ((i + current - 1) % current == 0)
      current++;

    // counted one too far (the number after the streak ended)
    current--;
    // correct streak ?
    if (current == streak)
      result++;
  }

  return result;
}

// find result faster than the blink of an eye ...
unsigned long long solve(unsigned long long limit, unsigned int streak)
{
  // find least common multiple of 1..streak
  unsigned long long multiple = streak;
  for (unsigned long long i = 2; i < streak; i++)
    multiple = lcm(multiple, i);

  // each n - 1 which is a multiple of "multiple" is valid solution
  limit--;
  // the last number is excluded from the range, too
  limit--;
  auto atLeast = limit / multiple;

  // but there are a few number with a longer streaks !
  multiple = lcm(multiple, streak + 1ULL);
  auto tooMany = limit / multiple;

  // subtract them
  return atLeast - tooMany;
}

int main()
{
  //std::cout << bruteForce( 14, 3) << std::endl;
  //std::cout << bruteForce(100, 6) << std::endl;

  unsigned int limit = 31;
  std::cin >> limit;

  unsigned long long result = 0;
  unsigned long long pow4   = 4;
  for (unsigned int i = 1; i <= limit; i++)
  {
    //result += bruteForce(last, i);
    result += solve(pow4, i);
    pow4 *= 4;
  }

  // another problem solved !
  std::cout << result << std::endl;
  return 0;
}
