#include "mypasscrack.hpp"
#include <iostream>
#include <bitset>

// Put in string, and get its form as a vector of bits. 
std::vector<int> paca::string_to_bitstring(std::string const &s)
{
    std::string bits = "";

    for (char c: s)
    {
        int letter = c;
        std::bitset<8> temp(letter);
        bits += temp.to_string();
    }

    std::vector<int> result;
    for (std::size_t i = 0; i < bits.size(); i++)
    {
        int j = bits[i];
        result.push_back(j);
    }

    return result;
}

// Takes in an ASCII string and outputs a 128-bit digest in hex as a string.
std::string paca::myMD5(std::string const &input)
{
    // Convert string into chunks of 512-bit blocks.
    std::vector<int> bits = paca::string_to_bitstring(input);

    // Pad the string so that it's divisible by 512.
    /*
        Padding is done by adding a single 1 bit at the end of the message, followed
        by as many 0 needed to bring the length of the message up to 64 bits less than
        a multiple of 512. The remaining bits are filled up with 64 bits representing
        the length of the original message, modulo 2^(64).
    */

   // TODO: How to convert bit string to hexadecimal ints.
   // TODO: Pad the message.
   // TODO: Split into 512-bit chunks.
    if (!(bits.size() % 512 == 0))
    {
        std::vector<int> original = bits;
        
        if (bits.size() % 512 < 448)
        {
            bits.push_back(1);
        }

    }


   std::vector<std::bitset<512>> allInputBits;

   // Main algorithm
   // Operates on 128-bit state, dividing into four 32-bit words, A thru D. 

   //

   return "";
}
