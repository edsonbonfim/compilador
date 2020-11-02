#ifndef COMP_PARSER_H
#define COMP_PARSER_H

#include "lex.h"

typedef struct Tree {
    Token token;
    struct Tree *brother;
    struct Tree *child;
} Tree;

Tree *parse_tree(Lex *_lex);

void print_parse_tree(Tree *tree);

Tree *dispose_parse_tree(Tree *node);

#endif //COMP_PARSER_H
