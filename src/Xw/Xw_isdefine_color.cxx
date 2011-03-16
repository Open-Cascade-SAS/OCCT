
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_COLOR
#endif

/*
   XW_STATUS Xw_isdefine_color(acolormap,index):
   XW_EXT_COLORMAP *acolormap	Colormap extension structure
   int index ;		Color index 0 >= x < MAXCOLOR

	Returns XW_ERROR if BadColor index or Color is not define
	Returns XW_SUCCESS if Color is defined

*/
#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_color (void* acolormap, int index)
#else
XW_STATUS Xw_isdefine_color (acolormap,index)
void *acolormap;
int index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XW_STATUS status = XW_ERROR ;
//int i ;

	if( pcolormap && ((index < 0) || 
		((index >= 0) && (index < pcolormap->maxcolor) &&
		    		(pcolormap->define[index]))) ) {
	    status = XW_SUCCESS ;
	}

#ifdef TRACE_ISDEFINE_COLOR
if( Xw_get_trace() > 2 ) {
    printf(" %d = Xw_isdefine_color(%lx,%d)\n",status,(long ) pcolormap,index) ;
}
#endif

	return (status);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_colorindex (XW_EXT_COLORMAP* acolormap, int index)
#else
XW_STATUS Xw_isdefine_colorindex (acolormap,index)
XW_EXT_COLORMAP *acolormap;
int index ;
#endif /*XW_PROTOTYPE*/
/*
	Verify Index range Only
*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XW_STATUS status = XW_ERROR ;

	if( pcolormap && (index >= 0) && (index < pcolormap->maxcolor) ) {
	    status = XW_SUCCESS ;
	}

#ifdef TRACE_ISDEFINE_COLOR
if( Xw_get_trace() > 2 ) {
    printf(" %d = Xw_isdefine_colorindex(%lx,%d)\n",status,(long ) pcolormap,index) ;
}
#endif

	return (status) ;
}
#ifdef XW_PROTOTYPE
XW_STATUS Xw_ifsystem_colorindex (XW_EXT_COLORMAP* acolormap, int index)
#else
XW_STATUS Xw_ifsystem_colorindex (acolormap,index)
XW_EXT_COLORMAP *acolormap;
int index ;
#endif /*XW_PROTOTYPE*/
/*
	Check if System color index
*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XW_STATUS status = XW_ERROR ;

	if( pcolormap && (index >= 0) && (index < pcolormap->maxcolor) &&
			(pcolormap->define[index] == SYSTEMCOLOR) ) {
	    status = XW_SUCCESS ;
	}

#ifdef TRACE_ISDEFINE_COLOR
if( Xw_get_trace() > 2 ) {
    printf(" %d = Xw_ifsystem_colorindex(%lx,%d)\n",status,(long ) pcolormap,index) ;
}
#endif

	return (status) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_ifimage_colorindex (XW_EXT_COLORMAP* acolormap, int index)
#else
XW_STATUS Xw_ifimage_colorindex (acolormap,index)
XW_EXT_COLORMAP *acolormap;
int index ;
#endif /*XW_PROTOTYPE*/
/*
	Check if image color index
*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XW_STATUS status = XW_ERROR ;

	if( pcolormap && (index >= 0) && (index < pcolormap->maxcolor) &&
			(pcolormap->define[index] == IMAGECOLOR) ) {
	    status = XW_SUCCESS ;
	}

#ifdef TRACE_ISDEFINE_COLOR
if( Xw_get_trace() > 2 ) {
    printf(" %d = Xw_ifimage_colorindex(%lx,%d)\n",status,(long ) pcolormap,index) ;
}
#endif

	return (status) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_ifbackground_colorindex (XW_EXT_COLORMAP* acolormap, int index)
#else
XW_STATUS Xw_ifbackground_colorindex (acolormap,index)
XW_EXT_COLORMAP *acolormap;
int index ;
#endif /*XW_PROTOTYPE*/
/*
	Check if background color index
*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XW_STATUS status = XW_ERROR ;

	if( pcolormap && ((index < 0) || 
		((index >= 0) && (index < pcolormap->maxcolor) &&
			(pcolormap->define[index] == BACKCOLOR))) ) {
	    status = XW_SUCCESS ;
	}

#ifdef TRACE_ISDEFINE_COLOR
if( Xw_get_trace() > 2 ) {
    printf(" %d = Xw_ifbackground_colorindex(%lx,%d)\n",status,(long ) pcolormap,index) ;
}
#endif

	return (status) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_ifhighlight_colorindex (XW_EXT_COLORMAP* acolormap, int index)
#else
XW_STATUS Xw_ifhighlight_colorindex (acolormap,index)
XW_EXT_COLORMAP *acolormap;
int index ;
#endif /*XW_PROTOTYPE*/
/*
	Check if highlight color index
*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XW_STATUS status = XW_ERROR ;

	if( pcolormap && (index >= 0) && (index < pcolormap->maxcolor) &&
			(pcolormap->define[index] == HIGHCOLOR) ) {
	    status = XW_SUCCESS ;
	}

#ifdef TRACE_ISDEFINE_COLOR
if( Xw_get_trace() > 2 ) {
    printf(" %d = Xw_ifhighlight_colorindex(%lx,%d)\n",status,(long ) pcolormap,index) ;
}
#endif

	return (status) ;
}
