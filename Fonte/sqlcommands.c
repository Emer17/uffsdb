#include <stdio.h>
#include <stdlib.h>
#include <string.h>
////
#ifndef FMACROS // garante que macros.h não seja reincluída
   #include "macros.h"
#endif
///
#ifndef FTYPES // garante que types.h não seja reincluída
  #include "types.h"
#endif
////
#ifndef FMISC // garante que misc.h não seja reincluída
  #include "misc.h"
#endif

#ifndef FBUFFER // garante que buffer.h não seja reincluída
  #include "buffer.h"
#endif

#ifndef FDICTIONARY // the same
  #include "dictionary.h"
#endif

/* ---------------------------------------------------------------------------------------------- 
    Objetivo:   Recebe o nome de uma tabela e engloba as funções leObjeto() e leSchema().
    Parametros: Nome da Tabela, Objeto da Tabela e tabela.
    Retorno:    tp_table
   ---------------------------------------------------------------------------------------------*/
tp_table *abreTabela(char *nomeTabela, struct fs_objects *objeto, tp_table **tabela) {
    *objeto     = leObjeto(nomeTabela);
    *tabela     = leSchema(*objeto);

    return *tabela;
}
// Se foram especificadas colunas no *s_insert, verifica se elas existem no esquema.
int allColumnsExists(rc_insert *s_insert, table *tabela) {
	int i;
	if (!s_insert->columnName) return 0;

	for (i = 0; i < s_insert->N; i++)
		if (retornaTamanhoTipoDoCampo(s_insert->columnName[i], tabela) == 0) {
			printf("ERROR: column \"%s\" of relation \"%s\" does not exist.\n", s_insert->columnName[i], tabela->nome);
			return 0;
		}

	return 1;
}
////
int typesCompatible(char table_type, char insert_type) {
	return (table_type == 'D' && insert_type == 'I')
		|| (table_type == 'D' && insert_type == 'D')
		|| (table_type == 'I' && insert_type == 'I')
		|| (table_type == 'S' && insert_type == 'S')
		|| (table_type == 'S' && insert_type == 'C')
		|| (table_type == 'C' && insert_type == 'C')
		|| (table_type == 'C' && insert_type == 'S');
}
////
// Busca o tipo do valor na inserção *s_insert do valor que irá para *columnName
// Se não existe em *s_insert, assume o tipo do esquema já que não receberá nada.
char getInsertedType(rc_insert *s_insert, char *columnName, table *tabela) {
	int i;
	char noValue;
	for (i = 0; i < s_insert->N; i++) {
		if (objcmp(s_insert->columnName[i], columnName) == 0) {
			return s_insert->type[i];
		}
	}

	noValue = retornaTamanhoTipoDoCampo(columnName, tabela);

	return noValue;
}
// Busca o valor na inserção *s_insert designado à *columnName.
// Se não existe, retorna 0, 0.0 ou \0
char *getInsertedValue(rc_insert *s_insert, char *columnName, table *tabela) {
	int i;
	char tipo, *noValue;

	for (i = 0; i < s_insert->N; i++) {
		if (objcmp(s_insert->columnName[i], columnName) == 0) {
			return s_insert->values[i];
		}
	}

	tipo = retornaTamanhoTipoDoCampo(columnName, tabela);
	noValue = (char *)malloc(sizeof(char)*3);

	if (tipo == 'I') {
		noValue = "0";
	} else if (tipo == 'D') {
		noValue = "0.0";
	} else
		noValue[0] = '\0';

	return noValue;
}
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Inicializa os atributos necessários para a verificação de FK e PK.
    Parametros: Objeto da tabela, Tabela, Buffer e nome da tabela.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
   ---------------------------------------------------------------------------------------------*/

int iniciaAtributos(struct fs_objects *objeto, tp_table **tabela, tp_buffer **bufferpool, char *nomeT){


    *objeto     = leObjeto(nomeT);
    *tabela     = leSchema(*objeto);
    *bufferpool = initbuffer();

    if(*tabela == ERRO_ABRIR_ESQUEMA) {
        return ERRO_DE_PARAMETRO;
    }

    if(*bufferpool == ERRO_DE_ALOCACAO)
        return ERRO_DE_PARAMETRO;

    return SUCCESS;
}
////
int verifyFK(char *tableName, char *column){
    if(verificaNomeTabela(tableName) == 1){
        struct fs_objects objeto = leObjeto(tableName);
        tp_table *esquema = leSchema(objeto);
		if(esquema == ERRO_ABRIR_ESQUEMA){
            printf("ERROR: cannot create schema.\n");
            return 0;
        }

        for(; esquema != NULL; esquema = esquema->next){
            if(objcmp(esquema->nome, column) == 0){
                if(esquema->chave == PK){
                    return 1;
                }
            }
        }
    }
	return 0;
}

