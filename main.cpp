#include <iostream>
#include "compiler.h"

int main(int argc, char* argv[])
{

	Compiler comp{ std::string{argv[1]}, std::string{argv[2]} };
	comp.analyse(std::string{argv[3]});
	
	if(argc > 4){
		std::cout << "\n";
		std::string arg4 = std::string{argv[4]};
	if(arg4 == "symbol")
		comp.printLexicalTable();
	else if(arg4 == "tree")
		comp.printSintaticTree();
	else if(arg4 == "scope")
		comp.printScopeStack();
	else if(arg4 == "type")
		comp.printTypeStack();
	}

	comp.printResults();

    return 0;
}
