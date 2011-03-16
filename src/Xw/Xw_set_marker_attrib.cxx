
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_MARKER_ATTRIB
#endif

/*
   XW_ATTRIB Xw_set_marker_attrib (awindow,color,type,width,mode):
   XW_EXT_WINDOW *awindow
   int color ;		Color index to used 0 >= x < MAXCOLOR
   int type ;		Type can be TRUE if marker must be filled with the
					set_poly_attrib attributes 
   int width ;		Width index to used 0 >= x < MAXWIDTH
   DRAWMODE mode ;	Draw Mode must be one of DRAWMODE enum 

	Set Marker Color,Type and Marker current graphic attrib .

	Returns Selected Marker Attribute Index if successful
	Returns 0 if Bad Color,Type or Marker Index

*/

#ifdef XW_PROTOTYPE
XW_ATTRIB Xw_set_marker_attrib (void *awindow,int color,int type,int width,XW_DRAWMODE mode)
#else
XW_ATTRIB Xw_set_marker_attrib (awindow,color,type,width,mode)
void *awindow;
int color,type ;
int width ;
XW_DRAWMODE mode ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XGCValues gc_values ;
XW_ATTRIB code ;
int i,j,k,function,mask = 0;
unsigned long planemask,hcolor ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_set_marker_attrib",pwindow) ;
            return (0) ;
        }

	if( _BINDEX > 0 ) return (1) ;

        if ( !Xw_isdefine_color(_COLORMAP,color) ) {
            /*ERROR*Bad Defined Color*/
            Xw_set_error(41,"Xw_set_marker_attrib",&color) ;
            return (0) ;
	}

        if ( !Xw_isdefine_width(_WIDTHMAP,width) ) {
            /*WARNING*Bad Defined Width*/
            Xw_set_error(52,"Xw_set_marker_attrib",&width) ;
            width = 0 ;
        }

	if( !_WIDTHMAP || (_WIDTHMAP->widths[width] <= 1) ) width = 0 ;
 
	code = QGCODE(color,type,width,mode) ;
        if( code == pwindow->qgmark[pwindow->markindex].code ) {
                                /* The last index computed is already set*/
            pwindow->qgmark[pwindow->markindex].count++ ;
            return (pwindow->markindex+1) ;
        }

        for( i=j=0,k=MAXQG ; i<MAXQG ; i++ ) {
             if( code == pwindow->qgmark[i].code ) k = i ;
             if( pwindow->qgmark[i].count < pwindow->qgmark[j].count ) j = i;
        }

	if( k < MAXQG ) {
					/* A GC index already exist,use it */
	    pwindow->markindex = k ;
	    pwindow->qgmark[k].count++ ;
	    return (k+1) ; 	
	}

	pwindow->markindex = j ;
	pwindow->qgmark[j].count = 1 ;

        Xw_get_color_attrib(pwindow,mode,color,&hcolor,&function,&planemask);
        if( mode != QGMODE(pwindow->qgmark[j].code) ) {
            mask |= GCFunction | GCPlaneMask | GCForeground ;
            gc_values.function = function ;
            gc_values.plane_mask = planemask ;
            gc_values.foreground = hcolor ;
        } else if( color != QGCOLOR(pwindow->qgmark[j].code) ) {
            mask |= GCForeground;
            gc_values.foreground = hcolor ;
        }

        if( width != QGWIDTH(pwindow->qgmark[j].code) ) {
            mask |= GCLineWidth ;
            if( width > 0 ) {
                gc_values.line_width = _WIDTHMAP->widths[width] ;
            } else {   
                gc_values.line_width = 0 ;
            }
        }
 
	k = j+1 ;

	if( mask ) {
	    XChangeGC(_DISPLAY,pwindow->qgmark[j].gc,mask,&gc_values) ;
            pwindow->qgmark[j].code = code ; 

#ifdef  TRACE_SET_MARKER_ATTRIB
if( Xw_get_trace() > 1 ) {
    printf(" %d = Xw_set_marker_attrib(%lx,%d,%d,%d,%d)\n",
				k,(long ) pwindow,color,type,width,mode) ;
}
#endif
	}

	return (k) ;	
}
