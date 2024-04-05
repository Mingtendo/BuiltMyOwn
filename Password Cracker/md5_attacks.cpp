#include "mypasscrack.hpp"
#include "md5_attacks.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <codecvt>

/*
    TODO:
    Get it so that it's a JSON Object containing other JSON objects.
*/
// Code from ChatGPT. Appends to the EOF.
inline bool writeToJSON(const std::string &fileOutput, nlohmann::json &jsonObject)
{
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

    TODO: 
    2. Figure out how to write UTF-8 characters to a file.

    Best practices for file reading and error output here: 
    https://gehrcke.de/2011/06/reading-files-in-c-using-ifstream-dealing-correctly-with-badbit-failbit-eofbit-and-perror/

*/
void md5_attacks::generateHashes(const std::string &fileInput, const std::string &fileOutput)
{
    std::cout << "input file: " << fileInput << "\n";
    std::cout << "output file: " << fileOutput << "\n";

    std::ofstream FO(fileOutput);
    if (!FO.is_open())
    {
        perror(("Error while opening output file " + fileOutput).c_str());
        return;
    }
    // Open JSON object.
    FO << "{\n";

    if (FO.bad())
    {
        perror(("Error while writing output file " + fileOutput).c_str());
        return;
    } 

    FO.close();

    std::ifstream FI(fileInput);
    if (!FI.is_open())
    {
        perror(("Error while opening input file " + fileInput).c_str());
        return;
    }

    std::string line;
    nlohmann::json J; 

    while (std::getline(FI, line))
    {
        // std::cout << line << std::endl;
        if (FI.bad())
        {
            perror(("Error while reading input file " + fileInput).c_str());
            break;
        }

        std::string hash = paca::myMD5(line);
        std::cout << "current line: " << line << std::endl;
        // Convert string to wstring
        std::wstring wideString = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(line);

        J[hash] = line;
        if (J.size() == 1000)
        {
            std::cout << "last to write was: " << line << "\n";
            bool res = writeToJSON(fileOutput, J);
            J = {};
            if (!res)
            {
                break;
            }
        }
        
    }
    FI.close();

    // Reopen file in append mode.
    FO.open(fileOutput, std::ios::app);
    if (!FO.is_open())
    {
        perror(("Error while opening output file " + fileOutput).c_str());
        return;
    }

    // Close JSON object.
    FO << "}\n";

    if (FO.bad())
    {
        perror(("Error while writing output file " + fileOutput).c_str());
        return;
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