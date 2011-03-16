
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_FONT
#endif

/*
   char* Xw_get_font (afontmap,index,size,bheight):
   XW_EXT_FONTMAP *afontmap
   int index			font index
   float *size			font size in MM
   float *bheight		baseline font height in MM

	Get Line Font Extended fontmap FONT values from index . 

	Returns NULL if BadFont Index or font is not defined
	Returns font string address if Successful      

*/

#ifdef XW_PROTOTYPE
char* Xw_get_font (void* afontmap,int index,float *size,float *bheight)
#else
char* Xw_get_font (afontmap,index,size,bheight)
void *afontmap;
int index ;
float *size ;
float *bheight ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_FONTMAP *pfontmap = (XW_EXT_FONTMAP*)afontmap;
//XW_EXT_DISPLAY *pdisplay = pfontmap->connexion;
char *font = NULL;
//int i ;

	*size = *bheight = 0.;
	if ( !Xw_isdefine_font(pfontmap,index) ) {
	    /*ERROR*Bad defined font*/
	    Xw_set_error(43,"Xw_get_font",&index) ;
	    return (NULL) ;
	}

	if( pfontmap->snames[index] ) font = pfontmap->snames[index] ;
	else if( pfontmap->gnames[index] ) font = pfontmap->gnames[index];

	if( !font ) {
	    /*ERROR*Bad defined font*/
	    Xw_set_error(43,"Xw_get_font",&index);
	    return NULL;
	}

	*size = pfontmap->fsizes[index];
	*bheight = *size * pfontmap->fratios[index];

#ifdef  TRACE_GET_FONT
if( Xw_get_trace() > 1 ) {
    printf (" '%s' = Xw_get_font(%lx,%d,%f,%f)\n",font,(long ) pfontmap,index,*size,*bheight) ;
}
#endif

	return (font);
}
