#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#ifndef FMACROS
   #include "../macros.h"
#endif
#ifndef FTYPES
   #include "../types.h"
#endif
#ifndef FMISC
   #include "../misc.h"
#endif
#ifndef FDATABASE
   #include "../database.h"
#endif
#ifndef FSQLCOMMANDS
   #include "../sqlcommands.h"
#endif
#ifndef FPARSER
   #include "parser.h"
#endif

/* Estrutura global que guarda as informações obtidas pelo yacc
 * na identificação dos tokens
 */
rc_insert GLOBAL_DATA;

/* Estrutura auxiliar do reconhecedor.
 */
rc_parser GLOBAL_PARSER;

void connect( const char *nome ) {
    int r;
    r = connectDB( nome );
	if ( r == SUCCESS ) {
        connected.db_name = malloc( sizeof(char) * ( strlen(nome) + 1 ) );

        strcpylower( connected.db_name, nome );

        connected.conn_active = 1;
        printf( "You are now connected to database \"%s\" as user \"uffsdb\".\n", nome );
    } else {
    	printf( "ERROR: Failed to establish connection with database named \"%s\". (Error code: %d)\n", nome, r );
    }
}

void invalidCommand( const char *command ) {
    printf( "ERROR: Invalid command '%s'. Type \"help\" for help.\n", command );
}

void notConnected() {
    printf("ERROR: you are not connected to any database.\n");
}

void setObjName( char **nome ) {
    if ( GLOBAL_PARSER.mode != OP_INVALID ) {
        GLOBAL_DATA.objName = malloc( sizeof(char) * ( strlen(*nome) + 1 ) );
        strcpylower( GLOBAL_DATA.objName, *nome );
        GLOBAL_DATA.objName[ strlen(*nome) ] = '\0';
        GLOBAL_PARSER.step++;
    }
}

void setColumnInsert( char **nome ) {
    GLOBAL_DATA.columnName = realloc(GLOBAL_DATA.columnName, (GLOBAL_PARSER.col_count+1)*sizeof(char *));

    GLOBAL_DATA.columnName[GLOBAL_PARSER.col_count] = malloc(sizeof(char)*(strlen(*nome)+1));
    strcpylower(GLOBAL_DATA.columnName[GLOBAL_PARSER.col_count], *nome);
    GLOBAL_DATA.columnName[GLOBAL_PARSER.col_count][strlen(*nome)] = '\0';

    GLOBAL_PARSER.col_count++;
}

void setValueInsert( char *nome, char type ) {
    int i = 0;
    GLOBAL_DATA.values  = realloc( GLOBAL_DATA.values, ( GLOBAL_PARSER.val_count+1 ) * sizeof(char *) );
    GLOBAL_DATA.type    = realloc( GLOBAL_DATA.type, ( GLOBAL_PARSER.val_count+1 ) * sizeof(char) );

    // Adiciona o valor no vetor de strings
    GLOBAL_DATA.values[GLOBAL_PARSER.val_count] = malloc( sizeof(char) * ( strlen(nome)+1 ) );
    if( type == 'I' || type == 'D' ) {
        strcpy( GLOBAL_DATA.values[GLOBAL_PARSER.val_count], nome );
        GLOBAL_DATA.values[GLOBAL_PARSER.val_count][strlen(nome)] = '\0';
    } else if ( type == 'S' ) {
        for( i = 1; i < strlen(nome)-1; i++ ) {
            GLOBAL_DATA.values[GLOBAL_PARSER.val_count][i-1] = nome[i];
        }
        GLOBAL_DATA.values[GLOBAL_PARSER.val_count][strlen(nome)-2] = '\0';
    }

    GLOBAL_DATA.type[GLOBAL_PARSER.val_count] = type;

    GLOBAL_PARSER.val_count++;
}

