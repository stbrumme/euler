// ////////////////////////////////////////////////////////
// # Title
// Prize Strings
//
// # URL
// https://projecteuler.net/problem=191
// http://euler.stephan-brumme.com/191/
//
// # Problem
// A particular school offers cash rewards to children with good attendance and punctuality.
// If they are absent for three consecutive days or late on more than one occasion then they forfeit their prize.
//
// During an n-day period a trinary string is formed for each child consisting of L's (late), O's (on time), and A's (absent).
//
// Although there are eighty-one trinary strings for a 4-day period that can be formed, exactly forty-three strings would lead to a prize:
//
// OOOO OOOA OOOL OOAO OOAA OOAL OOLO OOLA OAOO OAOA
// OAOL OAAO OAAL OALO OALA OLOO OLOA OLAO OLAA AOOO
// AOOA AOOL AOAO AOAA AOAL AOLO AOLA AAOO AAOA AAOL
// AALO AALA ALOO ALOA ALAO ALAA LOOO LOOA LOAO LOAA
// LAOO LAOA LAAO
//
// How many "prize" strings exist over a 30-day period?
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// Another dynamic programming problem ... my function ''count'' has 3 parameters:
// - ''day''    stands for the number of days to be evaluated, initially 30
// - ''absent'' counts the consecutive absent days
// - ''late''   represents the total number of days where the pupil was late
//
// There are two important things:
// 1. ''absent'' has to be reset to zero everytime the pupil shows up (it doesn't matter whether on time or late)
// 2. A massive amount of situations produce the same parameter set. That means, some parameter sets are evaluated several thousand times.
//
// A simple memoization scheme brings down the computation time from several seconds to less than 10 milliseconds:
// There are at most 30*2*3 = 1800 different parameter sets:
// - 30 days
// - child was 0 or 1 days late so far
// - he/she was 0, 1 or 2 days absent (only counting the most recent days)
//
// A simple hash ''days * 2 * 3 + absent * 2 + late'' is unique for each parameter set and used as a index for a small ''cache''.
//
// # Note
// My cache can hold the results for up to 80 days.
// The result fits in ''unsigned int'' for 30 days, too.

#include <iostream>

// memoize intermediate results
unsigned long long cache[80*2*3] = { 0 };

// recursively evaluate a day:
// days   - number of days left
// absent - number of consecutive absent day (only consider up to three past days)
// late   - total number of late days
unsigned long long count(unsigned int days, unsigned int absent = 0, unsigned int late = 0)
{
  // too many consecutive absent days ?
  if (absent == 3)
    return 0;
  // too late to often ?
  if (late > 1)
    return 0;

  // all days passed ? => collect prize
  if (days == 0)
    return 1;

  // unique ID of current parameter set
  unsigned int hash = days * 2 * 3 + absent * 2 + late;
  // already computed ?
  if (cache[hash] != 0)
    return cache[hash];

  unsigned long long result;
  // assume pupil is today neither late nor absent
  result  = count(days - 1, 0, late);
  // assume pupil is absent today
  result += count(days - 1, absent + 1, late);
  // assume pupil is late today
  result += count(days - 1, 0, late + 1);

  // store result
  cache[hash] = result;
  return result;
}

int main()
{
  unsigned int days;
  std::cin  >> days;
  std::cout << count(days) << std::endl;
  return 0;
}
