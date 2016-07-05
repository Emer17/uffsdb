#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FMACROS // garante que macros.h não seja reincluída
   #include "macros.h"
#endif

#ifndef FTYPES // garante que types.h não seja reincluída
  #include "types.h"
#endif

#ifndef FMISC // garante que misc.h não seja reincluída
  #include "misc.h"
#endif

#ifdef UFFS_DEBUG
	#ifndef DEBUG_PRINT
		#define DEBUG_PRINT(x) puts( "-----------------DEBUG--------------------" ); printf x; puts( "\n------------------------------------------\n" );
	#endif
	#ifndef ERROR_PRINT
		#define ERROR_PRINT(x) puts( "#################ERROR####################" ); printf x; printf( "\nArquivo: %s\nLinha: %d\n", __FILE__, __LINE__ ); puts( "##########################################\n" );
	#endif
#endif

char connectDB( const char *db_name ) {
	FILE *DB;
	int i;
	char vec_name 				[QTD_DB][TAMANHO_NOME_BANCO],
		 vec_directory 			[QTD_DB][TAMANHO_NOME_BANCO],
		 valid,
		 directory 				[TAMANHO_NOME_BANCO*2] = "data/";

    if((DB = fopen("data/DB.dat","rb")) == NULL) {
       	return ERRO_ABRIR_ARQUIVO;
    }

    for(i=0; fgetc (DB) != EOF; i++) {
    	fseek(DB, -1, 1);

    	fread(&valid			,sizeof(char), 			 1, DB);
        fread(vec_name[i]  		,sizeof(char), TAMANHO_NOME_BANCO, DB);
        fread(vec_directory[i] 	,sizeof(char), TAMANHO_NOME_BANCO, DB);

        if(objcmp(vec_name[i], db_name) == 0) {				//verifica se encontrou o banco
        	if(valid) {
        		strcat(directory, vec_directory[i]); 			// atualiza o diretorio do banco que esta conectado
	        	strcpylower(connected.db_directory, directory);
	        	fclose(DB);
	        	return SUCCESS;
	        }
        }
    }
    fclose(DB);

    return DB_NOT_EXISTS;

}

int createDB( const char *db_name ) {
	FILE * DB_file = NULL;
	char vec_name [QTD_DB][TAMANHO_NOME_BANCO];
	char vec_directory [QTD_DB][TAMANHO_NOME_BANCO];
	char create	[TAMANHO_NOME_BANCO+6] = "mkdir data/";
	char * aux_name_tolower = NULL;
	char valid;

    if( (DB_file = fopen("data/DB.dat", "a+b")) == NULL ) {
       	printf("ERROR: cannot open file\n");
		return 0;
    }

	char nomeBanco[TAMANHO_NOME_BANCO] = { '\0' };
	
    if( (strlen(db_name)+1) > TAMANHO_NOME_BANCO ) {		    	
		strncpy( nomeBanco, db_name, TAMANHO_NOME_BANCO-1 );
		nomeBanco[TAMANHO_NOME_BANCO] = '\0';
		printf( "WARNING: Nome do banco %s sera truncado para %s\n", db_name, nomeBanco );
    } else {
		strcpy( nomeBanco, db_name );
	}

	int db_count;
    for( db_count = 0; fgetc(DB_file) != EOF; db_count++ ) {
    	fseek( DB_file, -1, SEEK_CUR );

    	fread( &valid, sizeof(char), 1, DB_file );
        fread( vec_name[db_count], sizeof(char), TAMANHO_NOME_BANCO, DB_file );
        fread( vec_directory[db_count], sizeof(char), TAMANHO_NOME_BANCO, DB_file );

        if( objcmp( vec_name[db_count], nomeBanco ) == 0 ) { // Se há conflito de nomes
        	if( valid ) {
	        	fclose( DB_file );				
				if( objcmp( nomeBanco, "uffsdb" ) != 0 ) { 					
	        		printf( "ERROR: database already exists\n" );
				}				
	            return 0;
	        }
        }
    }
	
    if( db_count >= QTD_DB ) {
    	fclose( DB_file );
    	printf( "ERROR: The amount of databases in this machine exceeded the limit.\n" );
    	return 0;
    }

    data_base * DB = NULL;

	DB = (data_base *)malloc( sizeof(data_base) );
	int len = strlen( nomeBanco );
	memset( DB->db_name, 0, TAMANHO_NOME_BANCO );
	memset( DB->db_directory, 0, TAMANHO_NOME_BANCO );
	DB->valid = 1;
	
	strcpylower( DB->db_name, nomeBanco );
	strcpylower( DB->db_directory, nomeBanco );
	strcat( DB->db_directory, "/" );
	DB->db_name[len] = '\0';
	DB->db_directory[len+1] = '\0';
	fwrite( DB, sizeof( data_base ), 1, DB_file );

    aux_name_tolower = (char *)malloc( sizeof(char) * (strlen(nomeBanco) + 1) );
    strcpylower( aux_name_tolower, nomeBanco );
    strcat( create, aux_name_tolower );
    free( aux_name_tolower );
	free( DB );

	if( system(create) == -1 ) {			//verifica se foi possivel criar o diretorio
		printf( "ERROR: It was not possible to create the database\n" );		
		fclose( DB_file );
		return 0;
	}

    fclose( DB_file );
	
	/*
    free( SGBD );
	
    if( objcmp(db_name, "uffsdb") != 0 ) {
        printf( "CREATE DATABASE\n" );
	}
	*/
	
	printf( "CREATE DATABASE\n" );
	return 1;
}

