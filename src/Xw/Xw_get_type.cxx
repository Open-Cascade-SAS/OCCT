
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_TYPE
#endif

/*
   XW_STATUS Xw_get_type (atypemap,index,length,desc):
   XW_EXT_TYPEMAP *atypemap
   int index			Line type index
   int *length			Returned Descriptor Length
   float *desc			Returned Type descriptor

	Get Line Type Extended typemap TYPE descriptor from index . 
	NOTE than length can be NULL for Solid line

	Returns XW_ERROR if BadType Index
		      or type is not defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_type (void *atypemap,int index,int *length,float *desc)
#else
XW_STATUS Xw_get_type (atypemap,index,length,desc)
void *atypemap;
int index ;
int *length ;
float *desc ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_TYPEMAP *ptypemap = (XW_EXT_TYPEMAP*)atypemap;
Screen *screen ;
unsigned char *ptype ;
int i ;

	if ( !Xw_isdefine_type(ptypemap,index) ) {
	    /*ERROR*Bad Defined Type*/
	    Xw_set_error(50,"Xw_get_type",&index) ;
	    return (XW_ERROR) ;
	}

	screen = ScreenOfDisplay(_TDISPLAY,DefaultScreen(_TDISPLAY)) ;
	*length = 0 ;
	if(( ptype = ptypemap->types[index] )) {
	    *length = strlen((char*)ptype) ;
	    for( i=0 ; i<*length ; i++ ) {
		desc[i] = (float)ptype[i]*WidthMMOfScreen(screen)/
						(float)WidthOfScreen(screen) ;
	    }
	} 

#ifdef  TRACE_GET_TYPE
if( Xw_get_trace() > 1 ) {
    printf (" Xw_get_type(%lx,%d,%d,{",(long ) ptypemap,index,*length) ;
    for ( i=0 ; i<*length-1 ; i++ ) printf("%f,",desc[i]) ;
    printf("%f})\n",desc[*length-1]) ;
}
#endif

	return (XW_SUCCESS);
}
