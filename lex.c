#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "lex.h"

Lex *lex;

static char *texto;
static int prev_pos;
static int pos;        // posição do analisador no texto
static char prev_ch;
static char caracter;  // caracter do texto na posição do analisador
static int linha = 0;
static int coluna = 1;

/*
 * Classificação de caracteres
 */

#define eh_final_do_texto(ch)            ((ch) == '\0')
#define eh_espaco(ch)                    (!eh_final_do_texto(ch) && (ch) <= ' ')
#define eh_inicio_comentario(ch)         ((ch) == '{')
#define eh_final_comentario(ch)          ((ch) == '}')

#define eh_letra_maiuscula(ch)           ((ch) >= 'A' && (ch) <= 'Z')
#define eh_letra_minuscula(ch)           ((ch) >= 'a' && (ch) <= 'z')
#define eh_letra(ch)                     (eh_letra_maiuscula(ch) || eh_letra_minuscula(ch))
#define eh_digito(ch)                    ((ch) >= '0' && (ch) <= '9')
#define eh_letra_ou_digito(ch)           (eh_letra(ch) || eh_digito(ch))
#define eh_ponto(ch)                     ((ch) == '.')

/**
 * Inicializa o analizador lexico
 */
Lex *iniciar_lex(char *codigo) {
    lex = (Lex *) malloc(sizeof(Lex));
    lex->current_token.pos.linha = 0;
    lex->current_token.pos.coluna = 1;
    pos = 0;
    texto = codigo;
    caracter = texto[pos];
    return lex;
}

/**
 * Obtém o próximo caracter da entrada
 */
void avancar() {
    if (texto[pos + 1] == '\n') {
        linha++;
        coluna = 1;
    } else {
        coluna++;
    }

    caracter = texto[++pos];
}

/**
 * Ignora os espaços e comentários
 */
void pular_espaco_e_comentario(void) {
    while (eh_espaco(caracter)) {
        avancar();
    }

    while (eh_inicio_comentario(caracter)) {
        // Ignora o simbolo {
        avancar();

        while (!eh_final_comentario(caracter)) {
            if (eh_final_do_texto(caracter)) {
                return;
            }

            // Ignora o comentário
            avancar();
        }

        // Ignora o simbolo }
        avancar();

        while (eh_espaco(caracter)) {
            avancar();
        }
    }
}

/**
 * Armazena a posição do lex->current_token.
 *
 * Armazena a posição em que o token foi encontrado na entrada.
 * Isso será útil para identificar a posição do token em caso de erro,
 * fornecendo um feedback mais preciso para o usuário.
 */
void gravar_pos_token(void) {
    lex->current_token.pos.linha = linha;
    lex->current_token.pos.coluna = coluna;
}

char *obter_string(int pos_inicial) {
    size_t tamanho = pos - pos_inicial;

    char *retorno = malloc(tamanho + 1);
    assert(retorno);

    strncpy(retorno, texto + pos_inicial, tamanho);
    retorno[tamanho] = '\0';

    return retorno;
}

int *converte_string_para_inteiro(char *string) {
    int *num = calloc(1, sizeof(int));
    assert(num);

    *num = atoi(string);
    return num;
}

double *converte_string_para_real(char *string) {
    double *num = calloc(1, sizeof(double));
    assert(num);

    *num = strtod(string, &string);
    return num;
}

char *converte_char_para_string(char ch) {
    char *string = malloc(sizeof(char));
    *string = ch;
    return string;
}

void prev_token() {
    pos = prev_pos;
    caracter = prev_ch;
    lex->current_token = lex->prev_token;
}

