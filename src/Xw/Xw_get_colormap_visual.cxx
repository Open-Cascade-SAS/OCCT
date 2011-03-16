
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_COLORMAP_VISUAL
#endif

/*
   Xw_TypeOfVisual Xw_get_colormap_visual (acolormap):
   XW_EXT_COLORMAP *acolormap

	Returns the colormap  Visual class ,must be one of :

		STATICGRAY,GRAYSCALE,STATICCOLOR,PSEUDOCOLOR,
		TRUECOLOR,DIRECTCOLOR
*/

#ifdef XW_PROTOTYPE
Xw_TypeOfVisual Xw_get_colormap_visual (void* acolormap)
#else
Xw_TypeOfVisual Xw_get_colormap_visual (acolormap)
void *acolormap;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap;

        if( !Xw_isdefine_colormap(pcolormap) ) {
            /*ERROR*Bad EXT_COLORMAP Address*/
            Xw_set_error(42,"Xw_get_colormap_visual",pcolormap) ;
            return (Xw_TOV_DEFAULT) ;
        }

#ifdef  TRACE_GET_COLORMAP_VISUAL
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_colormap_visual(%lx)\n",_CCLASS,(long ) pcolormap) ;
}
#endif

	return (Xw_TypeOfVisual) _CCLASS ;
}
