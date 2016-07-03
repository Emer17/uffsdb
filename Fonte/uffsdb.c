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

#ifndef YY_YY_Y_TAB_H_INCLUDED
	#include "interface/y.tab.h"
#endif

#ifndef FPARSER
	#include "interface/parser.h"
#endif

db_connected connected;
db_options options;
select_data GLOBAL_SELECT;

int main( int argc, char ** argv ){
    if( dbInit( NULL ) ) {		
		interface( argc, argv );
	} else {
		puts( "Erro ao inicializar banco" );
	}      
    return 0;
}
