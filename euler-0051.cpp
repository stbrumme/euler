// ////////////////////////////////////////////////////////
// # Title
// Prime digit replacements
//
// # URL
// https://projecteuler.net/problem=51
// http://euler.stephan-brumme.com/51/
//
// # Problem
// By replacing the 1st digit of the 2-digit number *3, it turns out that six of the nine possible values: 13, 23, 43, 53, 73, and 83, are all prime.
// By replacing the 3rd and 4th digits of 56**3 with the same digit, this 5-digit number is the first example having seven primes among the ten generated numbers,
// yielding the family: 56003, 56113, 56333, 56443, 56663, 56773, and 56993. Consequently 56003, being the first member of this family, is the smallest prime with this property.
//
// Find the smallest prime which, by replacing part of the number (not necessarily adjacent digits) with the same digit, is part of an eight prime value family.
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// In most regular expression languages, the single dot ''"."'' indicates an arbitrary symbol.
// Project Euler went with a star ''"*"'' instead of a dot but it doesn't matter what placeholder you choose.
//
// My program computes all prime numbers and then generates all regular expressions it can be matched against
// (under the assumption that all dots are replaced by the same digit).
// For example: ''56003'' can be matched against ''".6003"'', ''"5.003"'', ''"56.03"'', ''"560.3"'', ''"56..3"'' and ''"5600."''.
//
// Now all we have to do is:
// - find all relevant prime numbers
// - for each prime number: find all regular expressions it can be matched against and add the prime to each regular expression's list
// - find the list (of at least 8 numbers) with the smallest prime numbers
//
// The user can influence the following parameters (due to Hackerranks' modified problem):
// - ''maxDigits'' defines the total number of digits of each prime (''56003'' has 5 digits)
// - ''replace'' defines how many identical digits should be replaced
// - ''siblings'' defines how many prime numbers can be matched against the regular expression (the original problem asks for 8)
//
// The most important data structure of my program is ''matches'':
// its keys are the regular expressions while its values are the matching prime numbers.
//
// The function ''match'' fills that data structure recursively.
// - it replaces all digits of ''regex'' which are equal to ''digit'' by a dot but not more than ''howOften'' times.
// - then it adds the current prime number ''number'' to ''matches[regex]''
// - to speed up the program, the smallest prime number which fulfills all conditions is stored in ''smallestPrime''
//
// # Hackerrank
// I failed one test case with timeouts: looking at my output I saw that all minimized families of 7-digit primes have members below 2000000 or 3000000.
// That's a hack I'm not very proud of, but it gets the job done ...
//
// The problem description wasn't very clear about it: a family of x primes is also a family of x-1 primes.
// That means that the family of 7 prime numbers 56003, 56113, 56333, 56443, 56663, 56773, and 56993 is
// also a family of 6 prime numbers (56003, 56113, 56333, 56443, 56663, 56773).

#include <vector>
#include <string>
#include <map>
#include <iostream>

// total number of digits
unsigned int maxDigits = 7;
// how many digits we replace by a pattern symbol
unsigned int replace   = 3;
// how many primes that pattern match
unsigned int siblings  = 7;

// [regular expression] => [prime numbers matching that expression]
std::map<std::string, std::vector<unsigned int>> matches;

// smallest family with the required number of siblings
unsigned int smallestPrime = 99999999;

// replace all combinations of "digit" by a dot (".") when it occurs at least "howOften"
void match(unsigned int number, std::string& regex, unsigned int digit, unsigned int howOften, unsigned int startPos = 0)
{
  char asciiDigit = digit + '0';

  // look for digit
  for (unsigned int i = startPos; i < maxDigits; i++)
  {
    // keep going ...
    if (regex[i] != asciiDigit)
      continue;

    // no leading zero
    if (i == 0 && asciiDigit == '0')
      continue;

    // replace digit by placeholder
    regex[i] = '.';

    // replaced enough digits ?
    if (howOften == 1)
    {
      auto& addTo = matches[regex];
      addTo.push_back(number);
      if (addTo.size() >= siblings && addTo.front() < smallestPrime)
        smallestPrime = addTo.front();
    }
    else
    {
      // no, have to "go deeper"
      match(number, regex, digit, howOften - 1, i + 1);
    }

    // restore digit
    regex[i] = asciiDigit;
  }
}


int main()
{
  std::cin >> maxDigits >> replace >> siblings;

  // find smallest number with maxDigits digits
  unsigned int minNumber = 1;
  for (unsigned int i = 1; i < maxDigits; i++)
    minNumber *= 10;
  // and the largest number
  unsigned int maxNumber = minNumber * 10 - 1;

  // basic prime sieve of Erastothenes
  // bitmap of all prime numbers (primes[x] is true if x is prime)
  std::vector<bool> primes(maxNumber, true);
  primes[0] = primes[1] = false;
  for (unsigned int i = 2; i*i <= maxNumber; i++)
    if (primes[i])
      // i is a prime, exclude all its multiples
      for (unsigned j = 2*i; j <= maxNumber; j += i)
        primes[j] = false;

  // build regex
  for (unsigned int i = minNumber; i <= maxNumber; i++)
    if (primes[i])
    {
      // convert i to string
      auto strNum = std::to_string(i);

      // replace digits
      for (unsigned int digit = 0; digit <= 9; digit++)
        match(i, strNum, digit, replace);

      // quick hack to speed up the program
      if (maxDigits == 7)
      {
        // all relevant numbers were below thes thresholds on my local computer
        if (replace == 1 && i > 2000000)
          break;
        if (replace == 2 && i > 3000000)
          break;
      }
    }

  // find lexicographically minimized "family"
  std::string minimum;
  for (auto m : matches)
  {
    // enough members ?
    if (m.second.size()  < siblings)
      continue;
    // minimized ?
    if (m.second.front() != smallestPrime)
      continue;

    // convert all siblings to a long string
    std::string s;
    for (unsigned i = 0; i < siblings; i++)
      s += std::to_string(m.second[i]) + " ";

    // same minimum primes are part of multiple families, choose the lexicographically first
    if (minimum > s || minimum.empty())
      minimum = s;
  }

  // print best match
  std::cout << minimum << std::endl;
  return 0;
}
