#include <iostream>
#include "lexical_analyser.h"
#include "sintatic_analyser.h"

int main(int argc, char* argv[])
{
    LexicalAnalyser lex( std::string{argv[1]} );
	SintaticAnalyser sint( std::string{argv[2]} );

    lex.analyse( std::string{argv[3]} );
	const std::vector< Token > tokens = lex.getTokenList();
	sint.analyse(tokens);
	sint.printErrors();

    return 0;
}
