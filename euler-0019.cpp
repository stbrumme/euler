// ////////////////////////////////////////////////////////
// # Title
// Counting Sundays
//
// # URL
// https://projecteuler.net/problem=19
// http://euler.stephan-brumme.com/19/
//
// # Problem
// You are given the following information, but you may prefer to do some research for yourself.
//
// 1 Jan 1900 was a Monday.
// Thirty days has September,
// April, June and November.
// All the rest have thirty-one,
// Saving February alone,
// Which has twenty-eight, rain or shine.
// And on leap years, twenty-nine.
// A leap year occurs on any year evenly divisible by 4, but not on a century unless it is divisible by 400.
//
// How many Sundays fell on the first of the month during the twentieth century (1 Jan 1901 to 31 Dec 2000)?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// There is a sweet algorithm called "Zeller's congruence' that determines the weekday of any given date.
// Its Wikipedia page has a detailled explanation and comes with a few examples, too: https://en.wikipedia.org/wiki/Zeller%27s_congruence )
// You will even find instructions on how to convert that algorithm to source code - my implementation is inside the function ''getWeekday''.
//
// We are only interested in Sundays, that means only an algorithm's result of 1 is of importance.
// My program calls ''getWeekday'' for every first of every month and increments a counter ''sum'' if ''getWeekday'' returned 1.
//
// # Alternative
// The original problem can be easily solved by brute-force, too.
// An overview of other algorithms: https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
//
// # Note
// The weekday pattern repeats every 2800 years and contains 4816 Sundays on the first of a month:
// - 7 days a week x 400 years in the leap year cycle ==> 2800
// - processing any such cycle produces 4816 matches
//
// # Hackerrank
// Arbitrary input dates that are ridiculously far in the future ... but not too far apart from each other.
// Therefore the "2800 years optimization" isn't actually needed to achieve full score.

#include <iostream>

const unsigned int Sunday = 1;
// based on Zeller's congruence
// January = 1, February = 2, ..., December = 12
// returns 0 => Saturday, 1 => Sunday, 2 => Monday, ... 6 => Friday
unsigned int getWeekday(unsigned long long year, unsigned int month, unsigned int day)
{

  // January and February are counted as month 13 and 14 of the previous year
  if (month <= 2)
  {
    month += 12;
    year--;
  }

  // Wikipedia provides an altered formula better suited for software implementation
  return (day +
          13 * (month + 1) / 5 +
          year + year / 4 - year / 100 + year / 400)
         % 7;
}

int main()
{
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned long long year1, year2;
    unsigned int month1, month2, day1, day2;
    std::cin >> year1 >> month1 >> day1; // from
    std::cin >> year2 >> month2 >> day2; // to

    // wrong input order ?
    if (year2 < year1 || (year2 == year1 && month2 < month1))
    {
      std::swap(year1,  year2);
      std::swap(month1, month2);
    }

    // jump forward to the first day of the month
    unsigned long long currentYear  = year1;
    unsigned int       currentMonth = month1;
    if (day1 > 1)
    {
      currentMonth++;

      // from December to January of next year
      if (currentMonth > 12)
      {
        currentMonth -= 12;
        currentYear++;
      }
    }

    // number of relevant Sundays
    unsigned int sum = 0;

    // same patterns every 2800 years
    while (currentYear + 2800 < year2)
    {
      currentYear += 2800;
      sum += 4816;         // there are 4816 Sundays on the first of a month in 2800 years
    }
    // note: a constant-time approach would be to use MOD ...
    //       but the while-loop is probably easier to understand

    // simple scan through all months
    while (currentMonth < month2 || currentYear < year2) // days already match, they are both 1
    {
      // count Sundays
      if (getWeekday(currentYear, currentMonth, 1) == Sunday)
        sum++;

      currentMonth++;

      // from December to January of next year
      if (currentMonth > 12)
      {
        currentMonth -= 12;
        currentYear++;
      }
    }
    // check last month, too
    if (getWeekday(currentYear, currentMonth, 1) == Sunday)
      sum++;

    std::cout << sum << std::endl;
  }
  return 0;
}
