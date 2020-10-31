//#include <cstdio>
//#include <cstdlib>
//#include <cstring>
//#include <iostream>
//#include <fstream>
//
//#include "lex2.h"
//
//using namespace std;
//
////struct Token *Token;
//static ifstream *source;
//static int pos;        // posição do analisador no texto
//static int caracter;  // caracter do texto na posição do analisador
//static int linha = 0;
//static int coluna = 1;
//
///*
// * Classificação de caracteres
// */
//
//#define eh_final_do_texto(ch)            ((ch) == '\0')
//#define eh_espaco(ch)                    (!eh_final_do_texto(ch) && (ch) <= ' ')
//#define eh_inicio_comentario(ch)         ((ch) == '{')
//#define eh_final_comentario(ch)          ((ch) == '}')
//
//#define eh_letra_maiuscula(ch)           ((ch) >= 'A' && (ch) <= 'Z')
//#define eh_letra_minuscula(ch)           ((ch) >= 'a' && (ch) <= 'z')
//#define eh_letra(ch)                     (eh_letra_maiuscula(ch) || eh_letra_minuscula(ch))
//#define eh_digito(ch)                    ((ch) >= '0' && (ch) <= '9')
//#define eh_letra_ou_digito(ch)           (eh_letra(ch) || eh_digito(ch))
//#define eh_ponto(ch)                     ((ch) == '.')
//
//#define eh_operador_logico(ch)           (strchr("><=!", (ch)) != 0)
//#define eh_operador_matematico(ch)       (strchr("+-*/", (ch)) != 0)

