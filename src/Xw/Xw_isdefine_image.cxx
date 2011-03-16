
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_IMAGE
#endif

/*
   XW_STATUS Xw_isdefine_image (aimage):
   XW_EXT_IMAGEDATA *aimage


	Returns XW_ERROR if Extended Image address is not properly defined
	Returns XW_SUCCESS if Successful      

*/
#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_image (void* aimage)
#else
XW_STATUS Xw_isdefine_image (aimage)
void* aimage;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*) aimage;
XW_STATUS status = XW_ERROR ;

	if( pimage && (pimage->type == IMAGE_TYPE) && _IIMAGE ) {
	    status = XW_SUCCESS ;
	}


#ifdef  TRACE_ISDEFINE_IMAGE
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_isdefine_image(%lx)\n",status,(long ) pimage) ;
}
#endif

	return (status);
}
