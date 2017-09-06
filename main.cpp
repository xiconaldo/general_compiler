#include <iostream>
#include "compiler.h"

int main(int argc, char* argv[])
{

	Compiler comp{ std::string{argv[1]}, std::string{argv[2]} };
	comp.analyse(std::string{argv[3]});

	if(argc > 4 && std::string{argv[4]} == "lex")
		comp.printLexicalTable();
	else if(argc > 4 && std::string{argv[4]} == "sint")
		comp.printSintaticTree();
	else if(argc > 4 && std::string{argv[4]} == "all"){
		comp.printLexicalTable();
		comp.printSintaticTree();
	}

	comp.printResults();

    return 0;
}
