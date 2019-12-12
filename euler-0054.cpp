// ////////////////////////////////////////////////////////
// # Title
// Poker hands
//
// # URL
// https://projecteuler.net/problem=54
// http://euler.stephan-brumme.com/54/
//
// # Problem
// In the card game poker, a hand consists of five cards and are ranked, from lowest to highest, in the following way:
//
// || 8                || 0
// || High Card:       || Highest value card.
// || One Pair:        || Two cards of the same value.
// || Two Pairs:       || Two different pairs.
// || Three of a Kind: || Three cards of the same value.
// || Straight:        || All cards are consecutive values.
// || Flush:           || All cards of the same suit.
// || Full House:      || Three of a kind and a pair.
// || Four of a Kind:  || Four cards of the same value.
// || Straight Flush:  || All cards are consecutive values of same suit.
// || Royal Flush:     || Ten, Jack, Queen, King, Ace, in same suit.
//
// The cards are valued in the order: 2, 3, 4, 5, 6, 7, 8, 9, 10, Jack, Queen, King, Ace.
//
// If two players have the same ranked hands then the rank made up of the highest value wins;
// for example, a pair of eights beats a pair of fives (see example 1 below).
// But if two ranks tie, for example, both players have a pair of queens, then highest cards in each hand are compared (see example 4 below);
// if the highest cards tie then the next highest cards are compared, and so on.
//
// Consider the following five hands dealt to two players:
// ||   4  ||        9            ||        9            ||   4      ||
// || Hand || Player 1            || Player 2            || Winner   ||
// ||------||---------------------||---------------------||----------||
// ||   1  || 5H 5C 6S 7S KD      || 2C 3S 8S 8D TD      || Player 2 ||
// ||      || Pair of Fives       || Pair of Eights      ||          ||
//
// ||   2  || 5D 8C 9S JS AC      || 2C 5C 7D 8S QH      || Player 1 ||
// ||      || Highest card Ace    || Highest card Queen  ||          ||
//
// ||   3  || 2D 9C AS AH AC      || 3D 6D 7D TD QD      || Player 2 ||
// ||      || Three Aces          || Flush with Diamonds ||          ||
//
// ||   4  || 4D 6S 9H QH QC      || 3D 6D 7H QD QS      || Player 1 ||
// ||      || Pair of Queens      || Pair of Queens      ||          ||
// ||      || Highest card Nine   || Highest card Seven  ||          ||
//
// ||   5  || 2H 2D 4C 4D 4S      || 3C 3D 3S 9S 9D      || Player 1 ||
// ||      || Full House          || Full House          ||          ||
// ||      || With Three Fours    || With Three Threes   ||          ||
//
// The file, [poker.txt](https://projecteuler.net/project/resources/p054_poker.txt), contains one-thousand random hands dealt to two players.
// Each line of the file contains ten cards (separated by a single space): the first five are Player 1's cards and the last five are Player 2's cards.
// You can assume that all hands are valid (no invalid characters or repeated cards), each player's hand is in no specific order, and in each hand there is a clear winner.
//
// How many hands does Player 1 win?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// ''cardMask'' converts a string to a 64 bit integer bitmask:
// There are 13 different poker cards (2, 3, 4, 5, 6, 7, 8, 9, 10, Jack, Queen, King, Ace) of 4 different suits (Diamonds, Hearts, Spades and Clubs).
// Each of those `13 * 4 = 52` cards is assigned to a certain bit position:
// ||   4   ||       5      ||       5      ||       5      ||       5      ||       5      ||       5      ||       5      ||       5
// || Card: || ''00000000'' || ''0000AKQJ'' || ''T9876543'' || ''2AKQJT98'' || ''765432AK'' || ''QJT98765'' || ''432AKQJT'' || ''98765432''
// || Suit: || ''00000000'' || ''0000CCCC'' || ''CCCCCCCC'' || ''CSSSSSSS'' || ''SSSSSSHH'' || ''HHHHHHHH'' || ''HHHDDDDD'' || ''DDDDDDDD''
//
// Each binary representation of a valid Poker hand has exactly 5 bits set.
// If a player holds Hearts 2, Diamonds 7, Diamonds King, Clubs King and Clubs Ace, then the binary representation is:
// ||   4   ||       5      ||       5      ||       5      ||       5      ||       5      ||       5      ||       5      ||       5
// ||       || ''00000000'' || ''00001100'' || ''00000000'' || ''00000000'' || ''00000000'' || ''00000000'' || ''00101000'' || ''00100000''
//
// ''rank'' assigns a value to a hand of five Poker cards: a hand with a lower value beats all hands with a higher value.
// There are a few steps I always do before actually evaluating a hand:
// 1. figure out whether we have a straight (5 consecutive bits are set or A2345)
// 2. count how often each card is present, ignoring its suit
// 3. do all cards share the same suit ? (a flush)
//
// Then a number is assigned to each hand:
// || 8               || 0
// || Royal Flush     || 1
// || Straight Flush  || 2..10
// || Four of a Kind  || 10000000000 + 100*four + 1*single
// || Full House      || 20000000000 + 100*three + 1*two
// || Flush           || 30000000000 + 100000000*highest + 100000*second + 10000*third + 100*fourth + 1*worst
// || Straight        || 40000000000 + 1..10
// || Three of a Kind || 50000000000 + 10000*three + 100*bestSingle + 1*worstSingle
// || Two Pairs       || 60000000000 + 10000*bestPair + 100*wordPair + 1*single
// || One Pair        || 70000000000 + 1000000*pair + 10000*bestSingle + 100*middleSingle + 1*worstSingle
// || High Card       || 80000000000 + 100000000*highest + 100000*second + 10000*third + 100*fourth + 1*worst
//
// My number system isn't very efficient but quite easy to debug.
// There are actually only 2598960 different hands `\binom{52}{5} = frac{52!}{(52-5)!5!}`.
//
// # Alternative
// There is an endless number of Poker hand evaluators available on the internet.
// And I am pretty sure you will find an endless number of ways to evaluate a Poker hand.
//
// # Note
// My program doesn't check its input. It must follow the syntax defined by the problem statement (which is a widely accepted Poker hand notation).

