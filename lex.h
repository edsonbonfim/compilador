#ifndef COMP_LEX_H
#define COMP_LEX_H

#include <iostream>
#include <fstream>

#include "token.h"

using namespace std;

class Lex {

public:
    explicit Lex(const string &file_name);

    Token *getNextToken();

    ~Lex();

private:
    ifstream *source;

    char ch;

    void next();

    void skipComments();

    void skipWhiteSpaces();

    void skipCommentsAndWhiteSpaces();

    Token *getNumberToken();

    Token *getId();
};

#endif //COMP_LEX_H
