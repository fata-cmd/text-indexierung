#include "trie.h"
#include <iostream>
#include "util.h"

int main(int argc, char** argv){
    if (argc != 2){
        std::cerr << "Usage: <path_to_file>\n";
        return 1;
    }
    std::string file_name = argv[1];
    std::cerr << "reading file: " << file_name << "\n";
    auto sdict = readFileIntoDict<Trie<VariableSIMD>>(file_name);
    std::cerr << "words inserted: " << sdict.size() << "\n";
    return 1;
}