void setColumnCreate( char **nome ) {
    GLOBAL_DATA.columnName  = realloc( GLOBAL_DATA.columnName, (GLOBAL_PARSER.col_count+1) * sizeof(char *) );
    GLOBAL_DATA.attribute   = realloc( GLOBAL_DATA.attribute, (GLOBAL_PARSER.col_count+1) * sizeof(int) );
    GLOBAL_DATA.fkColumn    = realloc( GLOBAL_DATA.fkColumn, (GLOBAL_PARSER.col_count+1) * sizeof(char *) );
    GLOBAL_DATA.fkTable     = realloc( GLOBAL_DATA.fkTable, (GLOBAL_PARSER.col_count+1) * sizeof(char *) );
    GLOBAL_DATA.values      = realloc( GLOBAL_DATA.values, (GLOBAL_PARSER.col_count+1) * sizeof(char *) );
    GLOBAL_DATA.type        = realloc( GLOBAL_DATA.type, (GLOBAL_PARSER.col_count+1) * sizeof(char *) );

    GLOBAL_DATA.values[GLOBAL_PARSER.col_count] = malloc( sizeof(char) );
    GLOBAL_DATA.fkTable[GLOBAL_PARSER.col_count] = malloc( sizeof(char) );
    GLOBAL_DATA.fkColumn[GLOBAL_PARSER.col_count] = malloc( sizeof(char) );
    GLOBAL_DATA.columnName[GLOBAL_PARSER.col_count] = malloc( sizeof(char) * (strlen(*nome)+1) );

    strcpylower( GLOBAL_DATA.columnName[GLOBAL_PARSER.col_count], *nome );

    GLOBAL_DATA.columnName[GLOBAL_PARSER.col_count][strlen(*nome)] = '\0';
    GLOBAL_DATA.type[GLOBAL_PARSER.col_count] = 0;
    GLOBAL_DATA.attribute[GLOBAL_PARSER.col_count] = NPK;

    GLOBAL_PARSER.col_count++;
    GLOBAL_PARSER.step = 2;
}

void setColumnTypeCreate( char type ) {
    GLOBAL_DATA.type[GLOBAL_PARSER.col_count-1] = type;
    GLOBAL_PARSER.step++;
}

void setColumnSizeCreate( char *size ) {
    GLOBAL_DATA.values[GLOBAL_PARSER.col_count-1] = realloc(GLOBAL_DATA.values[GLOBAL_PARSER.col_count-1], sizeof(char)*(strlen(size)+1));
    strcpy(GLOBAL_DATA.values[GLOBAL_PARSER.col_count-1], size);
    GLOBAL_DATA.values[GLOBAL_PARSER.col_count-1][strlen(size)-1] = '\0';
}

void setColumnPKCreate() {
    GLOBAL_DATA.attribute[GLOBAL_PARSER.col_count-1] = PK;
}

void setColumnFKTableCreate( char **nome ) {
    GLOBAL_DATA.fkTable[GLOBAL_PARSER.col_count-1] = realloc(GLOBAL_DATA.fkTable[GLOBAL_PARSER.col_count-1], sizeof(char)*(strlen(*nome)+1));
    strcpylower(GLOBAL_DATA.fkTable[GLOBAL_PARSER.col_count-1], *nome);
    GLOBAL_DATA.fkTable[GLOBAL_PARSER.col_count-1][strlen(*nome)] = '\0';
    GLOBAL_DATA.attribute[GLOBAL_PARSER.col_count-1] = FK;
    GLOBAL_PARSER.step++;
}

void setColumnFKColumnCreate( char **nome ) {
    GLOBAL_DATA.fkColumn[GLOBAL_PARSER.col_count-1] = realloc(GLOBAL_DATA.fkColumn[GLOBAL_PARSER.col_count-1], sizeof(char)*(strlen(*nome)+1));
    strcpylower( GLOBAL_DATA.fkColumn[GLOBAL_PARSER.col_count-1], *nome );
    GLOBAL_DATA.fkColumn[GLOBAL_PARSER.col_count-1][strlen(*nome)] = '\0';
    GLOBAL_PARSER.step++;
}


