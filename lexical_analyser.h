#ifndef LEXICAL_ANALYSER_H_INCLUDED
#define LEXICAL_ANALYSER_H_INCLUDED

#define NEW_LINE '\n'

#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

#include "automato.h"
#include "token.h"

struct LexicalErrorException{
    uint line_;
    std::string description_;

    LexicalErrorException(uint line, const std::string& description) :
        line_(line),
        description_(description)
    {}
};

class LexicalAnalyser{

private:

    std::string input_;
    uint cursor_pos_;
    uint line_pos_;

    Automato *automato_;

    std::vector<Symbol> recent_buffer_;
    std::vector<Symbol> accepted_buffer_;
    std::unordered_set<Symbol> separator_;
    std::unordered_set<TokenType> genericTokenType_;
    std::unordered_map< std::string, TokenType > special_;
    std::unordered_set<TokenType> ignoredTokenType_;

    TokenType currentTokenType_;
    uint currentTokenLine_;
    uint currentErrorLine_;

    std::vector<std::string> token_type_strings_;
    std::vector<std::string> error_type_strings_;
    std::vector< Token > outputTokeList_;
    std::vector< std::string > error_info_;

public:

    LexicalAnalyser(const std::string& config_file);

    ~LexicalAnalyser();

    void analyse(const std::string& input_file);

    void readNextSymbol();

	const std::vector< Token >& getTokenList();

	bool isSeparator( Symbol symbol );

    bool isSpecial(const std::string& token);

    bool isGenericType(TokenType token_type);

    bool isIgnoredType(TokenType token_type);

	void printGeneratedTable();

    void printResults();

	bool success();
};

#endif // LEXICAL_ANALYSER_H_INCLUDED
