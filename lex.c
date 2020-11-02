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

#define eh_operador_logico(ch)           (strchr("><=!", (ch)) != 0)
#define eh_operador_matematico(ch)       (strchr("+-*/", (ch)) != 0)

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

/**
 * Percorre a entrada até obter o próximo token válido.
 */
void next_token() {
    prev_pos = pos;
    prev_ch = caracter;

    lex->prev_token = lex->current_token;

    pular_espaco_e_comentario();

    // Agora estamos no inicio de um simbolo ou no fim do arquivo
    gravar_pos_token();

    // Posição da entrada no inicio do reconhecimento
    int pos_inicial = pos;

    if (eh_final_do_texto(caracter)) {
        lex->current_token.tipo = EOF;
        lex->current_token.valor = (void *) "<EOF>";
        return;
    } else if (eh_digito(caracter)) {
        lex->current_token.tipo = INTEIRO;

        while (eh_digito(caracter) || eh_ponto(caracter)) {
            if (eh_ponto(caracter)) lex->current_token.tipo = REAL;
            avancar();
        }

        char *resultado = obter_string(pos_inicial);

        if (lex->current_token.tipo == INTEIRO) {
            lex->current_token.valor = converte_string_para_inteiro(resultado);
        } else {
            lex->current_token.valor = converte_string_para_real(resultado);
        }

        return;
    } else if (eh_operador_logico(caracter)) {
        lex->current_token.tipo = OP_LOGICO;
        lex->current_token.valor = converte_char_para_string(caracter);
        avancar();
        return;
    } else if (eh_operador_matematico(caracter)) {
        lex->current_token.tipo = OP_MAT;
        lex->current_token.valor = converte_char_para_string(caracter);
        avancar();
        return;
    } else if (caracter == '[') {
        lex->current_token.tipo = L_BRACKET;
        lex->current_token.valor = converte_char_para_string(caracter);
        avancar();
        return;
    } else if (caracter == ']') {
        lex->current_token.tipo = R_BRACKET;
        lex->current_token.valor = converte_char_para_string(caracter);
        avancar();
        return;
    } else if (caracter == ';') {
        lex->current_token.tipo = PT_VIRGULA;
        lex->current_token.valor = converte_char_para_string(caracter);
        avancar();
        return;
    } else if (caracter == ',') {
        lex->current_token.tipo = VIRGULA;
        lex->current_token.valor = converte_char_para_string(caracter);
        avancar();
        return;
    } else if (caracter == ':' && texto[pos + 1] == '=') {
        lex->current_token.tipo = ATTR;
        lex->current_token.valor = ":=";
        avancar();
        avancar();
        return;
    } else if (caracter == ':') {
        lex->current_token.tipo = DOIS_PONTOS;
        lex->current_token.valor = converte_char_para_string(caracter);
        avancar();
        return;
    } else if (caracter == '(') {
        lex->current_token.tipo = L_PAREN;
        lex->current_token.valor = converte_char_para_string(caracter);
        avancar();
        return;
    } else if (caracter == ')') {
        lex->current_token.tipo = R_PAREN;
        lex->current_token.valor = converte_char_para_string(caracter);
        avancar();
        return;
    } else if (caracter == '.') {
        lex->current_token.tipo = PONTO;
        lex->current_token.valor = converte_char_para_string(caracter);
        avancar();
        return;
    } else if (caracter == '"') {
        lex->current_token.tipo = STRING;

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

        lex->current_token.valor = retorno;

        return;
    } else if (eh_letra(caracter)) {
        lex->current_token.tipo = ID;
        while (eh_letra_ou_digito(caracter)) avancar();

        // Tamanho da string reconhecida
        size_t tamanho = pos - pos_inicial;

        // String reconhecida
        char *retorno = malloc(tamanho + 1);
        assert(retorno);
        strncpy(retorno, texto + pos_inicial, tamanho);
        retorno[tamanho] = '\0';

        lex->current_token.valor = retorno;

        if (strcmp(retorno, "real") == 0 || strcmp(retorno, "integer") == 0) lex->current_token.tipo = TIPO_DADO;

        else if (strcmp(retorno, "program") == 0) lex->current_token.tipo = PROGRAMA;

        else if (strcmp(retorno, "begin") == 0) lex->current_token.tipo = INICIO;

        else if (strcmp(retorno, "end") == 0) lex->current_token.tipo = FIM;

        else if (strcmp(retorno, "const") == 0) lex->current_token.tipo = CONSTANTE;

        else if (strcmp(retorno, "type") == 0) lex->current_token.tipo = TIPO;

        else if (strcmp(retorno, "var") == 0) lex->current_token.tipo = VAR;

        else if (strcmp(retorno, "array") == 0) lex->current_token.tipo = VETOR;

        else if (strcmp(retorno, "of") == 0) lex->current_token.tipo = OF;

        else if (strcmp(retorno, "record") == 0) lex->current_token.tipo = RECORD;

        else if (strcmp(retorno, "function") == 0) lex->current_token.tipo = FUNCTION;

        else if (strcmp(retorno, "while") == 0) lex->current_token.tipo = WHILE;

        else if (strcmp(retorno, "if") == 0) lex->current_token.tipo = IF;

        else if (strcmp(retorno, "then") == 0) lex->current_token.tipo = THEN;

        else if (strcmp(retorno, "write") == 0) lex->current_token.tipo = WRITE;

        else if (strcmp(retorno, "read") == 0) lex->current_token.tipo = READ;

        else if (strcmp(retorno, "else") == 0) lex->current_token.tipo = ELSE;

        return;
    } else {
        lex->current_token.tipo = ERROR;
        lex->current_token.valor = NULL;
        return;
    }
}

