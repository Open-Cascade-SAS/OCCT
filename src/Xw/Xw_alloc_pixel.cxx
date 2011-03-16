
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_ALLOC_PIXEL
#define TRACE_SET_PIXEL
#define TRACE_FREE_PIXEL
#endif

/*
   XW_STATUS Xw_alloc_pixel(pcolormap,*pixel):
   XW_EXT_COLORMAP *pcolormap
   unsigned long *pixel;

	Allocates a pixel in the colormap and
	returns SUCCESS for a valid pixel value according of the free space of
	the colormap,
	or ERROR if no more room exist in the colormap.

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_alloc_pixel (void* acolormap, unsigned long *pixel)
#else
XW_STATUS Xw_alloc_pixel (acolormap,pixel)
void *acolormap;
unsigned long *pixel;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*) acolormap;
XW_STATUS status = XW_ERROR;

	*pixel = 0;
        if( !Xw_isdefine_colormap(pcolormap) ) {
          /*ERROR*Bad EXT_COLORMAP Address*/
          Xw_set_error(42,"Xw_alloc_pixel",pcolormap) ;
          return (XW_ERROR) ;
        }

        if( _CCLASS == PseudoColor ) {
          if( XAllocColorCells(_CDISPLAY,
                        _CINFO.colormap,False,NULL,0,pixel,1) ) {
	    status = XW_SUCCESS;
	  }
	}

#ifdef TRACE_ALLOC_PIXEL
if( Xw_get_trace() ) {
    printf(" %d = Xw_alloc_pixel(%lx,%ld)\n",status,(long ) pcolormap,*pixel) ;
}
#endif
    return (status);
}

/*
   XW_STATUS Xw_set_pixel(pcolormap,pixel,r,g,b):
   XW_EXT_COLORMAP *pcolormap
   unsigned long pixel;
   float r,g,b;

	Updates a pixel with the color {r,g,b} defined in [0-1] space
	returns SUCCESS for a valid PseudoColor pixel
*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_pixel (void* acolormap, unsigned long pixel, float r, float g, float b)
#else
XW_STATUS Xw_set_pixel (acolormap,pixel,r,g,b)
void *acolormap;
unsigned long pixel;
float r,g,b;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*) acolormap;
XW_STATUS status = XW_ERROR;

        if( !Xw_isdefine_colormap(pcolormap) ) {
          /*ERROR*Bad EXT_COLORMAP Address*/
          Xw_set_error(42,"Xw_set_pixel",pcolormap) ;
          return (XW_ERROR) ;
        }

        if( _CCLASS == PseudoColor ) {
	  XColor color;
	  int error,gravity;
	  char *serror;
	  color.pixel = pixel;
          color.red = (unsigned short) (r*65535.);
          color.green = (unsigned short) (g*65535.);
          color.blue = (unsigned short) (b*65535.);
	  color.flags = DoRed | DoGreen | DoBlue;

          Xw_print_error();
          if( !Xw_get_trace() ) Xw_set_synchronize(_CDISPLAY,True);
          XStoreColor(_CDISPLAY,_CINFO.colormap,&color);
          if( !Xw_get_trace() ) Xw_set_synchronize(_CDISPLAY,False);
          serror = Xw_get_error(&error,&gravity);
          if( error < 1000 ) status = XW_SUCCESS;
	}

#ifdef TRACE_SET_PIXEL
if( Xw_get_trace() ) {
    printf(" %d = Xw_set_pixel(%lx,%ld,%f,%f,%f)\n",status,(long ) pcolormap,pixel,r,g,b) ;
}
#endif
    return (status);
}

/*
   XW_STATUS Xw_free_pixel(pcolormap,pixel):
   XW_EXT_COLORMAP *pcolormap
   unsigned long pixel;

	Free a pixel from the colormap 
	returns SUCCESS for a valid PseudoColor pixel
*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_free_pixel (void* acolormap, unsigned long pixel)
#else
XW_STATUS Xw_free_pixel (acolormap,pixel)
void *acolormap;
unsigned long pixel;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*) acolormap;
XW_STATUS status = XW_ERROR;

        if( !Xw_isdefine_colormap(pcolormap) ) {
          /*ERROR*Bad EXT_COLORMAP Address*/
          Xw_set_error(42,"Xw_free_pixel",pcolormap) ;
          return (XW_ERROR) ;
        }

        if( _CCLASS == PseudoColor ) {
	  int error,gravity;
	  char *serror;

          Xw_print_error();
          if( !Xw_get_trace() ) Xw_set_synchronize(_CDISPLAY,True);
          XFreeColors(_CDISPLAY,_CINFO.colormap,&pixel,1,0);
          if( !Xw_get_trace() ) Xw_set_synchronize(_CDISPLAY,False);
          serror = Xw_get_error(&error,&gravity);
          if( error < 1000 ) status = XW_SUCCESS;
	}

#ifdef TRACE_FREE_PIXEL
if( Xw_get_trace() ) {
    printf(" %d = Xw_free_pixel(%lx,%ld)\n",status,(long ) pcolormap,pixel) ;
}
#endif
    return (status);
}
