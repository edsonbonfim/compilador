#include "token.h"

Token::~Token() = default;

Token::Token(void *value) {
    this->value = value;
}
