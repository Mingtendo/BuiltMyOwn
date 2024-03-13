//#include "mypasscrack.hpp"
#include <iostream>
#include <string>
#include <bitset>

int main()
{
    // Can't copy a string into a bitset. Must be converted to char first, and then bits.
    std::string test = "password";
    std::string bits;
    for (char c: test)
    {
        int letter = c;
        std::cout << "letter " << c << ": " << letter << std::endl;
        std::bitset<8> temp(letter);
        bits += temp.to_string();
        std::cout << "bits: " << bits << std::endl;
    }

    return 0;
}