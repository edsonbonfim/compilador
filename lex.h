#ifndef COMP_LEX_H
#define COMP_LEX_H

#define EOF 256

enum {
    PROGRAMA = 257,
    INICIO,
    FIM,
    CONSTANTE,
    TIPO,
    VAR,
    EQUAL,
    PT_VIRGULA,
    STRING,
    DOIS_PONTOS,
    COMMA,
    INTEIRO,
    REAL,
    VETOR,
    L_BRACKET,
    R_BRACKET,
    OF,
    RECORD,
    FUNCTION,
    L_PAREN,
    R_PAREN,
    ATTR,
    WHILE,
    IF,
    THEN,
    WRITE,
    READ,
    ELSE,
    BIGGER_THAN,
    LESS_THAN,
    EXCLAMATION,
    PLUS,
    MINUS,
    TIMES,
    SLASH,
    ID,
    ERROR,
    TIPO_DADO,
    OP_LOGICO,
    OP_MAT,
    VIRGULA,
    PONTO,
};

struct Posicao {
    int linha;
    int coluna;
};

typedef struct Token {
    int tipo;
    void *valor;
    struct Posicao pos;
} Token;

typedef struct Lex {
    Token current_token;
    Token prev_token;
} Lex;

extern Lex *iniciar_lex(char *);

extern void prev_token(void);

extern void next_token(void);

extern void imprimir_token(struct Token token);


Token peek();

Token peek2();

#endif //COMP_LEX_H