void clearGlobalStructs() {
    int i;

    if( GLOBAL_DATA.objName ) {
        free(GLOBAL_DATA.objName);
        GLOBAL_DATA.objName = NULL;
    }

    for( i = 0; i < GLOBAL_DATA.N; i++ ) {
        if( GLOBAL_DATA.columnName ) {
            free( GLOBAL_DATA.columnName[i] );
		}
        if( GLOBAL_DATA.values ){
            free( GLOBAL_DATA.values[i] );
		}
        if( GLOBAL_DATA.fkTable ) {
            free( GLOBAL_DATA.fkTable[i] );
		}
        if( GLOBAL_DATA.fkColumn ) {
            free( GLOBAL_DATA.fkColumn[i] );
		}
    }

    free( GLOBAL_DATA.columnName );
    GLOBAL_DATA.columnName = NULL;

    free( GLOBAL_DATA.values );
    GLOBAL_DATA.values = NULL;

    free( GLOBAL_DATA.fkTable );
    GLOBAL_DATA.fkTable = NULL;

    free( GLOBAL_DATA.fkColumn );
    GLOBAL_DATA.fkColumn = NULL;

    free( GLOBAL_DATA.type );
    GLOBAL_DATA.type = (char *)malloc( sizeof(char) );

    free( GLOBAL_DATA.attribute );
    GLOBAL_DATA.attribute = (int *)malloc( sizeof(int) );

	if( GLOBAL_PARSER.endOfFile ) {
		yylex_destroy();
		GLOBAL_PARSER.endOfFile = 0;
		GLOBAL_PARSER.readingFile = 0;
	} 
    

    GLOBAL_DATA.N = 0;

    GLOBAL_PARSER.mode              = OP_INVALID;
    GLOBAL_PARSER.parentesis        = 0;
    GLOBAL_PARSER.error           	= 0;
    GLOBAL_PARSER.col_count         = 0;
    GLOBAL_PARSER.val_count         = 0;
    GLOBAL_PARSER.step              = 0;
	
}

void setMode( const char mode ) {
    GLOBAL_PARSER.mode = mode;
    GLOBAL_PARSER.step++;
}

