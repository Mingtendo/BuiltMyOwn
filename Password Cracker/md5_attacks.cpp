#include "mypasscrack.hpp"
#include "md5_attacks.hpp"
#include <iostream>

/*
    Idea: Backtracking
    Basically, we want to try every combination possible. So we start with a-z, A-Z, 0-9. If those don't
    work, then we want to try every two-char combination. We'll set an artificial limit of 20 characters
    to keep it sane. 
*/
std::string md5_attacks::hashDFS(const std::string &hash, const int &maxLength, std::string &pass)
{
    if (pass.size() >= maxLength)
    {
        return (paca::myMD5(pass) == hash) ? pass : "";
    }

    // See if this prefix works.
    std::string hash_of_this = paca::myMD5(pass);
    if (hash_of_this == hash)
    {
        return pass;
    }

    // Try every letter combination, and recurse.
    for (char letter: md5_attacks::allChars)
    {
        // Explore: try new letter with prefix.
        // Recurse: call function again on this new string.
        std::string possible = pass+letter;
        if (possible != "")
        {
            return possible;
        }
        // Backtrack implicit since we just create a new prefix with the next letter.
    }

    // Nothing found, return empty.
    return "";
}

std::string md5_attacks::brute_force_cracker(const std::string &hash, uint16_t &maxLength)
{
    std::string temp = "";
    std::string result = hashDFS(hash, maxLength, temp);
    return (result == "") ? "No password found. :(" : result;
}