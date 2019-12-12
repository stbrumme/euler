// ////////////////////////////////////////////////////////
// # Title
// Anagramic squares
//
// # URL
// https://projecteuler.net/problem=98
// http://euler.stephan-brumme.com/98/
//
// # Problem
// By replacing each of the letters in the word CARE with 1, 2, 9, and 6 respectively, we form a square number: 1296 = 362.
// What is remarkable is that, by using the same digital substitutions, the anagram, RACE, also forms a square number: 9216 = 962.
// We shall call CARE (and RACE) a square anagram word pair and specify further that leading zeroes are not permitted, neither may a different letter have the same digital value as another letter.
//
// Using [words.txt](https://projecteuler.net/project/resources/p098_words.txt) (right click and 'Save Link/Target As...'), a 16K text file containing nearly two-thousand common English words,
// find all the square anagram word pairs (a palindromic word is NOT considered to be an anagram of itself).
//
// What is the largest square number formed by any member of such a pair?
//
// NOTE: All anagrams formed must be contained in the given text file.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// Two numbers are permutations of each other if they share the same digits.
// Two words are permutations of each other if they share the same letters.
//
// My ''fingerprint'' from problem 49 (and used in many more problems) returns the same result if two numbers share the same digits.
//
// Two words share the same letters if sorting their letters produces the same string: ''sort(KEEP)=EEKP'' and ''sort(PEEK)=EEKP''.
//
// After all input has been read, my problem looks for the longest word with at least one "anagram sibling".
// Hint: it's 9 for the anagram siblings ("INTRODUCE", "REDUCTION")
//
// Then all square numbers 1^2, 2^2, 3^2, ... with at most 9 digits are computed.
// Their fingerprints are stored in ''permutations'' along with the base number (where ''square = base^2'').
// A second container ''fingerprintLength'' keeps track of the number of digits of ''square''.
// C++'s ''log10'' produces minor rounding artefacts - that's why I had to subtract 1.
//
// All words with same anagram are then ''match''ed against each other.
//
// # Hackerrank
// The Hackerrank problem is such different that I decided to use completely different code:
// I have to find the largest square with n digits that is an anagram of another square.
// My solution is extremely short compared to the original problem.

#include <cmath>

#include <vector>
#include <set>
#include <map>

#include <string>
#include <algorithm>

#include <iostream>

// count digits, two numbers have the same fingerprint if they are permutations of each other
unsigned long long fingerprint(unsigned long long x)
{
  unsigned long long result = 0;
  while (x > 0)
  {
    auto digit = x % 10;
    x /= 10;

    result += 1LL << (4 * digit);
  }
  return result;
}


//#define ORIGINAL
#ifdef ORIGINAL // the original problem differs significantly from Hackerrank

// read a single word from STDIN, syntax: "abc","def","xyz"
std::string readWord()
{
  std::string result;
  while (true)
  {
    // read one character
    char c = std::cin.get();
    // no more input ?
    if (!std::cin)
      break;

    // ignore quotes
    if (c == '"')
      continue;
    // finish when a comma appears
    if (c == ',')
      break;

    // nope, just an ordinary letter (no further checks whether c in 'A'..'Z')
    result += c;
  }
  return result;
}

// return biggest square if both a and b can be mapped to anagram squares, else return 0
unsigned long long match(const std::string& a, const std::string& b, const std::vector<unsigned long long>& squares)
{
  unsigned long long result = 0;
  // try all combinations
  for (auto i : squares)
    for (auto j : squares)
    {
      // don't match a word with itself
      if (i == j)
        continue;

      // convert squares to strings
      auto replaceA = std::to_string(i);
      auto replaceB = std::to_string(j);

      // 1. replace all digits of squareA by the letters of a
      // 2. at the same time, whenever such a digit can be found in squareB replace it by the same letter

      // [digit] => letter
      std::map<char, char> replaceTable;
      bool valid = true;
      for (size_t k = 0; k < replaceA.size(); k++)
      {
        char original = replaceA[k];
        // no replacement rule found ? => abort
        if (replaceTable.count(original) != 0 &&
            replaceTable[original] != a[k])
          valid = false;

        // replacement successful
        replaceTable[original] = a[k];
      }

      // two digits must not map to the same letter, though
      std::set<char> used;
      for (auto x : replaceTable)
      {
        // already used ?
        if (used.count(x.second) != 0)
          valid = false;
        // mark as used
        used.insert(x.second);
      }

      // any constraint violation ?
      if (!valid)
        continue;

      // using that mapping, can "a" be constructed ?
      std::string aa;
      for (auto x : replaceA)
        aa += replaceTable[x];
      if (aa != a)
        continue;

      // using that mapping, can "b" be constructed ?
      std::string bb;
      for (auto x : replaceB)
        bb += replaceTable[x];
      if (bb != b)
        continue;

      // new bigger square ?
      if (result < i)
        result = i;
      if (result < j)
        result = j;
    }

  return result;
}

