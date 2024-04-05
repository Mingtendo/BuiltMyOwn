#include "mypasscrack.hpp"
#include "md5_attacks.hpp"
#include <iostream>
#include <sstream>
#include <limits>
#include <cassert>
#include <algorithm>
#include <exception>
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
MD5 ("a") = cc175b9c0f1b6a831c399e269772661
MD5 ("abc") = 900150983cd24fb0d6963f7d28e17f72
MD5 ("password") = 5f4dcc3b5aa765d61d8327deb882cf99
MD5 ("message digest") = f96b697d7cb7938d525a2f31aaf161d0
MD5 ("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b
MD5 ("The quick brown fox jumps over the lazy dog") = 9e107d9d372bb6826bd81d3542a419d6
MD5 ("The quick brown fox jumps over the lazy dog.") = e4d909c290d0fb1ca068ffaddf22cbd0
MD5 ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") = d174ab98d277d9f5a5611c2c9f419d9f
MD5 ("12345678901234567890123456789012345678901234567890123456789012345678901234567890") = 57edf4a22be3c955ac49da2e2107b67a
*/
inline void hashWords(std::vector<std::string> &passwords)
{
    for (std::string pass: passwords)
    {
        std::string out = paca::myMD5(pass);
        std::cout << "Result of hashing \'" << pass << "\':\n";
        std::cout << out << "\n------------------------------------------------------------------------\n";
    }
}

inline bool cmdOptionExists(char *begin[], char *end[], const std::string &option)
{
    return std::find(begin, end, option) != end;
}

// Uses the stat struct, and then calls the stat function (what great naming) to check if the
// struct contains data or a 0. If it does, that means such a file doesn't exist.
// Read more here: https://pubs.opengroup.org/onlinepubs/7908799/xsh/sysstat.h.html 
inline bool fileExists(const std::string &filename)
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

/*
    TODO:
    Turn this into a bunch of switches? This code is pretty ugly, though it works.
*/
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
        std::vector<std::string> passwords = {"password", "The quick brown fox jumps over the lazy dog", "", "abc", "!\"�%^&*()"};
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
            outputFilePath = (argc == 4) ? argv[3] : "md5hashdict.json";
            
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
    
    return 0;
}