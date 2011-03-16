
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ISDEFINE_FONT
#endif

/*
   XW_STATUS Xw_isdefine_font (afontmap,index):
   XW_EXT_FONTMAP *afontmap
   int index			font index

	Returns XW_ERROR if BadFont Index or font is not defined
	Returns XW_SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_font (void* afontmap,int index)
#else
XW_STATUS Xw_isdefine_font (afontmap,index)
void* afontmap;
int index;
#endif /*XW_PROTOTYPE*/

{
XW_EXT_FONTMAP *pfontmap = (XW_EXT_FONTMAP*)afontmap;
XW_STATUS status = XW_ERROR ;

	if ( index ) {
	    if ( pfontmap && (index < pfontmap->maxfont) &&
				 	pfontmap->fonts[index] ) {
	        status = XW_SUCCESS ;
	    }
	} else status = XW_SUCCESS ;

#ifdef  TRACE_ISDEFINE_FONT
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_font(%lx,%d)\n",status,(long ) pfontmap,index) ;
}
#endif

	return (status);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_isdefine_fontindex (XW_EXT_FONTMAP* afontmap,int index)
#else
XW_STATUS Xw_isdefine_fontindex (afontmap,index)
XW_EXT_FONTMAP* afontmap;
int index;
#endif /*XW_PROTOTYPE*/
/*
	Verify Range index only
*/
{
XW_EXT_FONTMAP *pfontmap = (XW_EXT_FONTMAP*)afontmap;
XW_STATUS status = XW_ERROR ;

	if ( pfontmap && (index > 0) && (index < pfontmap->maxfont) ) {
	    status = XW_SUCCESS ;
	}

#ifdef  TRACE_ISDEFINE_FONT
if( Xw_get_trace() > 2 ) {
    printf (" %d = Xw_isdefine_fontindex(%lx,%d)\n",status,(long ) pfontmap,index) ;
}
#endif

	return (status);
}
