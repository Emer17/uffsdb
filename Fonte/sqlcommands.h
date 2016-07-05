#define FSQLCOMMANDS 1   // flag para garantir a não reinclusão

#ifndef FMACROS // garante que macros.h não seja reincluída
   #include "macros.h"
#endif
///
#ifndef FTYPES // garante que types.h não seja reincluída
  #include "types.h"
#endif

/* insert: Recebe uma estrutura rc_insert e valida os tokens encontrados pela interface().
 *         Se os valores forem válidos, insere um novo valor.
 */
void insert(rc_insert *);

// Se foram especificadas colunas no *s_insert, verifica se elas existem no esquema
int allColumnsExists(rc_insert *, table *);

// Busca o tipo do valor na inserção *s_insert do valor que irá para *columnName
// Se não existe em *s_insert, assume o tipo do esquema já que não receberá nada.
char getInsertedType(rc_insert *, char *c, table *);

// Busca o valor na inserção *s_insert designado à *columnName.
// Se não existe, retorna 0, 0.0 ou \0
char *getInsertedValue(rc_insert *, char *, table *);

int typesCompatible( const char, const char );
/*
    Esta função finaliza a inserção de valores em uma tabela. Assume que o usuário entrou com todos
    os campos de uma tupla completa.
    Retorna:
        -> ERRO_ABRIR_ARQUIVO, quando ocorreu um erro ao abrir o arquivo fs_object.dat ou fs_schema.dat;
        -> ERRO_NO_TAMANHO_STRING, quando ocorreu um erro no tamanho da string inserida;
        -> ERRO_NOME_CAMPO, quando o nome do campo passado na estrutura;
        -> ERRO_NO_TIPO_INTEIRO, quando o valor passado não é um inteiro;
        -> ERRO_NO_TIPO_DOUBLE, quando o valor passado não é um double;
        -> ERRO_NO_TIPO_CHAR, quando o valor passado é maior que 1byte;
        -> SUCCESS, quando a função teve sucesso em sua operação de inserção na tabela.
    *nome - Nome da tabela que vai ser inserido os valores da estrutura *c.
    *c - Estrutura com o valores que vão ser inseridos na tabela *nome.
*/
int finalizaInsert(char *, column *);
/* ----------------------------------------------------------------------------------------------
    Objetivo:   Utilizada para impressão de tabelas.
    Parametros: Nome da tabela (char).
    Retorno:    void.
   ---------------------------------------------------------------------------------------------*/
void consulta( const char [] );
/* ----------------------------------------------------------------------------------------------
    
	Objetivo:   Função para exclusão de tabelas.
    Parametros: Nome da tabela (char).
    Retorno:    INT
                SUCCESS,
                ERRO_REMOVER_ARQUIVO_OBJECT,
                ERRO_REMOVER_ARQUIVO_SCHEMA,
                ERRO_LEITURA_DADOS.
   ---------------------------------------------------------------------------------------------*/
int excluirTabela(char *);

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Copia todas as informações menos a tabela do objeto, que será removida.
    Parametros: Objeto que será removido do schema.
    Retorno:    INT
                SUCCESS,
                ERRO_REMOVER_ARQUIVO_SCHEMA
   ---------------------------------------------------------------------------------------------*/
int procuraSchemaArquivo(struct fs_objects);

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Gera as verificações em relação a chave FK.
    Parametros: Nome da Tabela, Coluna C, Nome do Campo, Valor do Campo, Tabela Apontada e Atributo Apontado.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
                ERRO_CHAVE_ESTRANGEIRA
   ---------------------------------------------------------------------------------------------*/
int verificaChaveFK(char *, column *, char *, char *, char *, char *);

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Gera as verificações em relação a chave pK.
    Parametros: Nome da Tabela, Coluna C, Nome do Campo, Valor do Campo
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
                ERRO_CHAVE_PRIMARIA
   ---------------------------------------------------------------------------------------------*/
int verificaChavePK(char *, column *, char *, char *);

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Inicializa os atributos necessários para a verificação de FK e PK.
    Parametros: Objeto da tabela, Tabela, Buffer e nome da tabela.
    Retorno:    INT
                SUCCESS,
                ERRO_DE_PARAMETRO,
   ---------------------------------------------------------------------------------------------*/