#include <string>
#include <iostream>

const unsigned long long Card2 = 1ULL <<  0;
const unsigned long long Card3 = 1ULL <<  1;
const unsigned long long Card4 = 1ULL <<  2;
const unsigned long long Card5 = 1ULL <<  3;
const unsigned long long Card6 = 1ULL <<  4;
const unsigned long long Card7 = 1ULL <<  5;
const unsigned long long Card8 = 1ULL <<  6;
const unsigned long long Card9 = 1ULL <<  7;
const unsigned long long CardT = 1ULL <<  8;
const unsigned long long CardJ = 1ULL <<  9;
const unsigned long long CardQ = 1ULL << 10;
const unsigned long long CardK = 1ULL << 11;
const unsigned long long CardA = 1ULL << 12;

// convert a card to a 52-bitmask
unsigned long long cardMask(const std::string& card) // card format is 5H for 5 of hearts
{
  // bit mask structure:
  // bits from  0 to 12 are 23456789TJQKA of diamonds
  // bits from 13 to 25 are 23456789TJQKA of hearts
  // bits from 26 to 38 are 23456789TJQKA of spades
  // bits from 39 to 51 are 23456789TJQKA of clubs
  // bits 52+ are zero
  unsigned long long result = 0;
  switch (card[0])
  {
    case '2': result = Card2; break;
    case '3': result = Card3; break;
    case '4': result = Card4; break;
    case '5': result = Card5; break;
    case '6': result = Card6; break;
    case '7': result = Card7; break;
    case '8': result = Card8; break;
    case '9': result = Card9; break;
    case 'T': result = CardT; break;
    case 'J': result = CardJ; break;
    case 'Q': result = CardQ; break;
    case 'K': result = CardK; break;
    case 'A': result = CardA; break;
    default: break;
  }
  switch (card[1])
  {
    case 'D':                break;
    case 'H': result <<= 13; break;
    case 'S': result <<= 26; break;
    case 'C': result <<= 39; break;
    default: break;
  }

  return result;
}

