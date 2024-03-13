#include "mypasscrack.hpp"
#include <iostream>
#include <bitset>
#include <vector>

// Takes in an ASCII string and outputs a 128-bit digest in hex as a string.
std::string paca::myMD5(std::string const &input)
{
    std::string bits = "";
    // Convert string into chunks of 512-bit blocks.
    for (char c: input)
    {
        int letter = c;     // If c = 'a', then letter == the integer value of 'a' in ASCII.
        std::bitset<8> temp(letter);
        bits += temp.to_string();
    }

    // Pad the string so that it's divisible by 512.
    /*
        Padding is done by adding a single 1 bit at the end of the message, followed
        by as many 0 needed to bring the length of the message up to 64 bits less than
        a multiple of 512. The remaining bits are filled up with 64 bits representing
        the length of the original message, modulo 2^(64).
    */
   std::vector<std::bitset<512>> allInputBits;

   // Main algorithm
   // Operates on 128-bit state, dividing into four 32-bit words, A thru D. 

   //

   return "";
}
