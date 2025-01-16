#include "trie.h"
#include <iostream>
#include "util.h"
#include <chrono>
#include "malloc_count.h"
#include <filesystem>

std::string csv_file;

std::unordered_map<HashMapGlobal::key_t, HashMapGlobal *> HashMapGlobal::map;
unsigned HashMapGlobal::id_counter;

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
    if (argc < 3)
    {
        std::cerr << "Usage: <path_to_input_file> <path_to_query_file>\n";
        return 1;
    }

    std::string input_file = argv[1];
    std::string query_file = argv[2];
    std::string result_file = "results_" + input_file;
    csv_file = result_file;
    csv_file.erase(csv_file.find_last_of('.'));
    csv_file += ".csv";

    if (argc != 4)
    {
        benchmark<HashMapGlobal>(input_file, query_file, result_file);
        benchmark<Fixed>(input_file, query_file, result_file);
        benchmark<Variable>(input_file, query_file, result_file);
        benchmark<HashMap>(input_file, query_file, result_file);
        benchmark<VariableSIMD>(input_file, query_file, result_file);
    }
    else
    {
        std::string type = argv[3];
        std::cerr << "Received type: " << type << "\n";
        if (type == "Fixed")
            benchmark<Fixed>(input_file, query_file, result_file);
        else if (type == "Variable")
            benchmark<Variable>(input_file, query_file, result_file);
        else if (type == "VariableSIMD")
            benchmark<VariableSIMD>(input_file, query_file, result_file);
        else if (type == "HashMap")
            benchmark<HashMap>(input_file, query_file, result_file);
        else if (type == "HashMapGlobal")
            benchmark<HashMapGlobal>(input_file, query_file, result_file);
    }

    return 1;
}