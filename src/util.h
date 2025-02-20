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

constexpr u_char sentinel = '\0';

// return a type name as a string
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
    return "";
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
        if (word.back() != sentinel)
            word += sentinel;
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
        if (word.back() != sentinel)
            word += sentinel;
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
