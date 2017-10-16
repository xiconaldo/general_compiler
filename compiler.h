#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include "lexical_analyser.h"
#include "sintatic_analyser.h"
#include "semantic_analyser.h"

class Compiler{

private:

	LexicalAnalyser* lexical = nullptr;
	SintaticAnalyser* sintatic = nullptr;
	SemanticAnalyser* semantic = nullptr;

public:

	Compiler(const std::string& automato_file, const std::string& grammar_file);
	~Compiler();
	void analyse(const std::string& code);
	void printResults();
	void printLexicalTable();
	void printSintaticTree();
	void printScopeStack();
	void printTypeStack();
};

#endif
