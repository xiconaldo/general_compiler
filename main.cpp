#include <iostream>
#include "lexical_analyser.h"

using namespace std;

int main()
{
    LexicalAnalyser lex("config");
    lex.analyse("code");
    lex.printResult();
    return 0;
}
