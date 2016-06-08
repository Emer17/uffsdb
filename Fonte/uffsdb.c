#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifndef FMACROS
   #include "macros.h"
#endif
#ifndef FTYPES
   #include "types.h"
#endif
#ifndef FMISC
   #include "misc.h"
#endif
#ifndef FDICTIONARY
   #include "dictionary.h"
#endif
#ifndef FSQLCOMMANDS
   #include "sqlcommands.h"
#endif
#ifndef FDATABASE
   #include "database.h"
#endif

#include "interface/y.tab.h"
#include "interface/parser.h"

db_connected connected;

int main( int argc, char ** argv ){
    if( dbInit( NULL ) ) {		
		interface( argc, argv );
	} else {
		puts( "Erro ao inicializar banco" );
	}      
    return 0;
}
