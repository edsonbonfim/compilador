#include <iostream>

#include "tools.h"

using namespace std;

/**
 * Retorna uma string com o conteudo do arquivo
 */
ifstream *open_source(const string &file_name) {
    auto *source = new ifstream(file_name);

    if (!source->is_open()) {
        cerr << "Could not open file: " << file_name;
        exit(EXIT_FAILURE);
    }

    return source;
}
