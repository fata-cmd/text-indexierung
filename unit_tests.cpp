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

const auto strings = readAndProcessFile("/usr/share/dict/words");
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
            auto us = uc_str(s);
            t.insert(us);
        }
    }
    i = 0;
    for (const auto& s : strings)
    {
        auto us = uc_str(s);
        if (to_be_inserted[i++])
        {
            CHECK(!t.insert(us));
            CHECK(t.contains(us));
            CHECK(t.delete_word(us));
            CHECK(!t.contains(us));
            CHECK(!t.delete_word(us));
            CHECK(t.insert(us));
            CHECK(t.delete_word(us));
        }
        else
        {
            CHECK(!t.contains(us));
            CHECK(t.insert(us));
            CHECK(t.contains(us));
            CHECK(t.delete_word(us));
            CHECK(!t.delete_word(us));
            CHECK(t.insert(us));
            CHECK(t.delete_word(us));
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