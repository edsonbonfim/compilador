#ifndef COMP_TOKEN_H
#define COMP_TOKEN_H

#include <iostream>

using namespace std;

class Token {
public:
    void *value;
    int pos;

    Token();

    explicit Token(void *value);

    virtual ~Token();
};

class Error : public Token {
public:
    explicit Error() : Token(new string("error")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Error const &error) {
        return _stream << "(Error, " << *(string *) error.value << ")" << endl;
    }
};

class Program : public Token {
public:
    explicit Program() : Token(new string("program")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Program const &program) {
        return _stream << "(Program, " << *(string *) program.value << ")" << endl;
    }
};

class Begin : public Token {
public:
    explicit Begin() : Token(new string("begin")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Begin const &begin) {
        return _stream << "(Begin, " << *(string *) begin.value << ")" << endl;
    }
};

class End : public Token {
public:
    explicit End() : Token(new string("end")) {};

    friend std::ostream &operator<<(std::ostream &_stream, End const &end) {
        return _stream << "(End, " << *(string *) end.value << ")" << endl;
    }
};

class Const : public Token {
public:
    explicit Const() : Token(new string("const")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Const const &_const) {
        return _stream << "(Const, " << *(string *) _const.value << ")" << endl;
    }
};

class Type : public Token {
public:
    explicit Type() : Token(new string("type")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Type const &type) {
        return _stream << "(Type, " << *(string *) type.value << ")" << endl;
    }
};

class Var : public Token {
public:
    explicit Var(string *value) : Token(value) {};

    friend std::ostream &operator<<(std::ostream &_stream, Var const &var) {
        return _stream << "(Var, " << *(string *) var.value << ")" << endl;
    }
};

class Array : public Token {
public:
    explicit Array() : Token(new string("array")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Array const &array) {
        return _stream << "(Array, " << *(string *) array.value << ")" << endl;
    }
};

class Of : public Token {
public:
    explicit Of() : Token(new string("of")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Of const &of) {
        return _stream << "(Of, " << *(string *) of.value << ")" << endl;
    }
};

class Record : public Token {
public:
    explicit Record() : Token(new string("record")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Record const &record) {
        return _stream << "(Record, " << *(string *) record.value << ")" << endl;
    }
};

class Function : public Token {
public:
    explicit Function() : Token(new string("function")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Function const &function) {
        return _stream << "(Function, " << *(string *) function.value << ")" << endl;
    }
};

class Procedure : public Token {
public:
    explicit Procedure() : Token(new string("procedure")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Procedure const &procedure) {
        return _stream << "(Procedure, " << *(string *) procedure.value << ")" << endl;
    }
};

class Number : public Token {
public:
    explicit Number(void *value) : Token(value) {};
};

class Integer : public Number {
public:
    explicit Integer(int *value) : Number(value) {};

    friend std::ostream &operator<<(std::ostream &_stream, Integer const &integer) {
        return _stream << "(Integer, " << *(int *) integer.value << ")" << endl;
    }
};

class Real : public Number {
public:
    explicit Real(double *value) : Number(value) {};

    friend std::ostream &operator<<(std::ostream &_stream, Real const &real) {
        return _stream << "(Real, " << *(double *) real.value << ")" << endl;
    }
};

class While : public Token {
public:
    explicit While() : Token(new string("while")) {};

    friend std::ostream &operator<<(std::ostream &_stream, While const &_while) {
        return _stream << "(While, " << *(string *) _while.value << ")" << endl;
    }
};

class If : public Token {
public:
    explicit If() : Token(new string("if")) {};

    friend std::ostream &operator<<(std::ostream &_stream, If const &_if) {
        return _stream << "(If, " << *(string *) _if.value << ")" << endl;
    }
};

class Then : public Token {
public:
    explicit Then() : Token(new string("then")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Then const &then) {
        return _stream << "(Then, " << *(string *) then.value << ")" << endl;
    }
};

class Write : public Token {
public:
    explicit Write() : Token(new string("write")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Write const &write) {
        return _stream << "(Write, " << *(string *) write.value << ")" << endl;
    }
};

class Read : public Token {
public:
    explicit Read() : Token(new string("read")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Read const &read) {
        return _stream << "(Read, " << *(string *) read.value << ")" << endl;
    }
};

class Else : public Token {
public:
    explicit Else() : Token(new string("else")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Else const &_else) {
        return _stream << "(Else, " << *(string *) _else.value << ")" << endl;
    }
};

