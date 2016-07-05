%{
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#ifndef FMACROS
   #include "../macros.h"
#endif
#ifndef FTYPES
   #include "../types.h"
#endif
#ifndef FMISC
   #include "../misc.h"
#endif
#ifndef FDICTIONARY
   #include "../dictionary.h"
#endif
#ifndef FSQLCOMMANDS
   #include "../sqlcommands.h"
#endif
#ifndef FDATABASE
   #include "../database.h"
#endif
#ifndef FPARSER
   #include "parser.h"
#endif

#ifdef UFFS_DEBUG
	#ifndef DEBUG_PRINT
		#define DEBUG_PRINT(x) puts( "-----------------DEBUG--------------------" ); printf x; puts( "\n------------------------------------------\n" );
	#endif
	#ifndef ERROR_PRINT
		#define ERROR_PRINT(x) puts( "#################ERROR####################" ); printf x; printf( "\nArquivo: %s\nLinha: %d\n", __FILE__, __LINE__ ); puts( "##########################################\n" );
	#endif
#endif

extern char* yytext[];
extern int yyleng;
extern FILE * yyin;
extern FILE* outFile_p;

int yywrap() {
    return 1;
}

%}

%union {
    int intval;
    double floatval;
    int subtok;
    char *strval;
}

%%

%token  INSERT      	INTO        	VALUES      SELECT      FROM
        CREATE      	TABLE       	INTEGER     VARCHAR     DOUBLE
        CHAR        	PRIMARY     	KEY         REFERENCES  DATABASE
        DROP        	OBJECT      	NUMBER      VALUE       QUIT
        LIST_TABLES 	LIST_TABLE  	ALPHANUM    CONNECT     HELP
        LIST_DBASES 	CLEAR       	CONTR		RUN_SCRIPT	END_OF_FILE
		EDITOR			WHERE			EQUAL		LESS		GREATER
		LESS_EQUAL		GREATER_EQUAL	AND			OR			NOT_EQUAL;		

start: insert | select | create_table | create_database | drop_table | drop_database
     | table_attr | list_tables | connection | exit_program | semicolon {GLOBAL_PARSER.consoleFlag = 1; return 0;}
     | parentesis_open | parentesis_close| help | list_databases | clear | contributors
     | qualquer_coisa | run_script | end_of_file | editor |/*nothing*/;

/*--------------------------------------------------*/
/**************** GENERAL FUNCTIONS *****************/
/*--------------------------------------------------*/

/* CONNECTION */
connection: CONNECT OBJECT {connect(*yytext); GLOBAL_PARSER.consoleFlag = 1; return 0;};

qualquer_coisa: OBJECT {GLOBAL_PARSER.consoleFlag = 1; GLOBAL_PARSER.error = 1; return 0;};

/* EXIT */
exit_program: QUIT {exit(0);};

editor: EDITOR {
	// Aqui vai a parte de abrir um editor de texto
	/*
	const char * editorNames[] = {
		"vi", "vim", "gedit", "geany" 
	};	
	char editorPath[4][256] = { {0} }; //Mudar para algo dinamico mais tarde
		
	int i = 0;
	while( i < 4 ) {
		char cmd [32] = "which ";
		strcat( cmd, editorNames[i] );
		puts( cmd );
		FILE * pathPointer = popen( cmd, "r" );		
		while( fgets( editorPath[i], 256, pathPointer ) != NULL ) {
			printf( "Caminho e: %s\n", editorPath[i] );
		}
		cmd[6] = '\0'; // tamanho da string  'which '
		pclose(pathPointer);
		pathPointer = NULL;
		i++;
	}
	
	i = 0; 
	int j = 0;
	int opcao = 1;
	char editoresDisponiveis[4][256] = { {0} };
	
	if( editorPath[0][0] != '\0' ) {
		puts( "Os seguintes editores foram detectados:" );
		while( i < 4 ) {
			if( editorPath[i][0] != '\0' ) {
				printf( "%d. %s\n", opcao, editorNames[i] );
				strcpy( editoresDisponiveis[j], editorPath[i] );
				j++;
				opcao++;
			}	
			i++;		
		}
	} else {
		return 0;
	}
	
	int o = 0;
	scanf( "%d", &o );
	
	
	
	i = 0;
	while( editoresDisponiveis[i][0] != '\0' && i < 4 ) {
		printf( "Nome: %s\n", editoresDisponiveis[i] );
		i++;
	}
	*/
	return 0;
};

run_script: RUN_SCRIPT {
	FILE * scriptData = loadScript( *yytext );
	if( scriptData == NULL ) {
		return 0;
	}
	yyin = scriptData;	
	GLOBAL_PARSER.consoleFlag = 1;
	GLOBAL_PARSER.readingFile = 1;
	return 0; 	
};

