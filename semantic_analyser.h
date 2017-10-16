#ifndef SEMANTIC_ANALYSER_H_INCLUDED
#define SEMANTIC_ANALYSER_H_INCLUDED

#include "sintatic_analyser.h"

enum{ NO_FLAG, VAR_DECL, PROGRAM, PROCEDURE, VAR_USE };
enum VarType{ MISSING_TYPE, INTEGER, REAL, BOOLEAN, PROGRAM_TYPE, PROCEDURE_TYPE };

struct SuperToken{
    Token token_;
    VarType var_type_;

    bool operator==(const SuperToken& x) const;
};

struct SemanticErrorException{
    uint line_;
    std::string description_;

    SemanticErrorException(uint line, const std::string& description) :
        line_(line),
        description_(description)
    {}
};

class SemanticAnalyser{

    uint mode_flag = NO_FLAG;
    const SintaticTree* aux_node = nullptr;  // verificar essa solucao

    uint scope_count;
    std::vector<SuperToken> aux_symbol_table;
    std::vector<SuperToken> symbol_table;
    std::vector<VarType> type_stack;

    std::vector< std::string > error_info_;

public:
    void analyse(const SintaticTree& root);
    void search(const SintaticTree& root);
    bool findDeclaration(const SuperToken& st);
    bool findOnScope(const SuperToken& st);
    VarType getIdentifierType(const std::string& id);
    void printSymbolTable() const;
    void printTypeStack() const;
    void printResults();
    bool success();
};

#endif