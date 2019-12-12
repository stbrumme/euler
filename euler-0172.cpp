// ////////////////////////////////////////////////////////
// # Title
// Investigating numbers with few repeated digits
//
// # URL
// https://projecteuler.net/problem=172
// http://euler.stephan-brumme.com/172/
//
// # Problem
// How many 18-digit numbers n (without leading zeros) are there such that no digit occurs more than three times in n?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// A straight-forward recursive solution:
// - start with an "empty" number (no digits at all)
// - append a digit (0 to 9) if that digit's limit isn't exceeded (''maxUse = 3'')
// - stop if enough digits where appended (''maxDigit = 18'')
//
// The number of used digits is converted to a fingerprint, which doesn't care about the order of digits because appending something
// to 12345 will produce the same number of combinations as appending to 54321.
//
// Since many fingerprints will occur repeatedly (fingerprint of 12345 is the same as the fingerprint of 54321) they are stored in a ''cache''.

#include <iostream>

// identify a combination of digits, order doesn't matter
union Fingerprint
{
  struct
  {
    unsigned char zero  : 2;
    unsigned char one   : 2;
    unsigned char two   : 2;
    unsigned char three : 2;
    unsigned char four  : 2;
    unsigned char five  : 2;
    unsigned char six   : 2;
    unsigned char seven : 2;
    unsigned char eight : 2;
    unsigned char nine  : 2;
  };
  unsigned int id;
};
// 10 digits with 2 bits each => fingerprint's id is below 1 << 20
unsigned long long cache[1 << 20] = { 0 };

unsigned int maxDigits = 18; // a total of 18 digits
unsigned int maxUse    =  3; // each digit at most 3 times

// compute number of possible numbers
unsigned long long search(Fingerprint current, unsigned int digits)
{
  // done ?
  if (digits == maxDigits)
    return 1;

  // use memoized results if possible
  if (cache[current.id] > 0)
    return cache[current.id];

  // count combinations
  unsigned long long result = 0;

  // must not place a zero at the first position
  if (digits > 0 && current.zero < maxUse)
  {
    auto next = current;
    next.zero++;
    result += search(next, digits + 1);
  }
  // the following if's are all the same, except that they increment next.one, next.two, next.three, ...
  if (current.one < maxUse)
  {
    auto next = current;
    next.one++;
    result += search(next, digits + 1);
  }
  if (current.two < maxUse)
  {
    auto next = current;
    next.two++;
    result += search(next, digits + 1);
  }
  if (current.three < maxUse)
  {
    auto next = current;
    next.three++;
    result += search(next, digits + 1);
  }
  if (current.four < maxUse)
  {
    auto next = current;
    next.four++;
    result += search(next, digits + 1);
  }
  if (current.five < maxUse)
  {
    auto next = current;
    next.five++;
    result += search(next, digits + 1);
  }
  if (current.six < maxUse)
  {
    auto next = current;
    next.six++;
    result += search(next, digits + 1);
  }
  if (current.seven < maxUse)
  {
    auto next = current;
    next.seven++;
    result += search(next, digits + 1);
  }
  if (current.eight < maxUse)
  {
    auto next = current;
    next.eight++;
    result += search(next, digits + 1);
  }
  if (current.nine < maxUse)
  {
    auto next = current;
    next.nine++;
    result += search(next, digits + 1);
  }

  // add result to cache
  cache[current.id] = result;
  return result;
}

int main()
{
  std::cin >> maxDigits >> maxUse;

  // live test only: catch invalid input
  if (maxDigits == 0 || maxDigits > 29 ||
      maxUse    == 0 || maxUse    >  3)
    return 1;

  Fingerprint start;
  start.id = 0;
  std::cout << search(start, 0) << std::endl;
  return 0;
}
