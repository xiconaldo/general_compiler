#include "lexical_analyser.h"

LexicalAnalyser::LexicalAnalyser(const std::string& config_file){
    cursor_pos_ = 0;
    line_pos_ = 1;
    currentTokenType_ = NO_TYPE;
    currentErrorLine_ = currentTokenLine_ = 1;
    token_type_strings_.push_back("NO_TYPE");
    error_type_strings_.push_back("NO_TYPE");

    std::ifstream config{ config_file };
    if(!config.is_open()){
        std::cerr << "Can't open file " << config_file;
    }

    std::string line;
    while( getline(config, line) ){

        if(line.substr(0, 2) == "#z"){

            uint tmp_int;
            std::istringstream sstr_line(line.substr(3));

            while( true ){
                sstr_line >> tmp_int;

                if(sstr_line)
                    separator_.insert( (Symbol)tmp_int );
                else
                    break;
            }
        }
        else if(line.substr(0, 2) == "#t"){
            token_type_strings_.push_back(line.substr(3));

            if(token_type_strings_.back().back() == '*')
                ignoredTokenType_.insert(token_type_strings_.size()-1);
        }
        else if(line.substr(0, 2) == "#e"){
            error_type_strings_.push_back(line.substr(3));
        }
        else if(line.substr(0, 2) == "#g"){
            TokenType token_type;
            std::istringstream line_str(line.substr(3));

             while( true ){
                line_str >> token_type;

                if(line_str)
                    genericTokenType_.insert(token_type);
                else
                    break;
            }
        }
        else if(line.substr(0, 2) == "#k"){
            std::string token;
            TokenType token_type;
            std::istringstream(line.substr(3)) >> token >> token_type;
            special_.emplace(token, token_type);
        }
        else if(line.substr(0, 2) == "#s") break;
    }

    std::vector< TokenType > state_type;
    std::vector< uint > transitions;

    getline(config, line);
    {std::istringstream line_stream(line);
    while( true ){
        TokenType tmp_int;
        line_stream >> tmp_int;

        if(line_stream)
            state_type.push_back(tmp_int);
        else
            break;
    }}

    while( getline(config, line) ){
        uint current_state, next_state;
        std::string symbols;

        std::istringstream(line) >> current_state >> symbols >> next_state;

        if(symbols == "all"){

            for(uint i = 0; i < NUM_SYMBOLS; i++){
                transitions.push_back(current_state);
                transitions.push_back((uint)i);
                transitions.push_back(next_state);
            }
        }
        else if(symbols == "separator"){

            for(uint i = 0; i < NUM_SYMBOLS; i++){
                if(!isSeparator( (Symbol)i )) continue;
                transitions.push_back(current_state);
                transitions.push_back((uint)i);
                transitions.push_back(next_state);
            }
        }
        else if(symbols == "not_separator"){

            for(uint i = 0; i < NUM_SYMBOLS; i++){
                if(isSeparator( (Symbol)i )) continue;
                transitions.push_back(current_state);
                transitions.push_back((uint)i);
                transitions.push_back(next_state);
            }
        }
        else if(symbols == "new_line"){
            transitions.push_back(current_state);
            transitions.push_back((uint)'\n');
            transitions.push_back(next_state);
        }
        else if(symbols == "tab"){
            transitions.push_back(current_state);
            transitions.push_back((uint)'\t');
            transitions.push_back(next_state);
        }
        else if(symbols == "space"){
            transitions.push_back(current_state);
            transitions.push_back((uint)' ');
            transitions.push_back(next_state);
        }
        else{
            for(uint i = 0; i < symbols.size(); i++){
                transitions.push_back(current_state);
                transitions.push_back((uint)symbols[i]);
                transitions.push_back(next_state);
            }
        }
    }
    config.close();

    automato_ = new Automato{ state_type, transitions };
}

LexicalAnalyser::~LexicalAnalyser(){
    delete automato_;
}


void LexicalAnalyser::analyse(const std::string& input_file){

    std::ifstream input(input_file);

    std::stringstream sstr;
    sstr << input.rdbuf();
    input_ = sstr.str();
    input_.push_back(NEW_LINE);

    try{
        while( cursor_pos_ < input_.size() )
            readNextSymbol();

        if(!automato_->isCurrentFinal())
            throw LexicalErrorException(currentErrorLine_, error_type_strings_[-automato_->currentType()]);
    }
    catch (LexicalErrorException err){
        std::ostringstream ss;
        ss << "Error at line " << err.line_ << ": " << err.description_;
        error_info_.push_back(ss.str());
    }

    input.close();
}

