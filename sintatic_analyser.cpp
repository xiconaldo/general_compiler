#include "sintatic_analyser.h"

SintaticAnalyser::SintaticAnalyser(const std::string& config_file){

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

	error_info_.clear();
	cursor_pos_ = 0;

	token_input_ = token_input;
	currentToken = token_input_[cursor_pos_];

	try{
		expandNonTerminal(-1);

		if( cursor_pos_ < token_input_.size() )
			throw SintaticErrorException(currentToken.line_, "Unexpected symbols at end of file");
	}
	catch( SintaticErrorException err ){
		std::ostringstream ss;
		ss << "At line " << err.line_ << ": " << err.description_;
		error_info_.push_back(ss.str());
	}
}

void SintaticAnalyser::expandNonTerminal(int non_terminal_id){

	std::vector< Token > current_rule = rules_def_[ checkForRuleMatch(non_terminal_id) ];

	for(uint i = 0; i < current_rule.size(); i++){

		checkForEarlyEndOfFile();

		if( current_rule[i].type_ > 0 && current_rule[i].type_ == currentToken.type_ ||
			current_rule[i].type_ == 0 && current_rule[i].token_ == currentToken.token_ )
			nextToken();

		else if( current_rule[i].type_ < 0)
			expandNonTerminal(current_rule[i].type_);

		else if( current_rule[i].type_ == 0 && current_rule[i].token_.empty() )
			continue;

		else
			throw SintaticErrorException(currentToken.line_, "No match rule for token");
	}
}

int SintaticAnalyser::checkForRuleMatch(int non_terminal_id){

	int rule_id = matchRule(non_terminal_id);

	if(rule_id >= 0)
		return rule_id;
	else
		throw SintaticErrorException(currentToken.line_, "No match rule for token");


}

int SintaticAnalyser::matchRule(int non_terminal_id){

	for( uint i = 0; i < rules_id_[-non_terminal_id].size(); i++){

		uint rule_id = rules_id_[-non_terminal_id][i];
		const Token& rule_init = rules_def_[rule_id][0];

		if( rule_init.type_ > 0 && rule_init.type_ == currentToken.type_ ||
			rule_init.type_ == 0 && rule_init.token_ == currentToken.token_ ||
			rule_init.type_ < 0 && matchRule( rule_init.type_ ) >= 0 ||
			rule_init.type_ == 0 && rule_init.token_.empty() )
			return rule_id;
	}
	return -1;
}

void SintaticAnalyser::nextToken(){

	if(++cursor_pos_ < token_input_.size())
		currentToken = token_input_[cursor_pos_];
}

void SintaticAnalyser::checkForEarlyEndOfFile(){
	if( cursor_pos_ == token_input_.size() )
		throw SintaticErrorException(currentToken.line_, "Unexpected end of file");
}

void SintaticAnalyser::printResults(){
	if( !error_info_.empty() ){
		std::cout << "Sintatic error(s) detected:" << std::endl;
		for( std::string err : error_info_)
			std::cout << err << std::endl;
	}
}

bool SintaticAnalyser::success(){
	return error_info_.empty();
}
