#include "token.h"

Token::~Token() {
    delete value;
}

Token::Token(void *value) {
    this->value = value;
}

Token::Token() {

}
