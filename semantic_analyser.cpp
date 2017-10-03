#include "semantic_analyser.h"

bool SuperToken::operator==(const SuperToken& x) const{
    return this->token_.token_ == x.token_.token_;
}

void SemanticAnalyser::analyse(const SintaticTree& root){
    search(root);
    printTypeStack();
}

void SemanticAnalyser::search(const SintaticTree& node){

    if (node.token_.type_ == -2 || node.token_.type_ == -11){
        symbol_table_flag = VAR_DECL;
    }
    else if(!aux_node && node.token_.type_ == -18){
        type_stack_flag = SEARCH_NUMBERS;
        aux_node = &node;
    }
    else if (node.token_.token_ == "program"){
        symbol_table_flag = PROGRAM;
        scope_count = 0;
    }
    else if (node.token_.token_ == "procedure"){
        symbol_table_flag = PROCEDURE;
        scope_count = 0;
    }
    else if (node.token_.token_ == "begin"){
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
    else if (node.token_.token_ == "integer" ){
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
    else if (node.token_.type_ == 2){
        if(symbol_table_flag == PROGRAM){
            symbol_table.push_back( {{0,"$",0}, MISSING_TYPE} );
            if(findOnScope({node.token_, PROGRAM_TYPE} ))
                std::cout << "Multiple declaration " << node.token_.token_ << std::endl;
            symbol_table.push_back( {node.token_, PROGRAM_TYPE} );
            symbol_table_flag = NO_FLAG;
        }
        else if (symbol_table_flag == PROCEDURE){
            if(findOnScope({node.token_, PROCEDURE_TYPE} ))
                std::cout << "Multiple declaration " << node.token_.token_ << std::endl;
            symbol_table.push_back( {node.token_, PROCEDURE_TYPE} );
            symbol_table.push_back( {{0,"$",0}, MISSING_TYPE} );
            symbol_table_flag = NO_FLAG;
        }
        else if (symbol_table_flag == VAR_DECL){
            if(findOnScope({node.token_, MISSING_TYPE} ))
                std::cout << "Multiple declaration " << node.token_.token_ << std::endl;
            aux_symbol_table.push_back( {node.token_, MISSING_TYPE} );
        }
        else{
            if(!findDeclaration({node.token_, MISSING_TYPE} ))
                std::cout << "Undeclared symbol " << node.token_.token_ << std::endl;
        }
    }

    if(type_stack_flag == SEARCH_NUMBERS){
        if (node.token_.type_ == 3)
            type_stack.push_back(INTEGER);
        else if (node.token_.type_ == 4)
            type_stack.push_back(REAL);
        else if( node.token_.token_ == "true" || node.token_.token_ == "false" )
            type_stack.push_back(BOOLEAN);
        else if(node.token_.type_ == 2)
            type_stack.push_back( getIdentifierType(node.token_.token_) );
    }

    for(uint i = 0; i < node.children_.size(); i++){
        search(node.children_[i]);
    }

    if (node.token_.type_ == -2 || node.token_.type_ == -11){
        symbol_table_flag = MISSING_TYPE;
    }
    if(aux_node == &node){
        type_stack_flag = NO_FLAG;
        aux_node = nullptr;
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

VarType SemanticAnalyser::getIdentifierType(const std::string& id){

    for(int i = symbol_table.size()-1; i >= 0; i--)
        if(id == symbol_table[i].token_.token_)
            return symbol_table[i].var_type_;

    return MISSING_TYPE;
}

void SemanticAnalyser::printSymbolTable() const{

    for(int i = symbol_table.size()-1; i >= 0; i--){
        std::cout << symbol_table[i].token_.token_ << "\t" << symbol_table[i].var_type_ << std::endl;
    }
    std::cout << std::endl;
}

void SemanticAnalyser::printTypeStack() const{
    
    for(int i = 0; i < type_stack.size(); i++){
        std::cout << type_stack[i] << " ";
    }
    std::cout << std::endl;
}