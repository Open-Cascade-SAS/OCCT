// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
