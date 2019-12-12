// ////////////////////////////////////////////////////////
// # Title
// Rooms of Doom
//
// # URL
// https://projecteuler.net/problem=327
// http://euler.stephan-brumme.com/327/
//
// # Problem
// A series of three rooms are connected to each other by automatic doors.
//
// ![maze](p327_rooms_of_doom.gif)
//
// Each door is operated by a security card. Once you enter a room the door automatically closes and that security card cannot be used again.
// A machine at the start will dispense an unlimited number of cards, but each room (including the starting room) contains scanners
// and if they detect that you are holding more than three security cards or if they detect an unattended security card on the floor,
// then all the doors will become permanently locked.
// However, each room contains a box where you may safely store any number of security cards for use at a later stage.
//
// If you simply tried to travel through the rooms one at a time then as you entered room 3 you would have used all three cards and would be trapped in that room forever!
//
// However, if you make use of the storage boxes, then escape is possible. For example, you could enter room 1 using your first card,
// place one card in the storage box, and use your third card to exit the room back to the start.
// Then after collecting three more cards from the dispensing machine you could use one to enter room 1 and collect the card you placed in the box a moment ago.
// You now have three cards again and will be able to travel through the remaining three doors.
// This method allows you to travel through all three rooms using six security cards in total.
//
// It is possible to travel through six rooms using a total of 123 security cards while carrying a maximum of 3 cards.
//
// Let `C` be the maximum number of cards which can be carried at any time.
//
// Let `R` be the number of rooms to travel through.
//
// Let `M(C,R)` be the minimum number of cards required from the dispensing machine to travel through `R` rooms carrying up to a maximum of `C` cards at any time.
//
// For example, `M(3,6)=123` and `M(4,6)=23`.
// And, `sum{M(C,6)}=146` for `3 <= C <= 4`.
//
// You are given that `sum{M(C,10)}=10382` for `3 <= C <= 10`.
//
// Find `sum{M(C,30)}` for `3 <= C <= 40`.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// I process all rooms from the right to the left side. In each room I calculate the number of cards needed to escape.
// Only one card is required in the right-most room: just insert it into the scanner - done.
// I need two cards in the room left to the right-most room: one card to enter the last room and another card to escape.
// Similarly, I need three cards in the room before that room.
//
// Let's assume there are only three rooms - the problem statement gives away that a total of six cards are needed.
// And further assume that in front of the first room is room "zero".
// To exit the maze I need three cards when I'm inside the first room.
// However, I cannot hold more than three cards at the same time and already needed one card to enter the first room.
// So I need a few extra moves: go back to room zero, takes as many cards as possible, enter room one, deposit as many as possible and go back to room room.
// These extra moves "transport" cards: I can transport `C - (1+1)` cards from the previous to the current room with one extra move:
// one card is needed to enter the current room and another to leave the current room.
//
// In order to have three cards available in the first room, the following sequence needs to be followed:
// - take 3 cards from the dispenser in room zero
// - enter room one: I have two cards left, one will be stored, the second is needed to return to room zero
// - take 3 cards from the dispenser in room zero
// - enter room one: I have two cards left plus one card in the storage for a total of three cards
// - enter room two: I have two cards left
// - enter room three: I have one cards left and that's enough to open the final door
// ==> a total of six cards were taken from the dispenser
//
// So my algorithm is as follows:
// - start with the right-most room, I need one card to exit it, ''need = 1''
// - in the room its left side I need enough cards to enter the next room and enough cards to exit the maze from that room, too: ''need += 1''
// - if ''need > C'' then extra moves to the previous room (its left-side room) are required in order to get ''need'' cards into the current rooms
// - each extra move provides ''C - 2'' cards but increases the number of ''consumed'' cards by ''C''
//
// Surprisingly, a simple ''while'' loop finds the correct result if `C >= 4` almost instantly (disable ''#define FAST'').
// However, the results for `C = 3` are enormous and that loop would take a long time to finish.
// Each iteration of the simple ''while'' loop does the same job: adding as subtracting constants.
// So my ''#define FAST'' just calculates the number of iterations and finishes in less than 0.01 seconds.

#include <iostream>

unsigned long long search(unsigned int cards, unsigned int rooms)
{
  // need one card to exit the right-most room
  unsigned long long need = 1;

  // I need one card to enter the next room and one to come back, thus I can transport "cards - 2" to the next room in one step
  const auto Transport = cards - (1 + 1); // these 2 cards are "wasted"

  // compute in each room how many cards I need to exit, beginning with the right-most room
  for (auto currentRoom = rooms; currentRoom > 0; currentRoom--)
  {
    // how many cards are required to satisfy "need", including cards wasted moving forth and back
    auto consumed = 0ULL;

#define FAST
#ifdef  FAST
    // all iterations of the following "while"-loop are basically the same, let's process them quickly
    if (need >= cards)
    {
      // determine number of iterations
      auto moves = (need - cards) / Transport;

      // do I need one more to ensure need < cards at the end ?
      if (need - moves * Transport >= cards)
        moves++;

      // compute all iterations at once
      need     -= moves * Transport;
      consumed += moves * cards;
    }
#else
    // the basic algorithm becomes slow if needing many cards and Transport is small
    while (need >= cards)
    {
      need     -= Transport;
      consumed += cards;
    }
#endif

    // carry the remaining cards in the last move, including a card to enter the next room
    auto previous = need + consumed + 1;
    // ... and continue with the previous room (the one on the left side of the current room)
    need = previous;
  }

  return need;
}

int main()
{
  unsigned int rooms = 30;
  unsigned int cards = 40;
  std::cin >> rooms >> cards;

  unsigned long long result = 0;
  for (unsigned int i = 3; i <= cards; i++)
    result += search(i, rooms);

  std::cout << result << std::endl;
  return 0;
}
