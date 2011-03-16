
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_TYPE
#endif

/*
   XW_STATUS Xw_isdefine_type (atypemap,index):
   XW_EXT_TYPEMAP *atypemap
   int index			Line type index


	Returns XW_ERROR if BadType Index or type is not defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_type (void *atypemap,int index)
#else
XW_STATUS Xw_isdefine_type (atypemap,index)
void *atypemap;
int index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_TYPEMAP *ptypemap = (XW_EXT_TYPEMAP*)atypemap;
XW_STATUS status = XW_ERROR ;

	if ( index ) {
	   if ( ptypemap && (index < ptypemap->maxtype) &&
				ptypemap->types[index] ) {
	       status = XW_SUCCESS ;
	   } 
	} else status = XW_SUCCESS ;


#ifdef  TRACE_ISDEFINE_TYPE
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_type(%lx,%d)\n",status,(long ) ptypemap,index) ;
}
#endif

	return (status);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_typeindex (XW_EXT_TYPEMAP *atypemap,int index)
#else
XW_STATUS Xw_isdefine_typeindex (atypemap,index)
XW_EXT_TYPEMAP *atypemap;
int index ;
#endif /*XW_PROTOTYPE*/
/*
	Verify type index range Only
*/
{
XW_EXT_TYPEMAP *ptypemap = (XW_EXT_TYPEMAP*)atypemap;
XW_STATUS status = XW_ERROR ;

	if( ptypemap && (index > 0) && (index < ptypemap->maxtype) ) {
	   status = XW_SUCCESS ;
	}

#ifdef  TRACE_ISDEFINE_TYPE
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_typeindex(%lx,%d)\n",status,(long ) ptypemap,index) ;
}
#endif

	return (status) ;
}
