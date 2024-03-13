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
        int j = bits[i]-'0'; // Convert char to int.
        result.push_back(j);
    }

    return result;
}

// Takes in an ASCII string and outputs a 128-bit digest in hex as a string.
std::string paca::myMD5(std::string const &input)
{
    // Convert string into chunks of 512-bit blocks.
    std::vector<int> bits = paca::string_to_bitstring(input);

    std::cout << "bits of password to process: " << std::endl;
    for (std::size_t i = 0; i < bits.size(); i++)
    {
        std::cout << bits[i];
    }
    std::cout << std::endl;

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
            bits.push_back(1);
        }

        while (bits.size()%512 < 448)
        {
            bits.push_back(0);
        }

        std::cout << "0-padded bits size: " << bits.size() << ", size%512: " << bits.size()%512 << std::endl;

        // Convert to integer bits to finally pad the data.
        for (std::size_t i = 0; i < to_pad.size(); i++)
        {
            bits.push_back(to_pad[i]-'0');
        }

        std::cout << "padded to 512-bit multiple: " << bits.size() << ", size%512: " << bits.size()%512 << std::endl;

    }

    std::cout << "padded message in bits: " << std::endl;
    for (std::size_t i = 0; i < bits.size(); i++)
    {
        std::cout << bits[i];
    }
    std::cout << std::endl;

    // TODO: Break into 512-bit chunks.
    std::vector<std::bitset<512>> allInputBits;

    for (std::size_t chunk = 0; chunk < bits.size(); chunk+=512)
    {
        std::bitset<512> to_push;
        for (std::size_t i = 0; i < 512; i++)
        {
            to_push.set(512-i-1, bits[chunk+i]);    // Bit 0 is the right-farthest bit; so think of it like a reversed array.
        }
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