end_of_file: END_OF_FILE {	
	//GLOBAL_PARSER.error = 1;
	GLOBAL_PARSER.endOfFile = 1;
	GLOBAL_PARSER.readingFile = 0;
	GLOBAL_PARSER.consoleFlag = 1;
	return 0;
};

clear: CLEAR {clear(); GLOBAL_PARSER.consoleFlag = 1; return 0;};

parentesis_open: '(' {GLOBAL_PARSER.parentesis++;};

parentesis_close: ')' {GLOBAL_PARSER.parentesis--;};

/* TABLE ATTRIBUTES */
table_attr: LIST_TABLE OBJECT {
    if(connected.conn_active) {
        printTable(yylval.strval);
        GLOBAL_PARSER.consoleFlag = 1;
    } else
        notConnected();
    return 0;
};

/* LIST TABLES */
list_tables: LIST_TABLES {
    if(connected.conn_active) {
        printTable(NULL);
        GLOBAL_PARSER.consoleFlag = 1;
    } else
        notConnected();
    return 0;
};

/* LIST DATABASES */
list_databases: LIST_DBASES {
    showDB();
    GLOBAL_PARSER.consoleFlag = 1;
    return 0;
}

/* HELP */
help: HELP {help(); GLOBAL_PARSER.consoleFlag = 1; return 0;}

/* CONTRIBUTORS */
contributors: CONTR {contr(); GLOBAL_PARSER.consoleFlag = 1; return 0;}

/*--------------------------------------------------*/
/****************** SQL STATEMENTS ******************/
/*--------------------------------------------------*/

/* INSERT */
insert: INSERT INTO {setMode(OP_INSERT);} table opt_column_list VALUES parentesis_open value_list parentesis_close semicolon {
    if (GLOBAL_PARSER.col_count == GLOBAL_PARSER.val_count || GLOBAL_DATA.columnName == NULL)
        GLOBAL_DATA.N = GLOBAL_PARSER.val_count;
    else {
        printf("ERROR: The column counter doesn't match the value counter.\n");
        GLOBAL_PARSER.error = 1;
    }
    return 0;
};

semicolon: ';';

table: OBJECT {setObjName(yytext);};

opt_column_list: /*optional*/ | parentesis_open column_list parentesis_close;

column_list: column | column ',' column_list;

column: OBJECT {setColumnInsert(yytext);};

value_list: value | value ',' value_list;

value: VALUE {setValueInsert(yylval.strval, 'D');}
     | NUMBER {setValueInsert(yylval.strval, 'I');}
     | ALPHANUM {setValueInsert(yylval.strval, 'S');};

/* CREATE TABLE */
create_table: CREATE TABLE {setMode(OP_CREATE_TABLE);} table parentesis_open table_column_attr parentesis_close semicolon {
    GLOBAL_DATA.N = GLOBAL_PARSER.col_count;	
    return 0;
};

table_column_attr: column_create type attribute | column_create type attribute ',' table_column_attr;

type: INTEGER {setColumnTypeCreate('I');}
    | VARCHAR {setColumnTypeCreate('S');} parentesis_open NUMBER parentesis_close {setColumnSizeCreate(yylval.strval);}
    | DOUBLE {setColumnTypeCreate('D');};
    | CHAR {setColumnTypeCreate('C');};

column_create: OBJECT {setColumnCreate(yytext);};

attribute: /*optional*/
         | PRIMARY KEY {setColumnPKCreate();}
         | REFERENCES table_fk '(' column_fk ')';

table_fk: OBJECT {setColumnFKTableCreate(yytext);};

column_fk: OBJECT {setColumnFKColumnCreate(yytext);};

/* DROP TABLE */
drop_table: DROP TABLE {setMode(OP_DROP_TABLE);} OBJECT {setObjName(yytext);} semicolon  {return 0;};

/* CREATE DATABASE */
create_database: CREATE DATABASE {setMode(OP_CREATE_DATABASE);} OBJECT {setObjName(yytext);} semicolon {return 0;};

/* DROP DATABASE */
drop_database: DROP DATABASE {setMode(OP_DROP_DATABASE);} OBJECT {setObjName(yytext);} semicolon {return 0;};

/* SELECT */
select:  SELECT {setMode(OP_SELECT);} select_all FROM table_select opt_where semicolon {return 0;}
		| SELECT {setMode(OP_SELECT);} select_multiple FROM table_select opt_where semicolon {return 0;};

select_multiple: column_select | column_select ',' select_multiple;

column_select: 
		OBJECT 	{ strcpy( GLOBAL_SELECT.selColumn[GLOBAL_SELECT.qtdColunas], *yytext );
					++GLOBAL_SELECT.qtdColunas; 
				};

