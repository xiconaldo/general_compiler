#ifndef SINTATIC_ANALYSER_H_INCLUDED
#define SINTATIC_ANALYSER_H_INCLUDED

#include "token.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct SintaticErrorException{
    int line_;
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

	std::vector< std::vector< Token > > terminals_;

public:

	SintaticAnalyser(const std::string& config_file);

	void analyse(const std::vector< Token >& token_input);

	void expandNonTerminal(int non_terminal_id);

	int checkMatchRule(int non_terminal_id);

	void nextToken();

};

#endif // LEXICAL_ANALYSER_H_INCLUDED
