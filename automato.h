#ifndef AUTOMATO_H_INCLUDED
#define AUTOMATO_H_INCLUDED

#define NUM_SYMBOLS 256

#include <string>
#include <vector>
#include <iostream>

typedef unsigned char Symbol;
typedef unsigned int uint;
typedef int TokenType;

enum{ NO_TYPE};

struct State{
    uint id_;
    TokenType type_;
};

class Automato{

private:
    uint current_id_;
    uint **trans_;
    std::vector< State > states_;

public:

    Automato(const std::vector< TokenType >& state_type,
             const std::vector< uint >& transitions);

    void next_state(Symbol input);

    bool isCurrentFinal();

    bool isCurrentStart();

    TokenType currentType();

    State& current();

    State& start();

};

#endif // AUTOMATO_H_INCLUDED
