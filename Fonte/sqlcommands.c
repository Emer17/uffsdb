#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#ifndef FMACROS // garante que macros.h não seja reincluída
   #include "macros.h"
#endif

#ifndef FTYPES // garante que types.h não seja reincluída
  #include "types.h"
#endif

#ifndef FMISC // garante que misc.h não seja reincluída
  #include "misc.h"
#endif

#ifndef FBUFFER // garante que buffer.h não seja reincluída
  #include "buffer.h"
#endif

#ifndef FDICTIONARY // the same
  #include "dictionary.h"
#endif

#ifndef FPARSER
	#include "interface/parser.h"
#endif

#ifdef UFFS_DEBUG
	#ifndef DEBUG_PRINT
		#define DEBUG_PRINT(x) puts( "-----------------DEBUG--------------------" ); printf x; puts( "\n------------------------------------------\n" );
	#endif
	#ifndef ERROR_PRINT
		#define ERROR_PRINT(x) puts( "#################ERROR####################" ); printf x; printf( "\nArquivo: %s\nLinha: %d\n", __FILE__, __LINE__ ); puts( "##########################################\n" );
	#endif
#endif


tp_table * abreTabela( const char * nomeTabela, struct fs_objects * objeto, tp_table ** tabela ) {
    *objeto = leObjeto( nomeTabela );
    *tabela = leSchema( *objeto );
    return *tabela;
}

int allColumnsExists( rc_insert * s_insert, table * tabela ) {	
	if( !s_insert->columnName ) {
		return 0;
	}

	int i;
	for( i = 0; i < s_insert->N; i++ ) {
		if( retornaTamanhoTipoDoCampo( s_insert->columnName[i], tabela ) == 0 ) {
			printf( "ERROR: column \"%s\" of relation \"%s\" does not exist.\n", s_insert->columnName[i], tabela->nome );
			return 0;
		}		
	}
	return 1;
}

int typesCompatible( const char table_type, const char insert_type ) {
	return ( table_type == 'D' && insert_type == 'I' )
		|| ( table_type == 'D' && insert_type == 'D' )
		|| ( table_type == 'I' && insert_type == 'I' )
		|| ( table_type == 'S' && insert_type == 'S' )
		|| ( table_type == 'S' && insert_type == 'C' )
		|| ( table_type == 'C' && insert_type == 'C' )
		|| ( table_type == 'C' && insert_type == 'S' );
}

char getInsertedType( rc_insert * s_insert, char * columnName, table * tabela ) {
	int i;
	char noValue = '\0';
	
	for( i = 0; i < s_insert->N; i++ ) {
		if( objcmp( s_insert->columnName[i], columnName ) == 0 ) {
			return s_insert->type[i];
		}
	}
	noValue = retornaTamanhoTipoDoCampo( columnName, tabela );
	return noValue;
}

char * getInsertedValue( rc_insert * s_insert, char * columnName, table * tabela ) {
	int i;
	char tipo, * noValue = NULL;

	for( i = 0; i < s_insert->N; i++ ) {
		if( objcmp( s_insert->columnName[i], columnName ) == 0 ) {
			return s_insert->values[i];
		}
	}

	tipo = retornaTamanhoTipoDoCampo( columnName, tabela );
	noValue = (char *)malloc( sizeof(char) * 3 );

	if( tipo == 'I' ) {
		noValue = "0";
	} else if ( tipo == 'D' ) {
		noValue = "0.0";
	} else {
		noValue[0] = '\0';
	}
	return noValue;
}

int iniciaAtributos( struct fs_objects * objeto, tp_table ** tabela, tp_buffer ** bufferpool, char * nomeT ){
    *objeto     = leObjeto( nomeT );
    *tabela     = leSchema( *objeto );
    *bufferpool = initbuffer();

    if( *tabela == ERRO_ABRIR_ESQUEMA ) {
        return ERRO_DE_PARAMETRO;
    }
    if( *bufferpool == ERRO_DE_ALOCACAO ) {
        return ERRO_DE_PARAMETRO;		
	}
    return SUCCESS;
}

int verifyFK( char * tableName, char * column ) {
    if( verificaNomeTabela( tableName ) == 1 ) {
        struct fs_objects objeto = leObjeto( tableName );
        tp_table * esquema = leSchema( objeto );
		
		if( esquema == ERRO_ABRIR_ESQUEMA ) {
            printf( "ERROR: cannot create schema.\n" );
            return 0;
        }

        for(; esquema != NULL; esquema = esquema->next ) {
            if( objcmp( esquema->nome, column ) == 0 ) {
                if( esquema->chave == PK ) {
                    return 1;
                }
            }
        }
    }
	return 0;
}

int verificaChaveFK( char * nomeTabela, column * c, char * nomeCampo, char * valorCampo, char * tabelaApt, char * attApt ) {    
    char str[20] = { '\0' };
    char dat[5] = ".dat";
    struct fs_objects objeto = {};
    tp_table * tabela = NULL;
    tp_buffer * bufferpoll = NULL;
    column * pagina = NULL;

    strcpylower( str, tabelaApt );
    strcat( str, dat ); // Concatena e junta o nome com .dat

    int erro = existeAtributo( nomeTabela, c );
    /*if(erro != SUCCESS )
        return ERRO_DE_PARAMETRO;*/

    //if(existeAtributo(tabelaApt, c))
        //return ERRO_CHAVE_ESTRANGEIRA;

    if( iniciaAtributos( &objeto, &tabela, &bufferpoll, tabelaApt ) != SUCCESS ) {
        free( bufferpoll );
        free( tabela );
        return ERRO_DE_PARAMETRO;
    }

    erro = SUCCESS;
	int x;
    for( x = 0; erro == SUCCESS; x++ ) {
        erro = colocaTuplaBuffer(bufferpoll, x, tabela, objeto);		
	}

	int page;
    for( page = 0; page < QTD_PAGINAS; page++ ) {
        if( pagina ) {
			free(pagina);
		}
		
        pagina = getPage(bufferpoll, tabela, objeto, page);
        if( !pagina ) {
			break;
		}

		int j;
        for( j = 0; j < objeto.qtdCampos * bufferpoll[page].nrec; j++ ) {
            if( pagina[j].nomeCampo ) {
                if( objcmp( pagina[j].nomeCampo, attApt ) == 0 ) {
                    if( pagina[j].tipoCampo == 'S' ) {
                        if( objcmp( pagina[j].valorCampo, valorCampo ) == 0 ) {
                            free( pagina );
                            free( bufferpoll );
                            free( tabela );
                            return SUCCESS;
                        }
                    } else if( pagina[j].tipoCampo == 'I' ) {
                        int *n = (int *)&pagina[j].valorCampo[0];
                        if( *n == atoi(valorCampo) ){
                            free( pagina );
                            free( bufferpoll );
                            free( tabela );
                            return SUCCESS;
                        }
                    } else if( pagina[j].tipoCampo == 'D' ) {
                        double *nn = (double *)&pagina[j].valorCampo[0];

                        if( *nn == atof(valorCampo) ){
                            free( pagina );
                            free( bufferpoll );
                            free( tabela );
                            return SUCCESS;
                        }
                    } else if( pagina[j].tipoCampo == 'C' ) {
                        if( pagina[j].valorCampo == valorCampo ) {
                            free( pagina );
                            free( bufferpoll );
                            free( tabela );
                            return SUCCESS;
                        }
                    } else {
                        free( pagina );
                        free( bufferpoll );
                        free( tabela );
                        return ERRO_CHAVE_ESTRANGEIRA;
                    }
                }
            }
        }
    }

    if( pagina ) {
		free( pagina );
	}
    free( bufferpoll );
    free( tabela );
    return ERRO_CHAVE_ESTRANGEIRA;
}

