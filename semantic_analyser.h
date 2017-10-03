#ifndef SEMANTIC_ANALYSER_H_INCLUDED
#define SEMANTIC_ANALYSER_H_INCLUDED

#include "sintatic_analyser.h"

enum{ NO_FLAG, VAR_DECL, PROGRAM, PROCEDURE, SEARCH_NUMBERS };
enum VarType{ MISSING_TYPE, INTEGER, REAL, BOOLEAN, PROGRAM_TYPE, PROCEDURE_TYPE };

struct SuperToken{
    Token token_;
    VarType var_type_;

    bool operator==(const SuperToken& x) const;
};

class SemanticAnalyser{

    uint symbol_table_flag = NO_FLAG;
    uint type_stack_flag = NO_FLAG;
    const SintaticTree* aux_node = nullptr;  // verificar essa solucao

    uint scope_count;
    std::vector<SuperToken> aux_symbol_table;
    std::vector<SuperToken> symbol_table;
    std::vector<VarType> type_stack;

public:
    void analyse(const SintaticTree& root);
    void search(const SintaticTree& root);
    bool findDeclaration(const SuperToken& st);
    bool findOnScope(const SuperToken& st);
    VarType getIdentifierType(const std::string& id);
    void printSymbolTable() const;
    void printTypeStack() const;
};

#endif