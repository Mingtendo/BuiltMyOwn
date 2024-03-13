#include "mypasscrack.hpp"
#include <iostream>
#include <bitset>

/*
    Things Learned So Far:
    1. To convert char to int ('5' to 5), do char - '0'.
    2. std::bitset<> constructor allows std::strings as input.
    3a. std::bitset<>[0] is the rightmost bit (smallest bit); opposite of most array-like containers.
    3b. In order to manually turn integer bits (1/0) from a std::vector to a std::bitset, be sure to start at bitset.size()-1 and go backwards to 0 to maintain order.
    
*/

// Put in string, and get its form as a vector of bits. 
std::string paca::string_to_bitstring(std::string const &s)
{
    std::string result = "";

    for (char c: s)
    {
        int letter = c;
        std::bitset<8> temp(letter);
        result += temp.to_string();
    }

    return result;
}

// Takes in an ASCII string and outputs a 128-bit digest in hex as a string.
std::string paca::myMD5(std::string const &input)
{
    // Convert string into chunks of 512-bit blocks.
    std::string bits = paca::string_to_bitstring(input);

    std::cout << "bits of password to process: " << bits << std::endl;

    // Pad the string so that it's divisible by 512.
    /*
        Padding is done by adding a single 1 bit at the end of the message, followed
        by as many 0 needed to bring the length of the message up to 64 bits less than
        a multiple of 512. The remaining bits are filled up with 64 bits representing
        the length of the original message, modulo 2^(64).
    */

   // DONE: How to convert bit string to bits.
   // DONE: Pad the message.
    if (bits.size() % 512 != 0)
    {
        unsigned long long int padding = bits.size();  // Because unsigned ints handle overflowing numbers as defined by C++ standards, this is automatically modulo 2^64.
        std::cout << "padding: " << padding << std::endl;

        std::string to_pad = std::bitset<64>(padding).to_string();
        std::cout << "to_pad: " << to_pad << std::endl;
        
        if (bits.size()%512 < 448)
        {
            bits += '1';
        }

        while (bits.size()%512 < 448)
        {
            bits += '0';
        }

        std::cout << "0-padded bits size: " << bits.size() << ", size%512: " << bits.size()%512 << std::endl;

        // Add padding bits to message.
        bits += to_pad;

        std::cout << "padded to 512-bit multiple: " << bits.size() << ", size%512: " << bits.size()%512 << std::endl;

    }

    std::cout << "padded message in bits: " << bits << std::endl;

    // TODO: Break into 512-bit chunks.
    std::vector<std::bitset<512>> allInputBits;

    for (std::size_t chunk = 0; chunk < bits.size(); chunk+=512)
    {
        std::bitset<512> to_push(bits.substr(chunk, 512));  // Automatic conversion of bit string to bits.
        allInputBits.push_back(to_push);
        std::cout << "chunk " << chunk%512 << ": " << to_push << std::endl;
    }

    // Main algorithm
    // Operates on 128-bit state, dividing into four 32-bit words, A thru D. 
    /*
        ^= XOR, assign
        &= AND, assign
        |= OR, assign
        ~var NOT var
        <<=X Shift left by X bits, assign
        >>=X Shift right by X bits, assign
    */
    

    return "Done";
}
