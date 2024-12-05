#include "trie.h"
#include <iostream>

int main(){
    char word[] = "Hello\0";
    Trie<Variable> var{};
    var.insert(word); 
    Trie<Fixed> fix{};
    fix.insert(word); 
    Trie<HashMap> map{};
    map.insert(word);
    std::cerr << var.contains(word) << "\n";
    std::cerr << fix.contains(word) << "\n";
    std::cerr << map.contains(word) << "\n";
}