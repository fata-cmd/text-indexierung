#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>

auto readFileIntoSet(const std::string &filename)
{
    std::ifstream file(filename);
    std::unordered_set<std::string> processedWords;
    if (!file)
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return processedWords;
    }

    std::string word;

    while (file >> word)
    {                                 
        word += '\0';                 
        processedWords.emplace(word);
    }

    file.close();
    return processedWords;
}

template <typename SDict>
auto readFileIntoDict(const std::string &filename)
{
    std::ifstream file(filename);
    SDict dict{};
    if (!file)
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return dict;
    }

    std::string word;

    while (file >> word)
    {                              
        word += '\0';              
        dict.insert(word);
    }

    file.close();
    return dict;
}

auto uc_str(const char *s)
{
    return reinterpret_cast<unsigned char *>(const_cast<char *>(s));
}

auto uc_str(const std::string &s)
{
    return reinterpret_cast<unsigned char *>(const_cast<char *>(s.c_str()));
}
