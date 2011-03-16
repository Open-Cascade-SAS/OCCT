
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_FONTMAP_INFO
#endif

/*
   STATUS Xw_get_fontmap_info (afontmap,mfont,ufont,dfont,ffont):
   XW_EXT_FONTMAP *afontmap
   int *mfont		Return the maximum font number of the fontmap
   int *ufont		Return the User font number used in the fontmap
   int *dfont		Return the User font number defined in the fontmap
   int *ffont		Return the First Free User font index in the fontmap
   NOTE than if ffont is 0 No more Free font exist in the fontmap

	Returns ERROR if the Extended Fontmap is not defined
	Returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_fontmap_info (void* afontmap,
			int* mfont,int* ufont,int* dfont,int* ffont)
#else
XW_STATUS Xw_get_fontmap_info (afontmap,mfont,ufont,dfont,ffont)
void *afontmap;
int *mfont,*ufont,*dfont,*ffont ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_FONTMAP *pfontmap = (XW_EXT_FONTMAP*)afontmap;
int i ;

	if( !Xw_isdefine_fontmap(pfontmap) ) {
	    /*ERROR*Bad EXT_FONTMAP Address*/
	    Xw_set_error(44,"Xw_get_fontmap_info",pfontmap) ;
	    return (XW_ERROR) ;
	}

	*mfont = MAXFONT ;
	*ufont = pfontmap->maxfont ;
	*dfont = 0 ;
	*ffont = -1 ;
	for( i=0 ; i< *ufont ; i++ ) {
	    if( pfontmap->fonts[i] ) (*dfont)++ ;
	    else if( i && *ffont < 0 ) *ffont = i ;
	}
	
#ifdef  TRACE_GET_FONTMAP_INFO
if( Xw_get_trace() ) {
	printf (" Xw_get_fontmap_info(%lx,%d,%d,%d,%d)\n",
			(long ) pfontmap,*mfont,*ufont,*dfont,*ffont) ;
}
#endif

	return (XW_SUCCESS);
}
