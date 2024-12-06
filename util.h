#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>

auto readAndProcessFile(const std::string& filename) {
    std::ifstream file(filename);
    std::unordered_set<std::string> processedWords;
    if (!file) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return processedWords;
    }

    std::string word;

    while (file >> word) { // Read word by word
        word += '\0';      // Append null character
        processedWords.emplace(word); // Store the processed word
    }

    file.close();
    return processedWords;
}

auto uc_str(const std::string& s){
    return reinterpret_cast<unsigned char*>(const_cast<char*>(s.c_str()));
}