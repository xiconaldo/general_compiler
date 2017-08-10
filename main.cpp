#include <iostream>
#include "lexical_analyser.h"

int main()
{
    LexicalAnalyser lex("config");
    lex.analyse("code");
    lex.printResult();

    return 0;
}
