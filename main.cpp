#include <cstdio>
#include <cstdlib>

#include <iostream>
#include "lex.h"

using namespace std;

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("COMP\n");
        printf("Uso: COMP [programa_fonte]\n");
        exit(EXIT_FAILURE);
    }

    // Inicializa o analisador lexico
    auto *lex = new Lex(argv[1]);

    Token *token;

    do {
        token = lex->getNextToken();
    } while (token != nullptr);

    delete lex;

    return EXIT_SUCCESS;
}
