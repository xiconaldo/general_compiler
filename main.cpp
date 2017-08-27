#include <iostream>
#include "compiler.h"

int main(int argc, char* argv[])
{

	Compiler comp{ std::string{argv[1]}, std::string{argv[2]} };
	comp.analyse(std::string{argv[3]});
	comp.printResults();

    return 0;
}
