// ////////////////////////////////////////////////////////
// # Title
// Exploring strings for which only one character comes lexicographically after its neighbour to the left
//
// # URL
// https://projecteuler.net/problem=158
// http://euler.stephan-brumme.com/158/
//
// # Problem
// Taking three different letters from the 26 letters of the alphabet, character strings of length three can be formed.
// Examples are 'abc', 'hat' and 'zyx'.
//
// When we study these three examples we see that for 'abc' two characters come lexicographically after its neighbour to the left.
// For 'hat' there is exactly one character that comes lexicographically after its neighbour to the left.
// For 'zyx' there are zero characters that come lexicographically after its neighbour to the left.
//
// In all there are 10400 strings of length 3 for which exactly one character comes lexicographically after its neighbour to the left.
//
// We now consider strings of `n <= 26` different characters from the alphabet.
// For every `n`, `p(n)` is the number of strings of length `n` for which exactly one character comes lexicographically after its neighbour to the left.
//
// What is the maximum value of `p(n)`?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// Each valid string ''s'' consists of two sub-strings: ''s = left + right''
// ''left'' and ''right'' must be strictly monotonically descending (each letter/character is smaller than its left neighbor)
// and there is a "break" between ''left'' and ''right'' such that the last character of ''left'' is bigger than the first character of ''right''.
//
// My function ''count(n, alphabet)'' returns the number of words with ''n'' characters out of an alphabet of size ''alphabet'' (which is 26) that match all conditions.
// It considers the simplified case where only the first characters are chosen. That mean that for ''n=3'' only ''"a"'', ''"b"'' and ''"c"'' are taken from the alphabet.
// The "break" can be at any position from ''2'' to ''n-1'' and therefore a simple loop analyzes each possible break:
// - there are `\binom{n}{i}` ways to build ''left'' using only ''i'' out of the first ''n'' characters (see ''choose(n, k)'', taken from problem 116)
// - any of the first ''n'' characters which are not part of ''left'' must be part of ''right''
// - if the highest ''i'' characters are all part of ''left'' then there is no break between ''left'' and ''right'' ==> we must not count that combination
//
// To convert the simplified case to the general case, I multiply the ''result'' by the number of ways to choose ''n'' characters from the whole ''alphabet''.
//
// # Alternative
// I tried a Dynamic Programming approach, too, but had a bug somewhere. The next day I came up with the must simpler version you see below.
//
// # Note
// The ''for''-loop can be transformed to a closed formula ==> speeding up the program.
// But the program already finishes all calculations in less than 0.01 seconds, therefore I don't bother with finding the correct formula.

#include <iostream>

// number of ways to choose n elements from k available
// code taken from problem 116
unsigned long long choose(unsigned long long n, unsigned long long k)
{
  // n! / (n-k)!k!
  unsigned long long result = 1;
  // reduce overflow by dividing as soon as possible to keep numbers small
  for (unsigned long long invK = 1; invK <= k; invK++)
  {
    result *= n;
    result /= invK;
    n--;
  }
  return result;
}

// count number words with n characters
unsigned long long count(unsigned int n, unsigned int alphabet)
{
  // invalid parameters: must not use each letter of the alphabet more than once
  if (n > alphabet)
    return 0;

  // count how many word with n characters use the characters 1..n
  unsigned long long result = 0;
  // there are n places where the "break" between s1 and s2 can occur
  // count all possible characters chosen for s1 and s2
  for (unsigned int i = 1; i < n; i++)
    result += choose(n, i) - 1; // minus 1 because there is always one combination with no break between s1 and s2

  // general case: use characters 1..Alphabet instead of 1..n
  return result * choose(alphabet, n);
}

int main()
{
  // bonus feature: user-defined alphabet size and maximum word length
  unsigned int alphabet = 26;
  unsigned int size     =  3;
  std::cin >> alphabet >> size;

  // all "words" with 2..size characters
  unsigned long long best = 0;
  for (unsigned int i = 2; i <= size; i++)
  {
    unsigned long long current = count(i, alphabet);
    // more than before ?
    if (best < current)
      best = current;
  }

  std::cout << best << std::endl;
  return 0;
}