int verificaChavePK( char * nomeTabela, column * c, char * nomeCampo, char * valorCampo ) {    
    column *pagina = NULL;

    struct fs_objects objeto = {};
    tp_table * tabela = NULL;
    tp_buffer * bufferpoll = NULL;

    int erro = existeAtributo( nomeTabela, c );
    if( erro != SUCCESS ) {
        free( bufferpoll );
        return ERRO_DE_PARAMETRO;
    }

    if( iniciaAtributos( &objeto, &tabela, &bufferpoll, nomeTabela ) != SUCCESS ) {
        free( bufferpoll );
        free( tabela );
        return ERRO_DE_PARAMETRO;
    }
    erro = SUCCESS;
	
	int x;
    for( x = 0; erro == SUCCESS; x++ ) {
        erro = colocaTuplaBuffer( bufferpoll, x, tabela, objeto );		
	}

    int page;
    for( page = 0; page < QTD_PAGINAS; page++ ) {
        if( pagina ) {
			free( pagina );
		}
        pagina = getPage(bufferpoll, tabela, objeto, page);
		
        if( !pagina ) {
			break;
		}

		int j;
        for( j = 0; j < objeto.qtdCampos * bufferpoll[page].nrec; j++ ){
            if( pagina[j].nomeCampo ) {
                if( objcmp(pagina[j].nomeCampo, nomeCampo ) == 0 ) {
                    if( pagina[j].tipoCampo == 'S' ) {
                        if( objcmp( pagina[j].valorCampo, valorCampo ) == 0 ){
                            free( pagina );
                            free( bufferpoll );
                            free( tabela );
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    } else if( pagina[j].tipoCampo == 'I' ) {
                        int *n = (int *)&pagina[j].valorCampo[0];

                        if( *n == atoi(valorCampo) ) {
                            free( pagina );
                            free( bufferpoll );
                            free( tabela );
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    } else if( pagina[j].tipoCampo == 'D' ) {
                        double *nn = (double *)&pagina[j].valorCampo[0];

                        if( *nn == atof(valorCampo) ) {
                            free( pagina );
                            free( bufferpoll );
                            free( tabela );
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    } else if ( pagina[j].tipoCampo == 'C' ){
                        if( pagina[j].valorCampo == valorCampo ) {
                            free( pagina );
                            free( bufferpoll );
                            free( tabela );
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    }
                }
            }
        }
    }

    if( pagina ) {
		free( pagina );
	}
    free( bufferpoll );
    free( tabela );
    return SUCCESS;
}

int finalizaInsert( char * nome, column * c ) {
    column * auxC = NULL, * temp = NULL;
    int i = 0, x = 0, t, erro = SUCCESS, j = 0;
    FILE * dados = NULL;

    struct fs_objects objeto = {}, dicio = {};
    tp_table * auxT = NULL; // Le esquema
    auxT = abreTabela( nome, &dicio, &auxT );

    table * tab     = (table *)malloc( sizeof(table) );
    tp_table * tab2 = (tp_table *)malloc( sizeof(struct tp_table) );
    memset( tab2, 0, sizeof(tp_table) );

    tab->esquema = abreTabela( nome, &objeto, &tab->esquema );
    tab2 = procuraAtributoFK( objeto );

    for( j = 0, temp = c; j < objeto.qtdCampos && temp != NULL; j++, temp = temp->next ) {
        switch( tab2[j].chave ) {
            case NPK:
                erro = SUCCESS;
                break;

            case PK:
                erro = verificaChavePK( nome, temp , temp->nomeCampo, temp->valorCampo );
                if( erro == ERRO_CHAVE_PRIMARIA ) {
                    printf("ERROR: duplicate key value violates unique constraint \"%s_pkey\"\nDETAIL:  Key (%s)=(%s) already exists.\n", nome, temp->nomeCampo, temp->valorCampo);

					free(auxT); // Libera a memoria da estrutura.
					//free(temp); // Libera a memoria da estrutura.
					free(tab); // Libera a memoria da estrutura.
					free(tab2); // Libera a memoria da estrutura.
                    return ERRO_CHAVE_PRIMARIA;
                }
                break;

            case FK:
                if( tab2[j].chave == 2 && strlen(tab2[j].attApt) != 0 && strlen(tab2[j].tabelaApt) != 0 ) {
                    erro = verificaChaveFK(nome, temp, tab2[j].nome, temp->valorCampo, tab2[j].tabelaApt, tab2[j].attApt);

                    if( erro != SUCCESS ) {
                        printf("ERROR: invalid reference to \"%s.%s\". The value \"%s\" does not exist.\n", tab2[j].tabelaApt,tab2[j].attApt,temp->valorCampo);
						free(auxT); // Libera a memoria da estrutura.
						free(temp); // Libera a memoria da estrutura.
                        free(tab); // Libera a memoria da estrutura.
						free(tab2); // Libera a memoria da estrutura.
                        return ERRO_CHAVE_ESTRANGEIRA;
                    }
                }
                break;
        }
    }

    if( erro == ERRO_CHAVE_ESTRANGEIRA ) {
        printf("ERROR: unknown foreign key error.\n");
		free(auxT); // Libera a memoria da estrutura.
		free(temp); // Libera a memoria da estrutura.
        free(tab); // Libera a memoria da estrutura.
		free(tab2); // Libera a memoria da estrutura.
        return ERRO_CHAVE_ESTRANGEIRA;
    }

    if( erro == ERRO_CHAVE_PRIMARIA ) {
        printf("ERROR: unknown primary key error.\n");
		free(auxT); // Libera a memoria da estrutura.
		free(temp); // Libera a memoria da estrutura.
        free(tab); // Libera a memoria da estrutura.
		free(tab2); // Libera a memoria da estrutura.
        return ERRO_CHAVE_PRIMARIA;
    }
    if( erro == ERRO_DE_PARAMETRO ) {
        printf("ERROR: invalid parameter.\n");
		free(auxT); // Libera a memoria da estrutura.
		free(temp); // Libera a memoria da estrutura.
        free(tab); // Libera a memoria da estrutura.
		free(tab2); // Libera a memoria da estrutura.
        return ERRO_DE_PARAMETRO;
    }

    char directory[TAMANHO_NOME_BANCO*2] = { '\0' };
    strcpy(directory, connected.db_directory);
    strcat(directory, dicio.nArquivo);

    if( (dados = fopen(directory,"a+b")) == NULL ) {
        printf("ERROR: cannot open file.\n");
		free(auxT); // Libera a memoria da estrutura.
		free(temp); // Libera a memoria da estrutura.
        free(tab); // Libera a memoria da estrutura.
		free(tab2); // Libera a memoria da estrutura.
        return ERRO_ABRIR_ARQUIVO;

	}

	// Percorre todas as colunas c passadas no parametro da função
    for( auxC = c, t = 0; auxC != NULL; auxC = auxC->next, t++ ) {
        if( t >= dicio.qtdCampos ) {
            t = 0;		
		}

        if( auxT[t].tipo == 'S' ) { // Grava um dado do tipo string.

            if (sizeof(auxC->valorCampo) > auxT[t].tam && sizeof(auxC->valorCampo) != 8){
                printf("ERROR: invalid string length.\n");
				free(tab); // Libera a memoria da estrutura.
				free(tab2); // Libera a memoria da estrutura.
				free(auxT); // Libera a memoria da estrutura.
				free(temp); // Libera a memoria da estrutura.
				fclose(dados);
                return ERRO_NO_TAMANHO_STRING;
            }

            if (objcmp(auxC->nomeCampo, auxT[t].nome) != 0){
                printf("ERROR: column name \"%s\" is not valid.\n", auxC->nomeCampo);
				free(tab); // Libera a memoria da estrutura.
				free(tab2); // Libera a memoria da estrutura.
				free(auxT); // Libera a memoria da estrutura.
				free(temp); // Libera a memoria da estrutura.
				fclose(dados);
                return ERRO_NOME_CAMPO;
            }

            char valorCampo[auxT[t].tam];
            strncpy(valorCampo, auxC->valorCampo, auxT[t].tam);
            strcat(valorCampo, "\0");			
            fwrite(&valorCampo,sizeof(valorCampo),1,dados);

        } else if( auxT[t].tipo == 'I' ) { // Grava um dado do tipo inteiro.
            i = 0;
            while( i < strlen(auxC->valorCampo) ) {
                if( auxC->valorCampo[i] < 48 || auxC->valorCampo[i] > 57 ) {
                    printf( "ERROR: column \"%s\" expectet integer.\n", auxC->nomeCampo );
					free(tab); // Libera a memoria da estrutura.
					free(tab2); // Libera a memoria da estrutura.
					free(auxT); // Libera a memoria da estrutura.
					//free(temp); // Libera a memoria da estrutura.
					fclose(dados);
                    return ERRO_NO_TIPO_INTEIRO;
                }
                i++;
            }

            int valorInteiro = 0;
            sscanf( auxC->valorCampo,"%d",&valorInteiro );
            fwrite( &valorInteiro, sizeof(valorInteiro), 1, dados );

        } else if( auxT[t].tipo == 'D' ) { // Grava um dado do tipo double.
            x = 0;
            while( x < strlen(auxC->valorCampo) ) {
                if( (auxC->valorCampo[x] < 48 || auxC->valorCampo[x] > 57) && (auxC->valorCampo[x] != 46) ) {
                    printf("ERROR: column \"%s\" expect double.\n", auxC->nomeCampo);
					free(tab); // Libera a memoria da estrutura.
					free(tab2); // Libera a memoria da estrutura.
					free(auxT); // Libera a memoria da estrutura.
					free(temp); // Libera a memoria da estrutura.
					fclose(dados);
                    return ERRO_NO_TIPO_DOUBLE;
                }
                x++;
            }

            double valorDouble = convertD(auxC->valorCampo);
            fwrite(&valorDouble,sizeof(valorDouble),1,dados);
			
        } else if( auxT[t].tipo == 'C' ) { // Grava um dado do tipo char.

            if( strlen(auxC->valorCampo) > sizeof(char) ) {
                printf( "ERROR: column \"%s\" expect char.\n", auxC->nomeCampo );
				free(tab); // Libera a memoria da estrutura.
				free(tab2); // Libera a memoria da estrutura.
				free(auxT); // Libera a memoria da estrutura.
				free(temp); // Libera a memoria da estrutura.
				fclose(dados);
                return ERRO_NO_TIPO_CHAR;
            }
            char valorChar = auxC->valorCampo[0];
            fwrite( &valorChar, sizeof(valorChar), 1, dados );
        }

    }

    fclose( dados );
    free( tab ); // Libera a memoria da estrutura.
	free( tab2 ); // Libera a memoria da estrutura.
    free( auxT ); // Libera a memoria da estrutura.
    free( temp ); // Libera a memoria da estrutura.
    return SUCCESS;
}

void insert( rc_insert * s_insert ) {
	table * tabela = (table *)malloc( sizeof(table) );
	tabela->esquema = NULL;
	memset( tabela, 0, sizeof(table) );
	column * colunas = NULL;
	tp_table * esquema = NULL;
	struct fs_objects objeto = {};
	char flag = 0;

	abreTabela( s_insert->objName, &objeto, &tabela->esquema ); //retorna o esquema para a insere valor
	strcpylower( tabela->nome, s_insert->objName );

	if( s_insert->columnName != NULL ) {
		if( allColumnsExists(s_insert, tabela) ) {
			for( esquema = tabela->esquema; esquema != NULL; esquema = esquema->next ) {
				if( typesCompatible(esquema->tipo,getInsertedType(s_insert, esquema->nome, tabela)) ) {
					colunas = insereValor( tabela, colunas, esquema->nome, getInsertedValue(s_insert, esquema->nome, tabela));
				} else {
					printf( "ERROR: data type invalid to column '%s' of relation '%s' (expected: %c, received: %c).\n", esquema->nome, tabela->nome, esquema->tipo, getInsertedType(s_insert, esquema->nome, tabela) );
					flag = 1;
				}
			}
		} else {
			flag = 1;
		}
	} else {		
		// Se a quantidade de campos da inserção for igual a quantidade de campos da tabela
		if( s_insert->N == objeto.qtdCampos ) {
			int i;
			for( i=0; i < objeto.qtdCampos; i++ ) {

				if( s_insert->type[i] == 'S' && tabela->esquema[i].tipo == 'C' ) {
					printf( "\nWARNING: Attempted to write value of type \'string\' to attribute of type \'char\'.\n" );
					printf( "String \'%s\' will be truncated to \'%c\'\n", s_insert->values[i], s_insert->values[i][0] );
					s_insert->values[i][1] = '\0';
					s_insert->type[i] = 'C';
				}

				if( s_insert->type[i] == 'I' && tabela->esquema[i].tipo == 'D' ) {
					s_insert->type[i] = 'D';
				}

				if( s_insert->type[i] == tabela->esquema[i].tipo ) {
					colunas = insereValor( tabela, colunas, tabela->esquema[i].nome, s_insert->values[i]);
				} else {
					printf( "ERROR: data type invalid to column '%s' of relation '%s' (expected: %c, received: %c).\n", tabela->esquema[i].nome, tabela->nome, tabela->esquema[i].tipo, s_insert->type[i] );
					flag=1;
				}
			}
		} else {
			printf( "ERROR: INSERT has more expressions than target columns.\n" );
			flag = 1;
		}
	}

	if( !flag ) {
		if( finalizaInsert( s_insert->objName, colunas ) == SUCCESS ) {
			printf( "INSERT 0 1\n" );
		}
	}

	//freeTp_table(&esquema, objeto.qtdCampos);
	free(esquema);
	freeColumn(colunas);
	freeTable(tabela);
}

int contaColunasRepetidas() {
	int duplicates = 0, skip = 0, index = 0, i = 0, j = 0;
	// Array que armazena nome dos campos já verificados
	char camposVerificados[QTD_COLUNAS_PROJ/2][TAMANHO_NOME_CAMPO] = { {'\0'} }; 
	
	for( i = 0; i < GLOBAL_SELECT.qtdColunas; i++ ) {
		for( j = 0; camposVerificados[j][0] != '\0' ; j++ ) {
			// Verifica se selColumn nao coincide com o nome de alguma coluna ja verificada
			if( strcmp( GLOBAL_SELECT.selColumn[i], camposVerificados[j] ) == 0 ) {															
				skip = 1;
				break;
			}
		}		
		if( skip ) {					
			skip = 0; 
			continue;
		}		
		for( j = i+1; j < GLOBAL_SELECT.qtdColunas; j++ ) {					
			if( strcmp( GLOBAL_SELECT.selColumn[i], GLOBAL_SELECT.selColumn[j] ) == 0 ) {						
				duplicates++;
			}
		}
		
		// Armazena o nome da coluna verificada, evitando que seja comparada novamente
		//  em uma futura iteração
		strcpy( camposVerificados[index], GLOBAL_SELECT.selColumn[i] );
		camposVerificados[index][strlen( GLOBAL_SELECT.selColumn[i] )+1] = '\0';
		index++;
	}
	return duplicates;	
}

int existeColuna( const struct fs_objects * objeto, const tp_table * esquema ) {
	int i = 0, j = 0;
	
	for( i = 0; i < GLOBAL_SELECT.qtdColunas; i++ ) {
		#ifdef UFFS_DEBUG
			printf( "\n------------------DEBUG-----------------\n" );
		#endif
		
		for( j = 0; j < objeto->qtdCampos; j++ ) {
			#ifdef UFFS_DEBUG		
				printf( "GLOBAL_SELECT.selColumn[%d]: \'%s\'\n", i, GLOBAL_SELECT.selColumn[i] );
				printf( "esquema[%d].nome: \'%s\'\n", j, esquema[j].nome );
				printf( "Houve correspondencia? : %s",  
						( strcmp( esquema[j].nome, GLOBAL_SELECT.selColumn[i] ) == 0 )
							? "(SIM)\n"
							: "(NAO)\n"					
				);
				printf( "----------------------------------------\n\n" );
			#endif
			if( strcmp( esquema[j].nome, GLOBAL_SELECT.selColumn[i] ) == 0 ) { break; } // Encontrou, pula para proxima iteração
			if( j == objeto->qtdCampos-1 ) { // Iterou todos os campos e nao encontrou nenhuma correspondencia				
				printf( "ERRO: Campo \'%s\' nao foi encontrado na tabela \'%s\'\n", *GLOBAL_SELECT.selColumn, objeto->nome );
				return 0;
			}
		}
	}
	return 1;
}

int comparaValoresNumericos( const double * a, const double * b, const enum where_operator op  ) {
	switch( op ) {
		case COMPARISON_EQUAL:			
			return *a == *b;
		case COMPARISON_GREATER:
			return *a > *b;
		case COMPARISON_GREATER_EQUAL:
			return *a >= *b;
		case COMPARISON_LESS:
			return *a < *b;
		case COMPARISON_LESS_EQUAL:
			return *a <= *b;
		case COMPARISON_NOT_EQUAL:
			return *a != *b;
	}
	return 0;
}

int comparaValoresString( const char * a, const char * b, const enum where_operator op ) {
	switch( op ) {
		case COMPARISON_EQUAL:			
			return !( objcmp( a, b ) );
		case COMPARISON_GREATER:
			return ( objcmp( a, b ) > 0 ) ? 1 : 0;
		case COMPARISON_GREATER_EQUAL:
			return ( objcmp( a, b ) >= 0 ) ? 1 : 0;
		case COMPARISON_LESS:
			return ( objcmp( a, b ) < 0 ) ? 1 : 0;
		case COMPARISON_LESS_EQUAL:
			return ( objcmp( a, b ) <= 0 ) ? 1 : 0;
		case COMPARISON_NOT_EQUAL:
			return ( objcmp( a, b ) ) ? 1 : 0;
	}
	return 0;
}

int calculaResultadoWhere( const int inicio, const int fim, const int tamPagina, const column * pagina ) {
	char c_lvalue[512] = { '\0' }, c_rvalue[512] = { '\0' }, ltipo = '\0', rtipo = '\0';
	int y = 0, k = 0;
	double d_lvalue = 0, d_rvalue = 0;
	
	for( k = 0; k < GLOBAL_SELECT.qtdExp; k++ ) {				
		switch( GLOBAL_SELECT.expressoes[k].ltipo ) {
			case 'O':						
				for( y = inicio; y <= fim; y++ ) {							
					if( strcmp( pagina[y].nomeCampo, GLOBAL_SELECT.expressoes[k].lvalue ) == 0 ) {								
						if( pagina[y].tipoCampo == 'S' || pagina[y].tipoCampo == 'C' ) {
							strcpy( c_lvalue, pagina[y].valorCampo );									
						} else if( pagina[y].tipoCampo == 'I' || pagina[y].tipoCampo == 'D' ) {
							d_lvalue = *( &pagina[y].valorCampo[0] );
						} 
						ltipo = pagina[y].tipoCampo;
						break;
					}
				}
			break;
			
			case 'S':
				strcpy( c_lvalue, GLOBAL_SELECT.expressoes[k].lvalue );
				break;
			
			case 'I':
				d_lvalue = strtol( GLOBAL_SELECT.expressoes[k].lvalue, NULL, 10 );
				break;
				
			case 'D':
				d_lvalue = strtod( GLOBAL_SELECT.expressoes[k].lvalue, NULL );
				break;
		}
						
		switch( GLOBAL_SELECT.expressoes[k].rtipo ) {
			case 'O':						
				for( y = inicio; y <= fim; y++ ) {							
					if( strcmp( pagina[y].nomeCampo, GLOBAL_SELECT.expressoes[k].rvalue ) == 0 ) {
						if( pagina[y].tipoCampo == 'S' || pagina[y].tipoCampo == 'C' ) {
							strcpy( c_rvalue, pagina[y].valorCampo );
						} else if( pagina[y].tipoCampo == 'I' || pagina[y].tipoCampo == 'D' ) {
							d_rvalue = *( &pagina[y].valorCampo[0] );
						}
						rtipo = pagina[y].tipoCampo;								
						break;
					}
				}
			break;
			
			case 'S':
				strcpy( c_rvalue, GLOBAL_SELECT.expressoes[k].rvalue );						
			break;
			
			case 'I':						
				d_rvalue = strtol( GLOBAL_SELECT.expressoes[k].rvalue, NULL, 10 );						
			break;
				
			case 'D':
				d_rvalue = strtod( GLOBAL_SELECT.expressoes[k].rvalue, NULL );
			break;
		}
		
		if( GLOBAL_SELECT.expressoes[k].ltipo == 'O' ) {
			if( GLOBAL_SELECT.expressoes[k].rtipo == 'O' ) {
				if( ltipo != rtipo ) {
					#ifdef UFFS_DEBUG										
						ERROR_PRINT(( "WHERE: Comparacao com objetos de tipos diferentes" ));
					#endif
					return ERRO_WHERE_COMPARACAO;
				}
				if( ltipo == 'S' ) {
					GLOBAL_SELECT.expressoes[k].result =  comparaValoresString( c_lvalue, c_rvalue, GLOBAL_SELECT.expressoes[k].op );
				} else if( ltipo == 'I' || ltipo == 'D' ) {								
					GLOBAL_SELECT.expressoes[k].result = comparaValoresNumericos( &d_lvalue, &d_rvalue, GLOBAL_SELECT.expressoes[k].op );
				}
			} else if( GLOBAL_SELECT.expressoes[k].rtipo == 'S' ) {
				if( ltipo != 'S' ) { 
					#ifdef UFFS_DEBUG										
						ERROR_PRINT(( "WHERE: Nao e possivel comparar %c com STRING", ltipo ));
					#endif
					return ERRO_WHERE_COMPARACAO; 
				}
				GLOBAL_SELECT.expressoes[k].result =  comparaValoresString( c_lvalue, c_rvalue, GLOBAL_SELECT.expressoes[k].op );
			} else if( GLOBAL_SELECT.expressoes[k].rtipo == 'I' ) {	
				if( ltipo == 'S' ) { 
					#ifdef UFFS_DEBUG										
						ERROR_PRINT(( "WHERE: Nao e possivel comparar %c com STRING", ltipo ));
					#endif
					return ERRO_WHERE_COMPARACAO; 
				}
				if( ltipo == 'I' || ltipo == 'D' ) {								
					GLOBAL_SELECT.expressoes[k].result = comparaValoresNumericos( &d_lvalue, &d_rvalue, GLOBAL_SELECT.expressoes[k].op );
				}
			} else if( GLOBAL_SELECT.expressoes[k].rtipo == 'D' ) {
				if( ltipo == 'S' ) { 
					#ifdef UFFS_DEBUG										
						ERROR_PRINT(( "WHERE: Nao e possivel comparar %c com INTEGER", ltipo ));
					#endif
					return ERRO_WHERE_COMPARACAO; 
				}
				if( ltipo == 'I' || ltipo == 'D' ) {								
					GLOBAL_SELECT.expressoes[k].result = comparaValoresNumericos( &d_lvalue, &d_rvalue, GLOBAL_SELECT.expressoes[k].op );
				}							
			}
		} else if( GLOBAL_SELECT.expressoes[k].ltipo == 'S' ) {
			if( GLOBAL_SELECT.expressoes[k].rtipo == 'O' ) {
				if( rtipo != 'S' ) {
					#ifdef UFFS_DEBUG										
						ERROR_PRINT(( "WHERE: lvalue: STRING, porem rvalue nao e STRING" )); 										
					#endif	
					return ERRO_WHERE_COMPARACAO;
				}
				GLOBAL_SELECT.expressoes[k].result =  comparaValoresString( c_lvalue, c_rvalue, GLOBAL_SELECT.expressoes[k].op );
			} else if( GLOBAL_SELECT.expressoes[k].rtipo == 'S' ) {
				GLOBAL_SELECT.expressoes[k].result =  comparaValoresString( c_lvalue, c_rvalue, GLOBAL_SELECT.expressoes[k].op );
			} else if( 	GLOBAL_SELECT.expressoes[k].rtipo == 'I' ) {
				#ifdef UFFS_DEBUG									
					ERROR_PRINT(( "WHERE: Nao e possivel comparar %c com STRING", ltipo ));
				#endif
				return ERRO_WHERE_COMPARACAO;
			} else if( 	GLOBAL_SELECT.expressoes[k].rtipo == 'D' ) {
				#if DEBUG									
					ERROR_PRINT(( "WHERE: Nao e possivel comparar STRING com DOUBLE" ));
				#endif
				return ERRO_WHERE_COMPARACAO;
			}
		} else if( GLOBAL_SELECT.expressoes[k].ltipo == 'I' ) {
			if( GLOBAL_SELECT.expressoes[k].rtipo == 'O' ) {							
				if( rtipo != 'I' && rtipo != 'D' ) {
					#ifdef UFFS_DEBUG										
						ERROR_PRINT(( "WHERE: lvalue: INTEGER, porem rvalue nao e INTEGER nem DOUBLE" ));
					#endif	
					return ERRO_WHERE_COMPARACAO;
				}
				if( rtipo == 'I' || rtipo == 'D' ) {								
					GLOBAL_SELECT.expressoes[k].result = comparaValoresNumericos( &d_lvalue, &d_rvalue, GLOBAL_SELECT.expressoes[k].op );
				}							
			} else if( GLOBAL_SELECT.expressoes[k].rtipo == 'I' || GLOBAL_SELECT.expressoes[k].rtipo == 'D' ) {
				GLOBAL_SELECT.expressoes[k].result = comparaValoresNumericos( &d_lvalue, &d_rvalue, GLOBAL_SELECT.expressoes[k].op );
			} else if( GLOBAL_SELECT.expressoes[k].rtipo == 'S' ) {
				#ifdef UFFS_DEBUG									
					ERROR_PRINT(( "WHERE: Nao e possivel comparar INTEGER com STRING" ));
				#endif
				return ERRO_WHERE_COMPARACAO;
			}
		} else if( GLOBAL_SELECT.expressoes[k].ltipo == 'D' ) {
			if( GLOBAL_SELECT.expressoes[k].rtipo == 'O' ) {							
				if( rtipo != 'I' && rtipo != 'D' ) {
					#ifdef UFFS_DEBUG										
						ERROR_PRINT(( "WHERE: lvalue: DOUBLE, porem rvalue nao e INTEGER nem DOUBLE" ));
					#endif
					return ERRO_WHERE_COMPARACAO;
				}
				if( rtipo == 'I' || rtipo == 'D' ) {
					GLOBAL_SELECT.expressoes[k].result = comparaValoresNumericos( &d_lvalue, &d_rvalue, GLOBAL_SELECT.expressoes[k].op );
				}						
			} else if( GLOBAL_SELECT.expressoes[k].rtipo == 'I' ||  GLOBAL_SELECT.expressoes[k].rtipo == 'D' ) {
				GLOBAL_SELECT.expressoes[k].result = comparaValoresNumericos( &d_lvalue, &d_rvalue, GLOBAL_SELECT.expressoes[k].op );
			} else if( GLOBAL_SELECT.expressoes[k].rtipo == 'S' ) {	
				#ifdef UFFS_DEBUG									
					ERROR_PRINT(( "WHERE: Nao e possivel comparar DOUBLE com STRING" )); 
				#endif
				return ERRO_WHERE_COMPARACAO;
			}
		}

	}
	return SUCCESS;
}

int preencheCampos( struct campos_container * camposContainer, tp_buffer * bufferpool, const struct fs_objects * objeto, tp_table * esquema ) {	
	int camposIndex = 0, k = 0, e = 0, hit = 0, i = 0, j = 0, qtdTuplas = camposContainer->ntuples, ntuples = 0;
	for( i = 0; qtdTuplas > 0; i++ ) {
		column * pagina = getPage( bufferpool, esquema, *objeto, i );
		int tamPagina = bufferpool[i].nrec * objeto->qtdCampos;
			
		int inicio = 0, fim = objeto->qtdCampos-1;			
		for( j = 0; j < tamPagina; ) {
			if( GLOBAL_SELECT.where ) {
				int err = calculaResultadoWhere( inicio, fim, tamPagina, pagina );
				
				if( err != SUCCESS ) {
					camposContainer->ntuples = ntuples;
					for( j = 0; j < tamPagina; j++ ) {
						free( pagina[j].valorCampo );	
					}		
					free( pagina );
					return ERRO_WHERE_COMPARACAO;
				}
				
				
				int result = 1;
				if( GLOBAL_SELECT.op_bool[0] != OP_INVALID ) {
					if( GLOBAL_SELECT.op_bool[0] == OP_AND ) {
						result = GLOBAL_SELECT.expressoes[0].result & GLOBAL_SELECT.expressoes[1].result;
					} else if( GLOBAL_SELECT.op_bool[0] == OP_OR ) {
						result = GLOBAL_SELECT.expressoes[0].result | GLOBAL_SELECT.expressoes[1].result;
					}
					
					for( k = 2; k < GLOBAL_SELECT.qtdExp; k++ ) {
						if( GLOBAL_SELECT.op_bool[k-1] == OP_AND ) {
							result = result & GLOBAL_SELECT.expressoes[k].result;
						} else if( GLOBAL_SELECT.op_bool[k-1] == OP_OR ) {
							result = result | GLOBAL_SELECT.expressoes[k].result;
						}
					}
				} else {
					result = GLOBAL_SELECT.expressoes[0].result;
				}
				if( result == 0 ) {
					inicio = fim+1;
					fim = inicio + objeto->qtdCampos-1;
					j = inicio;
					continue;
				}
			}

			int tamanho = 0;
			char nomeCampo[TAMANHO_NOME_CAMPO] = { '\0' };
			if( pagina[j].tipoCampo == 'S' ) {
				if( strcmp( pagina[j].nomeCampo, camposContainer->campos[camposIndex]->nome ) == 0 ) {					
					camposContainer->campos[camposIndex]->valores = realloc( camposContainer->campos[camposIndex]->valores, sizeof( char* ) * (e+1) );
					camposContainer->campos[camposIndex]->valores[e] = malloc( sizeof(char) * strlen( pagina[j].valorCampo ) + 1 );
					strcpy( camposContainer->campos[camposIndex]->valores[e], pagina[j].valorCampo );
					tamanho = strlen( pagina[j].valorCampo ) + 1;
					
					if( i == 0 ) {
						camposContainer->campos[camposIndex]->tipo = 'S';
					}
					if( tamanho > camposContainer->campos[camposIndex]->maior ) {
						camposContainer->campos[camposIndex]->maior = tamanho;
					}
					camposIndex = ( camposIndex + 1 ) % camposContainer->ncampos;		
					hit++;						
				}				
			} else if( pagina[j].tipoCampo == 'I' ) {
				int *n = (int *)&pagina[j].valorCampo[0];
				snprintf( nomeCampo, TAMANHO_NOME_CAMPO, "%d", *n );
				tamanho = strlen( nomeCampo ) + 1;
				
				if( i == 0 ) {
					camposContainer->campos[camposIndex]->tipo = 'I';
				}
				
				if( strcmp( pagina[j].nomeCampo, camposContainer->campos[camposIndex]->nome ) == 0 ) {
					char * aux = malloc( sizeof(char) * tamanho );
					strcpy( aux, nomeCampo );					
					camposContainer->campos[camposIndex]->valores = realloc( camposContainer->campos[camposIndex]->valores, sizeof( char* ) * (e+1) );
					camposContainer->campos[camposIndex]->valores[e] = aux;
					
					if( tamanho > camposContainer->campos[camposIndex]->maior ) {
						camposContainer->campos[camposIndex]->maior = tamanho;
					}
					
					camposIndex = ( camposIndex + 1 ) % camposContainer->ncampos;
					hit++;
				}	
			} else if( pagina[j].tipoCampo == 'C' ) {
				if( strcmp( pagina[j].nomeCampo, camposContainer->campos[camposIndex]->nome ) == 0 ) {					
					camposContainer->campos[camposIndex]->valores = realloc( camposContainer->campos[camposIndex]->valores, sizeof( char* ) * (e+1) );
					camposContainer->campos[camposIndex]->valores[e] = malloc( sizeof(char) * strlen( pagina[j].valorCampo ) + 1 );
					strcpy( camposContainer->campos[camposIndex]->valores[e], pagina[j].valorCampo );
					tamanho = strlen( pagina[j].valorCampo ) + 1;
					
					if( i == 0 ) {
						camposContainer->campos[camposIndex]->tipo = 'C';
					}
					if( tamanho > camposContainer->campos[camposIndex]->maior ) {
						camposContainer->campos[camposIndex]->maior = tamanho;
					}
					
					camposIndex = ( camposIndex + 1 ) % camposContainer->ncampos;
					hit++;
				}
			} else if( pagina[j].tipoCampo == 'D' ) {
				double *n = (double *)&pagina[j].valorCampo[0];
				snprintf( nomeCampo, TAMANHO_NOME_CAMPO, "%.*f", options.numeric_precision, *n );
				tamanho = strlen( nomeCampo ) + 1;
				
				if( strcmp( pagina[j].nomeCampo, camposContainer->campos[camposIndex]->nome ) == 0 ) {
					char * aux = malloc( sizeof(char) * tamanho );
					strcpy( aux, nomeCampo );					
					camposContainer->campos[camposIndex]->valores = realloc( camposContainer->campos[camposIndex]->valores, sizeof( char* ) * (e+1) );
					camposContainer->campos[camposIndex]->valores[e] = aux;
					
					if( i == 0 ) {
						camposContainer->campos[camposIndex]->tipo = 'D';
					}
					if( tamanho > camposContainer->campos[camposIndex]->maior ) {
						camposContainer->campos[camposIndex]->maior = tamanho;
					}
					
					camposIndex = ( camposIndex + 1 ) % camposContainer->ncampos;
					hit++;
				}
			}

			
			// OBS: hit representa a quantidade de campos encontrados com sucesso
			// Se encontrou todos os campos necessários, avança p/ próxima tupla
			if( hit == camposContainer->ncampos ) {	
				hit = 0;				
				e++;
				inicio = fim+1;
				fim = inicio + objeto->qtdCampos-1;
				j = inicio;
				ntuples++;
			} else {
				if( j == fim ) {
					j = inicio;
				} else {
					j++;
				}
			} 	
		}	
		qtdTuplas -= bufferpool[i].nrec;
		for( j = 0; j < bufferpool[i].nrec * objeto->qtdCampos; j++ ) {
			free( pagina[j].valorCampo );	
		}		
		free( pagina );
	}
	camposContainer->ntuples = ntuples;
	return SUCCESS;
}

int terminalWidth() {
	struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
}

int terminalHeight() {
	struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_row;
}

int calculaTamLinha( const struct campos_container * camposContainer_t ) {
	int tamLinha = 0, i = 0;	
	for( i = 0; i < camposContainer_t->ncampos; i++ ) {
		int size = strlen( camposContainer_t->campos[i]->nome ) + 1;
		tamLinha += ( size  );
		
		if( camposContainer_t->maioresColunas[i] != size ) {
			int spaces = camposContainer_t->maioresColunas[i] - size;
			int k = 0;
			while( k < spaces ) {				
				tamLinha++;
				k++;
			}
		}				
		if( i+1 < camposContainer_t->ncampos ) { 			
			tamLinha++;
		}
	}
	return tamLinha;
}

int imprime_tela( const struct campos_container * camposContainer_t, int tamLinha ) {
	int i = 0;
	for( i = 0; i < camposContainer_t->ncampos; i++ ) {
		int size = strlen( camposContainer_t->campos[i]->nome ) + 1;		
		printf( " %s ", camposContainer_t->campos[i]->nome );		
		
		if( camposContainer_t->maioresColunas[i] != size ) {
			int spaces = camposContainer_t->maioresColunas[i] - size;
			int k = 0;
			while( k < spaces ) {
				printf( " ");			
				k++;
			}
		}				
		if( i+1 < camposContainer_t->ncampos ) { 
			printf( "|" ); 			
		}
	}
	printf( "\n" );
	
	// Imprime a linha abaixo do header
	for( i = 0; i < tamLinha; i++ ) {
		printf( "-" );
	}
	printf( "-\n" );

	int j = 0;	
	for( i = 0; i < camposContainer_t->ntuples; i ++ ) {
		for( j = 0; j < camposContainer_t->ncampos; j++ ) {
			printf( " %-*s", camposContainer_t->maioresColunas[j], camposContainer_t->campos[j]->valores[i] );
			
			if( ( (j+1) % camposContainer_t->ncampos ) == 0 ) {
				printf( "\n" );
			} else {
				printf( "|" );
			}
		}
	}
	
	printf( "\n(%d %s)\n\n", camposContainer_t->ntuples, ( 1 >= camposContainer_t->ntuples )
				? "row"
				: "rows" 
	);		
	return 1;
}

int imprime_arquivo( const struct campos_container * camposContainer_t, int tamLinha ) {
	FILE * temp = fopen( ".temp.txt", "w+" );	
	if( temp == NULL ) {
		#ifdef UFFS_DEBUG
			ERROR_PRINT(( "Arquivo temp.txt para impressao de tabela nao pode ser criado" ));
		#endif
		return 0;
	}
	
	int i = 0;
	for( i = 0; i < camposContainer_t->ncampos; i++ ) {
		int size = strlen( camposContainer_t->campos[i]->nome ) + 1;		
		fprintf( temp, " %s ", camposContainer_t->campos[i]->nome );		
		
		if( camposContainer_t->maioresColunas[i] != size ) {
			int spaces = camposContainer_t->maioresColunas[i] - size;
			int k = 0;
			while( k < spaces ) {
				fprintf( temp, " ");			
				k++;
			}
		}				
		if( i+1 < camposContainer_t->ncampos ) { 
			fprintf( temp, "|" ); 			
		}
	}
	fprintf( temp, "\n" );
	
	// Imprime a linha abaixo do header
	for( i = 0; i < tamLinha; i++ ) {
		fprintf( temp, "-" );
	}
	fprintf( temp, "-\n" );

	int j = 0;	
	for( i = 0; i < camposContainer_t->ntuples; i ++ ) {
				
		for( j = 0; j < camposContainer_t->ncampos; j++ ) {
			fprintf( temp, " %-*s", camposContainer_t->maioresColunas[j], camposContainer_t->campos[j]->valores[i] );
			
			if( ( (j+1) % camposContainer_t->ncampos ) == 0 ) {
				fprintf( temp, "\n" );
			} else {
				fprintf( temp, "|" );
			}
		}
	}
	
	fprintf( temp, "\n(%d %s)\n\n", camposContainer_t->ntuples, ( 1 >= camposContainer_t->ntuples )
				? "row"
				: "rows" 
	);
	
	fclose( temp );
		
	if( system( "less .temp.txt" ) == -1 ) {
		printf( "ERRO: Nao foi possivel exibir a tabela!!!" );
		#ifdef UFFS_DEBUG			
			ERROR_PRINT(( "Comando less nao pode ser executado ou o arquivo .temp.txt nao existe" ));
		#endif
		return 0;
	}	
	return 1;	
}

int imprime_tabela( const struct campos_container * camposContainer_t ) {
	int tamLinha = calculaTamLinha( camposContainer_t );
	int print_to_screen = ( terminalWidth() > tamLinha && terminalHeight() > camposContainer_t->ntuples );	
	return ( print_to_screen )
		? imprime_tela( camposContainer_t, tamLinha )
		: imprime_arquivo( camposContainer_t, tamLinha );	
}

void consulta( const char nomeTabela[] ) {    
	register int i = 0, j = 0;
    struct fs_objects objeto = {};
	
	#ifdef UFFS_DEBUG
		DEBUG_PRINT(( "Nome da tabela: \'%s\'", nomeTabela ));
		if( GLOBAL_SELECT.where ) {
			if( GLOBAL_SELECT.qtdExp > 0 ) {
				puts( "-------------------------DEBUG-------------------------\n" );
				for( i = 0; i < GLOBAL_SELECT.qtdExp; i++ ) {						
					printf( "GLOBAL_SELECT.expressoes[%d].lvalue: %s\n", i, GLOBAL_SELECT.expressoes[i].lvalue );
					printf( "GLOBAL_SELECT.expressoes[%d].ltipo: %c\n", i, GLOBAL_SELECT.expressoes[i].ltipo );
					printf( "GLOBAL_SELECT.expressoes[%d].rvalue: %s\n", i, GLOBAL_SELECT.expressoes[i].rvalue );
					printf( "GLOBAL_SELECT.expressoes[%d].rtipo: %c\n", i, GLOBAL_SELECT.expressoes[i].rtipo );				
					puts( "---------------------------------------------------------\n" );
				}				
			}
		}
	#endif

    if( !verificaNomeTabela(nomeTabela) ){
        printf( "\nERRO: A relacao \"%s\" nao foi encontrada.\n\n", nomeTabela );
        return;
    }

    objeto = leObjeto( nomeTabela );
    tp_table * esquema = leSchema( objeto );

    if( esquema == ERRO_ABRIR_ESQUEMA ){
		#ifdef UFFS_DEBUG
			ERROR_PRINT(( "Nao foi possivel criar o esquema" ));
		#endif
        free(esquema);
        return;
    }
	
	int qtdCampos = 0, select_all = 0; 
	if( *GLOBAL_SELECT.selColumn[0] == '*' ) {
		qtdCampos = objeto.qtdCampos;
		select_all = 1;
	} else {				
		qtdCampos = GLOBAL_SELECT.qtdColunas;
		
		// Verifica se a quantidade de colunas passadas no SELECT é <= à quantidade de colunas
		// 	que existem na tabela
		if( qtdCampos > objeto.qtdCampos ) {
			int duplicates = contaColunasRepetidas();
			
			#ifdef UFFS_DEBUG				
				DEBUG_PRINT(( "Quantidade de colunas duplicadas no comando SELECT: %d", duplicates ));
			#endif
			
			if( ( qtdCampos - duplicates ) > objeto.qtdCampos ) {
				printf( "Quantidade de colunas em SELECT excede a quantidade de colunas da tabela \'%s\'\n", objeto.nome );
				return;
			}			
		}
		
		// Verifica se os nomes das colunas passadas no SELECT existem na tabela
		if( existeColuna( &objeto, esquema ) == 0 ) {			
			return;
		}		
	}	 
	
	// Verifica se lvalue e rvalue estao relacionados da maneira correta
	for( i = 0; i < GLOBAL_SELECT.qtdExp; i++ ) {	
		if( GLOBAL_SELECT.expressoes[i].ltipo == 'S' ) {
			if( GLOBAL_SELECT.expressoes[i].rtipo != 'S' && 
				GLOBAL_SELECT.expressoes[i].rtipo != 'O' 
			) {
				printf( "rvalue nao e STRING ou OBJETO\n" );
				return;
			}
		} else if( GLOBAL_SELECT.expressoes[i].ltipo == 'I' ) {
			if( GLOBAL_SELECT.expressoes[i].rtipo == 'S' ) {
				printf( "rvalue nao pode ser STRING\n" );
				return;
			}
		} else if( GLOBAL_SELECT.expressoes[i].ltipo == 'D' ) {
			if( GLOBAL_SELECT.expressoes[i].rtipo == 'S' ) {
				printf( "rvalue nao pode ser STRING\n" );
				return;
			}
		}		
	}
	
	// Verifica se os campos passados no WHERE existem na tabela
	for( i = 0; i < GLOBAL_SELECT.qtdExp; i++ ) {
		if( GLOBAL_SELECT.expressoes[i].ltipo == 'O' ) {
			for( j = 0; j < objeto.qtdCampos; j++ ) {
				if( strcmp( GLOBAL_SELECT.expressoes[i].lvalue, esquema[j].nome ) == 0 ) {
					break;
				}
				if( j+1 == objeto.qtdCampos ) {
					printf( "Campo \'%s\' nao existe na tabela \'%s\'\n", GLOBAL_SELECT.expressoes[i].lvalue, nomeTabela );
					return;
				}
			}
		}
		if( GLOBAL_SELECT.expressoes[i].rtipo == 'O' ) {
			for( j = 0; j < objeto.qtdCampos; j++ ) {
				if( strcmp( GLOBAL_SELECT.expressoes[i].rvalue, esquema[j].nome ) == 0 ) {
					break;
				}				
				if( j+1 == objeto.qtdCampos ) {
					printf( "Campo \'%s\' nao existe na tabela %s\n", GLOBAL_SELECT.expressoes[i].rvalue, nomeTabela );
					return;
				}
			}
		}
	}

    tp_buffer * bufferpoll = initbuffer();
    if( bufferpoll == ERRO_DE_ALOCACAO ) {
        free( bufferpoll );
        free( esquema );
		#ifdef UFFS_DEBUG
			ERROR_PRINT(( "ERRO: Nao ha memoria disponivel para alocar o buffer" ));
		#endif
        return;
    }
    
	int qtdTuplas = 0, x = 0;		
	int erro = SUCCESS;
    for( x = 0; erro == SUCCESS; x++ ) {
        erro = colocaTuplaBuffer( bufferpoll, x, esquema, objeto );
		if( erro == SUCCESS ) { 
			qtdTuplas++;
		}
	}
		
	#ifdef UFFS_DEBUG		
		for( i = 0; i < objeto.qtdCampos; i++ ) {		
			DEBUG_PRINT(( "Capacidade do campo %s: %d bytes", esquema[i].nome, esquema[i].tam ));
		}		
	#endif
	
	struct campos_container camposContainer_t;
	camposContainer_t.campos 			= malloc( sizeof(struct campo*) * qtdCampos );
	camposContainer_t.ntuples 			= qtdTuplas;
	camposContainer_t.ncampos 			= 0;
	camposContainer_t.maioresColunas 	= malloc( sizeof(int) * qtdCampos );

	if( camposContainer_t.campos == NULL || camposContainer_t.maioresColunas == NULL ) {
		#ifdef UFFS_DEBUG
			ERROR_PRINT(( "Erro de alocacao de memoria"));
		#endif
		free( camposContainer_t.maioresColunas );
		free( camposContainer_t.campos );
		free( bufferpoll );
        free( esquema );
		return;
	}
		
	for( i = 0; i < qtdCampos; i++ ) {
		struct campo * campo_t = malloc( sizeof(struct campo) );
		if( select_all ) {
			campo_t->nome = malloc( sizeof(char) * strlen( esquema[i].nome ) + 1 );
			//campo_t->nome 	= esquema[i].nome;
			strcpy( campo_t->nome, esquema[i].nome );
		} else {
			campo_t->nome = malloc( sizeof(char) * strlen( GLOBAL_SELECT.selColumn[i] ) + 1 );
			strcpy( campo_t->nome, GLOBAL_SELECT.selColumn[i] );
		}
		campo_t->valores = NULL;
		campo_t->maior 	= 0;
		campo_t->tipo 	= '\0';
		camposContainer_t.campos[i] = campo_t;
		camposContainer_t.ncampos++;
	}
	
	int err = preencheCampos( &camposContainer_t, bufferpoll, &objeto, esquema );
	
	if( err != SUCCESS ) {
		#ifdef UFFS_DEBUG
			if( err == ERRO_WHERE_COMPARACAO ) {
				DEBUG_PRINT(( "Erro de comparacao em WHERE" ));				
			}
		#endif		
		free( camposContainer_t.maioresColunas );
		for( i = 0; i < camposContainer_t.ncampos; i++ ) {
			free( camposContainer_t.campos[i]->nome );
			free( camposContainer_t.campos[i] );
		}
		free( camposContainer_t.campos );
		free( bufferpoll );
        free( esquema );
		return;
	}
		
	#ifdef UFFS_DEBUG		
		for( i = 0; i < camposContainer_t.ncampos; i++ ) {			
			DEBUG_PRINT(( "Maior comprimento da coluna \'%s\': %d", camposContainer_t.campos[i]->nome, camposContainer_t.campos[i]->maior ));
		}		
	#endif
	
	// Verifica se o ( tam nome da coluna i ) > ( tam maior valor da coluna i )	
	for( i = 0; i < camposContainer_t.ncampos; i++ ) {
		int size = strlen( camposContainer_t.campos[i]->nome ) + 1;
		camposContainer_t.maioresColunas[i] = ( size > camposContainer_t.campos[i]->maior )
												? size
												: camposContainer_t.campos[i]->maior;		
	}
	
	if( imprime_tabela( &camposContainer_t ) == 0 ) {
		printf( "Erro ao exibir tabela\n" ); // Mostrar qual erro, muito ambiguo assim
	}
	
	for( i = 0; i < camposContainer_t.ncampos; i++ ) {
		for( j = 0; j < camposContainer_t.ntuples; j++ ) {
			free( camposContainer_t.campos[i]->valores[j] );
			camposContainer_t.campos[i]->valores[j] = NULL;
		}
		free( camposContainer_t.campos[i]->valores );
		camposContainer_t.campos[i]->valores = NULL;
		free( camposContainer_t.campos[i]->nome ); //NUNCA LIBERAR ESTE CAMPO, pois referencia variável global GLOBAL_SELECT.selColumn
		free( camposContainer_t.campos[i] );
		camposContainer_t.campos[i] = NULL;		
	}
	
	free( camposContainer_t.campos );
	free( camposContainer_t.maioresColunas );
    free( bufferpoll );
    free( esquema );	
}
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Copia todas as informações menos a tabela do objeto, que será removida.
    Parametros: Objeto que será removido do schema.
    Retorno:    INT
                SUCCESS,
                ERRO_REMOVER_ARQUIVO_SCHEMA
   ---------------------------------------------------------------------------------------------*/

int procuraSchemaArquivo( struct fs_objects objeto ) {
    FILE * schema = NULL; 
	FILE * newSchema = NULL;
    int cod = 0;
    char * tupla = (char *)malloc( sizeof(char) * 109 );
    memset( tupla, '\0', 109 );

    tp_table * esquema = (tp_table *)malloc( sizeof(tp_table) * objeto.qtdCampos );
    memset( esquema, 0, sizeof(tp_table) * objeto.qtdCampos );

    char directory[TAMANHO_NOME_BANCO*2] = { '\0' };
    memset( &directory, '\0', TAMANHO_NOME_BANCO*2 );

    strcpy( directory, connected.db_directory );
    strcat( directory, "fs_schema.dat" );

    if( (schema = fopen( directory, "a+b") ) == NULL ) {
        free( tupla );
		free( esquema );
        return ERRO_REMOVER_ARQUIVO_SCHEMA;
    }

    strcpy( directory, connected.db_directory );
    strcat( directory, "fs_nschema.dat" );

    if( (newSchema = fopen( directory, "a+b") ) == NULL ) {
        free(tupla);
		free( esquema );
        return ERRO_REMOVER_ARQUIVO_SCHEMA;
    }

    fseek( newSchema, 0, SEEK_SET );

    while( (fgetc( schema ) != EOF) ) { // Varre o arquivo ate encontrar todos os campos com o codigo da tabela.
        fseek( schema, -1, SEEK_CUR );
        fseek( newSchema, 0, SEEK_END );

        if( fread(&cod, sizeof(int), 1, schema) ) { // Le o codigo da tabela.
            if( cod != objeto.cod ) {
                fwrite( &cod, sizeof(int), 1, newSchema );
                fread( tupla, sizeof(char), TAMANHO_NOME_CAMPO, schema );
                strcpylower( esquema[0].nome,tupla ); // Copia dados do campo para o esquema.
                fwrite( tupla, sizeof(char), TAMANHO_NOME_CAMPO, newSchema );

                fread( &esquema[0].tipo , sizeof(char), 1 , schema );
                fread( &esquema[0].tam  , sizeof(int) , 1 , schema );
                fread( &esquema[0].chave, sizeof(int) , 1 , schema );

                fwrite( &esquema[0].tipo , sizeof(char), 1, newSchema );
                fwrite( &esquema[0].tam  , sizeof(int) , 1, newSchema );
                fwrite( &esquema[0].chave, sizeof(int) , 1, newSchema );

                fread( tupla, sizeof(char), TAMANHO_NOME_TABELA, schema );
                strcpylower( esquema[0].tabelaApt,tupla );
                fwrite( &esquema[0].tabelaApt, sizeof(char), TAMANHO_NOME_TABELA, newSchema );

                fread( tupla, sizeof(char), TAMANHO_NOME_CAMPO, schema );
                strcpylower( esquema[0].attApt,tupla );
                fwrite( &esquema[0].attApt, sizeof(char), TAMANHO_NOME_CAMPO, newSchema );
            } else {
                fseek( schema, 109, 1 );
            }
        }
    }

    fclose(newSchema);
    fclose(schema);

    char directoryex[TAMANHO_NOME_BANCO*4] = { '\0' };
    memset( &directoryex, '\0', TAMANHO_NOME_BANCO*4 );
    strcpy( directoryex, connected.db_directory );
    strcat( directoryex, "fs_schema.dat" );

    remove( directoryex );
    strcpy( directoryex, "mv " );
    strcat( directoryex, connected.db_directory );
    strcat( directoryex, "fs_nschema.dat " );
    strcat( directoryex, connected.db_directory );
    strcat( directoryex, "fs_schema.dat" );
    system( directoryex );
	
	free( esquema );
    free( tupla );
	
    return SUCCESS;
}

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Função para exclusão de tabelas.
    Parametros: Nome da tabela (char).
    Retorno:    INT
                SUCCESS,
                ERRO_REMOVER_ARQUIVO_OBJECT,
                ERRO_REMOVER_ARQUIVO_SCHEMA,
                ERRO_LEITURA_DADOS.
   ---------------------------------------------------------------------------------------------*/

int excluirTabela( char * nomeTabela ) {	
    struct fs_objects objeto, objeto1;
    tp_table * esquema = NULL; 
	tp_table * esquema1 = NULL;
    int x,erro, i, j, l, qtTable;
    char str[20] = { '\0' };
    char dat[5] = ".dat";
    //memset(str, '\0', 20);

    if ( !verificaNomeTabela( nomeTabela ) ) {
        printf("ERROR: table \"%s\" does not exist.\n", nomeTabela);
        return ERRO_NOME_TABELA;
    }
	

    strcpylower(str, nomeTabela);
    strcat(str, dat);              //Concatena e junta o nome com .dat

    abreTabela(nomeTabela, &objeto, &esquema);
    qtTable = quantidadeTabelas();
	

    char ** tupla = (char **)malloc( sizeof(char **) * qtTable );
    memset(tupla, 0, qtTable);

    for (i=0; i < qtTable; i++) {
        tupla[i] = (char *)malloc(sizeof(char)*TAMANHO_NOME_TABELA);
        memset(tupla[i], '\0', TAMANHO_NOME_TABELA);
    }

    tp_table *tab2 = NULL; //(tp_table *)malloc( sizeof(struct tp_table) );
    tab2 = procuraAtributoFK( objeto );   //retorna o tipo de chave que e cada campo

    FILE * dicionario = NULL;

    char directory[TAMANHO_NOME_BANCO*2];
    memset(directory, '\0', TAMANHO_NOME_BANCO*2);

    strcpy(directory, connected.db_directory);
    strcat(directory, "fs_object.dat");

    if((dicionario = fopen(directory,"a+b")) == NULL) {
		free( tab2 );
        return ERRO_ABRIR_ARQUIVO;	
	}
	

    int k = 0;
    while(fgetc (dicionario) != EOF){
        fseek( dicionario, -1, SEEK_CUR );

        //coloca o nome de todas as tabelas em tupla
        fread(tupla[k], sizeof(char), TAMANHO_NOME_TABELA , dicionario);
        k++;

        fseek( dicionario, 28, SEEK_CUR );
    }

    fclose(dicionario);

    for(i = 0; i < objeto.qtdCampos; i++){
        if(tab2[i].chave == PK){
            for(j=0; j<qtTable; j++) {                      //se tiver chave primaria verifica se ela e chave
                if(objcmp(tupla[j], nomeTabela) != 0) {     //estrangeira em outra tabela

                    abreTabela(tupla[j], &objeto1, &esquema1);

                    tp_table *tab3 = (tp_table *)malloc(sizeof(struct tp_table));
                    tab3 = procuraAtributoFK(objeto1);

                    for(l=0; l<objeto1.qtdCampos; l++) {
                        if(tab3[l].chave == FK) { //verifica se a outra tabela possui chave estrangeira. se sim, verifica se e da tabela anterior.
                            if(objcmp(nomeTabela, tab3[l].tabelaApt) == 0) {
                                printf("ERROR: cannot drop table \"%s\" because other objects depend on it.\n", nomeTabela);
                                return ERRO_CHAVE_ESTRANGEIRA;
                            }
                        }
                    }
                    free(tab3);
                }
            }
        }
    }

	free( tab2 );
	for( i = 0; i < qtTable; i++ ) {
        free( tupla[i] );
    }
	free( tupla );
	
    tp_buffer * bufferpoll = initbuffer();

    if( bufferpoll == ERRO_DE_ALOCACAO ) {
        printf( "ERROR: no memory available to allocate buffer.\n" );		
        return ERRO_LEITURA_DADOS;
    }

    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, esquema, objeto);

    if(procuraSchemaArquivo(objeto) != 0) {
        free(bufferpoll);		
        return ERRO_REMOVER_ARQUIVO_SCHEMA;
    }

    if(procuraObjectArquivo(nomeTabela) != 0) {
        free(bufferpoll);		
        return ERRO_REMOVER_ARQUIVO_OBJECT;
    }

   	strcpy(directory, connected.db_directory);
    strcat(directory, str);

    remove( directory );
    free( bufferpoll );	

    printf("DROP TABLE\n");

    return SUCCESS;
}

int verifyFieldName( char ** fieldName, int N ) {    
    if( N <= 1 ) {
		return 1;
	}
	
	int i, j;
    for( i = 0; i < N; i++ ) {
        for( j = i+1; j < N; j++ ) {
            if( objcmp( fieldName[i], fieldName[j] ) == 0 ){
                printf( "ERROR: column \"%s\" specified more than once\n", fieldName[i] );
                return 0;
            }
        }
    }
    return 1;
}

void createTable( rc_insert * t ) {	
    //strcpylower( t->objName, t->objName );        //muda pra minúsculo
	if( (strlen( t->objName) + 1) > TAMANHO_NOME_TABELA ) {
		printf( "ERRO: Nome da tabela excede tamanho maximo permitido\n" );
		return;
	}
    char * tableName = (char*)malloc( sizeof(char) * TAMANHO_NOME_TABELA );
	char fkTable[TAMANHO_NOME_TABELA] = { '\0' };
	char fkColumn[TAMANHO_NOME_CAMPO] = { '\0' };

    strncpylower( tableName, t->objName, TAMANHO_NOME_TABELA );
    strcat( tableName, ".dat\0" );                  //tableName.dat

    if( existeArquivo(tableName) ){
        printf("ERROR: Table already exists\n");
        free( tableName );
        return;
    }

    table * tab = NULL;
    tab = iniciaTabela( t->objName );    // Retorna um tabela inicializada

    if( 0 == verifyFieldName( t->columnName, t->N ) ){
        free( tableName );
        freeTable( tab );
        return;
    }
	
    int i;	
	int size = 0;	
    for( i = 0; i < t->N; i++ ) {		
		
    	if (t->type[i] == 'S') {   					
			size = atoi(t->values[i]);							
		} else if (t->type[i] == 'I') {
    		size = sizeof(int);
    	} else if (t->type[i] == 'D') {
    		size = sizeof(double);
        } else if (t->type[i] == 'C') {
    		size = sizeof(char);		
		}

    	if ( t->attribute[i] == FK ) {
    		strncpylower( fkTable, t->fkTable[i], TAMANHO_NOME_TABELA );
    		strncpylower( fkColumn,t->fkColumn[i], TAMANHO_NOME_CAMPO );
    	} else {
    		strcpy( fkTable, "" );
    		strcpy( fkColumn, "" );
    	}
				
		tp_table * temp = malloc( sizeof( tp_table ) );
		int tamanho = strlen( t->columnName[i] ) + 1;
		char nomeCampo[TAMANHO_NOME_CAMPO] = { '\0' };
		
		// Se o tamanho do nome do campo for muito grande, o mesmo é truncado
		if( tamanho > TAMANHO_NOME_CAMPO ) {			
			strncpy( nomeCampo, t->columnName[i], TAMANHO_NOME_CAMPO-1 );
			nomeCampo[TAMANHO_NOME_CAMPO] = '\0';
			printf( "WARNING: identificador %s sera truncado para %s\n", t->columnName[i], nomeCampo );			
		} else {
			strcpy( nomeCampo, t->columnName[i] );
		}
		
		strcpy( temp->nome, nomeCampo );
		temp->tipo 	= t->type[i];
		temp->tam 	= size;
		temp->chave = t->attribute[i];
		strcpy( temp->tabelaApt, fkTable );
		strcpy( temp->attApt, fkColumn );
		temp->next 	= NULL;				
		
        tab = adicionaCampo( tab, temp );
		
		free( temp );
		temp = NULL;		
		
        if( ( strcmp( fkTable, "") != 0 ) || ( strcmp(fkColumn, "" ) != 0 ) ) {
            if( verifyFK( fkTable, fkColumn ) == 0 ){
    			printf( "ERROR: attribute FK cannot be referenced\n" );				
                free( tableName );
                freeTable( tab );
                return;
    		}
        }		
		
    }	
	
    printf( "%s\n",( finalizaTabela(tab) == SUCCESS )? "CREATE TABLE" : "ERROR: table already exist\n" );	
    free( tableName );
    if( tab != NULL ) {		
		freeTable( tab );
	}
	
}

