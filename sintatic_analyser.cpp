#include "sintatic_analyser.h"

SintaticAnalyser::SintaticAnalyser(const std::string& config_file){
	// Construct grammar table
	cursor_pos_ = 0;

	std::ifstream config{ config_file };
    if(!config.is_open()){
        std::cerr << "Can't open file " << config_file;
    }

    std::string line;
	int non_terminal_id;
	int non_terminal_count;

	getline(config, line);
	std::istringstream(line) >> non_terminal_count;
	rules_id_.resize(non_terminal_count+1);

    while( getline(config, line) ){
		std::istringstream line_stream(line);

		line_stream >> non_terminal_id;
		rules_id_[-non_terminal_id].push_back( rules_def_.size() );

		std::vector< Token > new_rule;
		int token_type;

		while(true){
			line_stream >> token_type;
			if(!line_stream) break;

			if( token_type ){
				new_rule.push_back( {token_type, "", 0} );
			}
			else{
				std::string token_name;
				line_stream >> token_name;
				new_rule.push_back( {token_type, token_name, 0});
			}

		}

		rules_def_.push_back(new_rule);
	}

	config.close();
}

void SintaticAnalyser::analyse(const std::vector< Token >& token_input){
	// Create optimization table
	token_input_ = token_input;
	nextToken();

	try{
		expandNonTerminal(-1);
	}
	catch( SintaticErrorException err ){
		std::cerr << "At rule " << err.line_ << ": " << err.description_ << std::endl;
	}
}

void SintaticAnalyser::expandNonTerminal(int non_terminal_id){
	int rule_id = checkMatchRule(non_terminal_id);

	std::cerr << rule_id+2 << " ";

	if(rule_id < 0) throw SintaticErrorException(non_terminal_id, currentToken.token_);

	std::vector< Token > current_rule = rules_def_[rule_id];

	for(uint i = 0; i < current_rule.size(); i++){

		if( current_rule[i].type_ > 0 && current_rule[i].type_ == currentToken.type_ ||
			current_rule[i].type_ == 0 && current_rule[i].token_ == currentToken.token_ )
			nextToken();

		else if( current_rule[i].type_ < 0)
			expandNonTerminal(current_rule[i].type_);

		else if( current_rule[i].type_ == 0 && current_rule[i].token_.empty() )
			continue;

		else
			throw SintaticErrorException(currentToken.line_, currentToken.token_);
	}
}

int SintaticAnalyser::checkMatchRule(int non_terminal_id){

	for( uint i = 0; i < rules_id_[-non_terminal_id].size(); i++){

		uint rule_id = rules_id_[-non_terminal_id][i];
		const Token& rule_init = rules_def_[rule_id][0];

		if( rule_init.type_ > 0 && rule_init.type_ == currentToken.type_ ||
			rule_init.type_ == 0 && rule_init.token_ == currentToken.token_ ||
			rule_init.type_ < 0 && checkMatchRule( rule_init.type_ ) >= 0 ||
			rule_init.type_ == 0 && rule_init.token_.empty() )
			return rule_id;
	}
	return -1;
}

void SintaticAnalyser::nextToken(){
	if(cursor_pos_ < token_input_.size())
		currentToken = token_input_[cursor_pos_++];

	std::cerr << std::endl << std::endl << token_input_[cursor_pos_-1].token_ << std::endl;
}
