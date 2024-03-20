#include "mypasscrack.hpp"
#include <iostream>
#include <sstream>
#include <array>

/*
    Things Learned So Far:
    1. To convert char to int ('5' to 5), do char - '0'.
    2. std::bitset<> constructor allows std::strings as input.
    3a. std::bitset<>[0] is the rightmost bit (smallest bit); opposite of most array-like containers.
    3b. In order to manually turn integer bits (1/0) from a std::vector to a std::bitset, be sure to start at bitset.size()-1 and go backwards to 0 to maintain order.
    4. To convert integers into a string of hexadecimal digits, put them into a std::stringstream with std::hex, then pass the stream into a string constructor (and convert the stream into a string).
    5. Though bitsets can take strings, they cannot contain chars that are not '1' or '0'.
    6. How to bitshift and mask integers.
    7. How to break a 64-bit uint into 8 8-bit uints.
*/

/*
    Things to Try:
    I'm like 100% sure this algorithm doesn't work because of how the bits are converted, transformed, and stored.
    X1. Adding the letters in reverse order by appearance ("password" -> "drowssap").
    2. Add the letters in reverse endian order (add each bit to bitset individually), but not reverse appearance order.
    3. Both 1 and 2.
    4. Any combination of 1 and 2, plus padding in reverse?
    5. Treat the std::string like bytes and just pad with bytes instead of individual bits.
    6. Any combination of 1, 2, 4, and 5.

    New Idea:
    The password is already a bunch of bits. Each char in a string is 8 bits or 1 byte. There are
    512 bits per block, or 64 bytes. We can simply pad by adding 0x80 which is 1 followed by 7 0s.
    So we just pad by 0x80 and 0x00 until we get a string length of 448 (modulo by 512). We do 
    padding by converting the input length to uint64_t (auto mods by 2^64), then to a bitset and
    then into a string. We then split it into 512-bit chunks (may need to change how the chunking
    is done if it still doesn't work).

    New Idea 2:
    So, hashing '' also yields a wrong result. This means that the problem likely is not caused by the string-to-
    bitstring conversion, but rather something further down the line. Perhaps the process of dividing the 512-bit
    bitstrings into 32-bit bitstrings is not correct?
*/

// Put in string of chars, and get its form as a vector of bits. 
// TODO: Try adding the chunks backwards? So the bits are in order
std::vector<uint8_t> paca::string_to_bitstring(std::string const &s)
{
    std::vector<uint8_t> result;

    for (uint8_t c: s)
    {
        result.push_back(c);
    }

    return result;
}

