#include "lex.h"

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


Lex::Lex(const string &file_name) {
    source = new ifstream(file_name);

    if (!source->is_open()) {
        throw runtime_error("Could not open file: " + file_name);
    }

    next();
}

Lex::~Lex() {
    source->close();
    delete source;
}

Token *Lex::getNextToken() {
    skipCommentsAndWhiteSpaces();

    if (eh_digito(ch) || eh_ponto(ch)) {
        return getNumberToken();
    }

    if (eh_letra(ch)) {
        return getId();
    }

    return nullptr;
}

void Lex::next() {
    if (source->eof()) {
        ch = '\0';
    } else {
        ch = source->get();
    }
}

void Lex::skipComments() {
    if (eh_inicio_comentario(ch)) {
        while (!eh_final_comentario(ch)) {
            next();
        }
        next();
    }
}

void Lex::skipWhiteSpaces() {
    while (eh_espaco(ch)) {
        next();
    }
}

void Lex::skipCommentsAndWhiteSpaces() {
    skipWhiteSpaces();
    skipComments();
    skipWhiteSpaces();
}

/**
 * Retorna um token numérico.
 *
 * Retorna um token numérico se o início da entrada for um dígito ou um ponto,
 * caso contrário, retorna nullptr.
 *
 * É permitido o uso de um (e apenas um) ponto no número.
 *
 * Exemplos de entradas reconhecidas:
 * 20   -> (Integer, 20)
 * 12.0 -> (Real, 12)
 * 0.20 -> (Real, 0.2)
 * .20  -> (Real, 0.2)
 * 20.  -> (Real 20)
 *
 * Caso exista mais de um ponto na entrada, apenas os dígitos reconhecidos antes
 * do último ponto serão considerados no token.
 */
Token *Lex::getNumberToken() {
    if (!eh_digito(ch) && !eh_ponto(ch)) return nullptr;

    bool isReal = false;
    string value;

    while (eh_digito(ch) || eh_ponto(ch)) {
        if (eh_ponto(ch)) {
            if (isReal) {
                break;
            } else {
                isReal = true;
            }
        }
        value.push_back(ch);
        next();
    }

    if (isReal) {
        auto *num = new double;
        *num = stod(value);
        return new Real(num);
    } else {
        auto *num = new int;
        *num = stoi(value);
        return new Integer(num);
    }
}

Token *Lex::getId() {
    if (!eh_letra(ch)) return nullptr;

    auto *value = new string;

    while (eh_letra(ch)) {
        value->push_back(ch);
        next();
    }

    if (*value == "program") {
        return new Program();
    }

    if (*value == "begin") {
        return new Begin();
    }

    if (*value == "end") {
        return new End();
    }

    return new Var(value);
}
