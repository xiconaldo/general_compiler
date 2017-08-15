#include <iostream>
#include "lexical_analyser.h"

int main(int argc, char* argv[])
{
    LexicalAnalyser lex( std::string{argv[1]} );
    lex.analyse( std::string{argv[2]} );
    lex.printResult();

    float x = 5.;

    return 0;
}