int main()
{
  // find word anagrams: sort letters of each word
  // [sorted letters] => [list of words]
  std::map<std::string, std::vector<std::string>> anagrams;

  // read all words from STDIN and fill "anagram" container
  while (true)
  {
    // read a single word, abort when empty
    auto word = readWord();
    if (word.empty())
      break;

    auto sorted = word;
    std::sort(sorted.begin(), sorted.end());
    // add to word anagrams
    anagrams[sorted].push_back(word);
  }

  // find longest anagram
  size_t maxDigits = 0;
  for (auto i : anagrams)
    if (i.second.size() > 1) // at least two words share the same letters ?
      if (maxDigits < i.second.front().size())
        maxDigits = i.second.front().size();
  // maxDigits will be 9 for the given input ("INTRODUCE", "REDUCTION")

  unsigned long long maxNumber = 1;
  for (size_t i = 0; i < maxDigits; i++)
    maxNumber *= 10;

  // generate all squares
  // for each square, compute its fingerprint
  std::map<unsigned long long, std::vector<unsigned long long>> permutations;
  std::map<unsigned int,       std::set   <unsigned long long>> fingerprintLength;
  // walk through all square numbers (base^2)
  unsigned long long base = 1;
  while (base*base <= maxNumber)
  {
    auto square = base*base;
    auto id     = fingerprint(square);
    permutations[id].push_back(square);

    auto numDigits = log10(square - 1) + 1;
    fingerprintLength[numDigits].insert(id);

    base++;
  }

  // only process non-unique words (size > 1)
  unsigned long long result = 0;
  for (auto i : anagrams)
  {
    auto pairs = i.second;
    // no other word with the same letters ?
    if (pairs.size() == 1)
      continue;

    // there is a chance that not all words of a permutation group are squares,
    // there only need to be at least two matching words
    auto length = pairs.front().size();
    // compare each word with each other
    for (size_t i = 0; i < pairs.size(); i++)
      for (size_t j = i + 1; j < pairs.size(); j++)
      {
        // extract all relevant squares
        for (auto id : fingerprintLength[length])
        {
          // and perform the matching process ...
          auto best = match(pairs[i], pairs[j], permutations[id]);
          // bigger square found ?
          if (result < best)
            result = best;
        }
      }
  }

  std::cout << result << std::endl;
}


#else // ---------- completely different algorithms for Hackerrank problem ----------

int main()
{
  unsigned int digits;
  std::cin >> digits;

  // find smallest and largest number with the number of digits
  // e.g. digits = 5 ==> minNumber = 10000 and maxNumber = 99999
  unsigned long long minNumber = 1;
  for (unsigned int i = 1; i < digits; i++)
    minNumber *= 10;
  unsigned long long maxNumber = minNumber * 10 - 1;

  // generate all squares between minNumber and maxNumber
  // for each square, compute its fingerprint
  unsigned long long base = sqrt(minNumber);
  if (base*base < minNumber)
    base++;
  std::map<unsigned long long, std::vector<unsigned long long>> permutations;
  while (base*base <= maxNumber)
  {
    auto square = base*base;
    permutations[fingerprint(square)].push_back(square);
    base++;
  }

  // find most common fingerprint and its highest value
  size_t bestCount = 0;
  unsigned long long highestSquare = 0;
  for (auto p : permutations)
  {
    auto size = p.second.size();
    auto high = p.second.back();

    // same number of elements but highest square exceeds the previous record ?
    if (bestCount == size && highestSquare < high)
      highestSquare = high;

    // new record number of elements
    if (bestCount <  size)
    {
      bestCount     = size;
      highestSquare = high;
    }
  }

  std::cout << highestSquare << std::endl;
}

#endif
