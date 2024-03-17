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

/*
MD5 test suite:
MD5 ("") = d41d8cd98f00b204e9800998ecf8427e
MD5 ("a") = 0cc175b9c0f1b6a831c399e269772661
MD5 ("abc") = 900150983cd24fb0d6963f7d28e17f72
MD5 ("password") = 5f4dcc3b5aa765d61d8327deb882cf99
MD5 ("message digest") = f96b697d7cb7938d525a2f31aaf161d0
MD5 ("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b
MD5 ("The quick brown fox jumps over the lazy dog") = 9e107d9d372bb6826bd81d3542a419d6
MD5 ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") = d174ab98d277d9f5a5611c2c9f419d9f
MD5 ("12345678901234567890123456789012345678901234567890123456789012345678901234567890") = 57edf4a22be3c955ac49da2e2107b67a
*/
void hashWords(std::vector<std::string> &passwords)
{
    for (std::string pass: passwords)
    {
        std::string out = paca::myMD5(pass);
        std::cout << "Result of hashing \'" << pass << "\':" << std::endl;
        std::cout << out << "\n------------------------------------------------------------------------"<< std::endl;
    }
}

int main()
{
    // Can't copy a string into a bitset. Must be converted to char first, and then bits.
    /* 
    std::string test = "password";
    std::string bits;
    for (char c: test)
    {
        int letter = c;
        std::cout << "letter " << c << ": " << letter << std::endl;
        std::bitset<8> temp(letter);    // Convert to bits.
        for (int i = 0; i < 8; i++)
        {
            std::cout << "bit " << i << ": " << temp[i] << std::endl;
        }
        bits += temp.to_string();
        std::cout << "bits: " << bits << std::endl;
    }
    */
    // unsigned long long int x = std::numeric_limits<unsigned long long int>::max();
    // std::cout << x << std::endl;
    std::cout << (isBigEndian() ? "Big Endian" : "Little Endian") << std::endl;


    std::vector<std::string> passwords = {"password", "The quick brown fox jumps over the lazy dog", ""};
    // hashWords(passwords);
    std::cout << "char 0x00: " << (unsigned char) 0x00 << '\n';
    std::cout << "int 0x00: " << (int) 0x00 << '\n';

    // std::cout << "\'test\' as bit string: " << std::bitset<32>("test") << '\n'; // Illegal
    std::cout << "unsigned char 'c' to bitset: " << std::bitset<8>((unsigned char) 'c').to_string() << '\n';
    return 0;
}