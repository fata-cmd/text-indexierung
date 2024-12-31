#pragma once
#include <iostream>
#include <fstream>
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <typeinfo>
#ifdef __GNUG__
#include <cxxabi.h>
#include <memory>
#endif

template <typename T>
std::string type_name() {
#ifdef __GNUG__
    int status = 0;
    std::unique_ptr<char, void(*)(void*)> res{
        abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status),
        std::free
    };
    return (status == 0) ? res.get() : typeid(T).name();
#else
    return typeid(T).name(); // For MSVC or other compilers
#endif
}

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
        if (word.back() != '\0')
            word += '\0';
        dict.insert_word(word);
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
