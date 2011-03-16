
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_LINE_ATTRIB
#define TRACE_GET_LINE_ATTRIB
#endif

/*
   XW_ATTRIB Xw_set_line_attrib (awindow,color,type,width,mode):
   XW_EXT_WINDOW *awindow
   int color ;		Color index to used 0 >= x < MAXCOLOR
   int type ;		Type index to used 0 >= x < MAXTYPE
   int width ;		Width index to used 0 >= x < MAXWIDTH
   XW_DRAWMODE mode ;	Draw Mode must be one of DRAWMODE enum 

	Set Line Color,Type and Width current graphic attrib .

	Returns Selected Line Graphic Attribute Index if successful 
	Returns 0 if Bad Color,Type or Width Index

*/

#ifdef XW_PROTOTYPE
XW_ATTRIB Xw_set_line_attrib (void *awindow,int color,int type,int width,XW_DRAWMODE mode)
#else
XW_ATTRIB Xw_set_line_attrib (awindow,color,type,width,mode)
void *awindow;
int color,type,width ;
XW_DRAWMODE mode ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XGCValues gc_values ;
XW_ATTRIB code ;
int i,j,k,function,mask = 0 ;
unsigned long planemask,hcolor ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_set_line_attrib",pwindow) ;
            return (0) ;
        }

	if( _BINDEX > 0 ) return (1) ;

	if ( !Xw_isdefine_color(_COLORMAP,color) ) {
	    /*ERROR*Bad Defined Color*/
	    Xw_set_error(41,"Xw_set_line_attrib",&color) ;
	    return (0) ;
	}
	if ( !Xw_isdefine_type(_TYPEMAP,type) ) {
	    /*WARNING*Bad Defined Type*/
	    Xw_set_error(50,"Xw_set_line_attrib",&type) ;
	    type = 0 ;
	}
	if ( !Xw_isdefine_width(_WIDTHMAP,width) ) {
	    /*WARNING*Bad Defined Width*/
	    Xw_set_error(52,"Xw_set_line_attrib",&width) ;
	    width = 0 ;
	}

	if( !_TYPEMAP || !_TYPEMAP->types[type] ) type = 0 ;
	if( !_WIDTHMAP || (_WIDTHMAP->widths[width] <= 1) ) width = 0 ;

	code = QGCODE(color,type,width,mode) ;
	if( code == pwindow->qgline[pwindow->lineindex].code ) {
				/* The last index computed is already set*/
	    pwindow->qgline[pwindow->lineindex].count++ ;
	    return (pwindow->lineindex+1) ;	
	}

	for( i=j=0,k=MAXQG ; i<MAXQG ; i++ ) {
	     if( code == pwindow->qgline[i].code ) k = i ;
	     if( pwindow->qgline[i].count < pwindow->qgline[j].count ) j = i;
	}

	if( k < MAXQG ) {
				/* A GC index already exist,use it */
	    pwindow->lineindex = k ;
	    pwindow->qgline[k].count++ ;
	    return (k+1) ;	
	}
	
	pwindow->lineindex = j ;
	pwindow->qgline[j].count = 1 ;

	Xw_get_color_attrib(pwindow,mode,color,&hcolor,&function,&planemask);
	if( mode != QGMODE(pwindow->qgline[j].code) ) {
	    mask |= GCFunction | GCPlaneMask | GCForeground ;
	    gc_values.function = function ;
	    gc_values.plane_mask = planemask ;
       	    gc_values.foreground = hcolor ;
	} else if( color != QGCOLOR(pwindow->qgline[j].code) ) {  
	    mask |= GCForeground ;
            gc_values.foreground = hcolor ;
	}

   if( width != QGWIDTH(pwindow->qgline[j].code) ) {  
	    mask |= GCLineWidth ;
	    if( width > 0 ) {
	        gc_values.line_width = (_WIDTHMAP->widths[width] > 1) ?
						_WIDTHMAP->widths[width] : 0 ;
	    } else {
		gc_values.line_width = 0 ;
	    }
	}

	if( type != QGTYPE(pwindow->qgline[j].code) ) {
	    mask |= GCLineStyle ;
	    if( type > 0 ) { 
                gc_values.line_style = LineOnOffDash ; 
	        XSetDashes(_DISPLAY,pwindow->qgline[j].gc,0,
		(char*)_TYPEMAP->types[type],strlen((char*)_TYPEMAP->types[type])) ;
	    } else {
                gc_values.line_style = LineSolid ;
	    }
	}

	k = j+1 ;
	
	if( mask ) {
	    XChangeGC(_DISPLAY,pwindow->qgline[j].gc,mask,&gc_values) ;
            pwindow->qgline[j].code = code ; 

#ifdef  TRACE_SET_LINE_ATTRIB
if( Xw_get_trace() ) {
    printf(" %d = Xw_set_line_attrib(%lx,%d,%d,%d,%d)\n",
					k,(long ) pwindow,color,type,width,mode) ;
}
#endif
	}

	return (k) ;	
}

/*
   XW_ATTRIB Xw_get_poly_attrib (awindow,color,type,width,mode):
   XW_EXT_WINDOW *awindow
   int *color ;         Returns current Color index
   int *type ;          Returns current Type
   int *width ;         Returns current Width index
   DRAWMODE *mode ;     Returns current Draw Mode

        Returns Selected Polyline Attribute Index

*/

#ifdef XW_PROTOTYPE
XW_ATTRIB Xw_get_line_attrib (void *awindow,int *color,int *type,int *width,XW_DRAWMODE *mode)
#else
XW_ATTRIB Xw_get_line_attrib (awindow,color,type,width,mode)
void *awindow;
int *color,*width ;
int *type ;
XW_DRAWMODE *mode ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_ATTRIB code;
int j,k;

  j = pwindow->lineindex; k = j+1;

  code = pwindow->qgline[j].code;

  *color = QGCOLOR(code);
  *type = QGTYPE(code);
  *width = QGWIDTH(code); 
  *mode = (XW_DRAWMODE) QGMODE(code);

#ifdef  TRACE_GET_LINE_ATTRIB
if( Xw_get_trace() > 1 ) {
    printf(" %d = Xw_get_line_attrib(%lx,%d,%d,%d,%d)\n",
                                k,(long ) pwindow,*color,*type,*width,*mode) ;
}
#endif

  return code;
}
