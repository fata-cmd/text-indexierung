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

std::unordered_map<HashMapGlobal::key_t, HashMapGlobal *> HashMapGlobal::map;
HashMapGlobal::key_t HashMapGlobal::id_counter;

const std::string file = "/home/fata/repos/text_indexierung/resources/input/words.txt";

TEST_CASE("Read file into dict")
{
    auto dict = readFileIntoDict<Trie<VariableSIMD>>(file);
}

const auto strings = readFileIntoSet(file);
std::vector<bool> to_be_inserted;

template <typename T>
void test_trie()
{
    Trie<T> t{};
    size_t i = 0;
    for (const auto &s : strings)
    {
        if (to_be_inserted[i++])
        {
            t.insert_word(s);
        }
    }
    i = 0;
    unsigned tried = 100;
    for (const auto &s : strings)
    {
        if (tried)
        {
            --tried;
            auto tmp = s;
            tmp.pop_back();
            tmp.pop_back();
            unsigned chars = 10;
            while (chars--)
            {
                tmp = tmp + static_cast<char>((std::rand() % (UCHAR_MAX - 2)) + 1);
            }
            tmp.push_back(sentinel);
            REQUIRE_MESSAGE(!t.contains(tmp), tmp);
            REQUIRE_MESSAGE(t.insert_word(tmp), tmp);
            REQUIRE_MESSAGE(t.contains(tmp), tmp);
        }
        if (to_be_inserted[i++])
        {
            CHECK(!t.insert_word(s));
            CHECK(t.contains(s));
            CHECK(t.delete_word(s));
            CHECK(!t.contains(s));
            CHECK(!t.delete_word(s));
            CHECK(t.insert_word(s));
            CHECK(t.delete_word(s));
        }
        else
        {
            CHECK(!t.contains(s));
            CHECK(t.insert_word(s));
            CHECK(t.contains(s));
            CHECK(t.delete_word(s));
            CHECK(!t.delete_word(s));
            CHECK(t.insert_word(s));
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

TEST_CASE("HashMap")
{
    test_trie<HashMap>();
}

TEST_CASE("HashMapGlobal")
{
    test_trie<HashMapGlobal>();
}