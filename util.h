#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::vector<std::string> readAndProcessFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return {};
    }

    std::string word;
    std::vector<std::string> processedWords;

    while (file >> word) { // Read word by word
        word += '\0';      // Append null character
        processedWords.push_back(word); // Store the processed word
    }

    file.close();
    return processedWords;
}