#include "semantic_analyser.h"

bool SuperToken::operator==(const SuperToken& x) const{
    return this->token_.token_ == x.token_.token_;
}

void SemanticAnalyser::analyse(const SintaticTree& root){

    try{
        search(root);
    }
    catch( SemanticErrorException err){
        std::ostringstream ss;
        ss << "At line " << err.line_ << ": " << err.description_;
        error_info_.push_back(ss.str());
    }

}

void SemanticAnalyser::search(const SintaticTree& node){

    if (node.token_.type_ == -2 || node.token_.type_ == -11){
        mode_flag = VAR_DECL;
    }
    else if(!aux_node && node.token_.type_ == -18){
        mode_flag = VAR_USE;
        aux_node = &node;
    }
    else if (node.token_.token_ == "program"){
        mode_flag = PROGRAM;
        scope_count = 0;
    }
    else if (node.token_.token_ == "procedure"){
        mode_flag = PROCEDURE;
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
        if(mode_flag == PROGRAM){
            
            if(findOnScope({node.token_, PROGRAM_TYPE} )){
                throw SemanticErrorException{ node.token_.line_, "Multiple declaration of symbol \"" + node.token_.token_ + "\""};
            }

            symbol_table.push_back( {{0,"$",0}, MISSING_TYPE} );
            symbol_table.push_back( {node.token_, PROGRAM_TYPE} );
            mode_flag = NO_FLAG;
        }
        else if (mode_flag == PROCEDURE){

            if(findOnScope({node.token_, PROCEDURE_TYPE} )){
                throw SemanticErrorException{ node.token_.line_, "Multiple declaration of symbol \"" + node.token_.token_ + "\""};
            }

            symbol_table.push_back( {node.token_, PROCEDURE_TYPE} );
            symbol_table.push_back( {{0,"$",0}, MISSING_TYPE} );
            mode_flag = NO_FLAG;
        }
        else if (mode_flag == VAR_DECL){

            if(findOnScope({node.token_, MISSING_TYPE} )){
                throw SemanticErrorException{ node.token_.line_, "Multiple declaration of symbol \"" + node.token_.token_ + "\""};
            }

            aux_symbol_table.push_back( {node.token_, MISSING_TYPE} );
        }
        else{
            if(!findDeclaration({node.token_, MISSING_TYPE} )){
                throw SemanticErrorException{ node.token_.line_, "Undeclared symbol \"" + node.token_.token_ + "\""};
            }
        }
    }

    if(mode_flag == VAR_USE){
        if (node.token_.type_ == 3)
            type_stack.push_back(INTEGER);
        else if (node.token_.type_ == 4)
            type_stack.push_back(REAL);
        else if( node.token_.token_ == "true" || node.token_.token_ == "false" )
            type_stack.push_back(BOOLEAN);
        else if(node.token_.type_ == 2){
            VarType id_type = getIdentifierType(node.token_.token_);
            if( id_type == PROGRAM_TYPE )
                throw SemanticErrorException{ node.token_.line_,  "Forbidden identifier use. \"" + node.token_.token_ + "\" names the program."};
            type_stack.push_back( getIdentifierType(node.token_.token_) );
        }
    }

    for(uint i = 0; i < node.children_.size(); i++){
        search(node.children_[i]);
    }

    std::string op = "";
    if(!node.children_.empty()) 
        op = node.children_[0].token_.token_;

    if( node.token_.type_ != -32 && op == "+" || op == "-" || op == "*" || op == "/"){
        VarType op1 = type_stack.back();
        type_stack.pop_back();
        VarType op2 = type_stack.back();
        type_stack.pop_back();

        if( (op1 == INTEGER || op1 == REAL) && (op2 == INTEGER || op2 == REAL) ){
            if( op1 == op2 ){
                type_stack.push_back(op1);
            }
            else{
                type_stack.push_back(REAL);
            }
        }
        else{
            type_stack.push_back(op2);
            type_stack.push_back(op1);
            throw SemanticErrorException{ node.children_[0].token_.line_, 
                "Incompatible operands type on " + op + " operator. Expecting INTEGER or REAL operands."};
        }
    }
    else if(op == ":="){

        VarType op1 = type_stack.back();
        type_stack.pop_back();
        VarType op2 = type_stack.back();
        type_stack.pop_back();

        if( op1 == op2 ){
            type_stack.push_back(op1);
        }
        else{
            type_stack.push_back(op2);
            type_stack.push_back(op1);
            throw SemanticErrorException{ node.children_[0].token_.line_, 
                "Incompatible operands type on " + op + " operator. Expecting same type operands on left and right."};
        }

    }
    else if(op == "or" || op == "and"){

        VarType op1 = type_stack.back();
        type_stack.pop_back();
        VarType op2 = type_stack.back();
        type_stack.pop_back();

        if( op1 == BOOLEAN && op2 == BOOLEAN ){
            type_stack.push_back(op1);
        }
        else{
            type_stack.push_back(op2);
            type_stack.push_back(op1);
            throw SemanticErrorException{ node.children_[0].token_.line_, 
                "Incompatible operands type on " + op + " operator. Expecting BOOLEAN operands."};
        }

    }
    else if(op == "not"){
        
        VarType op1 = type_stack.back();

        if( op1 != BOOLEAN ){
            throw SemanticErrorException{ node.children_[0].token_.line_, 
                "Incompatible operand type on " + op + " operator. Expecting BOOLEAN operand."};
        }

    }
    else if(op == ">" || op == ">=" || op == "<" || op == "<=" || op == "=" || op == "<>"){
        
        VarType op1 = type_stack.back();
        type_stack.pop_back();
        VarType op2 = type_stack.back();
        type_stack.pop_back();

        if( (op1 == INTEGER || op1 == REAL) && (op2 == INTEGER || op2 == REAL) ){
            type_stack.push_back(BOOLEAN);
        }
        else{
            type_stack.push_back(op2);
            type_stack.push_back(op1);
            throw SemanticErrorException{ node.children_[0].token_.line_, 
                "Incompatible operands type on " + op + " operator. Expecting NUMERICAL operands."};
        }
    }

    if (node.token_.type_ == -2 || node.token_.type_ == -11){
        mode_flag = NO_FLAG;
    }
    if(aux_node == &node){
        mode_flag = NO_FLAG;
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

    const std::string names[6] = { "MISSING_TYPE", "INTEGER", "REAL", "BOOLEAN", "PROGRAM", "PROCEDURE" };

    for(int i = symbol_table.size()-1; i >= 0; i--){
        std::cout << symbol_table[i].token_.token_ << "\t" << names[symbol_table[i].var_type_] << std::endl;
    }
    std::cout << std::endl;
}

void SemanticAnalyser::printTypeStack() const{

    const std::string names[6] = { "MISSING_TYPE", "INTEGER", "REAL", "BOOLEAN", "PROGRAM", "PROCEDURE" };
    
    for(int i = type_stack.size()-1; i >= 0; i--){
        std::cout << names[type_stack[i]] << std::endl;
    }
    std::cout << std::endl;
}

void SemanticAnalyser::printResults(){
	if( !error_info_.empty() ){
        std::cout << "Semantic error(s) detected:" << std::endl;
        for( std::string err : error_info_)
            std::cout << err << std::endl;
    }
    std::cout << std::endl;
}

bool SemanticAnalyser::success(){
	return error_info_.empty();
}