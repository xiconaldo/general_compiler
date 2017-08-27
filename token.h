#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include <string>

typedef int TokenType;
typedef unsigned int uint;

enum{ NO_TYPE };

struct Token{
    TokenType type_;
    std::string token_;
    uint line_;
};

#endif // TOKEN_H
