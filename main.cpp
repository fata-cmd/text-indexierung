#include "src/trie.h"
#include <iostream>
#include "src/util.h"
#include <chrono>
#include "src/malloc_count.h"
#include <filesystem>
#include <sstream>

std::string csv_file;

std::unordered_map<HashMapGlobal::key_t, HashMapGlobal *> HashMapGlobal::map;
unsigned HashMapGlobal::id_counter;

template <typename SDict>
void processQueries(SDict &trie, const std::string &queryFile, const std::string &resultFile)
{
    std::ifstream queryStream(queryFile);
    std::ofstream resultStream(resultFile);
    std::string word;
    char op;

    if (!queryStream.is_open() || !resultStream.is_open())
    {
        std::cerr << "Error opening input/output files." << std::endl;
        return;
    }

    while (queryStream >> word)
    {
        queryStream >> op;
        bool result = false;

        switch (op)
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

template <typename NodeType>
void benchmark(const std::string &inputFile, const std::string &queryFile, const std::string &resultFile)
{
    std::string type = type_name<NodeType>();
    std::cerr << "Benchmark " << type << "\n";
    std::ofstream csv(csv_file, std::ios_base::app);

    std::chrono::_V2::steady_clock::time_point b, e;
    int64_t c_dur, q_dur;
    size_t tcm;
    {
        // construct
        b = std::chrono::steady_clock::now();
        auto trie = readFileIntoDict<Trie<NodeType>>(inputFile);
        e = std::chrono::steady_clock::now();
        c_dur = std::chrono::duration_cast<std::chrono::milliseconds>(e - b).count();
        tcm = malloc_count_peak() / (1024 * 1024);

        // query
        b = std::chrono::steady_clock::now();
        processQueries(trie, queryFile, resultFile);
        e = std::chrono::steady_clock::now();
        q_dur = std::chrono::duration_cast<std::chrono::milliseconds>(e - b).count();
    }
    // output
    std::cout << "RESULT name=fahri_taban trie_construction_time=" << c_dur
              << " trie_construction_memory=" << tcm
              << " query_time=" << q_dur << "\n";

    csv << type << "," << c_dur << "," << tcm << "," << q_dur << "\n";

    malloc_count_reset_peak();
    csv.close();

    std::cerr << "Query results written to " << resultFile << "\n\n";
}

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        std::cerr << "Usage: -variante=<1-3> <path_to_input_file> <path_to_query_file>\n";
        return 1;
    }

    std::cerr << "cwd: " << std::filesystem::current_path() << "\n";
    std::string input_file = std::filesystem::absolute(argv[2]).string();
    std::string query_file = std::filesystem::absolute(argv[3]).string();
    std::filesystem::path input_path = std::filesystem::absolute(argv[2]);
    std::string result_file = (std::filesystem::current_path() / ("result_" + input_path.filename().string())).string();

    csv_file = result_file;
    csv_file.erase(csv_file.find_last_of('.'));
    csv_file += ".csv";

    std::string type = argv[1];
    std::cerr << "Received type: " << type.back() << "\n";
    if (type.back() == '1')
        benchmark<Fixed>(input_file, query_file, result_file);
    else if (type.back() == '2')
        benchmark<VariableSIMD>(input_file, query_file, result_file);
    else if (type.back() == '3')
        benchmark<HashMapGlobal>(input_file, query_file, result_file);
    else
        std::cerr << "Invalid type: " << type.back() << "\n";

    return 1;
}