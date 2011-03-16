
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_BACKGROUND_COLOR
#endif

/*
   STATUS Xw_get_background_color(awindow,r,g,b):
   XW_EXT_WINDOW *awindow	Window extension structure
   float *r,*g,*b ;	Return Red,Green,Blue color value 0. >= x <= 1.

	Get R,G,B Background Color values from Default index .

	Returns ERROR if Extended Window Address is badly defined
	Returns SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_background_color (void* awindow,float* r,float* g,float* b)
#else
XW_STATUS Xw_get_background_color (awindow,r,g,b)
void *awindow;
float *r,*g,*b ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
unsigned long pixel;

	if ( !Xw_isdefine_window(pwindow) ) {
	    /*ERROR*Bad EXTENDED Window ADDRESS*/
	    Xw_set_error(24,"Xw_get_background_color",pwindow) ;
	    return (XW_ERROR) ;
	}

	if( pwindow->backindex >= 0 ) {
	  switch ( _CLASS ) {
	    case PseudoColor :
	    case StaticColor :
	    case TrueColor :
	      Xw_get_color (_COLORMAP,pwindow->backindex,r,g,b,&pixel) ;
	      break ;

            default :
              /*Unmatchable Visual class*/
              Xw_set_error(67,"Xw_get_background_color",&_CLASS) ;
              return (XW_ERROR) ;
	  }
	} else {
	  *r = *g = *b = 1. ;
	}


#ifdef TRACE_GET_BACKGROUND_COLOR
if( Xw_get_trace() ) {
    printf(" Xw_get_background_color(%lx,%f,%f,%f)\n",(long ) pwindow,*r,*g,*b) ;
}
#endif

	return (XW_SUCCESS);
}