// each hand with a certain rank beats all hands with a higher rank
unsigned long long rank(unsigned long long hand)
{
  // set the lowest 13 bits (= 13 cards of a suit)
  const auto Suit = (1LL << 13) - 1;

  // ignore color (convert all cards to diamonds)
  auto colorless = (hand | (hand >> 13) | (hand >> 26) | (hand >> 39)) & Suit;

  // greater zero if straight (better straights get higher value)
  unsigned int straight = 0;
  if (colorless == (CardT | CardJ | CardQ | CardK | CardA)) straight =  1;
  if (colorless == (Card9 | CardT | CardJ | CardQ | CardK)) straight =  2;
  if (colorless == (Card8 | Card9 | CardT | CardJ | CardQ)) straight =  3;
  if (colorless == (Card7 | Card8 | Card9 | CardT | CardJ)) straight =  4;
  if (colorless == (Card6 | Card7 | Card8 | Card9 | CardT)) straight =  5;
  if (colorless == (Card5 | Card6 | Card7 | Card8 | Card9)) straight =  6;
  if (colorless == (Card4 | Card5 | Card6 | Card7 | Card8)) straight =  7;
  if (colorless == (Card3 | Card4 | Card5 | Card6 | Card7)) straight =  8;
  if (colorless == (Card2 | Card3 | Card4 | Card5 | Card6)) straight =  9;
  if (colorless == (CardA | Card2 | Card3 | Card4 | Card5)) straight = 10;

  // pairs, triple, fours
  unsigned int count[13] = { 0,0,0,0,0,0,0,0,0,0,0,0,0 };
  for (unsigned int i = 0; i < 13; i++)
  {
    if (hand & (1ULL <<   i))
      count[i]++;
    if (hand & (1ULL << (i+13)))
      count[i]++;
    if (hand & (1ULL << (i+26)))
      count[i]++;
    if (hand & (1ULL << (i+39)))
      count[i]++;
  }

  // true, if all cards share the same colors
  bool isFlush = (hand ==  colorless)        ||
                 (hand == (colorless << 13)) ||
                 (hand == (colorless << 26)) ||
                 (hand == (colorless << 39));

  // allocate 10000000000 IDs per category (flush, straight, pairs, etc.)
  const unsigned long long GroupSize = 10000000000ULL; // burn up to two digits per card
  unsigned long long result = 0;

  // royal flush and straight flush
  if (isFlush && straight > 0)
    return result + straight;
  result += GroupSize;

  // four-of-a-kind
  for (unsigned int i = 0; i < 13; i++)
    if (count[i] == 4)
      for (unsigned int j = 0; j < 13; j++)
        if (count[j] == 1)
          return result + (13 - i) * 100 + (13 - j);
  result += GroupSize;

  // full-house
  for (unsigned int i = 0; i < 13; i++)
    if (count[i] == 3)
      for (unsigned int j = 0; j < 13; j++)
        if (count[j] == 2)
          return result + (13 - i) * 100 + (13 - j);
  result += GroupSize;

  // flush
  if (isFlush)
  {
    unsigned long long value = 0;
    for (int i = 12; i >= 0; i--)
      if (count[i] == 1)
      {
        value *= 100;
        value += 13 - i;
      }
    return result + value;
  }
  result += GroupSize;

  // straight
  if (straight > 0)
    return result + straight;
  result += GroupSize;

  // three
  for (unsigned int i = 0; i < 13; i++)
    if (count[i] == 3)
    {
      unsigned long long value = 13 - i;
      for (int j = 12; j >= 0; j--)
        if (count[j] == 1)
        {
          value *= 100;
          value += 13 - j;
        }
      return result + value;
    }
  result += GroupSize;

  // one or two pairs
  unsigned int numPairs = 0;
  for (unsigned int i = 0; i < 13; i++)
    if (count[i] == 2)
      numPairs++;
  if (numPairs > 0)
  {
    unsigned long long value = 0;
    // pairs
    for (int i = 12; i >= 0; i--)
      if (count[i] == 2)
      {
        value *= 100;
        value += 13 - i;
      }
    // single card(s)
    for (int i = 12; i >= 0; i--)
      if (count[i] == 1)
      {
        value *= 100;
        value += 13 - i;
      }

    if (numPairs == 1)
      result += GroupSize;
    return result + value;
  }
  result += 2 * GroupSize; // one and two pairs

  // high card
  unsigned long long value = 0;
  for (int i = 12; i >= 0; i--)
    if (count[i] == 1)
    {
      value *= 100;
      value += 13 - i;
    }

  return result + value;
}

int main()
{
  unsigned int tests = 1000;

//#define ORIGINAL
#ifdef ORIGINAL
  unsigned int won1 = 0;
  unsigned int won2 = 0;
#else
  std::cin >> tests;
#endif

  while (tests--)
  {
    std::string cards1[5], cards2[5];
    std::cin >> cards1[0] >> cards1[1] >> cards1[2] >> cards1[3] >> cards1[4];
    std::cin >> cards2[0] >> cards2[1] >> cards2[2] >> cards2[3] >> cards2[4];

    // convert to bitmask and merge with logical OR
    auto player1 = cardMask(cards1[0]) | cardMask(cards1[1]) | cardMask(cards1[2]) |
                   cardMask(cards1[3]) | cardMask(cards1[4]);
    auto player2 = cardMask(cards2[0]) | cardMask(cards2[1]) | cardMask(cards2[2]) |
                   cardMask(cards2[3]) | cardMask(cards2[4]);

    // lower rank wins
    auto rank1 = rank(player1);
    auto rank2 = rank(player2);
#ifdef ORIGINAL
    if (rank1 < rank2)
      won1++;
    if (rank1 > rank2)
      won2++;
#else
    std::cout << "Player " << (rank1 < rank2 ? "1" : "2") << std::endl;
#endif
  }

#ifdef ORIGINAL
  std::cout << won1 << std::endl;
  //std::cout << won2 << std::endl;
#endif

  return 0;
}
