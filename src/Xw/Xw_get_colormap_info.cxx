
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_COLORMAP_INFO
#endif

/*
   XW_STATUS Xw_get_colormap_info (acolormap,visual,tclass,visualid,
				mcolor,scolor,ucolor,dcolor,fcolor):
   XW_EXT_COLORMAP *acolormap
   Aspect_Handle **visual	Return Visual structure address
   Xw_TypeOfVisual *tclass	Return Visual Class of the creating colormap
   int *visualid	Return the visual identificator
   int *mcolor		Return the maximum color number of the colormap
   int *scolor		Return the System Color base pixel used in the colormap
   int *ucolor		Return the User color number used in the colormap
   int *dcolor		Return the User color number defined in the colormap
   int *fcolor		Return the First Free User color index in the colormap

   NOTE than if fcolor is < 0 No more free colors exist in the colormap
	Returns ERROR if the Extended Colormap is not defined
	Returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_colormap_info (void* acolormap,
				Aspect_Handle** visual,
				Xw_TypeOfVisual* tclass,
				int* visualid,
				int* mcolor,int* scolor,
				int* ucolor,int* dcolor,int* fcolor)
#else
XW_STATUS Xw_get_colormap_info (acolormap,visual,tclass,visualid,mcolor,scolor,ucolor,dcolor,fcolor)
void *acolormap;
Aspect_Handle **visual ;
Xw_TypeOfVisual *tclass ;
int *visualid;
int *mcolor,*scolor,*ucolor,*dcolor,*fcolor ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap;
int i ;

	if( !Xw_isdefine_colormap(pcolormap) ) {
	    /*ERROR*Bad EXT_COLORMAP Address*/
	    Xw_set_error(42,"Xw_get_colormap_info",pcolormap) ;
	    return (XW_ERROR) ;
	}

	*visual = (Aspect_Handle*) _CVISUAL ;
	*tclass = (Xw_TypeOfVisual) _CCLASS ;
	*visualid = _CVISUAL->visualid;
	*mcolor = pcolormap->maxhcolor ;
	*scolor = _CINFO.base_pixel ;
	*ucolor = pcolormap->maxucolor ;
	*dcolor = 0 ;
	*fcolor = -1 ;
	for( i=0 ; i< *ucolor ; i++ ) {
	    if( (pcolormap->define[i] != FREECOLOR) &&
			 (pcolormap->define[i] != HIGHCOLOR)) (*dcolor)++ ;
	    else if( *fcolor < 0 ) *fcolor = i ;
	}
	
#ifdef  TRACE_GET_COLORMAP_INFO
if( Xw_get_trace() ) {
    printf (" Xw_get_colormap_info(%lx,%lx,%d,%d,%d,%d,%d,%d,%d)\n",
	(long ) pcolormap,(long ) *visual,*tclass,*visualid,*mcolor,*scolor,*ucolor,*dcolor,*fcolor) ;
}
#endif

	return (XW_SUCCESS);
}
