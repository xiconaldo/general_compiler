#ifndef SINTATIC_ANALYSER_H_INCLUDED
#define SINTATIC_ANALYSER_H_INCLUDED

#include "token.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

struct SintaticTree{
    Token token_;
    std::vector< SintaticTree > children_;
	static std::vector<std::string> token_type_strings_;
	static std::vector<std::string> non_terminal_strings_;

    SintaticTree& createNewChild();
    void print();
};

struct SintaticErrorException{
    uint line_;
    std::string description_;

    SintaticErrorException(uint line, const std::string& description) :
        line_(line),
        description_(description)
    {}
};

class SintaticAnalyser{

private:
	std::vector< Token > token_input_;
	uint cursor_pos_;
	Token currentToken;

	std::vector< std::vector< uint > > rules_id_;
	std::vector< std::vector< Token > > rules_def_;

    SintaticTree root_;
    std::vector<std::string> token_type_strings_;
	std::vector<std::string> non_terminal_strings_;
	std::vector< std::string > error_info_;

public:

	SintaticAnalyser(const std::string& config_file);

	void analyse(const std::vector< Token >& token_input);

	void expandNonTerminal(int non_terminal_id, SintaticTree& node);

	int checkForRuleMatch(int non_terminal_id);

	void nextToken();

	void checkForEarlyEndOfFile();

    void printResults();

	bool success();

    void printTree();

};

#endif // LEXICAL_ANALYSER_H_INCLUDED
