#ifndef PASS_CRACK_H
#define PASS_CRACK_H
#include <string>

namespace paca
{
    std::string string_to_bitstring(std::string const &s);
    std::string myMD5(std::string const &input);
}

#endif