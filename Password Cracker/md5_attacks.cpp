#include "mypasscrack.hpp"
#include "md5_attacks.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

/*
    Generate & Store Values
    We read in a plain textfile (*.txt) and then generate the hashes. We store the results as a JSON file.

    TODO: 
    1. Figure out how to write individual lines to a file, because I literally ran out of buffer space.
    Code technically works though.""

    Best practices for file reading and error output here: 
    https://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/

*/
void md5_attacks::generateHashes(const std::string &fileInput, const std::string &fileOutput)
{
    std::cout << "input file: " << fileInput << "\n";
    std::cout << "output file: " << fileOutput << "\n";

    std::ifstream FI(fileInput);
    if (!FI.is_open())
    {
        perror(("Error while opening input file " + fileInput).c_str());
    }
    std::string line;
    nlohmann::json J; 
    while (std::getline(FI, line))
    {
        // std::cout << line << std::endl;
        std::string hash = paca::myMD5(line);
        J[hash] = line;
    }

    // Error handling if there was a problem.
    if (FI.bad())
    {
        perror(("Error while reading input file " + fileInput).c_str());
    }
    FI.close();

    std::ofstream FO(fileOutput);
    if (!FO.is_open())
    {
        perror(("Error while opening output file " + fileOutput).c_str());
    }
    // Pretty write JSON file.
    FO << std::setw(4) << J << std::endl;
    if (FI.bad())
    {
        perror(("Error while writing output file " + fileOutput).c_str());
    }

    FO.close();
}

/*
    Idea: Use Hashmap
    Assume we already have a hashmap mapping hashes to plaintext passwords. Then, we want to parse the
    file until we get a hash that matches, and return the password that it's attached to. If no match
    is found, return the string "None". We can take a filepath in, assuming it hashes mapped to passwords.
    If not, we try looking for "md5hashdict.json".
*/



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
    // std::cout << pass << " was not the hash" << std::endl;

    // Try every letter combination, and recurse.
    for (char letter: md5_attacks::allChars)
    {
        // Explore: try new letter with prefix.
        // Recurse: call function again on this new string.
        std::string possible = pass+letter;
        // std::cout << "Trying out: " << possible << std::endl;
        std::string downbelow = md5_attacks::hashDFS(hash, maxLength, possible);
        if (downbelow != "")
        {
            std::cout << downbelow << " was it!" << std::endl;
            return downbelow;
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