#include "automato.h"

Automato::Automato(uint num_states,
                 const std::vector< TokenType >& state_type,
                 const std::vector< uint >& transitions)
{

    num_symbols_ = 128;
    num_states_ = num_states;
    start_id_ = 0;
    current_id_ = start_id_;

    // Create States list
    states_.reserve(num_states_);

    for(uint i = 0; i < num_states_; i++){
        states_.push_back( {i, state_type[i]} );
    }

    // Create transitions
    trans_ = new uint*[num_states_];

    for(uint i = 0; i < num_states_; i++)
        trans_[i] = new uint[num_symbols_](); // Initializes to zero
    // https://stackoverflow.com/questions/7546620/operator-new-initializes-memory-to-zero

    for(uint i = 0; i < transitions.size(); i += 3)
        trans_[ transitions[i] ][ transitions[i+1] ] = transitions[i+2];

}

void Automato::next_state(Symbol input){
    //std::cout << current_id_ << " -- " << input << " --> ";
    current_id_ = trans_[current_id_][input];
    //std::cout << current_id_ << std::endl;
}

State& Automato::current(){
    return states_[current_id_];
}

State& Automato::start(){
    return states_[start_id_];
}

bool Automato::isCurrentFinal(){
    return current().type_ >= 0;
}

bool Automato::isCurrentStart(){
    return current_id_ == start_id_;
}

TokenType Automato::currentType(){
    return current().type_;
}