select_all: '*' { 
	if( GLOBAL_SELECT.qtdColunas < QTD_COLUNAS_PROJ ) {
		strcpy( GLOBAL_SELECT.selColumn[GLOBAL_SELECT.qtdColunas], *yytext ); 
		++GLOBAL_DATA.N;		
	} else {
		printf( "Quantidade maxima de colunas permitidas na operacao SELECT excedida.\n" );
		#ifdef UFFS_DEBUG			
			ERROR_PRINT(( "GLOBAL_SELECT.qtdColunas excedeu o valor de QTD_COLUNAS_PROJ" ));			
		#endif
	}
};

opt_where: 	| WHERE {GLOBAL_SELECT.where=1;} exp_where;

exp_where:	lvalue copy_lvalue op rvalue copy_rvalue exp_where_opt;	

copy_lvalue:
{	
	char temp[TAMANHO_NOME_CAMPO] = { '\0' };
	int i, index;
	
	if( yyleng+1 > TAMANHO_NOME_CAMPO ) {	
		if( GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].ltipo == 'S' ) {
			index = 1;
			for( i = 0; i < TAMANHO_NOME_CAMPO-1; i++, index++ ) {
				temp[i] = yytext[0][index];
			}
			temp[TAMANHO_NOME_CAMPO-1] = '\0';
			strcpy( GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].lvalue, temp);
		} else {			
			for( i = 0; i < TAMANHO_NOME_CAMPO-1; i++ ) {			
				temp[i] = yytext[0][i];
			}
			temp[TAMANHO_NOME_CAMPO-1] = '\0';
			strcpy( GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].lvalue, temp );
		}
	} else {
		if( GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].ltipo == 'S' ) {
			index = 1;
			for( i = 0; index < yyleng-1; i++, index++ ) {
				temp[i] = yytext[0][index];
			}
			temp[yyleng-1] = '\0';
			strcpy( GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].lvalue, temp);
		} else {		
			strcpy( GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].lvalue, *yytext );			
		}
	}				
	GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].lvalue[TAMANHO_NOME_CAMPO-1] = '\0';	
}

copy_rvalue:
{	
	char temp[TAMANHO_NOME_CAMPO] = { '\0' };
	int i, index;
	
	if( yyleng+1 > TAMANHO_NOME_CAMPO ) {	
		if( GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].rtipo == 'S' ) {
			index = 1;
			for( i = 0; i < TAMANHO_NOME_CAMPO-1; i++, index++ ) {
				temp[i] = yytext[0][index];
			}
			temp[TAMANHO_NOME_CAMPO-1] = '\0';
			strcpy( GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].rvalue, temp);
		} else {			
			for( i = 0; i < TAMANHO_NOME_CAMPO-1; i++ ) {			
				temp[i] = yytext[0][i];
			}
			temp[TAMANHO_NOME_CAMPO-1] = '\0';
			strcpy( GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].rvalue, temp );
		}
	} else {
		if( GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].rtipo == 'S' ) {
			index = 1;
			for( i = 0; index < yyleng-1; i++, index++ ) {
				temp[i] = yytext[0][index];
			}
			temp[yyleng-1] = '\0';
			strcpy( GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].rvalue, temp);
		} else {		
			strcpy( GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].rvalue, *yytext );			
		}
	}				
	GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].rvalue[TAMANHO_NOME_CAMPO-1] = '\0';
	GLOBAL_SELECT.qtdExp++;
}
			
lvalue:    OBJECT 	{GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].ltipo = 'O';}
		|  ALPHANUM {GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].ltipo = 'S';}
		|  NUMBER	{GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].ltipo = 'I';}
		|  VALUE	{GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].ltipo = 'D';};

rvalue:    OBJECT 	{GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].rtipo = 'O';}
		|  ALPHANUM {GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].rtipo = 'S';}
		|  NUMBER	{GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].rtipo = 'I';}
		|  VALUE	{GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].rtipo = 'D';};

exp_where_opt: | AND { GLOBAL_SELECT.op_bool[GLOBAL_SELECT.qtdExp-1] = OP_AND; } exp_where
			   | OR { GLOBAL_SELECT.op_bool[GLOBAL_SELECT.qtdExp-1] = OP_OR; } exp_where;
	

op:   EQUAL 		{ GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].op = COMPARISON_EQUAL; }
	| LESS_EQUAL 	{ GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].op = COMPARISON_LESS_EQUAL; } 
	| NOT_EQUAL 	{ GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].op = COMPARISON_NOT_EQUAL; } 
	| GREATER_EQUAL { GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].op = COMPARISON_GREATER_EQUAL; }
	| GREATER 		{ GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].op = COMPARISON_GREATER; } 
	| LESS 			{ GLOBAL_SELECT.expressoes[GLOBAL_SELECT.qtdExp].op = COMPARISON_LESS; };

table_select: OBJECT { setObjName(yytext); };

/* END */
%%