Token next() {
    Token token;

    pular_espaco_e_comentario();

    // Agora estamos no inicio de um simbolo ou no fim do arquivo
    gravar_pos_token();

    // Posição da entrada no inicio do reconhecimento
    int pos_inicial = pos;

    if (eh_final_do_texto(caracter)) {
        token.tipo = EOF;
        token.valor = (void *) "<EOF>";
    } else if (eh_digito(caracter)) {
        token.tipo = INTEIRO;

        while (eh_digito(caracter) || eh_ponto(caracter)) {
            if (eh_ponto(caracter)) token.tipo = REAL;
            avancar();
        }

        char *resultado = obter_string(pos_inicial);

        if (token.tipo == INTEIRO) {
            token.valor = converte_string_para_inteiro(resultado);
        } else {
            token.valor = converte_string_para_real(resultado);
        }
    } else if (caracter == '>') {
        token.tipo = BIGGER_THAN;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == '<') {
        token.tipo = LESS_THAN;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == '=') {
        token.tipo = EQUAL;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == '!') {
        token.tipo = EXCLAMATION;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == '+') {
        token.tipo = PLUS;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == '-') {
        token.tipo = MINUS;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == '*') {
        token.tipo = TIMES;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == '/') {
        token.tipo = SLASH;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == '[') {
        token.tipo = L_BRACKET;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == ']') {
        token.tipo = R_BRACKET;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == ';') {
        token.tipo = PT_VIRGULA;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == ',') {
        token.tipo = VIRGULA;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == ':' && texto[pos + 1] == '=') {
        token.tipo = ATTR;
        token.valor = ":=";
        avancar();
        avancar();
    } else if (caracter == ':') {
        token.tipo = DOIS_PONTOS;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == '(') {
        token.tipo = L_PAREN;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == ')') {
        token.tipo = R_PAREN;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == '.') {
        token.tipo = PONTO;
        token.valor = converte_char_para_string(caracter);
        avancar();
    } else if (caracter == '"') {
        token.tipo = STRING;

        avancar();

        pos_inicial = pos;

        while (caracter != '"') avancar();

        // Tamanho da string reconhecida
        size_t tamanho = pos - pos_inicial;

        avancar();

        // String reconhecida
        char *retorno = malloc(tamanho + 1);
        assert(retorno);
        strncpy(retorno, texto + pos_inicial, tamanho);
        retorno[tamanho] = '\0';

        token.valor = retorno;
    } else if (eh_letra(caracter)) {
        token.tipo = ID;
        while (eh_letra_ou_digito(caracter)) avancar();

        // Tamanho da string reconhecida
        size_t tamanho = pos - pos_inicial;

        // String reconhecida
        char *retorno = malloc(tamanho + 1);
        assert(retorno);
        strncpy(retorno, texto + pos_inicial, tamanho);
        retorno[tamanho] = '\0';

        token.valor = retorno;

        if (strcmp(retorno, "real") == 0 || strcmp(retorno, "integer") == 0) token.tipo = TIPO_DADO;

        else if (strcmp(retorno, "program") == 0) token.tipo = PROGRAMA;

        else if (strcmp(retorno, "begin") == 0) token.tipo = INICIO;

        else if (strcmp(retorno, "end") == 0) token.tipo = FIM;

        else if (strcmp(retorno, "const") == 0) token.tipo = CONSTANTE;

        else if (strcmp(retorno, "type") == 0) token.tipo = TIPO;

        else if (strcmp(retorno, "var") == 0) token.tipo = VAR;

        else if (strcmp(retorno, "array") == 0) token.tipo = VETOR;

        else if (strcmp(retorno, "of") == 0) token.tipo = OF;

        else if (strcmp(retorno, "record") == 0) token.tipo = RECORD;

        else if (strcmp(retorno, "function") == 0) token.tipo = FUNCTION;

        else if (strcmp(retorno, "while") == 0) token.tipo = WHILE;

        else if (strcmp(retorno, "if") == 0) token.tipo = IF;

        else if (strcmp(retorno, "then") == 0) token.tipo = THEN;

        else if (strcmp(retorno, "write") == 0) token.tipo = WRITE;

        else if (strcmp(retorno, "read") == 0) token.tipo = READ;

        else if (strcmp(retorno, "else") == 0) token.tipo = ELSE;
    } else {
        token.tipo = ERROR;
        token.valor = NULL;
    }
    return token;
}

Token peek() {
    prev_ch = caracter;
    prev_pos = pos;

    Token token = next();

    pos = prev_pos;
    caracter = prev_ch;

    return token;
}

Token peek2() {
    prev_ch = caracter;
    prev_pos = pos;

    next();
    Token token = next();

    pos = prev_pos;
    caracter = prev_ch;

    return token;
}

/**
 * Percorre a entrada até obter o próximo token válido.
 */
void next_token() {
    prev_pos = pos;
    prev_ch = caracter;

    lex->prev_token = lex->current_token;
    lex->current_token = next();
}