////////
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Gera as verificações em relação a chave FK.
    Parametros: Nome da Tabela, Coluna C, Nome do Campo, Valor do Campo, Tabela Apontada e Atributo Apontado.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
                ERRO_CHAVE_ESTRANGEIRA
   ---------------------------------------------------------------------------------------------*/

int verificaChaveFK(char *nomeTabela,column *c, char *nomeCampo, char *valorCampo, char *tabelaApt, char *attApt){
    int x,j, erro, page;
    char str[20];
    char dat[5] = ".dat";
    struct fs_objects objeto;
    tp_table *tabela;
    tp_buffer *bufferpoll;
    column *pagina = NULL;

    strcpylower(str, tabelaApt);
    strcat(str, dat);              //Concatena e junta o nome com .dat

    erro = existeAtributo(nomeTabela, c);
    /*if(erro != SUCCESS )
        return ERRO_DE_PARAMETRO;*/

    //if(existeAtributo(tabelaApt, c))
        //return ERRO_CHAVE_ESTRANGEIRA;

    if(iniciaAtributos(&objeto, &tabela, &bufferpoll, tabelaApt) != SUCCESS) {
        free(bufferpoll);
        free(tabela);
        return ERRO_DE_PARAMETRO;
    }


    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, tabela, objeto);

    for (page = 0; page < QTD_PAGINAS; page++) {
        if (pagina) free(pagina);
        pagina = getPage(bufferpoll, tabela, objeto, page);
        if (!pagina) break;

        for(j = 0; j < objeto.qtdCampos * bufferpoll[page].nrec; j++){
            if (pagina[j].nomeCampo) {
                if(objcmp(pagina[j].nomeCampo, attApt) == 0){

                    if(pagina[j].tipoCampo == 'S'){
                        if(objcmp(pagina[j].valorCampo, valorCampo) == 0){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return SUCCESS;
                        }
                    } else if(pagina[j].tipoCampo == 'I'){
                        int *n = (int *)&pagina[j].valorCampo[0];
                        if(*n == atoi(valorCampo)){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return SUCCESS;
                        }
                    } else if(pagina[j].tipoCampo == 'D'){
                        double *nn = (double *)&pagina[j].valorCampo[0];

                        if(*nn == atof(valorCampo)){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return SUCCESS;
                        }
                    } else if(pagina[j].tipoCampo == 'C'){
                        if(pagina[j].valorCampo == valorCampo){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return SUCCESS;
                        }
                    } else {
                        free(pagina);
                        free(bufferpoll);
                        free(tabela);
                        return ERRO_CHAVE_ESTRANGEIRA;
                    }
                }
            }
        }
    }

    if (pagina) free(pagina);
    free(bufferpoll);
    free(tabela);
    return ERRO_CHAVE_ESTRANGEIRA;
}
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Gera as verificações em relação a chave PK.
    Parametros: Nome da Tabela, Coluna C, Nome do Campo, Valor do Campo
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
                ERRO_CHAVE_PRIMARIA
   ---------------------------------------------------------------------------------------------*/

