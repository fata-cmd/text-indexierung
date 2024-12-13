#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <cmath>
#include <cassert>
#include <random>
#include <chrono>
#include <random>
#include <immintrin.h>
#include "trie.h"
#include "util.h"

const std::string file = "/usr/share/dict/words";

TEST_CASE("Read file into dict"){
    auto dict = readFileIntoDict<Trie<VariableSIMD>>(file);
}

const auto strings = readFileIntoSet(file);
std::vector<bool> to_be_inserted;

template <typename T>
void test_trie()
{
    Trie<T> t{};
    size_t i = 0;
    for (const auto& s : strings)
    {
        if (to_be_inserted[i++])
        {
            t.insert(s);
        }
    }
    i = 0;
    for (const auto& s : strings)
    {
        if (to_be_inserted[i++])
        {
            CHECK(!t.insert(s));
            CHECK(t.contains(s));
            CHECK(t.delete_word(s));
            CHECK(!t.contains(s));
            CHECK(!t.delete_word(s));
            CHECK(t.insert(s));
            CHECK(t.delete_word(s));
        }
        else
        {
            CHECK(!t.contains(s));
            CHECK(t.insert(s));
            CHECK(t.contains(s));
            CHECK(t.delete_word(s));
            CHECK(!t.delete_word(s));
            CHECK(t.insert(s));
            CHECK(t.delete_word(s));
        }
    }
}

TEST_CASE("init")
{
    for (size_t i = 0; i < strings.size(); ++i)
    {
        to_be_inserted.emplace_back(rand() % 2);
    }
}

TEST_CASE("Fixed")
{
    test_trie<Fixed>();
}

TEST_CASE("Variable")
{
    test_trie<Variable>();
}

TEST_CASE("VariableSIMD")
{
    test_trie<VariableSIMD>();
}

TEST_CASE("Hash")
{
    test_trie<HashMap>();
}