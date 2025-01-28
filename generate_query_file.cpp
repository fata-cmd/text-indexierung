#include <iostream>
#include <fstream>
#include "src/util.h"

std::random_device rd;
std::mt19937 rng(rd());
std::bernoulli_distribution bernoulli(0.1);

char generateRandomChar()
{
    std::uniform_int_distribution<int> dist(0, 51);

    // Generate random number and map to character
    int randomNum = dist(rng);
    if (randomNum < 26)
    {
        return 'A' + randomNum; // Uppercase letters
    }
    else
    {
        return 'a' + (randomNum - 26); // Lowercase letters
    }
}

void replace_random_char(std::string &s)
{
    auto idx = rng() % s.size();
    char new_char = generateRandomChar();
    s[idx] = new_char;
}

void generateQueryFile(const std::string &input,const std::string &output)
{
    size_t num_queries = 1e6;
    size_t word_count = 0;
    std::ofstream qfile(output);
    std::ifstream file(input);

    if (!file)
    {
        std::cerr << "Error: Unable to open file " << input << std::endl;
        return;
    }

    std::string word;

    std::string operations[3] = {"i", "c", "d"};

    while (file >> word && num_queries--)
    {
        auto op_idx = rand() % 3;
        if (bernoulli(rng))
        {
            replace_random_char(word);
        }
        if (word.back() != sentinel)
            word += sentinel;

        qfile << word << " " << operations[rand() % 3] << "\n";
        ++word_count;
    }
    std::cerr << "Number of queries: " << word_count << "\n";
    file.close();
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: <path_to_words_file>\n";
        return 0;
    }

    std::cerr << "cwd: " << std::filesystem::current_path() << "\n";
    std::string input_file = std::filesystem::absolute(argv[1]).string();
    std::filesystem::path input_path = std::filesystem::absolute(argv[1]);
    std::string query_file = (std::filesystem::current_path() / ("resources/queries/" + input_path.filename().string())).string();

    generateQueryFile(input_file, query_file);
    std::cout << query_file;
    std::cerr << "\n";
    return 1;
}