int verificaChavePK(char *nomeTabela, column *c, char *nomeCampo, char *valorCampo) {
    int j, x, erro, page;
    column *pagina = NULL;

    struct fs_objects objeto;
    tp_table *tabela;
    tp_buffer *bufferpoll;

    erro = existeAtributo(nomeTabela, c);
    if (erro != SUCCESS ) {
        free(bufferpoll);
        return ERRO_DE_PARAMETRO;
    }


    if (iniciaAtributos(&objeto, &tabela, &bufferpoll, nomeTabela) != SUCCESS) {
        free(bufferpoll);
        free(tabela);
        return ERRO_DE_PARAMETRO;
    }

    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, tabela, objeto);

    page = 0;
    for (page = 0; page < QTD_PAGINAS; page++) {
        if (pagina) free(pagina);
        pagina = getPage(bufferpoll, tabela, objeto, page);
        if (!pagina) break;

        for(j = 0; j < objeto.qtdCampos * bufferpoll[page].nrec; j++){
            if (pagina[j].nomeCampo) {
                if (objcmp(pagina[j].nomeCampo, nomeCampo) == 0) {
                    if (pagina[j].tipoCampo == 'S') {
                        if (objcmp(pagina[j].valorCampo, valorCampo) == 0){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    } else if (pagina[j].tipoCampo == 'I') {
                        int *n = (int *)&pagina[j].valorCampo[0];

                        if (*n == atoi(valorCampo)) {
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    } else if (pagina[j].tipoCampo == 'D'){
                        double *nn = (double *)&pagina[j].valorCampo[0];

                        if (*nn == atof(valorCampo)){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    } else if (pagina[j].tipoCampo == 'C'){
                        if (pagina[j].valorCampo == valorCampo){
                            free(pagina);
                            free(bufferpoll);
                            free(tabela);
                            return ERRO_CHAVE_PRIMARIA;
                        }
                    }
                }
            }
        }
    }

    if (pagina) free(pagina);
    free(bufferpoll);
    free(tabela);
    return SUCCESS;
}

/////
int finalizaInsert(char *nome, column *c){
    column *auxC, *temp;
    int i = 0, x = 0, t, erro, j = 0;
    FILE *dados;

    struct fs_objects objeto,dicio; // Le dicionario
    tp_table *auxT ; // Le esquema
    auxT = abreTabela(nome, &dicio, &auxT);

    table *tab     = (table *)malloc(sizeof(table));
    tp_table *tab2 = (tp_table *)malloc(sizeof(struct tp_table));
    memset(tab2, 0, sizeof(tp_table));

    tab->esquema = abreTabela(nome, &objeto, &tab->esquema);
    tab2 = procuraAtributoFK(objeto);

    for(j = 0, temp = c; j < objeto.qtdCampos && temp != NULL; j++, temp = temp->next){
        switch(tab2[j].chave){
            case NPK:
                erro = SUCCESS;
                break;

            case PK:
                erro = verificaChavePK(nome, temp , temp->nomeCampo, temp->valorCampo);
                if (erro == ERRO_CHAVE_PRIMARIA){
                    printf("ERROR: duplicate key value violates unique constraint \"%s_pkey\"\nDETAIL:  Key (%s)=(%s) already exists.\n", nome, temp->nomeCampo, temp->valorCampo);

					free(auxT); // Libera a memoria da estrutura.
					//free(temp); // Libera a memoria da estrutura.
					free(tab); // Libera a memoria da estrutura.
					free(tab2); // Libera a memoria da estrutura.
                    return ERRO_CHAVE_PRIMARIA;
                }

                break;

            case FK:
                if (tab2[j].chave == 2 && strlen(tab2[j].attApt) != 0 && strlen(tab2[j].tabelaApt) != 0){

                    erro = verificaChaveFK(nome, temp, tab2[j].nome, temp->valorCampo, tab2[j].tabelaApt, tab2[j].attApt);

                    if (erro != SUCCESS){
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


    if (erro == ERRO_CHAVE_ESTRANGEIRA){
        printf("ERROR: unknown foreign key error.\n");

		free(auxT); // Libera a memoria da estrutura.
		free(temp); // Libera a memoria da estrutura.
        free(tab); // Libera a memoria da estrutura.
		free(tab2); // Libera a memoria da estrutura.
        return ERRO_CHAVE_ESTRANGEIRA;
    }

    if (erro == ERRO_CHAVE_PRIMARIA){
        printf("ERROR: unknown primary key error.\n");

		free(auxT); // Libera a memoria da estrutura.
		free(temp); // Libera a memoria da estrutura.
        free(tab); // Libera a memoria da estrutura.
		free(tab2); // Libera a memoria da estrutura.
        return ERRO_CHAVE_PRIMARIA;
    }
    if (erro == ERRO_DE_PARAMETRO) {
        printf("ERROR: invalid parameter.\n");
		free(auxT); // Libera a memoria da estrutura.
		free(temp); // Libera a memoria da estrutura.
        free(tab); // Libera a memoria da estrutura.
		free(tab2); // Libera a memoria da estrutura.
        return ERRO_DE_PARAMETRO;
    }

    char directory[TAMANHO_NOME_BANCO*2];
    strcpy(directory, connected.db_directory);
    strcat(directory, dicio.nArquivo);

    if((dados = fopen(directory,"a+b")) == NULL){
        printf("ERROR: cannot open file.\n");
		free(auxT); // Libera a memoria da estrutura.
		free(temp); // Libera a memoria da estrutura.
        free(tab); // Libera a memoria da estrutura.
		free(tab2); // Libera a memoria da estrutura.
        return ERRO_ABRIR_ARQUIVO;

	}

    for(auxC = c, t = 0; auxC != NULL; auxC = auxC->next, t++){
        if (t >= dicio.qtdCampos)
            t = 0;

        if (auxT[t].tipo == 'S'){ // Grava um dado do tipo string.

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

        } else if (auxT[t].tipo == 'I'){ // Grava um dado do tipo inteiro.
            i = 0;
            while (i < strlen(auxC->valorCampo)){
                if (auxC->valorCampo[i] < 48 || auxC->valorCampo[i] > 57){
                    printf("ERROR: column \"%s\" expectet integer.\n", auxC->nomeCampo);
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

            sscanf(auxC->valorCampo,"%d",&valorInteiro);
            fwrite(&valorInteiro,sizeof(valorInteiro),1,dados);

        } else if (auxT[t].tipo == 'D'){ // Grava um dado do tipo double.
            x = 0;
            while (x < strlen(auxC->valorCampo)){
                if((auxC->valorCampo[x] < 48 || auxC->valorCampo[x] > 57) && (auxC->valorCampo[x] != 46)){
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
        }
        else if (auxT[t].tipo == 'C'){ // Grava um dado do tipo char.

            if (strlen(auxC->valorCampo) > (sizeof(char))) {
                printf("ERROR: column \"%s\" expect char.\n", auxC->nomeCampo);
				free(tab); // Libera a memoria da estrutura.
				free(tab2); // Libera a memoria da estrutura.
				free(auxT); // Libera a memoria da estrutura.
				free(temp); // Libera a memoria da estrutura.
				fclose(dados);
                return ERRO_NO_TIPO_CHAR;
            }
            char valorChar = auxC->valorCampo[0];
            fwrite(&valorChar,sizeof(valorChar),1,dados);
        }

    }

    fclose(dados);
    free(tab); // Libera a memoria da estrutura.
	free(tab2); // Libera a memoria da estrutura.
    free(auxT); // Libera a memoria da estrutura.
    free(temp); // Libera a memoria da estrutura.
    return SUCCESS;
}

/* insert: Recebe uma estrutura rc_insert e valida os tokens encontrados pela interface().
 *         Se os valores forem válidos, insere um novo valor.
 */
void insert( rc_insert * s_insert ) {
	int i;
	table *tabela = (table *)malloc(sizeof(table));
	tabela->esquema = NULL;
	memset(tabela, 0, sizeof(table));
	column *colunas = NULL;
	tp_table *esquema = NULL;
	struct fs_objects objeto;
	memset(&objeto, 0, sizeof(struct fs_objects));
	char  flag=0;

	abreTabela(s_insert->objName, &objeto, &tabela->esquema); //retorna o esquema para a insere valor
	strcpylower(tabela->nome, s_insert->objName);

	if(s_insert->columnName != NULL) {
		if (allColumnsExists(s_insert, tabela)) {
			for (esquema = tabela->esquema; esquema != NULL; esquema = esquema->next) {
				if(typesCompatible(esquema->tipo,getInsertedType(s_insert, esquema->nome, tabela))) {
					colunas = insereValor(tabela, colunas, esquema->nome, getInsertedValue(s_insert, esquema->nome, tabela));
				} else {
					printf("ERROR: data type invalid to column '%s' of relation '%s' (expected: %c, received: %c).\n", esquema->nome, tabela->nome, esquema->tipo, getInsertedType(s_insert, esquema->nome, tabela));
					flag = 1;
				}
			}
		} else {
			flag = 1;
		}
	} else {
		if (s_insert->N == objeto.qtdCampos) {
			for(i=0; i < objeto.qtdCampos; i++) {

				if(s_insert->type[i] == 'S' && tabela->esquema[i].tipo == 'C') {
					printf( "\nWARNING: Attempted to write value of type \'string\' to attribute of type \'char\'.\n" );
					printf( "String \'%s\' will be truncated to \'%c\'\n", s_insert->values[i], s_insert->values[i][0] );
					s_insert->values[i][1] = '\0';
					s_insert->type[i] = 'C';
				}

				if(s_insert->type[i] == 'I' && tabela->esquema[i].tipo == 'D') {

					s_insert->type[i] = 'D';
				}

				if(s_insert->type[i] == tabela->esquema[i].tipo) {
					colunas = insereValor(tabela, colunas, tabela->esquema[i].nome, s_insert->values[i]);
				} else {
					printf("ERROR: data type invalid to column '%s' of relation '%s' (expected: %c, received: %c).\n", tabela->esquema[i].nome, tabela->nome, tabela->esquema[i].tipo, s_insert->type[i]);
					flag=1;
				}
			}
		} else {
			printf( "ERROR: INSERT has more expressions than target columns.\n" );
			flag = 1;
		}
	}

	if( !flag ) {
		if( finalizaInsert(s_insert->objName, colunas) == SUCCESS ) {
			printf( "INSERT 0 1\n" );
		}
	}

	//freeTp_table(&esquema, objeto.qtdCampos);
	free(esquema);
	freeColumn(colunas);
	freeTable(tabela);
}


///////////////
void imprime(char nomeTabela[]) {

    int j,erro, x, p, cont=0;
    struct fs_objects objeto;

    if(!verificaNomeTabela(nomeTabela)){
        printf("\nERROR: relation \"%s\" was not found.\n\n\n", nomeTabela);
        return;
    }

    objeto = leObjeto(nomeTabela);

    tp_table *esquema = leSchema(objeto);

    if(esquema == ERRO_ABRIR_ESQUEMA){
        printf("ERROR: schema cannot be created.\n");
        free(esquema);
        return;
    }

    tp_buffer *bufferpoll = initbuffer();

    if(bufferpoll == ERRO_DE_ALOCACAO){
        free(bufferpoll);
        free(esquema);
        printf("ERROR: no memory available to allocate buffer.\n");
        return;
    }

    erro = SUCCESS;
    for(x = 0; erro == SUCCESS; x++)
        erro = colocaTuplaBuffer(bufferpoll, x, esquema, objeto);

    int ntuples = --x;
	p = 0;
	while(x){
	    column *pagina = getPage(bufferpoll, esquema, objeto, p);
	    if(pagina == ERRO_PARAMETRO){
            printf("ERROR: could not open the table.\n");
            free(bufferpoll);
            free(esquema);
            return;
	    }

	    if(!cont) {
	        for(j=0; j < objeto.qtdCampos; j++){
	            if(pagina[j].tipoCampo == 'S')
	                printf(" %-20s ", pagina[j].nomeCampo);
	        	else
	                printf(" %-10s ", pagina[j].nomeCampo);
	            if(j<objeto.qtdCampos-1)
	            	printf("|");
	        }
	        printf("\n");
	        for(j=0; j < objeto.qtdCampos; j++){
	            printf("%s",(pagina[j].tipoCampo == 'S')? "----------------------": "------------");
	            if(j<objeto.qtdCampos-1)
	            	printf("+");
	        }
	        printf("\n");
	    }
	    cont++;
		for(j=0; j < objeto.qtdCampos*bufferpoll[p].nrec; j++){
        	if(pagina[j].tipoCampo == 'S')
            	printf(" %-20s ", pagina[j].valorCampo);
        	else if(pagina[j].tipoCampo == 'I'){
            	int *n = (int *)&pagina[j].valorCampo[0];
            	printf(" %-10d ", *n);
        	} else if(pagina[j].tipoCampo == 'C'){
            	printf(" %-10c ", pagina[j].valorCampo[0]);
        	} else if(pagina[j].tipoCampo == 'D'){
            	double *n = (double *)&pagina[j].valorCampo[0];
    	        printf(" %-10f ", *n);
        	}
            if(j>=1 && ((j+1)%objeto.qtdCampos)==0)
            	printf("\n");
        	else
        		printf("|");
    	}
    	x-=bufferpoll[p++].nrec;
    }
    printf("\n(%d %s)\n\n",ntuples,(1>=ntuples)?"row": "rows");

    free(bufferpoll);
    free(esquema);
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
		strcpy( temp->nome, t->columnName[i] );
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

