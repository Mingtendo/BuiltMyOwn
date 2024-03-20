#ifndef PASS_CRACK_H
#define PASS_CRACK_H
#include <string>
#include <vector>
#include <bitset>
#include <cstdint>

namespace paca
{
    std::vector<uint8_t> string_to_bitstring(std::string const &s);
    std::string myMD5(std::string const &input);
    std::vector<std::bitset<32>> chunk512_to_chunk32(std::bitset<512> huge);
    uint32_t rotl32(uint32_t n, unsigned int c);
    std::vector<uint8_t> uint64_t_to_vector(const uint64_t &huge);
    std::vector<std::array<uint32_t, 16>> separate_into_16(std::vector<uint8_t> &bytesVector);
    std::vector<uint8_t> uint64_t_to_vector_chatgpt(uint64_t huge);
    uint32_t uint32_t_little_to_big_endian(uint32_t bytes);
}

#endif