#include "trie.h"
#include <iostream>
#include "util.h"
#include <chrono>
#include "malloc_count.h"
#include <filesystem>

template <typename SDict>
void processQueries(SDict &trie, const std::string &queryFile, const std::string &resultFile)
{
    std::ifstream queryStream(queryFile);
    std::ofstream resultStream(resultFile);
    std::string line;

    if (!queryStream.is_open() || !resultStream.is_open())
    {
        std::cerr << "Error opening input/output files." << std::endl;
        return;
    }

    while (std::getline(queryStream, line))
    {
        size_t spaceIndex = line.find('$');

        std::string word = line.substr(0, spaceIndex);
        char operation = line.back();
        bool result = false;

        switch (operation)
        {
        case 'i':
            result = trie.insert_word(word);
            break;
        case 'c':
            result = trie.contains(word);
            break;
        case 'd':
            result = trie.delete_word(word);
            break;
        default:
            continue;
        }

        resultStream << (result ? "true" : "false") << "\n";
    }

    queryStream.close();
    resultStream.close();
}

template <typename SDict>
void benchmark(const std::string &inputFile, const std::string &queryFile, const std::string &resultFile)
{
    std::cerr << "Benchmark " << type_name<SDict>() << "\n";
    // construct
    std::chrono::_V2::steady_clock::time_point b, e;
    int64_t c_dur, q_dur;
    {
        b = std::chrono::steady_clock::now();
        SDict trie = readFileIntoDict<SDict>(inputFile);
        e = std::chrono::steady_clock::now();
        c_dur = std::chrono::duration_cast<std::chrono::milliseconds>(e - b).count();

        // query
        b = std::chrono::steady_clock::now();
        processQueries(trie, queryFile, resultFile);
        e = std::chrono::steady_clock::now();
        q_dur = std::chrono::duration_cast<std::chrono::milliseconds>(e - b).count();
    }
    // output
    std::cout << "RESULT name=fahri_taban trie_construction_time=" << c_dur
              << " trie_construction_memory=" << malloc_count_peak()
              << " query_time=" << q_dur << "\n";
    malloc_count_reset_peak();
    
    std::cerr << "Query results written to " << resultFile << "\n\n";
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: <path_to_input_file> <path_to_query_file>\n";
        return 1;
    }
    std::string input_file = argv[1];
    std::filesystem::path query_file = argv[2];
    std::string result_file = "result_" + query_file.filename().string() + ".txt";

    benchmark<Trie<VariableSIMD>>(input_file, query_file, result_file);
    benchmark<Trie<Variable>>(input_file, query_file, result_file);
    benchmark<Trie<Fixed>>(input_file, query_file, result_file);
    benchmark<Trie<HashMap>>(input_file, query_file, result_file);
    return 1;
}