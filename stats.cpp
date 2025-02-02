#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cctype>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>  // For ceil

using namespace std;

int main(int argc, char** argv) {
    string filename = argv[1];

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    unordered_map<string, int> wordCountMap;
    int totalWords = 0;
    int totalLength = 0;
    vector<int> wordLengths;

    string word;
    while (file >> word) {
        wordCountMap[word]++;
        totalWords++;
        int wordLength = word.length();
        totalLength += wordLength;
        wordLengths.push_back(wordLength);
    }

    file.close();

    // Counting unique words
    int uniqueWords = wordCountMap.size();

    // Counting non-unique words
    int nonUniqueWords = 0;
    for (const auto& entry : wordCountMap) {
        if (entry.second > 1) {
            nonUniqueWords += entry.second;
        }
    }

    // Calculating average word length
    double averageWordLength = totalWords > 0 ? static_cast<double>(totalLength) / totalWords : 0;

    // Calculating median word length
    double medianWordLength = 0;
    if (!wordLengths.empty()) {
        sort(wordLengths.begin(), wordLengths.end());
        int mid = wordLengths.size() / 2;
        if (wordLengths.size() % 2 == 0) {
            medianWordLength = (wordLengths[mid - 1] + wordLengths[mid]) / 2.0;
        } else {
            medianWordLength = wordLengths[mid];
        }
    }

    // Calculating 90th percentile
    double percentile90 = 0;
    if (!wordLengths.empty()) {
        sort(wordLengths.begin(), wordLengths.end());
        int index90 = static_cast<int>(ceil(0.9 * wordLengths.size())) - 1; // Ceil and then subtract 1 for 0-indexed
        percentile90 = wordLengths[index90];
    }

    // Output results
    cout << "Total words: " << totalWords << endl;
    cout << "Unique words: " << uniqueWords << endl;
    cout << "Non-unique words: " << nonUniqueWords << endl;
    cout << "Average word length: " << averageWordLength << endl;
    cout << "Median word length: " << medianWordLength << endl;
    cout << "90th percentile word length: " << percentile90 << endl;

    return 0;
}
