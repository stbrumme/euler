// ////////////////////////////////////////////////////////
// # Title
// Passcode derivation
//
// # URL
// https://projecteuler.net/problem=79
// http://euler.stephan-brumme.com/79/
//
// # Problem
// A common security method used for online banking is to ask the user for three random characters from a passcode.
// For example, if the passcode was 531278, they may ask for the 2nd, 3rd, and 5th characters; the expected reply would be: 317.
//
// The text file, [keylog.txt](https://projecteuler.net/project/resources/p079_keylog.txt), contains fifty successful login attempts.
//
// Given that the three characters are always asked for in order, analyse the file so as to determine the shortest possible secret passcode of unknown length.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// A very important facts __is missing__ in the problem description: each character is unique in the passcode.
//
// My input routine works as follows:
// - for each logged character: keep track of all characters that were entered immediately before it (in the same login attempt)
// - store this information in ''previous'', e.g. for ''317'' it contains ''{ 1 => {3}, 3 => {}, 7 => {1} }''
//
// Assuming we tracked more logins: ''518'', ''538'' and ''327'':
// ''previous = { 1 => {3,5}, 2 => {3}, 3 => {5}, 5 => {}, 7 => {1,2}, 8 => {1,3} }''
//
// When all logins are processed, I look for the lexicographically smallest (due to Hackerrank's problem modifications) without any successor.
// In the example above, ''5'' points to an empty set and would be printed first.
//
// Then my algorithm removes ''5'' from ''previous'': remove it whereever it appears as a key __and__ as a value.
// ''previous = { 1 => {3}, 2 => {3}, 3 => {}, 7 => {1,2}, 8 => {1,3} }''
//
// The next character without successor is ''3'' ... print ''3'' and remove it from ''previous'':
// ''previous = { 1 => {}, 2 => {}, 7 => {1,2}, 8 => {1} }''
//
// Now there are several possible keyphrases. We don't know for sure whether the next character is ''1'' or ''2''.
// ''1'' is lexicographically smaller and is chosen by my program.
// ''previous = { 2 => {}, 7 => {2}, 8 => {} }''
//
// Then ''2'' (again: ambiguous !):
// ''previous = { 7 => {}, 8 => {} }''
//
// After that the program picks ''7'' and finally ''8''.
//
// My program accepts numbers as well as letters. The "lexicographical order" is based on the ASCII code.
//
// The input can be scrambled in a way that no solution is possible (Hackerrank only, an example would be ''123 321'').
// Then ''"SMTH WRONG"'' is printed.

#include <set>
#include <map>
#include <string>
#include <iostream>

int main()
{
  unsigned int logins = 50; // p079_keylog.txt contains 50 logins

//#define ORIGINAL
#ifndef ORIGINAL
  std::cin >> logins;
#endif

  // read all logged inputs
  // for each digit/letter, store its predecessor
  std::map<char, std::set<char>> previous;
  while (logins--)
  {
    std::string line;
    std::cin >> line;
    // create an empty set for the initial letter (if it doesn't exist yet)
    previous[line[0]];
    // and for the other letters, store their predecessors
    for (unsigned int i = 1; i < line.size(); i++)
      previous[line[i]].insert(line[i - 1]);
  }

  // until we have no characters left ...
  std::string result;
  while (!previous.empty())
  {
    // find lexicographically smallest letter with no predecessor
    auto emptySet = previous.begin();
    while (emptySet != previous.end() && !emptySet->second.empty())
      emptySet++;

    // invalid ?
    if (emptySet == previous.end())
    {
      result = "SMTH WRONG"; // Hackerrank's message if code cannot be decrypted
      break;
    }

    // print letter
    auto current = emptySet->first;
    result += current;

    // that letter won't appear in the keyphrase anymore
    previous.erase(current);

    // remove from the predecessor list of all other letters
    for (auto& p : previous)
      p.second.erase(current);
  }

  // print keyphrase
  std::cout << result << std::endl;
  return 0;
}

