#include "compiler.h"

Compiler::Compiler(const std::string& automato_file, const std::string& grammar_file){
	lexical = new LexicalAnalyser( automato_file );
	sintatic = new SintaticAnalyser( grammar_file );
}

Compiler::~Compiler(){
	delete lexical;
	delete sintatic;
}

void Compiler::analyse(const std::string& code){
	lexical->analyse(code);
	const std::vector< Token > tokens = lexical->getTokenList();
	sintatic->analyse(tokens);

	sintatic->printTree();
}

void Compiler::printResults(){
	if( lexical->success() && sintatic->success() ){
		std::cout << "Successfully compilation!" << std::endl;
		return;
	}

	std::cout << "Compilation failed!" << std::endl << std::endl;
	lexical->printResults();
	std::cout << std::endl;
	sintatic->printResults();
}