int iniciaAtributos(struct fs_objects *, tp_table **, tp_buffer **, char *);

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Recebe o nome de uma tabela e engloba as funções leObjeto() e leSchema().
    Parametros: Nome da Tabela, Objeto da Tabela e tabela.
    Retorno:    tp_table
   ---------------------------------------------------------------------------------------------*/
tp_table * abreTabela( const char *, struct fs_objects *, tp_table ** );

/* createTable: Recebe uma estrutura contendo os parâmetros do CREATE TABLE
 *              e cria uma tabela no banco corrente
 */
void createTable(rc_insert *);

int verifyFieldName(char **, int );

int verifyFK(char *, char *);

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Conta as colunas repetidas no comando SELECT. ( Ex: SELECT nome,nome retorna 2 )
    Parametros: Nenhum
    Retorno:    Valor inteiro que representa colunas repetidas
   ---------------------------------------------------------------------------------------------*/
int contaColunasRepetidas( void );

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Verifica se a coluna passada no comando SELECT existe na tabela passada após FROM
    Parametros: fs_objects( campo qtdCampos é utilizado ), tp_table( possuí o nome dos campos na tabela )
    Retorno:    Valor inteiro que representa colunas repetidas
   ---------------------------------------------------------------------------------------------*/
int existeColuna( const struct fs_objects *, const tp_table * );

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Compara 'a' e 'b' sobre o operador 'op'
    Parametros: Variaveis 'a' e 'b' do tipo DOUBLE, um valor ENUM para determinar o operador
    Retorno:    Resultado da operação realizada
   ---------------------------------------------------------------------------------------------*/
int comparaValoresNumericos( const double *, const double *, const enum where_operator );

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Compara 'a' e 'b' sobre o operador 'op'
    Parametros: Variaveis 'a' e 'b' do tipo CHAR *, um valor ENUM para determinar o operador
    Retorno:    Resultado da operação realizada
   ---------------------------------------------------------------------------------------------*/
int comparaValoresString( const char *, const char *, const enum where_operator );

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Armazenar na estrutura campos_container todas as tuplas resultantes do comando SELECT e WHERE
    Parametros: struct para armazenar tuplas, bufferpool, informação de tabela e campos
    Retorno:    SUCCESS ou ERRO
   ---------------------------------------------------------------------------------------------*/
int preencheCampos( struct campos_container *, tp_buffer *, const struct fs_objects *, tp_table * );

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Calcular a largura do terminal
    Parametros: 
    Retorno:    Um INT que representa a largura do terminal
   ---------------------------------------------------------------------------------------------*/
int terminalWidth( void );

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Calcular a altura do terminal
    Parametros: 
    Retorno:    Um INT que representa altura do terminal
   ---------------------------------------------------------------------------------------------*/
int terminalHeight( void );

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Calcular a largura do terminal
    Parametros: 
    Retorno:    A largura do terminal
   ---------------------------------------------------------------------------------------------*/
int calculaTamLinha( const struct campos_container * );

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Imprime os campos e tuplas contidos em campos_container na tela
    Parametros: struct com campos e tuplas, e um INT que representa o tamanho da linha do header
    Retorno:    Resultado da operação ( 1 sucesso, 0 falha )
   ---------------------------------------------------------------------------------------------*/
int imprime_tela( const struct campos_container *, const int );

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Imprime na tela ( a partir de um arquivo ) os campos e tuplas contidos em campos_container
    Parametros: struct com campos e tuplas, e um INT que representa o tamanho da linha do header
    Retorno:    Resultado da operação ( 1 sucesso, 0 falha )
   ---------------------------------------------------------------------------------------------*/
int imprime_arquivo( const struct campos_container *, const int );

/* ----------------------------------------------------------------------------------------------
    Objetivo:   Função que determina se a impressao ocorerrá por meio de um arquivo ou não
    Parametros: struct com campos e tuplas
    Retorno:    Resultado da operação ( 1 sucesso, 0 falha )
   ---------------------------------------------------------------------------------------------*/
int imprime_tabela( const struct campos_container * );


