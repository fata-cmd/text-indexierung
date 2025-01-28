#include "src/trie.h"
#include <iostream>
#include "src/util.h"
#include <chrono>
#include "src/malloc_count.h"
#include <filesystem>
#include <sstream>

std::unordered_map<HashMapGlobal::key_t, HashMapGlobal *> HashMapGlobal::map;
HashMapGlobal::key_t HashMapGlobal::id_counter;

template <typename SDict>
void processQueries(SDict &trie, const std::string &queryFile, const std::string &resultFile)
{
    std::ifstream queryStream(queryFile);
    std::string word;
    char op;

    if (!queryStream.is_open())
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

    }

    queryStream.close();
}

template <typename NodeType>
void benchmark(const std::string &inputFile, const std::string &queryFile, const std::string &resultFile)
{
    std::string type = type_name<NodeType>();

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
    std::cout << type << "," << c_dur << "," << tcm << "," << q_dur << "\n";
}

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        std::cerr << "Usage: -variante=<1-3> <path_to_input_file> <path_to_query_file>\n";
        return 1;
    }

    std::string input_file = std::filesystem::absolute(argv[2]).string();
    std::string query_file = std::filesystem::absolute(argv[3]).string();
    std::filesystem::path input_path = std::filesystem::absolute(argv[2]);
    std::string result_file = (std::filesystem::current_path() / ("result_" + input_path.filename().string())).string();

    std::string type = argv[1];
    if (type.back() == '1')
        benchmark<Fixed>(input_file, query_file, result_file);
    else if (type.back() == '2')
        benchmark<Variable>(input_file, query_file, result_file);
    else if (type.back() == '3')
        benchmark<VariableSIMD>(input_file, query_file, result_file);
    else if (type.back() == '4')
        benchmark<HashMap>(input_file, query_file, result_file);
    else if (type.back() == '5')
        benchmark<HashMapGlobal>(input_file, query_file, result_file);
    else
        std::cerr << "Invalid type: " << type.back() << "\n";

    return 1;
}