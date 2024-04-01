#include "mypasscrack.hpp"
#include "md5_attacks.hpp"
#include <iostream>
#include <sstream>
#include <limits>
#include <cassert>
#include <algorithm>
#include <exception>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

bool isBigEndian()
{
    uint16_t word = 1; // 0x0001
    uint8_t *firstbyte = (uint8_t *) &word; // points to first byte of word
    bool biglittle = !(*firstbyte);
    return biglittle;
}

/*
MD5 test suite:

MD5 ("") = d41d8cd98f00b204e9800998ecf8427e
using ints: 227b7f48d21283f63bc9bbc15b44ea1a
fixed padding endianness, fixed construction of 16x32-bit chunks: 2520df5474d1b17877bf08654076c48

MD5 ("a") = cc175b9c0f1b6a831c399e269772661
MD5 ("abc") = 900150983cd24fb0d6963f7d28e17f72

MD5 ("password") = 5f4dcc3b5aa765d61d8327deb882cf99
using ints: 5caf9f3fb5f5a16e36bc1224a47a1587
fixed padding endianness, fixed construction of 16x32-bit chunks: db90b03bf0837cc7b550575a6a6d4ad8

MD5 ("message digest") = f96b697d7cb7938d525a2f31aaf161d0
MD5 ("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b

MD5 ("The quick brown fox jumps over the lazy dog") = 9e107d9d372bb6826bd81d3542a419d6
using ints: da0992e2976c86cdd0bfe0a79d5a5555
fixed padding endianness, fixed construction of 16x32-bit chunks: 4b0334e4c0f11a0fa4bdc119a8b7e41d

MD5 ("The quick brown fox jumps over the lazy dog.") = e4d909c290d0fb1ca068ffaddf22cbd0

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

bool cmdOptionExists(char *begin[], char *end[], const std::string &option)
{
    return std::find(begin, end, option) != end;
}

inline bool fileExists(const std::string &filename)
{
    struct stat buffer;
    return (stat (filename.c_str(), &buffer) == 0);
}

int main(int argc, char *argv[])
{
    // argv is an array of pointers to char, argv+argc is a pointer to the end of the array
    if (cmdOptionExists(argv, argv+argc, "-h") || cmdOptionExists(argv, argv+argc, "-hash"))
    {
        try
        {
            std::string arg1(argv[1]);
            std::string password(argv[2]);

            std::vector<std::string> tohash = {password};
            hashWords(tohash);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception caught: " << e.what() << std::endl;
        }
    }
    else if (cmdOptionExists(argv, argv+argc, "-help"))
    {
        std::cout << "-help                     display this message \n";
        std::cout << "-hash [password]          [alias -h] produces MD5 hash of password \n";
        std::cout << "-brute [hash] [p_len]     [alias -bf] uses brute-force to crack hash using passwords up to p_len chars \n";
        std::cout << "-gen [abs_file_path] <output_file_name>  [alias -g] generates hashes from file \n";
        std::cout << "Using no parameter input displays hashes of test strings." << std::endl;
    }
    else if (cmdOptionExists(argv, argv+argc, "-brute") || cmdOptionExists(argv, argv+argc, "-bf"))
    {   
        try
        {
            std::string password(argv[2]);
            std::string length = argv[3];
            uint16_t maxlength = std::stoi(length);
            std::string cracked = md5_attacks::brute_force_cracker(password, maxlength);
            std::cout << cracked << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception caught: " << e.what() << std::endl;
        }
        
    }
    else if (cmdOptionExists(argv, argv+argc, "-test"))
    {
        std::vector<std::string> passwords = {"password", "The quick brown fox jumps over the lazy dog", "", "abc"};
        hashWords(passwords);
    }
    // Generate hashes. Check first that inputs are valid and output file doesn't exist.
    else if (cmdOptionExists(argv, argv+argc, "-gen") || cmdOptionExists(argv, argv+argc, "-g"))
    {
        if (argc < 3)
        {
            std::cout << "Not enough arguments." << std::endl;
            return -1;
        }

        std::string inputFilePath, outputFilePath;
        try
        {
            inputFilePath = argv[2];
            outputFilePath = (argc == 4) ? argv[3] : "md5hashdict.txt";
            
            if (!fileExists(inputFilePath))
            {
                throw std::invalid_argument("file doesn't exist; give absolute path");
            }
            if (fileExists(outputFilePath))
            {
                throw std::invalid_argument("file already exists");
            }

            // md5_attacks::generateHashes(inputFilePath, outputFilePath);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception caught: " << e.what() << std::endl;
            return 1;
        }

        md5_attacks::generateHashes(inputFilePath, outputFilePath);
        
    }
    else if (cmdOptionExists(argv, argv+argc, "-da"))
    {
        std::string input = "2bdb742fc3d075ec6b73ea414f27819a";
        
    }
    else
    {
        std::cout << "Flag not found. Please use -help for more info." << std::endl;
    }

    
    // This is all stuff I learned or used to learn. Keeping for posterity.
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

    // std::cout << "char 0x00: " << (unsigned char) 0x00 << '\n'; // Shows nothing.
    // std::cout << "int 0x00: " << (int) 0x00 << '\n';

    // std::cout << "\'test\' as bit string: " << std::bitset<32>("test") << '\n'; // Illegal

    // These 2 are the same.
    // std::cout << "unsigned char 'c' to bitset: " << std::bitset<8>((unsigned char) 'c').to_string() << '\n';
    // std::cout << "unsigned char 0b01100011: " << (unsigned char) 0b01100011 << '\n';

    // std::cout << (isBigEndian() ? "Big Endian" : "Little Endian") << std::endl;
    // uint32_t a = 0x0a1a56, b = 0x561a0a;  // 0x0a1a56 is 662102. This is the correct way to store hex nums.
    // std::cout << "a: " << a << "\n" << "b: " << b << "\n";

    // uint32_t testA = 0xd98c1dd4;
    // uint32_t testA_result = paca::uint32_t_little_to_big_endian(testA);

    // std::stringstream ss;
    // ss << std::hex << testA_result;
    // std::string converted(ss.str());
    // std::cout << converted << std::endl;

    // Create new 32-bit value from uint8_t[4]. Top two lines are little-endian, last line is big-endian.
    // uint32_t newvalue = (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | buffer[0];
    // uint32_t newvalue = (uint32_t) buffer[0] | (uint32_t)(buffer[1] << 8) | (uint32_t)(buffer[2] << 16) | (uint32_t)(buffer[3] << 24);
    // uint32_t newvalue = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
    
    return 0;
}