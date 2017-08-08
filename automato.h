#ifndef AUTOMATO_H_INCLUDED
#define AUTOMATO_H_INCLUDED

#include <string>
#include <vector>
#include <iostream>

typedef unsigned char Symbol;
typedef unsigned int uint;
typedef unsigned int TokenType;

enum{ NO_TYPE};

struct State{
    uint id_;
    TokenType type_;
};

class Automato{

private:
    uint num_states_;
    uint num_symbols_;

    uint start_id_;
    uint current_id_;
    uint **trans_;
    std::vector< State > states_;

    State& current();
    State& start();

public:

    Automato(uint num_states,
             const std::vector< TokenType >& state_type,
             const std::vector< uint >& transitions);

    void next_state(Symbol input);

    bool isCurrentFinal();

    bool isCurrentStart();

    TokenType currentType();

};

#endif // AUTOMATO_H_INCLUDED
