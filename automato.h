#ifndef AUTOMATO_H_INCLUDED
#define AUTOMATO_H_INCLUDED

#include <string>
#include <vector>
#include <iostream>
#include "token.h"

typedef unsigned char Symbol;

enum{NUM_SYMBOLS = 256};

struct State{
    uint id_;
    TokenType type_;
};

class Automato{

private:
    uint current_id_;
    uint **trans_;
    std::vector< State > states_;

    State& current();

    State& start();

public:

    Automato(const std::vector< TokenType >& state_type,
             const std::vector< uint >& transitions);

    ~Automato();

    void next_state(Symbol input);

    bool isCurrentFinal();

    bool isCurrentStart();

    TokenType currentType();

};

#endif // AUTOMATO_H_INCLUDED
