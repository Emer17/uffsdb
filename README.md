# uffsdb
A simple teaching DBMS

# Contributors
  André Luiz Maso	  		1221101059
  Eduardo Ogliari			1211100008
  Emerson Martins			1611101009
  João Ricardo Barp Neto	1311100026

# Informações sobre a implementação:
  As informações sobre os comandos SELECT e WHERE 
  podem ser encontradas no final do arquivo "Leia-me[Instrucoes].txt"
  
# Listagem com todas as novas funcionalidades
  Presente no arquivo "Novas Funcionalidades.txt"

# How to compile:
  Release:	make 
  Debug:	make debug

#Ps: How to compile on Fedora
	dnf install byacc flex
	dnf install flex-static
	uffsdb/Fonte/make

# How to execute
 `uffsdb/Fonte/./uffsdb`
 
# Compiler
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
