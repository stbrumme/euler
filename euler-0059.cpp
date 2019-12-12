// ////////////////////////////////////////////////////////
// # Title
// XOR decryption
//
// # URL
// https://projecteuler.net/problem=59
// http://euler.stephan-brumme.com/59/
//
// # Problem
// Each character on a computer is assigned a unique code and the preferred standard is ASCII (American Standard Code for Information Interchange).
// For example, uppercase A = 65, asterisk (*) = 42, and lowercase k = 107.
//
// A modern encryption method is to take a text file, convert the bytes to ASCII, then XOR each byte with a given value, taken from a secret key.
// The advantage with the XOR function is that using the same encryption key on the cipher text, restores the plain text; for example, 65 XOR 42 = 107, then 107 XOR 42 = 65.
//
// For unbreakable encryption, the key is the same length as the plain text message, and the key is made up of random bytes.
// The user would keep the encrypted message and the encryption key in different locations, and without both "halves", it is impossible to decrypt the message.
//
// Unfortunately, this method is impractical for most users, so the modified method is to use a password as a key.
// If the password is shorter than the message, which is likely, the key is repeated cyclically throughout the message.
// The balance for this method is using a sufficiently long password key for security, but short enough to be memorable.
//
// Your task has been made easy, as the encryption key consists of three lower case characters.
// Using [cipher.txt](https://projecteuler.net/project/resources/p059_cipher.txt) (right click and 'Save Link/Target As...'), a file containing the encrypted ASCII codes, and the knowledge that the plain text must contain common English words,
// decrypt the message and find the sum of the ASCII values in the original text.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// Three nested loops try all possible keys ''"aaa"'' to ''"zzz"'' until a match is found.
// The only valid characters are ''0-9 a-z A-Z ;:,.'?-!()'' and space.
// I __don't__ analyze the contents and just got lucky that the key which produces only valid characters is indeed correct.
//
// # Alternative
// I expected to write some clever AI code to figure out whether the text "sounds" English.
// But my simple logic was already successful. Somehow I feel disappointed ...

#include <vector>
#include <iostream>

int main()
{
  // read encrypted message from STDIN
  std::vector<unsigned char> encrypted;

//#define ORIGINAL
#ifdef ORIGINAL
  // read numbers in CSV format, e.g. 74,2,999,1,...
  while (true)
  {
    unsigned int current;
    std::cin >> current;

    // no more data
    if (!std::cin)
      break;

    encrypted.push_back(current);

    // skip comma
    std::cin.get();
  }

#else

  // text length
  unsigned int size;
  std::cin >> size;
  encrypted.clear();
  // read text from STDIN
  while (size--)
  {
    unsigned int current;
    std::cin >> current;
    encrypted.push_back(current);
  }
#endif

  // try all possible keys
  for (unsigned char i = 'a'; i <= 'z'; i++)
    for (unsigned char j = 'a'; j <= 'z'; j++)
      for (unsigned char k = 'a'; k <= 'z'; k++)
      {
        // current key
        const unsigned char key[] = { i, j, k };

        // apply XOR to all bytes
        std::vector<unsigned char> decoded;
        for (size_t pos = 0; pos < encrypted.size(); pos++)
          decoded.push_back(encrypted[pos] ^ key[pos % 3]);

        // classify result
        bool valid = true;
        for (auto d : decoded)
        {
          // only allow:
          // 0-9 a-z A-Z ;:,.'?-!() and space
          valid  = (d >= '0' && d <= '9');
          valid |= (d >= 'a' && d <= 'z');
          valid |= (d >= 'A' && d <= 'Z');
          valid |= (d == ' ' || d == ',' || d == '.' || d == '?' || d == '!');
          valid |= (d == ';' || d == '-' || d == '\'');
          valid |= (d == '(' || d == ')');

          // reject any invalid character
          if (!valid)
            break;
        }

        // try next key
        if (!valid)
          continue;

        // bonus feature: show decoded text
//#define SHOW_DECODED
#ifdef  SHOW_DECODED
        for (auto d : decoded)
          std::cout << d;
        std::cout << std::endl;
#endif

#ifdef ORIGINAL
        // we found the key, now add all ASCII codes of the decrypted text
        // add ASCII codes
        unsigned int asciiSum = 0;
        for (auto d : decoded)
          asciiSum += d;
        std::cout << asciiSum << std::endl;
        return 0;
#else
        // print key
        std::cout << i << j << k << std::endl;
#endif
      }

  return 0;
}
