#define FPARSER 1
////
/* FUNÇÕES AUXILIARES DO INTERPRETADOR DE COMANDOS (yacc + lex)
 * O yacc vai chamando essas funções conforme vai identificando os tokens.
 * Essas funcções irão preencher uma estrutura, ou parte dela, e chamarão
 * as funções do banco de dados para executar as operações.
 */
#define OP_INVALID			0
#define OP_INSERT 			1
#define OP_CREATE_TABLE 	2
#define OP_DROP_TABLE 		3
#define OP_CREATE_DATABASE 	4
#define OP_DROP_DATABASE 	5
#define OP_SELECT_ALL 		6
#define OP_SELECT			7
#define OP_AND				8
#define OP_OR				9

/* Estrutura global que guarda as informações obtidas pelo yacc
 * na identificação dos tokens
 */
extern rc_insert GLOBAL_DATA;

/* Estrutura auxiliar do reconhecedor.
 */
extern rc_parser GLOBAL_PARSER;

/* Funcções do yacc
 */
int yyparse();
int yylex();
int yylex_destroy();
extern int  yylineno;

/* Função padrão do yacc chamada quando um erro sintático é
 * identificado.
 */
void yyerror( char *s, ... );

/* Imprime o erro caso o comando seja inválido
 */
void invalidCommand( const char *command );

/* Mensagem exibida quando o usuário tenta executar um comando
 * sem estar conectado à nenhuma base de dados.
 */
void notConnected( void );

/* Estabelece a conexão com uma base de dados. 
 * Se conectar, define os parâmetros globais para a exibição do console 
 * e retorna 1. Caso contrário, retorna 0.
 */
int connect( const char *nome );


/* Define o nome do banco de dados ou da tabela identificado
 * no token
 */
void setObjName( char **nome );

/* Adiciona na estrutura uma nova coluna identificada no
 * comando INSERT.
 */
void setColumnInsert( char **nome );

/* Define o valor que será inserido, identificado no token
 * do comando INSERT
 */
void setValueInsert( char *nome, char type );

/* Adiciona na estrutura uma nova coluna identificada no
 * comando CREATE TABLE, isso inicializará outras variáveis
 * também (type, attribute, ...)
 */
void setColumnCreate( char **nome );

/* Define o tipo da coluna identificada no comando CREATE
 * TABLE.
 */
void setColumnTypeCreate(char type);

/* Define o tamanho da coluna identificada no comando
 * CREATE TABLE
 */
void setColumnSizeCreate( char *size );

/* Define a coluna identificada no comando CREATE TABLE
 * como PRIMARY KEY
 */
void setColumnPKCreate( void );

/* Define a tabela que a coluna identificada no CREATE TABLE
 * irá referenciar
 */
void setColumnFKTableCreate( char **nome );

/* Define a coluna da tabela que a coluna identificada
 * no CREATE TABLE irá referenciar.
 */
void setColumnFKColumnCreate( char **nome );

/* Reinicializa todas as propriedades da estrutura que armazena
 * os dados identificados pelos tokens SQL.
 */
void clearGlobalStructs( void );

/* Define a operação que será realizada pelo identificador através
 * dos tokens iniciais.
 */
void setMode( const char mode );

/* interface: Parte gerada pelo yacc, presente no arquivo yacc.y
 *            É responsável por fazer a conexão ao banco e chamar yyparse()
 *            para fazer a validação dos comandos enviados pelo usuário.
 */
void interface( int argc, char ** argv );

// Esta função carrega o script passado no comando \i <nome_script.sql>
FILE * loadScript( char * scriptPath );

// Inicializa algumas variáveis globais
void initGlobalStructs( void );