void LexicalAnalyser::readNextSymbol(){

    if(input_[cursor_pos_] == NEW_LINE) line_pos_++;

    // Update Automato
    automato_->next_state(input_[cursor_pos_]);

    // If it backs to begin, a new token is generated
    if( automato_->isCurrentStart() ){
        if( !accepted_buffer_.empty() ){
            std::string currentToken( accepted_buffer_.begin(), accepted_buffer_.end() );
            accepted_buffer_.clear();

            if(isGenericType(currentTokenType_) && isSpecial(currentToken) )
                currentTokenType_ = special_[currentToken];

            Token new_token = {currentTokenType_, currentToken, currentTokenLine_};

            currentErrorLine_ = currentTokenLine_ + (input_[cursor_pos_] == NEW_LINE);

            if( !isIgnoredType(currentTokenType_) )
                outputTokeList_.push_back(new_token);
        }

        if(isSeparator(input_[cursor_pos_]) && recent_buffer_.empty()){
            cursor_pos_++;
        }
        else{
            if(input_[cursor_pos_] == NEW_LINE) line_pos_--;
            cursor_pos_ -= recent_buffer_.size();
            recent_buffer_.clear();
        }

        return;
    }

    // Update buffers
    recent_buffer_.push_back(input_[cursor_pos_]);

    if( automato_->isCurrentFinal() ){
        accepted_buffer_.insert(accepted_buffer_.end(), recent_buffer_.begin(), recent_buffer_.end());
        recent_buffer_.clear();
        currentTokenType_ = automato_->currentType();
        currentTokenLine_ = line_pos_;
    }

    cursor_pos_++;
}

bool LexicalAnalyser::isSeparator( Symbol symbol ){
    if(separator_.find(symbol) != separator_.end())
        return true;
    return false;
}

void LexicalAnalyser::printResult(){

    uint space = 12;
    for( Token t : outputTokeList_)
        space = std::max( (uint)t.token_.size(), space);
    space += 3;

    std::cout << std::setw( space ) << std::left << std::setfill('-') << "|";
    std::cout << std::setw(22) << std::setfill('-') << "+";
    std::cout << std::setw(7) << std::setfill('-') << "+" << "|" << std::endl;
    std::cout << std::setfill(' ');
    std::cout << std::setw( space ) << std::left << "| TOKEN";
    std::cout << std::setw(22) << std::left << "| CLASSIFICATION";
    std::cout << std::setw(7) << std::left << "| LINE" << "|" << std::endl;
    std::cout << std::setw( space ) << std::setfill('-') << "|";
    std::cout << std::setw(22) << std::setfill('-') << "+";
    std::cout << std::setw(7) << std::setfill('-') << "+" << "|" << std::endl;

    for( Token t : outputTokeList_){
        std::cout << std::setfill(' ');
        std::cout << std::setw(0) << std::left << "| ";
        std::cout << std::setw( space-2 ) << std::left << t.token_;
        std::cout << std::setw(0) << std::left << "| ";
        std::cout << std::setw(20) << std::left << token_type_strings_[t.type_];
        std::cout << std::setw(0) << std::left << "| ";
        std::cout << std::setw(5) << std::left << t.line_ << "|" << std::endl;
        std::cout << std::setw( space ) << std::setfill('-') << "|";
        std::cout << std::setw(22) << std::setfill('-') << "+";
        std::cout << std::setw(7) << std::setfill('-') << "+" << "|" << std::endl;
    }

    std::cout << std::endl;
    printErrors();
}

void LexicalAnalyser::printErrors(){
    if(error_info_.empty()){
        std::cout << "ANALYSIS COMPLETED SUCCESSFULLY" << std::endl;
    }
    else{
        std::cout << "ANALYSIS FAILED:" << std::endl;
        for( std::string err : error_info_)
            std::cout << err << std::endl;
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

bool LexicalAnalyser::isIgnoredType(TokenType token_type){
    if(ignoredTokenType_.find(token_type) != ignoredTokenType_.end())
        return true;
    return false;
}
