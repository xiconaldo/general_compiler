# General Compiler

A flexible compiler that allow lexical and sintatic configuration through 
automata and grammar files passed by user.

## Use

To build the project, use:

`$ make compiler`

To compile some code, type:

`$ ./compiler automato.conf grammar.conf code.pp [ symbol | tree | scope | type ]`

The first parameter is the automato configuration file. A functional example 
can be found on the project. The second one is the grammar configuration file.
A example is provided too. The next one is the code file to be compiled. Lastly, 
came the optional flags that allows print some information about the process.

symbol: Lexical symbols table
tree  : Sinatic tree
scope : Scope stack
type  : Type stack