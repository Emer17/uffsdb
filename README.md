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
 
# Erros não corrigidos
 

*4) Prompt aparece várias vezes na tela ( Não corrigido )*

Ao copiar e colar as seguintes linhas no terminal:

	create table classe(
	cod integer primary key,
	descr varchar(30)
	);

O que irá aparecer na tela é:

	nautica=# create table classe(
	cod integer primary key,
	descr varchar(30)
	);nautica(# nautica(# nautica(# 
	CREATE TABLE

O erro provavelmente ocorre pois foram identificados vários '\n' após cada linha -- no caso 3 -- o que causou o prompt a aparecer várias vezes


*5) Prompt é exibido de acordo com a quantidade de ; ( Não corrigido )*

Depois de se conectar a um banco, digitar ;;;;;;;;;;;;;;;; resulta em:

	uffsdb=# uffsdb=# uffsdb=# uffsdb=# uffsdb=# uffsdb=# uffsdb=# uffsdb=# uffsdb=#

Imprime o nome do banco tantas vezes quanto foi digitado ;
