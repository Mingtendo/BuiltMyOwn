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
    6. To split larger ints into individual bytes, use bitshift operations, and mask with 0xFF to get only the right most bits.
    7. Use bitshifting to break a uint64_t into 8 bytes.
    8. At the end, gotta turn the little-endian bytes into big-endian bytes to display them in the correct order.
*/

/*
    Things to Try:
    Nothing, it actually works!!!
*/

/// @brief Returns vector of unsigned bytes in the same order as the chars appear.
/// @param s std::string reference
/// @return std::vector<uint8_t>
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

    for (std::size_t i = 0; i < huge_string.size(); i+=32)
    {
        std::string chunk = huge_string.substr(i, 32);
        result.push_back(std::bitset<32>(chunk));
    }

    return result;
}

/*
    Rotates a uint32_t n to the left by c bits, and returns the result.
    Taken from https://stackoverflow.com/questions/776508/best-practices-for-circular-shift-rotate-operations-in-c 
    Honestly I did not want to write a function to shift bits so I hope this works.
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

// UNUSED. See source for this code: https://stackoverflow.com/a/35153234.
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

/// @brief Modified code from ChatGPT. Stores result in little-endian.
/// @param huge an unsigned 64-bit integer
/// @return std::vector containing 8 unsigned 8-bit ints in little endian
std::vector<uint8_t> paca::uint64_t_to_vector_chatgpt(uint64_t huge)
{
    std::vector<uint8_t> result(sizeof(uint64_t));
    for (size_t i = 0; i < sizeof(uint64_t); i++)
    {
        result[i] = (huge>>(i*8)) & 0xFF;   // Bitshifts and masks to extract bytes, then stores in little-endian order.
    }

    return result;
}

// Code here inspired from: https://forum.arduino.cc/t/convert-4-uint8_t-into-one-uint32_t/577243/2. 
// TODO: Require input validation that bytesVector's size is multiple of 64.
/// @brief Takes a vector of unsigned bytes and arranges them in little-endian order 32-bit words. Stores them in arrays of 16 at a time. Requires input to have a size that is a multiple of 64.
/// @param bytesVector reference to std::vector<uint8_t> with size congruent to 0 (modulo 64)
/// @return std::vector containing arrays of 16 32-bit words each stored in little-endian order (assumes input bytes are big-endian order)
std::vector<std::array<uint32_t, 16>> paca::combine_into_little_endian_32_bit_words(std::vector<uint8_t> &bytesVector)
{
    std::vector<std::array<uint32_t, 16>> result;
    size_t pointer = 0;
    while (pointer < bytesVector.size())
    {
        std::array<uint32_t, 16> temp;
        int i = 0;
        while (i < 16 && pointer < bytesVector.size())
        {
            uint8_t buffer[4];
            // Create buffer of four bytes.
            for (int j = 0; j < 4; j++)
            {
                buffer[j] = bytesVector[pointer+j];
            }
            
            uint32_t newvalue = (uint32_t) buffer[0] | (uint32_t)(buffer[1] << 8) | (uint32_t)(buffer[2] << 16) | (uint32_t)(buffer[3] << 24);
            temp[i] = newvalue;
            i++;
            pointer += 4;
        }
        result.push_back(temp);
    }

    return result;
}

/// @brief Swaps the bytes around so that little endian bytes are re-ordered to big endian.
/// @param bytes takes in a uint32_t
/// @returns the bytes reordered in big endian
uint32_t paca::uint32_t_little_to_big_endian(uint32_t bytes)
{
    uint8_t temp[4];
    for (int i = 0; i < 4; i++)
    {
        temp[i] = (bytes >> (i*8)) & 0xFF;
        // std::cout << std::hex << temp[i] << "\n";    // Doesn't actually display in hex.
    }

    uint32_t reformed = temp[3] | (temp[2] << 8) | (temp[1] << 16) | (temp[0] << 24);
    return reformed;
}

// Takes in an ASCII string and outputs a 128-bit digest in hex as a string.
/// @brief Returns a string containing the MD5 hash of an input string. Not suitable for cryptography.
/// @param input std::string of ASCII characters
/// @returns std::string of hash in hexadecimal format
std::string paca::myMD5(std::string const &input)
{
    // Store string as bytes in the same order.
    std::vector<uint8_t> bitstring = paca::string_to_bitstring(input);

    /*
        Padding is done by adding a single 1 bit at the end of the message, followed
        by as many 0 needed to bring the length of the message up to 64 bits less than
        a multiple of 512. The remaining bits are filled up with 64 bits representing
        the length of the original message, modulo 2^(64).
    */
   
    // The 64-bit padding length refers to the length of the message in bits (from RFC 1321).
    uint64_t padding = input.size()*8;  // Because unsigned ints handle overflowing numbers as defined by C++ standards, this is automatically modulo 2^64.
    
    
    // Add 1 bit, regardless of whether the string is 448-bits exactly or not.
    uint8_t one = 0x80, zero = 0x00;
    bitstring.push_back(one);

    // Add bytes of 0 until we have 8 bytes left.
    while (bitstring.size()%64 != 56)
    {
        bitstring.push_back(zero);
    }

    // Add padding bits to message.
    std::vector<uint8_t> to_pad = paca::uint64_t_to_vector_chatgpt(padding);
    for (uint8_t byte: to_pad)
    {
        bitstring.push_back(byte);
    }

    // DONE: Break into arrays containing 16 32-bit words each.
    std::vector<std::array<uint32_t, 16>> allInputBits = paca::combine_into_little_endian_32_bit_words(bitstring);

    // MAIN ALGORITHM
    // Initial variables; written normally. These DO need to be in little endian.
    uint32_t a0, b0, c0, d0;
    a0 = 0x67452301;
    b0 = 0xefcdab89;
    c0 = 0x98badcfe;
    d0 = 0x10325476;
    
    // Operates on 128-bit state, dividing into four 32-bit words, A thru D. 
    /*
        --Bitwise Operations--
        A^B is A XOR B
        A&B is A AND B
        A|B is A OR B
        ~A  is NOT A
        A<<=B is leftshift A by B bits, assign new value to A
        A>>=B is rightshift A by B bits, assign new value to A
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
            // std::cout<< "M[" << g << "]: " << std::hex << M[g] << std::dec << "\n";
            F = F+A+paca::K[i]+M[g];
            A = D;
            D = C;
            C = B;
            B = B+paca::rotl32(F, paca::s[i]);
        }

        // Add chunk's hash to overall result.
        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }

    // Convert digest buffers into big-endian, and pass into stringstream as hexadecimal.
    std::stringstream strings;
    uint32_t bigEnd_a0, bigEnd_b0, bigEnd_c0, bigEnd_d0;
    bigEnd_a0 = paca::uint32_t_little_to_big_endian(a0);
    bigEnd_b0 = paca::uint32_t_little_to_big_endian(b0);
    bigEnd_c0 = paca::uint32_t_little_to_big_endian(c0);
    bigEnd_d0 = paca::uint32_t_little_to_big_endian(d0);
    strings << std::hex << bigEnd_a0 << bigEnd_b0 << bigEnd_c0 << bigEnd_d0;
    std::string digest(strings.str());

    return digest;
}
