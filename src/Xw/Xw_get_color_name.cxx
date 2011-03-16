
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_COLOR_NAME
#endif

/*
   STATUS Xw_get_color_name (acolormap,colorname,r,g,b):
   XW_EXT_COLORMAP *acolormap
   char *colorname		Name of color	from file /usr/lib/X11/rgb.txt
   float *r,*g,*b	Returns Color values in [0,1] space

	Returns ERROR if Name is not founded in the database
	Returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_color_name (void* acolormap,
			const char* colorname,float* r,float* g,float* b)
#else
XW_STATUS Xw_get_color_name (acolormap,colorname,r,g,b)
void *acolormap ;
const char *colorname ;
float *r,*g,*b ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XColor color ;
//int red,green,blue ;

	if( !Xw_isdefine_colormap(pcolormap) ) {
	    /*ERROR*Bad EXT_COLORMAP Address*/
	    Xw_set_error(42,"Xw_get_color_name",pcolormap) ;
	    return (XW_ERROR) ;
	}
	if( XParseColor(_CDISPLAY,_CINFO.colormap,colorname,&color) ) {
            *r = (float)color.red/0xFFFF ;
            *g = (float)color.green/0xFFFF;
            *b = (float)color.blue/0xFFFF ;
	} else return (XW_ERROR) ;

#ifdef  TRACE_GET_COLOR_NAME
if( Xw_get_trace() ) {
    printf (" Xw_get_color_name(%lx,%s,%f,%f,%f)\n",
				(long ) pcolormap,colorname,*r,*g,*b);
}
#endif

	return (XW_SUCCESS);
}
