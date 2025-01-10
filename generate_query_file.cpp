#include <iostream>
#include <fstream>

void generateQueryFile(const std::string &filename)
{
    size_t num_queries = 1e7;
    size_t word_count = 0;
    std::string query_filename = "queries_" + filename;
    std::ofstream qfile(query_filename, std::ios_base::app);
    std::ifstream file(filename);

    if (!file)
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    std::string word;

    std::string operations[3] = {"i", "c", "d"};

    while (file >> word && num_queries--)
    {
        if (word.back() != '\0')
            word += '\0';

        qfile << word << " " << operations[rand() % 3] << "\n";
        ++word_count;
    }
    std::cerr << "Number of queries: " << word_count << "\n"; 
    file.close();
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: <path_to_words_file>\n";
        return 0;
    }

    std::string filename(argv[1]);
    generateQueryFile(filename);
    return 1;
}