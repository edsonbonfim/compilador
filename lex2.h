//#ifndef COMP_LEX2_H
//#define COMP_LEX2_H
//
//#define EOF 256
//
//#include <iostream>
//#include <fstream>
//
//
//enum {
//    PROGRAMA = 257,
//    INICIO,
//    FIM,
//    CONSTANTE,
//    TIPO,
//    VAR,
//    EQUAL,
//    PT_VIRGULA,
//    STRING,
//    DOIS_PONTOS,
//    COMMA,
//    INTEIRO,
//    REAL,
//    VETOR,
//    L_BRACKET,
//    R_BRACKET,
//    OF,
//    RECORD,
//    FUNCTION,
//    L_PAREN,
//    R_PAREN,
//    ATTR,
//    WHILE,
//    IF,
//    THEN,
//    WRITE,
//    READ,
//    ELSE,
//    BIGGER_THAN,
//    LESS_THAN,
//    EXCLAMATION,
//    PLUS,
//    MINUS,
//    TIMES,
//    SLASH,
//    ID,
//    ERROR,
//    TIPO_DADO,
//    OP_LOGICO,
//    OP_MAT,
//    VIRGULA,
//    PONTO,
//};
//
//struct Posicao {
//    int linha;
//    int coluna;
//};
//
////struct Token {
////    int tipo;
////    void *valor;
////    struct Posicao pos;
////};
//
////extern struct Token *Token;
//
//extern void iniciar_lex(std::ifstream *file);
//
//extern struct Token *get_prox_token(void);
//
//extern void imprimir_token(struct Token *token);
//
//#endif //COMP_LEX2_H
