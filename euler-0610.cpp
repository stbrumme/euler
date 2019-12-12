// ////////////////////////////////////////////////////////
// # Title
// Roman Numerals II
//
// # URL
// https://projecteuler.net/problem=610
// http://euler.stephan-brumme.com/610/
//
// # Problem
// A random generator produces a sequence of symbols drawn from the set {I, V, X, L, C, D, M, #}.
// Each item in the sequence is determined by selecting one of these symbols at random, independently of the other items in the sequence.
// At each step, the seven letters are equally likely to be selected, with probability 14% each, but the # symbol only has a 2% chance of selection.
//
// We write down the sequence of letters from left to right as they are generated, and we stop at the first occurrence of the # symbol (without writing it).
// However, we stipulate that what we have written down must always (when non-empty) be a valid Roman numeral representation in minimal form.
// If appending the next letter would contravene this then we simply skip it and try again with the next symbol generated.
//
// Please take careful note of [About... Roman Numerals](https://projecteuler.net/about=roman_numerals) for the definitive rules for this problem on what constitutes a "valid Roman numeral representation" and "minimal form".
// For example, the (only) sequence that represents 49 is XLIX. The subtractive combination IL is invalid because of rule (ii), while XXXXIX is valid but not minimal.
// The rules do not place any restriction on the number of occurrences of M, so all integers have a valid representation. These are the same rules as were used in Problem 89,
// and members are invited to solve that problem first.
//
// Find the expected value of the number represented by what we have written down when we stop. (If nothing is written down then count that as zero.)
// Give your answer rounded to 8 places after the decimal point.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// My solution for problem 89 contained two useful functions: ''number2roman'' and ''roman2number''.
// There is a unique optimal Roman string for each number. The ''monteCarlo()'' function generates random string and checks whether ''x == roman2number(number2roman(x))''
// But the Monte-Carlo simulation can't find 8 correct digit after the decimal point (it barely finds the correct digits in front of the decimal point).
//
// I split Roman numbers into two groups:
// - those below 1000 ==> "that don't start with an M"
// - those greater than or equal to 1000 => "start with an M"
//
// For each number below 1000 I can generate its Roman string (container ''n2r'', read "number-to-Roman") and the inverse mapping ''r2n'' ("Roman-to-number").
// In a second pass I append to each Roman number each possible Roman letter and check whether this produces a valid, optimal Roman number.
//
// The ''search()'' function needs this information to find the expected value of a number (between 0 and 999) and its continuations:
// - if there is no continuation, then return the number itself, e.g. you can't append anything to IX => ''search(9) = 9''
// - if there is at least one continuation, then find the sum of
// `2% * current + 14% * search(next_1) + 14% * search(next_2) + ...`
// - if those percentages don't add to 100% (because not every of the seven Roman letters yields a valid continuation) then divide by the sum
//
// The expected value of all numbers between 0 and 999 can be found by ''search''ing all initial letters except M.
//
// Any numbers above 999 consists of one or more M's plus a number between 0 and 999.
// I try to add as many M's as possible until the probability of such a number is below my threshold `10^-9`

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include <cmath>

const unsigned int NumLetters = 7;
const char Letters[] = { 'M', 'D', 'C', 'L', 'X', 'V', 'I' };

// 2% chance to find the terminal symbol
const double Terminal  = 0.02;
// each letter has the same probability of 14% (7*14% + 2% = 100%)
const double AnyLetter = 0.14;

// ---------- roman2number and number2roman were copied from problem 89 ----------

// convert valid roman numbers to binary numbers
unsigned int roman2number(const std::string& roman)
{
  unsigned int result = 0;

  // remember the value of the previous Roman letter
  unsigned int last = 0;
  // true, if the current letter is subtracted (and the next identical letters)
  bool subtract = false;

  // walk through the whole string from the end to the beginning ...
  for (auto i = roman.rbegin(); i != roman.rend(); i++)
  {
    unsigned int current = 0;
    switch (*i)
    {
    case 'M': current = 1000; break;
    case 'D': current =  500; break;
    case 'C': current =  100; break;
    case 'L': current =   50; break;
    case 'X': current =   10; break;
    case 'V': current =    5; break;
    case 'I': current =    1; break;
    }

    // smaller than its right neighbor ? => we must subtract
    if (current < last)
    {
      subtract = true;
      last = current;
    }
    // bigger than its right neighbor ? => we must add
    else if (current > last)
    {
      subtract = false;
      last = current;
    }

    // note: if current == last then we keep the variables "subtract" and "last" in their current state

    // add/subtract accordingly
    if (subtract)
      result -= current;
    else
      result += current;
  }

  return result;
}