void imprimir_token(struct Token token) {
    switch (lex->current_token.tipo) {
        case EOF:
            printf("(EOF, NULL)\n");
            break;
        case PROGRAMA:
            printf("(PROGRAMA, %s)\n", (char *) lex->current_token.valor);
            break;
        case INICIO:
            printf("(INICIO, %s)\n", (char *) lex->current_token.valor);
            break;
        case FIM:
            printf("(FIM, %s)\n", (char *) lex->current_token.valor);
            break;
        case CONSTANTE:
            printf("(CONSTANTE, %s)\n", (char *) lex->current_token.valor);
            break;
        case TIPO:
            printf("(TIPO, %s)\n", (char *) lex->current_token.valor);
            break;
        case VAR:
            printf("(VAR, %s)\n", (char *) lex->current_token.valor);
            break;
        case EQUAL:
            printf("(EQUAL, %s)\n", (char *) lex->current_token.valor);
            break;
        case PT_VIRGULA:
            printf("(PT_VIRGULA, %s)\n", (char *) lex->current_token.valor);
            break;
        case STRING:
            printf("(STRING, %s)\n", (char *) lex->current_token.valor);
            break;
        case DOIS_PONTOS:
            printf("(DOIS_PONTOS, %s)\n", (char *) lex->current_token.valor);
            break;
        case COMMA:
            printf("(COMMA, %s)\n", (char *) lex->current_token.valor);
            break;
        case INTEIRO:
            printf("(INTEIRO, %d)\n", *(int *) lex->current_token.valor);
            break;
        case REAL:
            printf("(REAL, %g)\n", *(double *) lex->current_token.valor);
            break;
        case VETOR:
            printf("(VETOR, %s)\n", (char *) lex->current_token.valor);
            break;
        case L_BRACKET:
            printf("(L_BRACKET, %s)\n", (char *) lex->current_token.valor);
            break;
        case R_BRACKET:
            printf("(R_BRACKET, %s)\n", (char *) lex->current_token.valor);
            break;
        case OF:
            printf("(OF, %s)\n", (char *) lex->current_token.valor);
            break;
        case RECORD:
            printf("(RECORD, %s)\n", (char *) lex->current_token.valor);
            break;
        case FUNCTION:
            printf("(FUNCTION, %s)\n", (char *) lex->current_token.valor);
            break;
        case L_PAREN:
            printf("(L_PAREN, %s)\n", (char *) lex->current_token.valor);
            break;
        case R_PAREN:
            printf("(R_PAREN, %s)\n", (char *) lex->current_token.valor);
            break;
        case ATTR:
            printf("(ATTR, %s)\n", (char *) lex->current_token.valor);
            break;
        case WHILE:
            printf("(WHILE, %s)\n", (char *) lex->current_token.valor);
            break;
        case IF:
            printf("(IF, %s)\n", (char *) lex->current_token.valor);
            break;
        case THEN:
            printf("(THEN, %s)\n", (char *) lex->current_token.valor);
            break;
        case WRITE:
            printf("(WRITE, %s)\n", (char *) lex->current_token.valor);
            break;
        case READ:
            printf("(READ, %s)\n", (char *) lex->current_token.valor);
            break;
        case ELSE:
            printf("(ELSE, %s)\n", (char *) lex->current_token.valor);
            break;
        case BIGGER_THAN:
            printf("(BIGGER_THAN, %s)\n", (char *) lex->current_token.valor);
            break;
        case LESS_THAN:
            printf("(LESS_THAN, %s)\n", (char *) lex->current_token.valor);
            break;
        case EXCLAMATION:
            printf("(EXCLAMATION, %s)\n", (char *) lex->current_token.valor);
            break;
        case PLUS:
            printf("(PLUS, %s)\n", (char *) lex->current_token.valor);
            break;
        case MINUS:
            printf("(MINUS, %s)\n", (char *) lex->current_token.valor);
            break;
        case TIMES:
            printf("(TIMES, %s)\n", (char *) lex->current_token.valor);
            break;
        case SLASH:
            printf("(SLASH, %s)\n", (char *) lex->current_token.valor);
            break;
        case ID:
            printf("(ID, %s)\n", (char *) lex->current_token.valor);
            break;
        case TIPO_DADO:
            printf("(TIPO_DADO, %s)\n", (char *) lex->current_token.valor);
            break;
        case OP_LOGICO:
            printf("(OP_LOGICO, %s)\n", (char *) lex->current_token.valor);
            break;
        case OP_MAT:
            printf("(OP_MAT, %s)\n", (char *) lex->current_token.valor);
            break;
        case VIRGULA:
            printf("(VIRGULA, %s)\n", (char *) lex->current_token.valor);
            break;
        case PONTO:
            printf("(PONTO, %s)\n", (char *) lex->current_token.valor);
            break;
    }
}