void interface( int argc, char **argv ) {
    pthread_t pth;
    pthread_create( &pth, NULL, (void*)clearGlobalStructs, NULL );
    pthread_join( pth, NULL );
	
	struct db_options options;
	options.db_name = NULL;
	
	int i = 1;	
	while( i < argc ) {
		if( argv[i][0] == '-' ) {
			if( argv[i][1] == '\0' ) {
				printf( "ERRO: Nenhuma opcao especificada apos -\n" );
				return;
			} else if( argv[i][2] != '\0' ) {
				printf( "ERRO: Opcao possui mais de uma letra\n" );
				return;
			}
			
			char optionName = argv[i][1];
			i++;
			if( i >= argc ) {
				printf( "ERRO: A opcao %s nao recebeu nenhum argumento\n", argv[i-1] );
				return;
			} else if( argv[i][0] == '-' ) {
				printf( "ERRO: Argumentos de opcao nao podem inciar com -\n" );
				return;
			}
			
			switch( optionName ) {
				case 'd':										
					if( options.db_name != NULL ) {
						printf( "ERRO: A opcao -d foi configurada multiplas vezes\n" );
						return;
					}
					char * name = argv[i];
					options.db_name = malloc( sizeof(char) * ( strlen(name) + 1 ) );
					options.db_name = name;
					break;
					
				default:
					// Em caso de a opcao for desconhecida	
					printf( "ERRO: Opcao \"%s\" desconhecida\n", argv[i-1] );
					return;					
			}
		} else {
			printf( "ERRO: Argumento desconhecido \"%s\"\n", argv[i] );
			return;
		}
		
		i++;
	}

	if( options.db_name == NULL ) {
		// Conecta automaticamente no banco padrão caso nenhum nome tenha sido passado
		connect( "uffsdb" ); 
	} else {
		connect( options.db_name );
	}    
	
	printf( "uffsdb (15.1).\nType \"help\" for help.\n\n" );

    while( 1 ) {
        if( !connected.conn_active ) {
            printf( ">" );
        } else {
            printf( "%s=# ", connected.db_name );
        }

        pthread_create( &pth, NULL, (void*)yyparse, &GLOBAL_PARSER );
        pthread_join( pth, NULL );

        if( !GLOBAL_PARSER.error ) {
            if ( GLOBAL_PARSER.mode != OP_INVALID ) {
                if( !connected.conn_active ) {
                    notConnected();
                } else {
                    switch( GLOBAL_PARSER.mode ) {
                        case OP_INSERT:
                            if ( GLOBAL_DATA.N > 0 ) {
                                insert( &GLOBAL_DATA );
                            } else {
                                printf( "WARNING: Nothing to be inserted. Command ignored.\n" );								
							}
                            break;
                        case OP_SELECT_ALL:
                            imprime( GLOBAL_DATA.objName );
                            break;
                        case OP_CREATE_TABLE:
                            createTable( &GLOBAL_DATA );
                            break;
                        case OP_CREATE_DATABASE:
                            createDB( GLOBAL_DATA.objName );
                            break;
                        case OP_DROP_TABLE:
                            excluirTabela( GLOBAL_DATA.objName );
                            break;
                        case OP_DROP_DATABASE:
                            dropDatabase( GLOBAL_DATA.objName );
                            break;
                        default: 
							break;
                    }
                }
            } else {
				// Só entra neste bloco "else" se a operação terminada por ;  
				// 		não for identificada?
			
			}
        } else {
            GLOBAL_PARSER.consoleFlag = 1;
            switch( GLOBAL_PARSER.mode ) {
                case OP_CREATE_DATABASE:
                case OP_DROP_DATABASE:
                case OP_CREATE_TABLE:
                case OP_DROP_TABLE:
                case OP_SELECT_ALL:
                case OP_INSERT:
                    if ( GLOBAL_PARSER.step == 1 ) {
                        GLOBAL_PARSER.consoleFlag = 0;
                        printf( "Expected object name.\n" );
                    }
					break;

                default: 
					break;
            }

            if( GLOBAL_PARSER.mode == OP_CREATE_TABLE ) {
                if( GLOBAL_PARSER.step == 2 ) {
                    printf( "Column not specified correctly.\n" );
                    GLOBAL_PARSER.consoleFlag = 0;
                }
            } else if ( GLOBAL_PARSER.mode == OP_INSERT ) {
                if ( GLOBAL_PARSER.step == 2 ) {
                    printf( "Expected token \"VALUES\" after object name.\n" );
                    GLOBAL_PARSER.consoleFlag = 0;
                }
            }
						
			printf( "ERROR: syntax error.\n" );							
            GLOBAL_PARSER.error = 0;
        }

		
        if ( GLOBAL_PARSER.mode != OP_INVALID || GLOBAL_PARSER.endOfFile ) {
            pthread_create( &pth, NULL, (void*)clearGlobalStructs, NULL );
            pthread_join( pth, NULL );
        }
		
		// getchar();
    }
    
}

FILE * loadScript( char * scriptPath ) {
	if( scriptPath == NULL ) {
		printf( "ERRO: Nenhum caminho para o script foi especificado!" );
		return NULL;
	}		
	scriptPath += 3;
	char * path = malloc( sizeof( char ) * strlen(scriptPath) - 2 );
	strcpy( path, scriptPath );	
	char * dot = strchr( path, '.' );	
	if( strcmp( dot, ".sql" ) != 0 ) {
		puts( "ERRO: Script nao possui extensao .sql" );
		return NULL;
	}	
	
	FILE * script = fopen( path, "r" );	
	if( script == NULL ) {
		printf( "ERRO: Nao foi possivel abrir o arquivo %s\n", path );
	}	
	return script;	
}

void yyerror(char *s, ...) {
    GLOBAL_PARSER.error = 1;
    /*extern yylineno;

    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
    */
}
