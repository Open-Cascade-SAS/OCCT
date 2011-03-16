
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_TYPEMAP
#endif

/*
   XW_STATUS Xw_isdefine_typemap (atypemap):
   XW_EXT_TYPEMAP *atypemap


	Returns XW_ERROR if Extended Typemap address is not properly defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_typemap (void *atypemap)
#else
XW_STATUS Xw_isdefine_typemap (atypemap)
void *atypemap;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_TYPEMAP *ptypemap = (XW_EXT_TYPEMAP*)atypemap;
XW_STATUS status = XW_ERROR ;

	if( ptypemap && (ptypemap->type == TYPEMAP_TYPE) ) {
	    status = XW_SUCCESS ;
	}


#ifdef  TRACE_ISDEFINE_TYPEMAP
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_isdefine_typemap(%lx)\n",status,(long ) ptypemap) ;
}
#endif

	return (status);
}
