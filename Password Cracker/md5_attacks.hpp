#ifndef MD5_ATTACKS_H
#define MD5_ATTACKS_H
#include <string>
#include <cstdint>


namespace md5_attacks
{
    std::string hashDFS(const std::string &hash, const int &maxLength, std::string &pass);
    std::string brute_force_cracker(const std::string &hash, uint16_t &maxLength);

    const std::string allChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%&";
}

#endif