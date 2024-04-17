#include "mypasscrack.hpp"
#include "md5_attacks.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <codecvt>

/*
    Generating Dictionary TODO:
    1. Memory map passwords file and tokenize using LF ('\n') as the delimeter.
    2. For each password, record its offset from the beginning of the file and its MD5 hash in the index file (write records sequentially to a buffered output stream to a file).
    3. Memory map the index file and sort it in-place.

    Lookup TODO:
    1. Memory map the password and index files.
    2. Do a binary search in the index file for the hash.
    3. Use the found offset to get the password from the password memory map (from the offset to the next LF char).

    Use POSIX's mmap library to handle this. Can read more about it here: https://pubs.opengroup.org/onlinepubs/009604499/functions/mmap.html.
*/

// From ChatGPT
/// @brief Checks if a given string is valid UTF-8
/// @param input const std::string
/// @return true if string is UTF-8; false otherwise
inline bool isValidUTF8(const std::string& input) 
{
    for (size_t i = 0; i < input.size(); i++) 
    {
        unsigned char byte = static_cast<unsigned char>(input[i]);

        // Check if it's a single-byte character (0xxxxxxx)
        if (byte <= 0x7F) 
        {
            continue;
        }

        // Check if it's the start of a multi-byte character
        if (byte >= 0xC2 && byte <= 0xDF) 
        {
            if (i + 1 >= input.size() || (input[i + 1] & 0xC0) != 0x80) 
            {
                return false;
            }
            i++;
        } 
        else if (byte >= 0xE0 && byte <= 0xEF) 
        {
            if (i + 2 >= input.size() || (input[i + 1] & 0xC0) != 0x80 || (input[i + 2] & 0xC0) != 0x80)
            {
                return false;
            }
            i += 2;
        } 
        else if (byte >= 0xF0 && byte <= 0xF4) 
        {
            if (i + 3 >= input.size() || (input[i + 1] & 0xC0) != 0x80 || (input[i + 2] & 0xC0) != 0x80 || (input[i + 3] & 0xC0) != 0x80) 
            {
                return false;
            }
            i += 3;
        } 
        else 
        {
            // Invalid leading byte
            return false;
        }
    }

    return true;
}

/*
    DEPRECATED
    Get it so that it's a JSON Object containing other JSON objects.
*/
// Code from ChatGPT. Appends to the EOF.
inline bool writeToJSON(const std::string &fileOutput, nlohmann::json &jsonObject)
{
    // Used for testing to avoid generating 1 GB+ files.
    return true;
    // Open file in append mode.
    std::ofstream FO(fileOutput, std::ios::app);
    if (!FO.is_open())
    {
        perror(("Error while opening output file: " + fileOutput).c_str());
        return false;
    }

    // Move file pointer to the end of the file.
    FO.seekp(0, std::ios::end);

    // Pretty write the JSON object.
    FO << std::setw(4) << jsonObject << "," << std::endl;
    if (FO.bad())
    {
        perror(("Error while writing output file " + fileOutput).c_str());
        return false;
    }
    FO.close();
    return true;
}

/*
    Generate & Store Values
    We read in a plain textfile (*.txt) and then generate the hashes. We store the results as a JSON file.

    What Works:
    MD5 Hashing algorithm works with UTF-8 characters. After all, MD5 only cares about the bytes that are
    fed in, not how they are interpreted. So while it can generate the hashes correctly, there are issues
    writing to the file.

    Things learned thus far:
    1. Convert regular strings to UTF-8 using std::wstring_convert<std::codecvt_utf8<wchar_t>>.
    2. Can NOT write nested JSON objects.
    3. Can NOT write JSON objects line by line.
    4. Write to the end of a JSON file, to finagle writing to end of JSON in chunks.
    5. std::wstring_convert is deprecated in C++17, and pending removal in C++26.

    TODO: 
    2. Figure out how to write UTF-8 characters to a file.

    Best practices for file reading and error output here: 
    https://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/

*/

/// @brief Generate all MD5 hashes for passwords from a file.
/// @param fileInput Absolute path for the input file containing LF-delimited passwords
/// @param fileOutput Absolute path for the output file containing LF-delimited passwords
void md5_attacks::generateHashes(const std::string &fileInput, const std::string &fileOutput)
{
    std::cout << "input file: " << fileInput << "\n";
    std::cout << "output file: " << fileOutput << "\n";

    // Start reading from input file.
    std::ifstream FI(fileInput);
    if (!FI.is_open())
    {
        perror(("Error while opening input file " + fileInput).c_str());
        return;
    }

    std::string line;

    while (std::getline(FI, line))
    {
        // std::cout << line << std::endl;
        if (FI.bad())
        {
            perror(("Error while reading input file " + fileInput).c_str());
            break;
        }

        std::string hash = paca::myMD5(line);
        
    }
    FI.close();
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