
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_WIDTHMAP_INFO
#endif

/*
   XW_STATUS Xw_get_widthmap_info (awidthmap,mwidth,uwidth,dwidth,fwidth):
   XW_EXT_WIDTHMAP *awidthmap
   int *mwidth		Return the maximum width number of the widthmap
   int *uwidth		Return the User width number used in the widthmap
   int *dwidth		Return the User width number defined in the widthmap
   int *fwidth		Return the First Free width index in the widthmap
   NOTE than if fwidth < 0 , No more Free width exist in the widthmap 

	Returns XW_ERROR if the Extended Widthmap is not defined
	Returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_widthmap_info (void *awidthmap,int *mwidth,int *uwidth,int *dwidth,int *fwidth)
#else
XW_STATUS Xw_get_widthmap_info (awidthmap,mwidth,uwidth,dwidth,fwidth)
void *awidthmap;
int *mwidth,*uwidth,*dwidth,*fwidth ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WIDTHMAP *pwidthmap = (XW_EXT_WIDTHMAP*)awidthmap;
  int i ;

	if( !Xw_isdefine_widthmap(pwidthmap) ) {
	    /*ERROR*Bad EXT_WIDTHMAP Address*/
	    Xw_set_error(53,"Xw_get_widthmap_info",pwidthmap) ;
	    return (XW_ERROR) ;
	}

	*mwidth = MAXWIDTH ;
	*uwidth = pwidthmap->maxwidth ;
	*dwidth = 1 ;
	*fwidth = -1 ;
	for( i=0 ; i< *uwidth ; i++ ) {
	    if( pwidthmap->widths[i] ) (*dwidth)++ ;
	    else if( i && *fwidth < 0 ) *fwidth = i ;
	}
	
#ifdef  TRACE_GET_WIDTHMAP_INFO
if( Xw_get_trace() ) {
    printf (" Xw_get_widthmap_info(%lx,%d,%d,%d,%d)\n",
			(long ) pwidthmap,*mwidth,*uwidth,*dwidth,*fwidth) ;
}
#endif

	return (XW_SUCCESS);
}
