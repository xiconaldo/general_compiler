#include "automato.h"

Automato::Automato(const std::vector< TokenType >& state_type,
                   const std::vector< uint >& transitions)
{
    uint num_states = state_type.size();
    current_id_ = 0;

    // Create States list
    states_.reserve(num_states);

    for(uint i = 0; i < num_states; i++){
        states_.push_back( {i, state_type[i]} );
    }

    // Create transitions
    trans_ = new uint*[num_states];

    for(uint i = 0; i < num_states; i++)
        trans_[i] = new uint[NUM_SYMBOLS](); // Initializes to zero
    // https://stackoverflow.com/questions/7546620/operator-new-initializes-memory-to-zero

    for(uint i = 0; i < transitions.size(); i += 3)
        trans_[ transitions[i] ][ transitions[i+1] ] = transitions[i+2];

}

void Automato::next_state(Symbol input){
//    std::cout << current_id_ << " -- ";
//    if ((uint)input > 31U ) std::cout << input;
//    else std::cout << "\\" << (uint)input;
//    std::cout << " --> ";
    current_id_ = trans_[current_id_][input];
//    std::cout << current_id_ << std::endl;
}

State& Automato::current(){
    return states_[current_id_];
}

State& Automato::start(){
    return states_[0];
}

bool Automato::isCurrentFinal(){
    return current().type_ >= 0;
}

bool Automato::isCurrentStart(){
    return current_id_ == 0;
}

TokenType Automato::currentType(){
    return current().type_;
}
