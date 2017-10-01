#include "semantic_analyser.h"

bool SuperToken::operator==(const SuperToken& x) const{
    return this->token_.token_ == x.token_.token_;
}

void SemanticAnalyser::analyse(const SintaticTree& root){

    search(root);

    for(int i = symbol_table.size()-1; i >= 0; i--){
        std::cout << symbol_table[i].token_.token_ << "\t" << symbol_table[i].var_type_ << std::endl;
    }

}

void SemanticAnalyser::search(const SintaticTree& node){

    if (node.token_.type_ == -2 || node.token_.type_ == -11){
        flag = VAR_DECL;
    }
    if (node.token_.token_ == "program"){
        flag = PROGRAM;
        scope_count = 0;
    }
    else if (node.token_.token_ == "procedure"){
        flag = PROCEDURE;
        scope_count = 0;
    }

    if (node.token_.token_ == "begin"){
        scope_count++;
    }
    else if (node.token_.token_ == "end"){
        scope_count--;
        if(scope_count == 0){
            while(symbol_table.back().token_.token_ != "$")
                symbol_table.pop_back();
            symbol_table.pop_back();
        }
    }

    if (node.token_.type_ == 2){
        if(flag == PROGRAM){
            symbol_table.push_back( {{0,"$",0}, MISSING_TYPE} );
            if(findOnScope({node.token_, PROGRAM_TYPE} ))
                std::cout << "Multiple declaration " << node.token_.token_ << std::endl;
            symbol_table.push_back( {node.token_, PROGRAM_TYPE} );
            flag = NO_FLAG;
        }
        else if (flag == PROCEDURE){
            if(findOnScope({node.token_, PROCEDURE_TYPE} ))
                std::cout << "Multiple declaration " << node.token_.token_ << std::endl;
            symbol_table.push_back( {node.token_, PROCEDURE_TYPE} );
            symbol_table.push_back( {{0,"$",0}, MISSING_TYPE} );
            flag = NO_FLAG;
        }
        else if (flag == VAR_DECL){
            if(findOnScope({node.token_, MISSING_TYPE} ))
                std::cout << "Multiple declaration " << node.token_.token_ << std::endl;
            aux_symbol_table.push_back( {node.token_, MISSING_TYPE} );
        }
        else{
            if(!findDeclaration({node.token_, MISSING_TYPE} ))
                std::cout << "Undeclared symbol " << node.token_.token_ << std::endl;
        }
    }

    if (node.token_.token_ == "integer" ){
        for(uint i = 0; i < aux_symbol_table.size(); i++){
            aux_symbol_table[i].var_type_ = INTEGER;  
            symbol_table.push_back(aux_symbol_table[i]);
        }
        aux_symbol_table.clear();
    }
    else if (node.token_.token_ == "real" ){
        for(uint i = 0; i < aux_symbol_table.size(); i++){
            aux_symbol_table[i].var_type_ = REAL;   
            symbol_table.push_back(aux_symbol_table[i]);
        }
        aux_symbol_table.clear();
    }
    else if (node.token_.token_ == "boolean" ){
        for(uint i = 0; i < aux_symbol_table.size(); i++){
            aux_symbol_table[i].var_type_ = BOOLEAN;   
            symbol_table.push_back(aux_symbol_table[i]);
        }
        aux_symbol_table.clear();
    }

    for(uint i = 0; i < node.children_.size(); i++){
        search(node.children_[i]);
    }

    if (node.token_.type_ == -2 || node.token_.type_ == -11){
        flag = MISSING_TYPE;
    }

}

bool SemanticAnalyser::findDeclaration(const SuperToken& st){

    for(int i = symbol_table.size()-1; i >= 0; i--)
        if(st == symbol_table[i])
            return true;

    return false;
}

bool SemanticAnalyser::findOnScope(const SuperToken& st){
    
    for(int i = aux_symbol_table.size()-1; i >= 0; i--)
        if(st == aux_symbol_table[i])
            return true;
        
    for(int i = symbol_table.size()-1; i >= 0; i--)
        if(st == symbol_table[i])
            return true;
        else if(symbol_table[i].token_.token_ == "$")
            return false;

    return false;
}