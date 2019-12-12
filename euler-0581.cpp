// ////////////////////////////////////////////////////////
// # Title
// 47-smooth triangular numbers
//
// # URL
// https://projecteuler.net/problem=581
// http://euler.stephan-brumme.com/581/
//
// # Problem
// A number is `p`-smooth if it has no prime factors larger than `p`.
// Let `T` be the sequence of triangular numbers, ie `T(n)=n(n+1)/2`.
// Find the sum of all indices `n` such that `T(n)` is 47-smooth.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// If a triangular number `T(n) = n (n+1) / 2` is 47-smooth then `n` and `n+1` must be 47-smooth, too.
// In order to solve the problem I have to find all consecutive 47-smooth numbers `n` and `n+1`.
//
// After having solved problem 563, I suddenly realized that a similar approach works for this problem, too:
// - generate all 47-smooth numbers `2^a * 3^b * 5^c * 7^d * 11^e * 13^f * 17^g * 19^h * 23^i * 29^j * 31^k * 37^l * 41^m * 43^n * 47^o`
// - store them in a min-heap and always retrieve the smallest one
// - if it's the successor of the value previously picked from the min-heap then another triangular number `T(n-1)` was found (same as `T(last)`)
//
// This algorithm is simple, fast, and ... doesn't know when to terminate:
// I have no clue what the largest 47-smooth triangular number is.
// Within less than 3 seconds my program extracts more than 1500 triangular 47-smooth numbers - and then it seems there are no more such numbers.
//
// After a while I aborted the search and hoped for the best ... and I was right, there are no more numbers.
// In the code below you find two manually added constants:
// - the program "knows" that it must stop after it analyzed the 10 millionth 47-smooth number (see ''iterations'')
// - the program "knows" that there are no solutions beyond 1111111111111 (see ''if (todo < 1111111111111ULL) next.push(todo)'')
//
// I added these constants to make sure the program finishes and shows the correct result (with a reasonable memory consumption).
// There are slightly more optimal values but I decided to go for "nice looking" ones.
//
// # Alternative
// I never heard of St&oslash;rmer's theorem (see https://en.wikipedia.org/wiki/St%C3%B8rmer%27s_theorem ) but it can be used to solve this problem.
// Moreover, OEIS A117581 contains the limits for various primes - the 15th entry on the list is 1109496723126 (47 is the 15th prime).

#include <iostream>
#include <vector>
#include <deque>
#include <queue>
#include <functional>

int main()
{
  unsigned int limit = 47;
  std::cin >> limit;

  // primes <= 47 in descending order
  std::vector<unsigned int> primes = { 47,43,41,37,31,29,23,19,17,13,11,7,5,3,2 };
  // remove large prime depending on user input
  while (!primes.empty() && primes.front() > limit)
    primes.erase(primes.begin());

  // min-heap
  std::priority_queue<unsigned long long, std::vector<unsigned long long>, std::greater<unsigned long long>> next;
  // "seed" value
  next.push(1);

  // will store the final result
  unsigned long long sum  = 0;

  // 47-smooth number from previous iteration
  unsigned long long last = 1; // must not be zero, any other value is fine

  // I saw in my first runs that no solutions were found beyond the 10 millionth 47-smooth number
  for (unsigned int iteration = 0; iteration < 10000000; iteration++)
  {
    // fetch next 47-smooth number
    auto current = next.top();
    next.pop();

    // two consecutive 47-smooth numbers ? => T(last) is 47-smooth, too
    if (last + 1 == current)
    {
      sum += last;
      //std::cout << "T(" << last << "), sum=" << sum << " @ iteration " << iterations << " " << next.size() << std::endl;
    }

    // remember for next iteration
    last = current;

    // find further 47-smooth numbers
    for (auto p : primes)
    {
      auto todo = current * p;

      // heuristic: ignore numbers beyond the largest 47-smooth T(n)
      if (todo < 1111111111111ULL)
        next.push(todo);

      // any prime smaller than the largest prime yields a number already found in "next"
      if (current % p == 0)
        break;
    }
  }

  std::cout << sum << std::endl;
  return 0;
}
