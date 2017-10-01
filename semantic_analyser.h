#ifndef SEMANTIC_ANALYSER_H_INCLUDED
#define SEMANTIC_ANALYSER_H_INCLUDED

#include "sintatic_analyser.h"

enum{ NO_FLAG, VAR_DECL, PROGRAM, PROCEDURE };
enum VarType{ MISSING_TYPE, INTEGER, REAL, BOOLEAN, PROGRAM_TYPE, PROCEDURE_TYPE };

struct SuperToken{
    Token token_;
    VarType var_type_;

    bool operator==(const SuperToken& x) const;
};

class SemanticAnalyser{

    uint flag;
    uint scope_count;
    std::vector<SuperToken> aux_symbol_table;
    std::vector<SuperToken> symbol_table;

public:
    void analyse(const SintaticTree& root);
    void search(const SintaticTree& root);
    bool findDeclaration(const SuperToken& st);
    bool findOnScope(const SuperToken& st);
};

#endif