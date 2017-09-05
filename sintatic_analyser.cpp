#include "sintatic_analyser.h"

std::vector<std::string> SintaticTree::token_type_strings_ = std::vector<std::string>();
std::vector<std::string> SintaticTree::non_terminal_strings_ = std::vector<std::string>();

SintaticTree& SintaticTree::createNewChild(){
	this->children_.push_back(SintaticTree());
	return this->children_.back();
}

void SintaticTree::print(){
	static int depth = -1;
	static std::vector<int> path;

	depth++;

	for(int i = 0; i < depth; i++){
		if(i == depth-1)
			std::cout << "+--";
		else if( path[i] == 0)
			std::cout << "|  ";
		else
			std::cout << "   ";
	}

	if(this->token_.type_ > 0 && this->token_.token_.empty())
		std::cout << token_type_strings_[this->token_.type_] << std::endl;
	else if(this->token_.type_ > 0)
		std::cout << this->token_.token_ << std::endl;
	else if(this->token_.type_ < 0)
		std::cout << non_terminal_strings_[-this->token_.type_] << std::endl;
	else if(this->token_.type_ == 0 && this->token_.token_.empty())
		std::cout << token_type_strings_[this->token_.type_] << std::endl;
	else
		std::cout << this->token_.token_ << std::endl;

	for(int i = 0; i < depth; i++){
		if( path[i] == 0)
			std::cout << "|  ";
		else
			std::cout << "   ";
	}

	if(!this->children_.empty())
		std::cout << "|  ";

	std::cout << std::endl;

	for(int i = 0; i < children_.size(); i++){
		if(i == children_.size()-1)
			path.push_back(1);
		else
			path.push_back(0);
		children_[i].print();
		path.pop_back();
	}

	depth--;
}

SintaticAnalyser::SintaticAnalyser(const std::string& config_file){

	cursor_pos_ = 0;
	token_type_strings_.push_back("EMPTY");
	non_terminal_strings_.push_back("NO_TYPE");

	std::ifstream config{ config_file };
	if(!config.is_open()){
		std::cerr << "Can't open file " << config_file;
	}

	std::string line;
	int non_terminal_id;
	int non_terminal_count;

    while( getline(config, line) ){
        if(line.substr(0, 2) == "#t"){
            token_type_strings_.push_back(line.substr(3));
		}

		else if(line.substr(0, 2) == "#n"){
            non_terminal_strings_.push_back(line.substr(3));
		}

        else if(line.substr(0, 2) == "#g"){
			std::istringstream(line.substr(3)) >> non_terminal_count;
			rules_id_.resize(non_terminal_count+1);
			break;
		}
    }

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

	SintaticTree::token_type_strings_ = token_type_strings_;
	SintaticTree::non_terminal_strings_ = non_terminal_strings_;
}

void SintaticAnalyser::analyse(const std::vector< Token >& token_input){

	error_info_.clear();
	cursor_pos_ = 0;

	token_input_ = token_input;
	currentToken = token_input_[cursor_pos_];

	try{
		expandNonTerminal(-1, root_);

		if( cursor_pos_ < token_input_.size() )
			throw SintaticErrorException(currentToken.line_, "Unexpected symbols at end of file");
	}
	catch( SintaticErrorException err ){
		std::ostringstream ss;
		ss << "At line " << err.line_ << ": " << err.description_;
		error_info_.push_back(ss.str());
	}
}

void SintaticAnalyser::expandNonTerminal(int non_terminal_id, SintaticTree& node){

	node.token_ = {non_terminal_id, "", 1};

	int rule_id = checkForRuleMatch(non_terminal_id);
	std::vector< Token > current_rule = rules_def_[ rule_id < 0 ? -rule_id-1 : rule_id ];

	for(uint i = 0; i < current_rule.size(); i++){

		checkForEarlyEndOfFile();

		if( current_rule[i].type_ > 0 && current_rule[i].type_ == currentToken.type_ ||
			current_rule[i].type_ == 0 && current_rule[i].token_ == currentToken.token_ ){
			node.createNewChild().token_ = currentToken;
			nextToken();
		}

		else if( current_rule[i].type_ < 0){
			expandNonTerminal(current_rule[i].type_, node.createNewChild() );
		}

		else if( current_rule[i].type_ == 0 && current_rule[i].token_.empty() ){
			node.createNewChild().token_ = current_rule[i]; // empty
			continue;
		}

		else{
			std::ostringstream msg;

			msg << "\""
				<< (current_rule[i].type_ ? token_type_strings_[current_rule[i].type_] : current_rule[i].token_)
				<< "\" expected but \""
				<< (currentToken.type_ ? token_type_strings_[currentToken.type_] : currentToken.token_)
				<< "\" found";

			throw SintaticErrorException(currentToken.line_, msg.str());
		}
	}
}

int SintaticAnalyser::checkForRuleMatch(int non_terminal_id){

	for( uint i = 0; i < rules_id_[-non_terminal_id].size(); i++){

		uint rule_id = rules_id_[-non_terminal_id][i];
		const Token& rule_init = rules_def_[rule_id][0];

		if( rule_init.type_ > 0 && rule_init.type_ == currentToken.type_ ||
			rule_init.type_ == 0 && rule_init.token_ == currentToken.token_ ||
			rule_init.type_ < 0 && checkForRuleMatch( rule_init.type_ ) >= 0 ||
			rule_init.type_ == 0 && rule_init.token_.empty() )
			return rule_id;
	}
	return -rules_id_[-non_terminal_id][0]-1;
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

void SintaticAnalyser::printTree(){
	root_.print();
}