// Used to break chunks of 512 bits into 16 x 32-bit words.
std::vector<std::bitset<32>> paca::chunk512_to_chunk32(std::bitset<512> huge)
{
    std::vector<std::bitset<32>> result;
    std::string huge_string = huge.to_string();

    // std::size_t i = 0;
    // while (i < huge.size())
    // {
    //     std::bitset<32> chunka;
    //     for (int j = 0; j < 32; j++)
    //     {
    //         chunka[j] = huge[i];
    //         i++;
    //     }
    //     std::cout << "chunka: " << chunka << "\n";
    //     result.push_back(chunka);
    // }

    for (std::size_t i = 0; i < huge_string.size(); i+=32)
    {
        std::string chunk = huge_string.substr(i, 32);
        result.push_back(std::bitset<32>(chunk));
        std::cout << "chunk " << i/32 << ": " << chunk << std::endl;
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
/// \param n the 32-bit integer to rotate
/// \param c the number of bits to rotate by
/// \return the integer rotated by that many bits
uint32_t paca::rotl32(uint32_t n, unsigned int c)
{
    const unsigned int mask = (CHAR_BIT*sizeof(n)-1);

    c &= mask;
    return (n<<c)|(n>>((-c)&mask));
}

// See source for this code: https://stackoverflow.com/a/35153234.
/// @brief Takes a 64-bit integer and splits it into 8 bytes. Endian-agnostic.
/// @param huge 
/// @return std::vector containing 8 unsigned 8-bit ints (same as unsigned char)
std::vector<uint8_t> paca::uint64_t_to_vector(const uint64_t &huge)
{
    std::vector<uint8_t> result(sizeof(uint64_t));
    for (size_t i = 0; i < sizeof(uint64_t); i++)
    {
        result[i] = uint8_t((huge>>8*(7 - i)) & 0xFF);
    }

    return result;
}

// Code by ChatGPT.
/// @param huge an unsigned 64-bit integer
/// @return std::vector containing 8 unsigned 8-bit ints in little endian
std::vector<uint8_t> paca::uint64_t_to_vector_chatgpt(uint64_t huge)
{
    std::vector<uint8_t> result(sizeof(uint64_t));
    for (size_t i = 0; i < sizeof(uint64_t); i++)
    {
        result[sizeof(uint64_t)-i-1] = (huge>>(i*8)) & 0xFF;
    }

    return result;
}

// Code here inspired from: https://forum.arduino.cc/t/convert-4-uint8_t-into-one-uint32_t/577243/2. 
std::vector<std::array<uint32_t, 16>> paca::separate_into_16(std::vector<uint8_t> &bytesVector)
{
    std::vector<std::array<uint32_t, 16>> result;
    size_t pointer = 0;
    while (pointer < bytesVector.size())
    {
        std::array<uint32_t, 16> temp;
        for (int i = 0; i < 16; i++)
        {
            uint8_t buffer[4];
            // Create buffer of four bytes.
            for (int j = 0; j < 4; j++)
            {
                buffer[j] = bytesVector[pointer+j];   
            }
            // Create new 32-bit value from uint8_t[4]. Top line is little-endian, bot line is big-endian.
            // uint32_t newvalue = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
            uint32_t newvalue = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
            temp[i] = newvalue;
            pointer += 4;
        }
        result.push_back(temp);
    }

    return result;
}

// Takes in an ASCII string and outputs a 128-bit digest in hex as a string.
std::string paca::myMD5(std::string const &input)
{
    // Store string.
    std::vector<uint8_t> bitstring = paca::string_to_bitstring(input);

    // Pad the string so that it's divisible by 512.
    /*
        Padding is done by adding a single 1 bit at the end of the message, followed
        by as many 0 needed to bring the length of the message up to 64 bits less than
        a multiple of 512. The remaining bits are filled up with 64 bits representing
        the length of the original message, modulo 2^(64).
    */

   // DONE: How to convert bit string to bits.
   // DONE: Pad the message.
   // The 64-bit padding length refers to the length of the message in bits (from RFC 1321).
    uint64_t padding = input.size()*8;  // Because unsigned ints handle overflowing numbers as defined by C++ standards, this is automatically modulo 2^64.
    std::cout << "padding: " << padding << std::endl;
    
    // Add 1 bit, regardless of whether the string is 448-bits exactly or not.
    uint8_t one = 0x80, zero = 0x00;
    bitstring.push_back(one);

    // Add bytes of 0 until we have 8 bytes left.
    while (bitstring.size()%64 != 56)
    {
        bitstring.push_back(zero);
    }

    std::cout << "0-padded bits size: " << bitstring.size() << ", size%512: " << (bitstring.size())%512 << std::endl;

    // Add padding bits to message.
    std::vector<uint8_t> to_pad = paca::uint64_t_to_vector_chatgpt(padding);
    for (uint8_t byte: to_pad)
    {
        bitstring.push_back(byte);
    }

    std::cout << "padded to 512-bit multiple: " << bitstring.size()*8 << ", size%512: " << (bitstring.size()*8)%512 << std::endl;

    // DONE: Break into arrays containing 16 32-bit words each.
    std::vector<std::array<uint32_t, 16>> allInputBits = separate_into_16(bitstring);

    // MAIN ALGORITHM
    // Initial variables
    uint32_t a0 = 0x01234567;
    uint32_t b0 = 0x89abcdef;
    uint32_t c0 = 0xfedcba98;
    uint32_t d0 = 0x76543210;
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
    for (std::array<uint32_t, 16> M: allInputBits)
    {
        uint32_t A = a0, B = b0, C = c0, D = d0;
        
        // Calculate hashes for this chunk.
        for (uint32_t i = 0; i < 64; i++)
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

            F = F+A+K[i]+M[g];
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
