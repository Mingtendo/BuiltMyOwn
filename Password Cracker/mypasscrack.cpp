#include "mypasscrack.hpp"
#include <iostream>
#include <sstream>

/*
    Things Learned So Far:
    1. To convert char to int ('5' to 5), do char - '0'.
    2. std::bitset<> constructor allows std::strings as input.
    3a. std::bitset<>[0] is the rightmost bit (smallest bit); opposite of most array-like containers.
    3b. In order to manually turn integer bits (1/0) from a std::vector to a std::bitset, be sure to start at bitset.size()-1 and go backwards to 0 to maintain order.
    4. To convert integers into a string of hexadecimal digits, put them into a std::stringstream with std::hex, then pass the stream into a string constructor (and convert the stream into a string).
*/

// Put in string of chars, and get its form as a vector of bits. 
std::string paca::string_to_bitstring(std::string const &s)
{
    std::string result = "";

    for (unsigned char c: s)
    {
        int letter = c;
        std::bitset<8> temp(letter);
        result += temp.to_string();
    }

    return result;
}

// Used to break chunks of 512 bits into 16 x 32-bit words.
std::vector<std::bitset<32>> paca::chunk512_to_chunk32(std::bitset<512> huge)
{
    std::vector<std::bitset<32>> result;
    std::string huge_string = huge.to_string();

    for (std::size_t i = 0; i < huge_string.size(); i+=32)
    {
        std::string chunk = huge_string.substr(i, 32);
        // std::cout << "chunk " << i/32 << ": " << chunk << std::endl;
        result.push_back(std::bitset<32>(chunk));
    }

    return result;
}

/*
    Rotates a uint32_t n to the left by c bits, and returns the result.
    Taken from https://stackoverflow.com/questions/776508/best-practices-for-circular-shift-rotate-operations-in-c 
    Honestly I did not want to write a function to shift bits so I hope this works.

    DONE: NO FURTHER TESTING NEEDED.
    WHY: Did not change the hash output (even though the hashes are wrong).
*/ 
uint32_t paca::rotl32(uint32_t n, unsigned int c)
{
    const unsigned int mask = (CHAR_BIT*sizeof(n)-1);

    c &= mask;
    return (n<<c)|(n>>((-c)&mask));
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
    uint64_t padding = bits.size();  // Because unsigned ints handle overflowing numbers as defined by C++ standards, this is automatically modulo 2^64.
    std::cout << "padding: " << padding << std::endl;

    std::string to_pad = std::bitset<64>(padding).to_string();
    std::cout << "to_pad: " << to_pad << std::endl;
    
    // Add 1 bit, regardless of whether the string is 448-bits exactly or not.
    bits += '1';

    // Add '0' until message length in bits % 512 == 448.
    while (bits.size()%512 < 448)
    {
        bits += '0';
    }

    std::cout << "0-padded bits size: " << bits.size() << ", size%512: " << bits.size()%512 << std::endl;

    // Add padding bits to message.
    bits += to_pad;

    std::cout << "padded to 512-bit multiple: " << bits.size() << ", size%512: " << bits.size()%512 << std::endl;

    std::cout << "padded message in bits:\n" << bits << std::endl;



    // DONE: Break into 512-bit chunks.
    std::vector<std::bitset<512>> allInputBits;

    for (std::size_t chunk = 0; chunk < bits.size(); chunk+=512)
    {
        std::bitset<512> to_push(bits.substr(chunk, 512));  // Automatic conversion of bit string to bits.
        allInputBits.push_back(to_push);
        std::cout << "chunk " << chunk/512 << ":\n" << to_push << std::endl;
    }

    // Main algorithm

    // Initial variables
    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe;
    uint32_t d0 = 0x10325476;
    // Specifies per-round shift amounts. Taken from Wikipedia.
    uint32_t s[64] = 
    {
        7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22, 
        5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20, 
        4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23, 
        6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
    };
    // Constants generated using K[i] := floor(2^32*abs(sin(i+1))), where i is [0, 63] inclusive.
    uint32_t K[64] = 
    {  
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
    };
    
    // Operates on 128-bit state, dividing into four 32-bit words, A thru D. 
    /*
        ^= XOR, assign
        &= AND, assign
        |= OR, assign
        ~var NOT var
        <<=X Shift left by X bits, assign
        >>=X Shift right by X bits, assign
    */
    for (std::bitset<512> bs: allInputBits)
    {
        std::vector<std::bitset<32>> M = chunk512_to_chunk32(bs);
        uint32_t A = a0, B = b0, C = c0, D = d0;
        
        // Calculate hashes for this chunk.
        for (unsigned int i = 0; i < 64; i++)
        {
            uint32_t F, g;
            if (0 <= i && i <= 15)
            {
                F = D^(B&(C^D));
                g = i;
            }
            else if (16 <= i && i <= 31)
            {
                F = C^(D&(B^C));
                g = (5*i+1)%16;
            }
            else if (32 <= i && i <= 47)
            {
                F = B^C^D;
                g = (3*i+5)%16;
            }
            else if (48 <= i && i <= 63)
            {
                F = C^(B|(~D));
                g = (7*i)%16;
            }

            uint32_t cur = M[g].to_ulong();
            F = F+A+K[i]+cur;
            A = D;
            D = C;
            C = B;
            B = B+paca::rotl32(F, s[i]);
        }

        // Add chunk's hash to overall result.
        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }

    std::stringstream strings;
    strings << std::hex << a0 << b0 << c0 << d0;
    std::string digest(strings.str());

    return digest;
}
