
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_ENV
#define TRACE_PUT_ENV
#endif

/*
   STATUS Xw_get_env (symbol,value,length):
   char *symbol			Symbol string to read
   char *value			return Symbol value to fill
   int  length			Value max length in chars

	Get external Symbol value (Set by setenv UNIX)

	Returns ERROR if No symbol exist
		      or Length is too small for value
	Returns SUCCESS if Successful      

   STATUS Xw_put_env (symbol,value):
   char *symbol			Symbol string to write
   char *value			Symbol value to write

	Put external Symbol value (Set by putenv UNIX)

	Returns ERROR if symbol is empty 
	Returns SUCCESS if Successful      
*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_env (const char* symbol,char* value,int length)
#else
XW_STATUS Xw_get_env (symbol,value,length)
const char *symbol;
char *value ;
int length ;
#endif /*XW_PROTOTYPE*/
{
char *string ;

    	if( !(string = getenv (symbol)) ) {
	  if( Xw_get_trace() > 1 )
		printf("*UNDEFINED SYMBOL*Xw_get_env('%s',...)\n",symbol);
	  return (XW_ERROR);
	}

	if( strlen(string) >= (unsigned int ) length ) return (XW_ERROR) ;

	strcpy(value,string) ;

#ifdef  TRACE_GET_ENV
if( Xw_get_trace() > 1 ) {
    printf (" '%s' = Xw_get_env('%s','%s',%d)\n",value,symbol,string,length) ;
}
#endif

	return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_put_env (char* symbol,char* value)
#else
XW_STATUS Xw_put_env (symbol,value)
char *symbol,*value ;
#endif /*XW_PROTOTYPE*/
{
char *string ;

	if( !strlen(symbol) ) return (XW_ERROR) ;

	string = (char*) malloc(strlen(symbol) + strlen(value) + 4) ;

	sprintf(string,"%s=%s",symbol,value) ;

	if( putenv(string) ) return (XW_ERROR) ;

#ifdef  TRACE_PUT_ENV
if( Xw_get_trace() > 1 ) {
    printf (" Xw_put_env('%s','%s')\n",symbol,value) ;
}
#endif

	return (XW_SUCCESS);
}