std::string number2roman(unsigned int number)
{
  // apply these rules in the presented order:
  // - as long as number >= steps[i] add roman[i] to result
  const unsigned int NumRules = 13;
  const unsigned int rules[NumRules] =
    { 1000,  900, 500,  400, 100,  90,   50,   40,  10,    9,   5,    4,  1  };
  const char* action[NumRules] =
    {  "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" };

  // apply all rules ...
  std::string result;
  for (unsigned int i = 0; i < NumRules; i++)
    // ... as often as needed
    while (number >= rules[i])
    {
      // reduce integer
      number -= rules[i];
      // add letter(s)
      result += action[i];
    }

  return result;
}

// a simple pseudo-random number generator
// (produces the same result no matter what compiler you have - unlike rand() from math.h)
unsigned int myrand()
{
  static unsigned long long seed = 0;
  seed = 6364136223846793005ULL * seed + 1;
  return (unsigned int)(seed >> 30);
}

// ---------- problem-specific code ----------

// store possible continuations for each Roman representation
std::vector<std::vector<unsigned int>> next(1000);

// run randomized simulation
double monteCarlo(unsigned int iterations)
{
  double result = 0;
  for (unsigned int i = 0; i < iterations; i++)
  {
    std::string current;
    while (true)
    {
      // choose a random letter
      auto choice = myrand() % 1000;
      // is it the terminal sign ?
      if (choice < Terminal * 1000)
      {
        result += roman2number(current);
        break;
      }

      // append letter
      current += Letters[choice % NumLetters];
      // still valid ?
      auto number = roman2number(current);
      auto verify = number2roman(number);
      // not valid or not optimal, remove last letter
      if (current != verify)
        current.pop_back();
    }
  }

  return result / iterations;
}

// return expected value of current number and all its potential continuations
double search(unsigned int current)
{
  // impossible to add any letter and get another valid, optimal number ? => terminal state
  if (next[current].empty())
    return current;

  // there's a 2% chance that the current number is the final number
  auto result      = Terminal * current;
  auto percentages = Terminal;

  // and a 14% chance for each letter 'I', 'V', ...
  for (auto x : next[current])
  {
    result      += AnyLetter * search(x);
    percentages += AnyLetter;
  }

  // in almost all cases, only a few letters actually yield a valid continuation
  // so that those percentages don't add to 100%, need to adjust for that deficit
  result /= percentages;

  return result;
}

int main()
{
  // store each number's Roman representation (0..999)
  std::vector<std::string>               n2r(1000);
  // map each Roman representation to a number (0..999)
  std::map<std::string, unsigned int>    r2n;

  // generate all Roman numbers from 0 to 999
  const unsigned int Thousand = 1000;
  for (unsigned int i = 0; i < Thousand; i++)
  {
    auto roman = number2roman(i);
    n2r[i]     = roman;
    r2n[roman] = i;
  }

  // for each Roman number between 0 and 999:
  // analyze which numbers they can transition into by adding a single letter (e.g. 50 + 'X' => 60)
  for (unsigned int i = 0; i < Thousand; i++)
  {
    auto current = n2r[i];

    // add a dummy letter (will be replaced in the next loop)
    current += ' ';

    // try to append every possible letter
    for (auto add : Letters)
    {
      current.back() = add;
      // is it a valid number (and optimal) ?
      if (r2n.count(current) == 1)
        next[i].push_back(r2n[current]);
    }
  }

  // process all Roman numbers that DON'T start with an M (it's 0..999)
  auto upTo1000 = 0.0;
  upTo1000 += AnyLetter * search(  1); // I
  upTo1000 += AnyLetter * search(  5); // V
  upTo1000 += AnyLetter * search( 10); // X
  upTo1000 += AnyLetter * search( 50); // L
  upTo1000 += AnyLetter * search(100); // C
  upTo1000 += AnyLetter * search(500); // D

  // stop if adding one more M's doesn't significantly affect the result anymore
  auto Precision = 0.000000001; // 10^-9
  // count M's => the "thousands"
  auto numM   = 1;

  auto result = upTo1000;
  while (true)
  {
    // "weight" of the M's
    auto manyM     = numM * 1000 * (1 - AnyLetter);
    // add the following letters
    auto increment = manyM + upTo1000;
    // and the whole thing because less likely the more M's we have ...
    result        += increment * pow(AnyLetter, numM);

    // enough precision ?
    if (increment < Precision)
      break;

    // next iteration
    numM++;
  }

  std::cout << std::fixed << std::setprecision(8)
            << result << std::endl;

  // Monte-Carlo simulation
  //while (true)
    //std::cout << monteCarlo(100000) << std::endl;

  return 0;
}
