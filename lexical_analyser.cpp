#include "lexical_analyser.h"

LexicalAnalyser::LexicalAnalyser(const std::string& config_file){
    cursor_pos_ = 0;
    line_pos_ = 1;
    currentTokenType_ = NO_TYPE;
    token_type_strings_.push_back("NO_TYPE");

    comment_begin_ = "";
    comment_end_ = "";

    std::ifstream config{ config_file };
    if(!config.is_open()){
        std::cerr << "Can't open file " << config_file;
    }

    std::string line;
    while( getline(config, line) ){

        if(line.substr(0, 2) == "#a"){
            alphabet_.resize(128);
            std::string tmp_alpha;
            std::istringstream(line) >> tmp_alpha >> tmp_alpha;
            for( uint i = 0; i < tmp_alpha.size(); i++ ){
                alphabet_[ (uint)tmp_alpha[i] ] = 1;
            }
            alphabet_[ (uint)'\n' ] = 2;
            alphabet_[ (uint)'\t' ] = 2;
            alphabet_[ (uint)'\r' ] = 2;
            alphabet_[ (uint)'\b' ] = 2;
            alphabet_[ (uint)' ' ] = 2;
        }
        else if(line.substr(0, 2) == "#c"){
            std::istringstream(line.substr(3)) >> comment_begin_ >> comment_end_;
            if(comment_end_.empty()) comment_end_ = "\n";
        }
        else if(line.substr(0, 2) == "#t"){
            std::string type_name;
            std::istringstream(line.substr(3)) >> type_name;
            token_type_strings_.push_back(type_name);
        }
        else if(line.substr(0, 2) == "#g"){
            TokenType token_type;
            std::istringstream(line.substr(3)) >> token_type;
            genericTokenType_.insert(token_type);
        }
        else if(line.substr(0, 2) == "#k"){
            std::string token;
            TokenType token_type;
            std::istringstream(line.substr(3)) >> token >> token_type;
            special_.emplace(token, token_type);
        }
        else if(line.substr(0, 2) == "#s") break;
    }

    uint num_states;
    std::vector< TokenType > state_type;
    std::vector< uint > transitions;

    getline(config, line);
    std::istringstream(line) >> num_states;

    getline(config, line);
    {std::istringstream line_stream(line);
    for(uint i = 0; i < num_states; i++){
        TokenType tmp_int;
        line_stream >> tmp_int;
        state_type.push_back(tmp_int);
    }}

    while( getline(config, line) ){
        uint current_state, next_state;
        std::string symbols;

        std::istringstream(line) >> current_state >> symbols >> next_state;

        for(uint i = 0; i < symbols.size(); i++){
            transitions.push_back(current_state);
            transitions.push_back((uint)symbols[i]);
            transitions.push_back(next_state);
        }
    }
    config.close();

    automato_ = new Automato{ num_states, state_type, transitions };
}

void LexicalAnalyser::analyse(const std::string& input_file){

    std::ifstream input(input_file);

    std::stringstream sstr;
    sstr << input.rdbuf();
    input_ = sstr.str();

    while( cursor_pos_ < input_.size() ){
        try{
            readNextSymbol();
        }
        catch (LexicalErrorException err){
            std::cout << "\n\nAt line " << err.line_ << ": " << err.description_ << std::endl;
            break;
        }
    }

    input.close();
}

void LexicalAnalyser::readNextSymbol(){

    bool flag_comment = false;

    // Discard comments
    if(input_.substr(cursor_pos_, comment_begin_.size()) == comment_begin_){
        while(input_.substr(cursor_pos_, comment_end_.size()) != comment_end_)
            if(cursor_pos_ >= input_.size())
                throw LexicalErrorException{line_pos_, "Undefined end of comment error"};
            else if(input_[cursor_pos_++] == NEW_LINE)
                line_pos_++;
        flag_comment = true;
        cursor_pos_ += comment_end_.size()-1;
    }

    if(input_[cursor_pos_] == NEW_LINE) line_pos_++;

    if( !isOnLexicalAlphabet( input_[cursor_pos_] ) &&
        !isSeparator( input_[cursor_pos_] ) &&
        (input_[cursor_pos_] != comment_end_.back()  || !flag_comment) )
        throw LexicalErrorException{line_pos_, "Undefined symbol error"};

    // Update Automato
    automato_->next_state(flag_comment ? ' ' : input_[cursor_pos_]);

    // If it backs to begin, a new token is generated
    if( automato_->isCurrentStart() ){
        if( !accepted_buffer_.empty() ){
            std::string currentToken( accepted_buffer_.begin(), accepted_buffer_.end() );
            accepted_buffer_.clear();

            if(isGenericType(currentTokenType_) && isSpecial(currentToken) )
                currentTokenType_ = special_[currentToken];

            Token new_token = {currentTokenType_, currentToken, currentTokenLine_};
            outputTokeList_.push_back(new_token);
        }

        if( !isOnLexicalAlphabet( input_[cursor_pos_] ) || flag_comment )
            cursor_pos_++;
        return;
    }

    // Update buffers
    if( isOnLexicalAlphabet( input_[cursor_pos_] ) )
       recent_buffer_.push_back(input_[cursor_pos_]);

    if( automato_->isCurrentFinal() ){
        accepted_buffer_.insert(accepted_buffer_.end(), recent_buffer_.begin(), recent_buffer_.end());
        recent_buffer_.clear();
        currentTokenType_ = automato_->currentType();
        currentTokenLine_ = line_pos_;
    }

    cursor_pos_++;
}

bool LexicalAnalyser::isOnLexicalAlphabet( Symbol symbol ){
    return alphabet_[symbol] == 1;
}

bool LexicalAnalyser::isSeparator( Symbol symbol ){
    return alphabet_[symbol] == 2;
}

void LexicalAnalyser::printResult(){

    std::cout << std::setw(12) << std::left << std::setfill('-') << "|";
    std::cout << std::setw(22) << std::setfill('-') << "+";
    std::cout << std::setw(7) << std::setfill('-') << "+" << "|" << std::endl;
    std::cout << std::setfill(' ');
    std::cout << std::setw(12) << std::left << "| TOKEN";
    std::cout << std::setw(22) << std::left << "| CLASSIFICATION";
    std::cout << std::setw(7) << std::left << "| LINE" << "|" << std::endl;
    std::cout << std::setw(12) << std::setfill('-') << "|";
    std::cout << std::setw(22) << std::setfill('-') << "+";
    std::cout << std::setw(7) << std::setfill('-') << "+" << "|" << std::endl;

    for( Token t : outputTokeList_){
        std::cout << std::setfill(' ');
        std::cout << std::setw(0) << std::left << "| ";
        std::cout << std::setw(10) << std::left << t.token_;
        std::cout << std::setw(0) << std::left << "| ";
        std::cout << std::setw(20) << std::left << token_type_strings_[t.type_];
        std::cout << std::setw(0) << std::left << "| ";
        std::cout << std::setw(5) << std::left << t.line_ << "|" << std::endl;
        std::cout << std::setw(12) << std::setfill('-') << "|";
        std::cout << std::setw(22) << std::setfill('-') << "+";
        std::cout << std::setw(7) << std::setfill('-') << "+" << "|" << std::endl;
    }
}

bool LexicalAnalyser::isSpecial(const std::string& token){
    if(special_.find(token) != special_.end())
        return true;
    return false;
}

bool LexicalAnalyser::isGenericType(TokenType token_type){
    if(genericTokenType_.find(token_type) != genericTokenType_.end())
        return true;
    return false;
}