///**
// * Inicializa o analizador lexico
// */
//void iniciar_lex(ifstream *file) {
//    (struct Token *) calloc(1, sizeof(struct Token));
//    Token->pos.linha = 0;
//    Token->pos.coluna = 1;
//    pos = 0;
//    source = file;
//    caracter = file->get();
//}
//
///**
// * Obtém o próximo caracter da entrada
// */
//void avancar() {
////    if (texto[pos + 1] == '\n') {
////        linha++;
////        coluna = 1;
////    } else {
////        coluna++;
////    }
//
//    pos++;
//    caracter = source->get();
//}
//
///**
// * Ignora os espaços em branco
// */
//void ignore_empty_spaces() {
//    while (eh_espaco(caracter)) {
//        avancar();
//    }
//}
//
///**
// * Ignora os comentários
// */
//void ignore_comments() {
//    if (eh_inicio_comentario(caracter)) {
//        while (!eh_final_comentario(caracter)) {
//            // Ignora o comentário
//            avancar();
//        }
//
//        // Ignora o caracter final do comentário
//        avancar();
//    }
//}
//
///**
// * Ignora os espaços e comentários
// */
//void ignore_empty_spaces_and_comments() {
//    ignore_empty_spaces();
//    ignore_comments();
//    ignore_empty_spaces();
//}
//
///**
// * Armazena a posição do token.
// *
// * Armazena a posição em que o token foi encontrado na entrada.
// * Isso será útil para identificar a posição do token em caso de erro,
// * fornecendo um feedback mais preciso para o usuário.
// */
//void gravar_pos_token() {
//    Token->pos.linha = linha;
//    Token->pos.coluna = coluna;
//}
//
//char *obter_string(int pos_inicial) {
//    size_t tamanho = pos - pos_inicial;
//
//    char *retorno = (char *) malloc(tamanho + 1);
//
////    strncpy(retorno, texto + pos_inicial, tamanho);
//    retorno[tamanho] = '\0';
//
//    return retorno;
//}
//
//int *converte_string_para_inteiro(char *string) {
//    auto *num = (int *) calloc(1, sizeof(int));
//
//    *num = atoi(string);
//    return num;
//}
//
//double *converte_string_para_real(char *string) {
//    auto *num = (double *) calloc(1, sizeof(double));
//
//    *num = strtod(string, &string);
//    return num;
//}
//
//char *converte_char_para_string(char ch) {
//    auto *string = (char *) malloc(sizeof(char));
//    *string = ch;
//    return string;
//}
//
///**
// * Percorre a entrada até obter o próximo token válido.
// */
//struct Token *get_prox_token() {
//    ignore_empty_spaces_and_comments();
//
//    // Agora estamos no inicio de um simbolo ou no fim do arquivo
//    gravar_pos_token();
//
//    // Posição da entrada no inicio do reconhecimento
//    int pos_inicial = pos;
//
//    if (eh_final_do_texto(caracter)) {
//        Token->tipo = EOF;
//        Token->valor = (void *) "<EOF>";
//        return Token;
//    } else if (eh_digito(caracter)) {
//        Token->tipo = INTEIRO;
//
//        while (eh_digito(caracter) || eh_ponto(caracter)) {
//            if (eh_ponto(caracter)) Token->tipo = REAL;
//            avancar();
//        }
//
//        char *resultado = obter_string(pos_inicial);
//
//        if (Token->tipo == INTEIRO) {
//            Token->valor = converte_string_para_inteiro(resultado);
//        } else {
//            Token->valor = converte_string_para_real(resultado);
//        }
//
//        return Token;
//    } else if (eh_operador_logico(caracter)) {
//        Token->tipo = OP_LOGICO;
//        Token->valor = converte_char_para_string(caracter);
//        avancar();
//        return Token;
//    } else if (eh_operador_matematico(caracter)) {
//        Token->tipo = OP_MAT;
//        Token->valor = converte_char_para_string(caracter);
//        avancar();
//        return Token;
//    } else if (caracter == '[') {
//        Token->tipo = L_BRACKET;
//        Token->valor = converte_char_para_string(caracter);
//        avancar();
//        return Token;
//    } else if (caracter == ']') {
//        Token->tipo = R_BRACKET;
//        Token->valor = converte_char_para_string(caracter);
//        avancar();
//        return Token;
//    } else if (caracter == ';') {
//        Token->tipo = PT_VIRGULA;
//        Token->valor = converte_char_para_string(caracter);
//        avancar();
//        return Token;
//    } else if (caracter == ',') {
//        Token->tipo = VIRGULA;
//        Token->valor = converte_char_para_string(caracter);
//        avancar();
//        return Token;
//    }
////    else if (caracter == ':' && texto[pos + 1] == '=') {
////        Token->tipo = ATTR;
////        Token->valor = ":=";
////        avancar();
////        avancar();
////        return Token;
////    }
//    else if (caracter == ':') {
//        Token->tipo = DOIS_PONTOS;
//        Token->valor = converte_char_para_string(caracter);
//        avancar();
//        return Token;
//    } else if (caracter == '(') {
//        Token->tipo = L_PAREN;
//        Token->valor = converte_char_para_string(caracter);
//        avancar();
//        return Token;
//    } else if (caracter == ')') {
//        Token->tipo = R_PAREN;
//        Token->valor = converte_char_para_string(caracter);
//        avancar();
//        return Token;
//    } else if (caracter == '.') {
//        Token->tipo = PONTO;
//        Token->valor = converte_char_para_string(caracter);
//        avancar();
//        return Token;
//    } else if (caracter == '"') {
//        Token->tipo = STRING;
//
//        avancar();
//
//        pos_inicial = pos;
//
//        while (caracter != '"') avancar();
//
//        // Tamanho da string reconhecida
//        size_t tamanho = pos - pos_inicial;
//
//        avancar();
//
//        // String reconhecida
//        auto *retorno = (char *) malloc(tamanho + 1);
////        strncpy(retorno, texto + pos_inicial, tamanho);
//        retorno[tamanho] = '\0';
//
//        Token->valor = retorno;
//
//        return Token;
//    } else if (eh_letra(caracter)) {
//        Token->tipo = ID;
//        while (eh_letra_ou_digito(caracter)) avancar();
//
//        // Tamanho da string reconhecida
//        size_t tamanho = pos - pos_inicial;
//
//        // String reconhecida
//        auto *retorno = (char *) malloc(tamanho + 1);
////        strncpy(retorno, texto + pos_inicial, tamanho);
//        retorno[tamanho] = '\0';
//
//        Token->valor = retorno;
//
//        if (strcmp(retorno, "real") == 0 || strcmp(retorno, "integer") == 0) Token->tipo = TIPO_DADO;
//
//        else if (strcmp(retorno, "program") == 0) Token->tipo = PROGRAMA;
//
//        else if (strcmp(retorno, "begin") == 0) Token->tipo = INICIO;
//
//        else if (strcmp(retorno, "end") == 0) Token->tipo = FIM;
//
//        else if (strcmp(retorno, "const") == 0) Token->tipo = CONSTANTE;
//
//        else if (strcmp(retorno, "type") == 0) Token->tipo = TIPO;
//
//        else if (strcmp(retorno, "var") == 0) Token->tipo = VAR;
//
//        else if (strcmp(retorno, "array") == 0) Token->tipo = VETOR;
//
//        else if (strcmp(retorno, "of") == 0) Token->tipo = OF;
//
//        else if (strcmp(retorno, "record") == 0) Token->tipo = RECORD;
//
//        else if (strcmp(retorno, "function") == 0) Token->tipo = FUNCTION;
//
//        else if (strcmp(retorno, "while") == 0) Token->tipo = WHILE;
//
//        else if (strcmp(retorno, "if") == 0) Token->tipo = IF;
//
//        else if (strcmp(retorno, "then") == 0) Token->tipo = THEN;
//
//        else if (strcmp(retorno, "write") == 0) Token->tipo = WRITE;
//
//        else if (strcmp(retorno, "read") == 0) Token->tipo = READ;
//
//        else if (strcmp(retorno, "else") == 0) Token->tipo = ELSE;
//
//        return Token;
//    } else {
//        Token->tipo = ERROR;
//        Token->valor = NULL;
//        return Token;
//    }
//}
//
//void imprimir_token(struct Token *token) {
//    switch (token->tipo) {
//        case EOF:
//            printf("(EOF, NULL)\n");
//            break;
//        case PROGRAMA:
//            printf("(PROGRAMA, %s)\n", (char *) token->valor);
//            break;
//        case INICIO:
//            printf("(INICIO, %s)\n", (char *) token->valor);
//            break;
//        case FIM:
//            printf("(FIM, %s)\n", (char *) token->valor);
//            break;
//        case CONSTANTE:
//            printf("(CONSTANTE, %s)\n", (char *) token->valor);
//            break;
//        case TIPO:
//            printf("(TIPO, %s)\n", (char *) token->valor);
//            break;
//        case VAR:
//            printf("(VAR, %s)\n", (char *) token->valor);
//            break;
//        case EQUAL:
//            printf("(EQUAL, %s)\n", (char *) token->valor);
//            break;
//        case PT_VIRGULA:
//            printf("(PT_VIRGULA, %s)\n", (char *) token->valor);
//            break;
//        case STRING:
//            printf("(STRING, %s)\n", (char *) token->valor);
//            break;
//        case DOIS_PONTOS:
//            printf("(DOIS_PONTOS, %s)\n", (char *) token->valor);
//            break;
//        case COMMA:
//            printf("(COMMA, %s)\n", (char *) token->valor);
//            break;
//        case INTEIRO:
//            printf("(INTEIRO, %d)\n", *(int *) token->valor);
//            break;
//        case REAL:
//            printf("(REAL, %g)\n", *(double *) token->valor);
//            break;
//        case VETOR:
//            printf("(VETOR, %s)\n", (char *) token->valor);
//            break;
//        case L_BRACKET:
//            printf("(L_BRACKET, %s)\n", (char *) token->valor);
//            break;
//        case R_BRACKET:
//            printf("(R_BRACKET, %s)\n", (char *) token->valor);
//            break;
//        case OF:
//            printf("(OF, %s)\n", (char *) token->valor);
//            break;
//        case RECORD:
//            printf("(RECORD, %s)\n", (char *) token->valor);
//            break;
//        case FUNCTION:
//            printf("(FUNCTION, %s)\n", (char *) token->valor);
//            break;
//        case L_PAREN:
//            printf("(L_PAREN, %s)\n", (char *) token->valor);
//            break;
//        case R_PAREN:
//            printf("(R_PAREN, %s)\n", (char *) token->valor);
//            break;
//        case ATTR:
//            printf("(ATTR, %s)\n", (char *) token->valor);
//            break;
//        case WHILE:
//            printf("(WHILE, %s)\n", (char *) token->valor);
//            break;
//        case IF:
//            printf("(IF, %s)\n", (char *) token->valor);
//            break;
//        case THEN:
//            printf("(THEN, %s)\n", (char *) token->valor);
//            break;
//        case WRITE:
//            printf("(WRITE, %s)\n", (char *) token->valor);
//            break;
//        case READ:
//            printf("(READ, %s)\n", (char *) token->valor);
//            break;
//        case ELSE:
//            printf("(ELSE, %s)\n", (char *) token->valor);
//            break;
//        case BIGGER_THAN:
//            printf("(BIGGER_THAN, %s)\n", (char *) token->valor);
//            break;
//        case LESS_THAN:
//            printf("(LESS_THAN, %s)\n", (char *) token->valor);
//            break;
//        case EXCLAMATION:
//            printf("(EXCLAMATION, %s)\n", (char *) token->valor);
//            break;
//        case PLUS:
//            printf("(PLUS, %s)\n", (char *) token->valor);
//            break;
//        case MINUS:
//            printf("(MINUS, %s)\n", (char *) token->valor);
//            break;
//        case TIMES:
//            printf("(TIMES, %s)\n", (char *) token->valor);
//            break;
//        case SLASH:
//            printf("(SLASH, %s)\n", (char *) token->valor);
//            break;
//        case ID:
//            printf("(ID, %s)\n", (char *) token->valor);
//            break;
//        case TIPO_DADO:
//            printf("(TIPO_DADO, %s)\n", (char *) token->valor);
//            break;
//        case OP_LOGICO:
//            printf("(OP_LOGICO, %s)\n", (char *) token->valor);
//            break;
//        case OP_MAT:
//            printf("(OP_MAT, %s)\n", (char *) token->valor);
//            break;
//        case VIRGULA:
//            printf("(VIRGULA, %s)\n", (char *) token->valor);
//            break;
//        case PONTO:
//            printf("(PONTO, %s)\n", (char *) token->valor);
//            break;
//    }
//}
