
int nbr_ret_func = 5 ;
PVOID_FUNCTION TABRETFUNC[5] ; 

#define nbrptrfunc 0
PVOID_FUNCTION TABFUNC[1] ;

#define nbrptr 3
void *TABPTR[nbrptr] ;

#define nbrval 1
int TABVAL[nbrval] ;

TABFUNC[0] = NULL ;
TABPTR[0] = NULL ;
TABVAL[0] = *KVERBOSEINIT ;
