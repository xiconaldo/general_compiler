#include "compiler.h"

Compiler::Compiler(const std::string& automato_file, const std::string& grammar_file){
	lexical = new LexicalAnalyser( automato_file );
	sintatic = new SintaticAnalyser( grammar_file );
	semantic = new SemanticAnalyser();
}

Compiler::~Compiler(){
	delete lexical;
	delete sintatic;
	delete semantic;
}

void Compiler::analyse(const std::string& code){
	lexical->analyse(code);
	const std::vector< Token > tokens = lexical->getTokenList();
	sintatic->analyse(tokens);
	const SintaticTree& tree =  sintatic->getSintaticTree();
	semantic->analyse(tree);
}

void Compiler::printResults(){
	if( lexical->success() && sintatic->success() /* && semantic->sucess() */){
		std::cout << "Successfully compilation!" << std::endl << std::endl;
		return;
	}

	std::cout << "Compilation failed!" << std::endl << std::endl;
	lexical->printResults();
	std::cout << std::endl;
	sintatic->printResults();
	//std::cout << std::endl;
	//semantic->printResults();
}

void Compiler::printLexicalTable(){
	lexical->printGeneratedTable();
}

void Compiler::printSintaticTree(){
	sintatic->printTree();
}
