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
std::vector<bool> inserted;

template <typename T>
void test_trie(){
    Trie<T> t{};
    for(size_t i = 0; i < strings.size(); ++i){
        if (inserted[i])
            t.insert(strings[i].c_str());
    }
    for(size_t i = 0; i < strings.size(); ++i){
        auto word = strings[i].c_str();
        if (inserted[i]){
            CHECK(!t.insert(word));
            CHECK(t.contains(word));
            CHECK(t.delete_word(word));
            CHECK(!t.contains(word));
            CHECK(!t.delete_word(word));
            CHECK(t.insert(word));
            CHECK(t.delete_word(word));
        } else {
            CHECK(!t.contains(word));
            CHECK(t.insert(word));
            CHECK(t.contains(word));
            CHECK(t.delete_word(word));
            CHECK(!t.delete_word(word));
            CHECK(t.insert(word));
            CHECK(t.delete_word(word));
        }
    }
}

TEST_CASE("init"){
    for(size_t i = 0; i < strings.size(); ++i){
        inserted.emplace_back(rand() % 2);
    }
}

TEST_CASE("Fixed"){
    test_trie<Fixed>();
}

TEST_CASE("Variable"){
    test_trie<Variable>();
}

TEST_CASE("VariableSIMD"){
    test_trie<VariableSIMD>();
}

TEST_CASE("Hash"){
    test_trie<HashMap>();
}