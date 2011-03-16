/* PRO12499	OPTIMISATION & DEBUG GG_100398
//		Utiliser plutot une variable statique que allouer de
  		la memoire.
*/

#define IMP010200       /* GG
                        Add protection when NO extension is defined
                        and the path contains a '.'
*/

#include <Xw_Extension.h>
#include <string.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_FILENAME
#endif

/*
   char* Xw_get_filename (filename,extension):
   char *filename		Filename to translate 
   char *extension		File extension

	Translate Filename depending of environment and extension 

	Returns Updated Filename depending of environment and extension
	     or Returns NULL is translating can't be done      

	NOTE: Filename can be given under the forms :

		1) xxxxxx.ext
		2) xxxxxx		(.extension is added)
		3) /usr/..../xxxxxx.ext
		4) $XXXXX/yyyyyy.ext	($XXXXX symbol is get and insert  

*/
#define MAXSTRING 512
static char string1[MAXSTRING];
static char string2[MAXSTRING];

#ifdef XW_PROTOTYPE
char* Xw_get_filename (char* filename,const char* extension)
#else
char* Xw_get_filename (filename,extension)
char *filename ;
const char *extension ;
#endif /*XW_PROTOTYPE*/
{
char *pname,*pext,*pslash;

        if( !filename || strlen(filename) > MAXSTRING ) {
          printf (" *TOO BIG PATH*Xw_get_filename('%s','%s')\n",
							filename,extension) ;
          return (NULL) ;
        }

        pname = strcpy(string1,filename) ;

        if( *pname == '$' ) {
            pname = (char*) strchr(string1,'/') ;
            if( pname ) {               /* Filename is $XXXX/yyyyy */
		XW_STATUS status;
                *pname = '\0' ;
                status = Xw_get_env(&string1[1],string2,MAXSTRING) ;
                *pname = '/' ;
                if( status ) {
		  if( (strlen(string2) + strlen(pname) + 1) < MAXSTRING ) {
		    pname = strcat(string2,pname) ;
		  } else {
          	    printf (" *TOO BIG SYMBOL PATH*Xw_get_filename('%s','%s')\n",
							filename,extension) ;
		    return NULL;
		  }
		}
            }
        }

        pext = strrchr(pname,'.') ;
#ifdef IMP010200
        pslash = strrchr(pname,'/') ;
        if( pext && (pslash > pext) ) pext = NULL;
#endif

        if( !pext ) {                  /* Add file extension ".extension" */
	    if( (strlen(pname) + strlen(extension) + 2) < MAXSTRING ) {
                strcat(pname,".") ;
                strcat(pname,extension) ;
            } else {
          	printf (" *TOO BIG EXTENSION*Xw_get_filename('%s','%s')\n",
							filename,extension) ;
	        return NULL;
	    }
        }

#ifdef  TRACE_GET_FILENAME
if( Xw_get_trace() > 1 ) {
    printf (" '%s'= Xw_get_filename('%s','%s')\n",pname,filename,extension) ;
}
#endif

	return (pname);
}
