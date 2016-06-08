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

extern char* yytext[];
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

%token  INSERT      INTO        VALUES      SELECT      FROM
        CREATE      TABLE       INTEGER     VARCHAR     DOUBLE
        CHAR        PRIMARY     KEY         REFERENCES  DATABASE
        DROP        OBJECT      NUMBER      VALUE       QUIT
        LIST_TABLES LIST_TABLE  ALPHANUM    CONNECT     HELP
        LIST_DBASES CLEAR       CONTR		RUN_SCRIPT	END_OF_FILE
		EDITOR;

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
	//printf( "FIM\n" );
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
select:  SELECT select_all FROM table_select semicolon {setMode(OP_SELECT);return 0;}
		| SELECT select_multiple FROM table_select semicolon {setMode(OP_SELECT); return 0;};

column_select: OBJECT { strcpy( GLOBAL_DATA.selColumn[GLOBAL_DATA.N], *yytext );
++GLOBAL_DATA.N; };

select_all: '*' { strcpy( GLOBAL_DATA.selColumn[GLOBAL_DATA.N], *yytext ); 
++GLOBAL_DATA.N;};

select_multiple: column_select | column_select ',' select_multiple;

table_select: OBJECT { setObjName(yytext); };

/* END */
%%
