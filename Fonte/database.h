/* createDB: Recebe um nome para criar um banco de dados
 *           cria o banco, caso ainda não exista
 */
void createDB( const char * );

/* connectDB: Recebe o nome de um banco de dadados
 *            conecta-se com o banco, caso exista
 */
char connectDB( const char * );

/* dbInit: cria o banco de dados padrão
 *         com o nome "ibetres", caso não exista
 */
int dbInit( const char *db );

/* dropDatabase: Recebe o nome de um banco de dados
 *               deleta o banco caso o banco exista
 *               e o usuário esteja conectado em outro banco
 */
void dropDatabase( const char * );

/* showDB: lista todos os bancos do SGBD "\l"
 *
 */
void showDB();


