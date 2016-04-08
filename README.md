# uffsdb
A simple teaching DBMS

# how to compile
 uffsdb/Fonte/make

#Ps: how to compile on Fedora
	dnf install byacc flex
	dnf install flex-static
	uffsdb/Fonte/make

# how to execute
 `uffsdb/Fonte/./uffsdb`
 
# compiler
 uffsdb commands are interpreted using `yacc` and `lex`.
 In the `interface` folder type `make` to compile both.
 You can edit the following files: `parser.h`, `parser.c`, `lex.l`, and `yacc.y`.