void dropDatabase( const char *db_name ) {
	FILE * DB = NULL;	
	char vec_name 				[QTD_DB][TAMANHO_NOME_BANCO],
		 vec_directory 			[QTD_DB][TAMANHO_NOME_BANCO],
		 valid;

	if( strcmp(db_name, connected.db_name) == 0 ) {
		printf( "ERROR: You can not delete the database that you are connected to.\n" );
		return;
	}

    if((DB = fopen("data/DB.dat","r+b")) == NULL) {
       	printf("ERROR: cannot open file\n");
		return;
    }

	int i;
    for( i = 0; fgetc( DB ) != EOF; i++ ) {
    	fseek( DB, -1, SEEK_CUR );

    	fread( &valid, sizeof(char), 1, DB );
        fread( vec_name[i], sizeof(char), TAMANHO_NOME_BANCO, DB );
        fread( vec_directory[i], sizeof(char), TAMANHO_NOME_BANCO, DB );

        if( objcmp( vec_name[i], db_name ) == 0 ) {
        	if( valid ) {
	        	valid = 0;
	        	fseek( DB, ( TAMANHO_NOME_BANCO * 2 + 1 ) * i, SEEK_SET ); 	// posiciona o cabecote sobre o byte de validade
	        	fwrite( &valid, sizeof(char), 1, DB );			// do banco e apaga ele

	        	char directory[TAMANHO_NOME_BANCO*2] = "rm data/";
	        	strcat( directory, vec_directory[i] );
	        	strcat( directory, " -R\0" );

	        	system( directory );

	        	fclose( DB );
	        	printf( "DROP DATABASE\n" );
	        	return;
	        }
        }
    }
    fclose( DB );

    printf( "ERROR: database does not exist\n" );

}

void showDB() {
	FILE *DB = NULL;
	int i, qtdDB=0;
	char vec_name 				[QTD_DB][TAMANHO_NOME_BANCO],
		 vec_directory 			[QTD_DB][TAMANHO_NOME_BANCO],
		 valid;

    if((DB = fopen("data/DB.dat","r+b")) == NULL) {
       	printf("ERROR: cannot open file\n");
		return;
    }

    printf(" 					List of databases\n");
    printf("       Name  	    |  Owner   | Encoding |   Collate   |    Ctype    |   Access privileges  \n");
    printf("--------------------+----------+----------+-------------+-------------+-----------------------\n");
    for(i=0; fgetc (DB) != EOF; i++) {
    	fseek(DB, -1, 1);

    	fread(&valid			,sizeof(char), 			 1, DB);
        fread(vec_name[i]  		,sizeof(char), TAMANHO_NOME_BANCO, DB);
        fread(vec_directory[i] 	,sizeof(char), TAMANHO_NOME_BANCO, DB);

       	if(valid) {
       		printf("%-20s| uffsdb  | UTF8     | pt_BR.UTF-8 | pt_BR.UTF-8 | \n", vec_name[i]);
       		qtdDB++;
        }
    }

    printf("(%d %s)\n\n\n\n", qtdDB, (1 >= qtdDB)? "row": "rows");
    fclose(DB);
}

int dbInit( const char *db ) {	
	if( system("mkdir data > /dev/null 2>&1") == -1 ) {
		printf("ERROR: Nao foi possivel inicializar o uffsdb\n");
		return 0;
	}
	
	// Se o nome do banco não foi especificado, cria o banco padrão
    if( db == NULL ) {		
		createDB( "uffsdb" );		
    } else {
		createDB( db );
	}	
	
	return 1;
}

