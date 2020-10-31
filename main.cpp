#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <fstream>

#include "lex2.h"
#include "tools.h"
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

        if (dynamic_cast<Real *>(token)) {
            cout << *dynamic_cast<Real *>(token);
        }

        if (dynamic_cast<Integer *>(token)) {
            cout << *dynamic_cast<Integer *>(token);
        }

        if (dynamic_cast<Var *>(token)) {
            cout << *dynamic_cast<Var *>(token);
        }

        if (dynamic_cast<Program *>(token)) {
            cout << *dynamic_cast<Program *>(token);
        }

        if (dynamic_cast<Begin *>(token)) {
            cout << *dynamic_cast<Begin *>(token);
        }

        if (dynamic_cast<End *>(token)) {
            cout << *dynamic_cast<End *>(token);
        }

        delete token;
    } while (token != nullptr);

    delete lex;

    return EXIT_SUCCESS;
}
