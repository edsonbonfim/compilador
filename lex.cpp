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

    if (ch == '\0') {
        return nullptr;
    }

    if (eh_digito(ch) || eh_ponto(ch)) {
        return getNumberToken();
    }

    if (ch == ';') {
        next();
        return new PontoVirgula();
    }

    if (ch == ',') {
        next();
        return new Virgula();
    }

    if (ch == '[') {
        next();
        return new AbreColchetes();
    }

    if (ch == ']') {
        next();
        return new FechaColchetes();
    }

    if (ch == '(') {
        next();
        return new AbreParenteses();
    }

    if (ch == ')') {
        next();
        return new FechaParenteses();
    }

    if (ch == ':') {
        next();
        skipWhiteSpaces();

        if (ch == '=') {
            next();
            return new Atribuicao();
        }

        return new DoisPontos();
    }

    if (ch == '=') {
        next();
        return new Igual();
    }

    if (ch == '.') {
        next();
        return new Ponto();
    }

    if (ch == '"') {
        next();
        return getCommentToken();
    }

    if (ch == '*') {
        next();
        return new Mul();
    }

    if (ch == '/') {
        next();
        return new Div();
    }

    if (ch == '+') {
        next();
        return new Soma();
    }

    if (ch == '-') {
        next();
        return new Sub();
    }

    if (ch == '>') {
        next();
        return new Maior();
    }

    if (ch == '<') {
        next();
        return new Menor();
    }

    if (ch == '!' && peek() == '=') {
        next();
        next();
        return new Diferente();
    }

    if (eh_letra(ch)) {
        return getId();
    }

    return new Error();
}

void Lex::next() {
    if (source->eof()) {
        ch = '\0';
    } else {
        ch = source->get();
    }
}


char Lex::peek() {
    if (source->eof()) {
        return '\0';
    }

    char result = source->get();
    source->unget();

    return result;
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

    if (*value == "const") {
        return new Const();
    }

    if (*value == "type") {
        return new Type();
    }

    if (*value == "array") {
        return new Array();
    }

    if (*value == "of") {
        return new Of();
    }

    if (*value == "record") {
        return new Record();
    }

    if (*value == "function") {
        return new Function();
    }

    if (*value == "procedure") {
        return new Procedure();
    }

    if (*value == "while") {
        return new While();
    }

    if (*value == "if") {
        return new If();
    }

    if (*value == "then") {
        return new Then();
    }

    if (*value == "write") {
        return new Write();
    }

    if (*value == "read") {
        return new Read();
    }

    if (*value == "else") {
        return new Else();
    }

    return new Var(value);
}

Token *Lex::getCommentToken() {
    if (ch != '"') {
        return nullptr;
    }

    next();

    auto *value = new string;

    while (ch != '"') {
        value->push_back(ch);
        next();
    }

    next();

    return new String(value);
}