class Virgula : public Token {
public:
    explicit Virgula() : Token(new string(",")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Virgula const &virgula) {
        return _stream << "(Virgula, " << *(string *) virgula.value << ")" << endl;
    }
};

class Maior : public Token {
public:
    explicit Maior() : Token(new string(">")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Maior const &maior) {
        return _stream << "(Maior, " << *(string *) maior.value << ")" << endl;
    }
};

class Menor : public Token {
public:
    explicit Menor() : Token(new string("<")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Menor const &menor) {
        return _stream << "(Menor, " << *(string *) menor.value << ")" << endl;
    }
};

class Igual : public Token {
public:
    explicit Igual() : Token(new string("=")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Igual const &igual) {
        return _stream << "(Igual, " << *(string *) igual.value << ")" << endl;
    }
};

class Diferente : public Token {
public:
    explicit Diferente() : Token(new string("!=")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Diferente const &diferente) {
        return _stream << "(Diferente, " << *(string *) diferente.value << ")" << endl;
    }
};

class Soma : public Token {
public:
    explicit Soma() : Token(new string("+")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Soma const &soma) {
        return _stream << "(Soma, " << *(string *) soma.value << ")" << endl;
    }
};

class Sub : public Token {
public:
    explicit Sub() : Token(new string("-")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Sub const &sub) {
        return _stream << "(Sub, " << *(string *) sub.value << ")" << endl;
    }
};

class Mul : public Token {
public:
    explicit Mul() : Token(new string("*")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Mul const &mul) {
        return _stream << "(Mul, " << *(string *) mul.value << ")" << endl;
    }
};

class Div : public Token {
public:
    explicit Div() : Token(new string("/")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Div const &div) {
        return _stream << "(Div, " << *(string *) div.value << ")" << endl;
    }
};

class PontoVirgula : public Token {
public:
    explicit PontoVirgula() : Token(new string(";")) {};

    friend std::ostream &operator<<(std::ostream &_stream, PontoVirgula const &pontoVirgula) {
        return _stream << "(PontoVirgula, " << *(string *) pontoVirgula.value << ")" << endl;
    }
};

class AbreColchetes : public Token {
public:
    explicit AbreColchetes() : Token(new string("[")) {};

    friend std::ostream &operator<<(std::ostream &_stream, AbreColchetes const &abreColchetes) {
        return _stream << "(AbreColchetes, " << *(string *) abreColchetes.value << ")" << endl;
    }
};

class FechaColchetes : public Token {
public:
    explicit FechaColchetes() : Token(new string("]")) {};

    friend std::ostream &operator<<(std::ostream &_stream, FechaColchetes const &fechaColchetes) {
        return _stream << "(FechaColchetes, " << *(string *) fechaColchetes.value << ")" << endl;
    }
};

class AbreParenteses : public Token {
public:
    explicit AbreParenteses() : Token(new string("(")) {};

    friend std::ostream &operator<<(std::ostream &_stream, AbreParenteses const &abreParenteses) {
        return _stream << "(AbreParenteses, " << *(string *) abreParenteses.value << ")" << endl;
    }
};

class FechaParenteses : public Token {
public:
    explicit FechaParenteses() : Token(new string(")")) {};

    friend std::ostream &operator<<(std::ostream &_stream, FechaParenteses const &fechaParenteses) {
        return _stream << "(FechaParenteses, " << *(string *) fechaParenteses.value << ")" << endl;
    }
};

class Ponto : public Token {
public:
    explicit Ponto() : Token(new string(".")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Ponto const &ponto) {
        return _stream << "(Ponto, " << *(string *) ponto.value << ")" << endl;
    }
};

class String : public Token {
public:
    explicit String(string *value) : Token(value) {};

    friend std::ostream &operator<<(std::ostream &_stream, String const &_string) {
        return _stream << "(String, " << *(string *) _string.value << ")" << endl;
    }
};

class DoisPontos : public Token {
public:
    explicit DoisPontos() : Token(new string(":")) {};

    friend std::ostream &operator<<(std::ostream &_stream, DoisPontos const &doisPontos) {
        return _stream << "(DoisPontos, " << *(string *) doisPontos.value << ")" << endl;
    }
};

class Atribuicao : public Token {
public:
    explicit Atribuicao() : Token(new string(":=")) {};

    friend std::ostream &operator<<(std::ostream &_stream, Atribuicao const &atribuicao) {
        return _stream << "(Atribuicao, " << *(string *) atribuicao.value << ")" << endl;
    }
};

#endif //COMP_TOKEN_H
