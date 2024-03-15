#include "mypasscrack.hpp"
#include <iostream>
#include <limits>

bool isBigEndian()
{
    uint16_t word = 1; // 0x0001
    uint8_t *firstbyte = (uint8_t*) &word; // points to first byte of word
    bool biglittle = !(*firstbyte);
    return biglittle;
}

int main()
{
    // Can't copy a string into a bitset. Must be converted to char first, and then bits.
    std::string test = "password";
    // std::string bits;
    // for (char c: test)
    // {
    //     int letter = c;
    //     std::cout << "letter " << c << ": " << letter << std::endl;
    //     std::bitset<8> temp(letter);    // Convert to bits.
    //     for (int i = 0; i < 8; i++)
    //     {
    //         std::cout << "bit " << i << ": " << temp[i] << std::endl;
    //     }
    //     bits += temp.to_string();
    //     std::cout << "bits: " << bits << std::endl;
    // }

    // unsigned long long int x = std::numeric_limits<unsigned long long int>::max();
    // std::cout << x << std::endl;
    std::cout << (isBigEndian() ? "Big Endian" : "Little Endian") << std::endl;

    std::string testMD5algorithm = paca::myMD5(test);
    std::cout << "Result of hashing \'" << test << "\': " << testMD5algorithm << std::endl;
    test = "The quick brown fox jumps over the lazy dog"; 
    testMD5algorithm = paca::myMD5(test);
    std::cout << "Result of hashing \'" << test << "\': " << testMD5algorithm << std::endl;
    return 0;
}