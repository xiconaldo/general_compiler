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

struct Token{
    TokenType type_;
    std::string token_;
    uint line_;
};

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
    std::vector<uint> alphabet_;
    std::unordered_set<TokenType> genericTokenType_;
    std::unordered_map< std::string, TokenType > special_;
    std::string comment_begin_;
    std::string comment_end_;

    TokenType currentTokenType_;
    TokenType insideCommentTokenType_;
    TokenType commentTokenType_;
    uint currentTokenLine_;

    std::vector<std::string> token_type_strings_;
    std::vector< Token > outputTokeList_;
    std::vector< std::string > error_info_;

public:

    LexicalAnalyser(const std::string& config_file);

    void analyse(const std::string& input_file);

    void readNextSymbol();

    bool isOnLexicalAlphabet( Symbol symbol );

    bool isSeparator( Symbol symbol );

    void printResult();

    void printErrors();

    bool isSpecial(const std::string& token);

    bool isGenericType(TokenType token_type);
};

#endif // LEXICAL_ANALYSER_H_INCLUDED
