#ifndef PASS_CRACK_H
#define PASS_CRACK_H
#include <string>
#include <vector>
#include <bitset>

namespace paca
{
    std::string string_to_bitstring(std::string const &s);
    std::string myMD5(std::string const &input);
    std::vector<std::bitset<32>> chunk512_to_chunk32(std::bitset<512> huge);
}

#endif