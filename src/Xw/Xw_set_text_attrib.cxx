
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_TEXT_ATTRIB
#define TRACE_GET_TEXT_ATTRIB
#endif

/*
   XW_ATTRIB Xw_set_text_attrib (awindow,color,type,font,mode):
   XW_EXT_WINDOW *awindow
   int color ;		Color index to used 0 >= x < MAXCOLOR
   int type ;		Type index to used 0 >= x < MAXTYPE
   int font ;		Font index to used 0 >= x < MAXFONT
   DRAWMODE mode ;	Draw Mode must be one of DRAWMODE enum 

	Set Text Color,Type and Font current graphic attrib .

        Returns Selected Text Attribute Index if successful
        Returns 0 if Bad Color,Type or Font Index

*/

#ifdef XW_PROTOTYPE
XW_ATTRIB Xw_set_text_attrib (void *awindow,int color,int type,int font,XW_DRAWMODE mode)
#else
XW_ATTRIB Xw_set_text_attrib (awindow,color,type,font,mode)
void *awindow;
int color,type,font ;
XW_DRAWMODE mode ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XGCValues gc_values ;
XW_ATTRIB code ;
int i,j,k,function,mask= 0 ;
unsigned long planemask,hcolor ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_set_text_attrib",pwindow) ;
            return (0) ;
        }

	if( _BINDEX > 0 ) return (1) ;

        if ( !Xw_isdefine_color(_COLORMAP,color) ) {
            /*ERROR*Bad Defined Color*/
            Xw_set_error(41,"Xw_set_text_attrib",&color) ;
            return (0) ;
        }

	if( type != 0 ) {
	    /*ERROR*Unimplemented Text type*/
	    Xw_set_error(81,"Xw_set_text_attrib",&type) ;
	    type = 0 ;
	}

	if ( !Xw_isdefine_font(_FONTMAP,font) ) {
	    /*WARNING*Bad Defined Font*/
	    Xw_set_error(43,"Xw_set_text_attrib",&font) ;
	    font = 0 ;
	}

	if( !_FONTMAP || !_FONTMAP->fonts[font] ) font = 0 ;

	code = QGCODE(color,type,font,mode) ;
        if( code == pwindow->qgtext[pwindow->textindex].code ) {
                                /* The last index computed is already set*/
            pwindow->qgtext[pwindow->textindex].count++ ;
            return (pwindow->textindex+1) ;
        }

        for( i=j=0,k=MAXQG ; i<MAXQG ; i++ ) {
             if( code == pwindow->qgtext[i].code ) k = i ;
             if( pwindow->qgtext[i].count < pwindow->qgtext[j].count ) j = i;
        }

	if( k < MAXQG ) {
					/* A GC index already exist,use it */
	    pwindow->textindex = k ;
	    pwindow->qgtext[k].count++ ;
	    return (k+1) ;	
	}

	pwindow->textindex = j ;
	pwindow->qgtext[j].count = 1 ;

        Xw_get_color_attrib(pwindow,mode,color,&hcolor,&function,&planemask); 
        if( mode != QGMODE(pwindow->qgtext[j].code) ) {
            mask |= GCFunction | GCPlaneMask | GCForeground ;
            gc_values.function = function ;
            gc_values.plane_mask = planemask ;
	    gc_values.foreground = hcolor ;
        } else if( color != QGCOLOR(pwindow->qgtext[j].code) ) {
	    mask |= GCForeground;
	    gc_values.foreground = hcolor ;
	}

	if( type != QGTYPE(pwindow->qgtext[j].code) ) {
	}

	if( font != QGFONT(pwindow->qgtext[j].code) ) {
	    if( _FONTMAP && _FONTMAP->fonts[font] ) {
 	        mask |= GCFont ;
	        gc_values.font = _FONTMAP->fonts[font]->fid ;
	    }
	}

	k = j+1 ;

	if( mask ) {	
	    XChangeGC(_DISPLAY,pwindow->qgtext[j].gc,mask,&gc_values) ;
            pwindow->qgtext[j].code = code ; 

#ifdef  TRACE_SET_TEXT_ATTRIB
if( Xw_get_trace() > 1 ) {
    printf(" %d = Xw_set_text_attrib(%lx,%d,%d,%d,%d)\n",
				k,(long ) pwindow,color,type,font,mode) ;
}
#endif
	}

	return (k) ;	
}

/*
   XW_ATTRIB Xw_get_text_attrib (awindow,color,type,font,mode):
   XW_EXT_WINDOW *awindow
   int *color ;         Returns current Color index
   int *type ;          Returns current Type
   int *font ;          Returns current Font index
   DRAWMODE *mode ;     Returns current Draw Mode

        Returns Selected Polygon Attribute Index

*/

#ifdef XW_PROTOTYPE
XW_ATTRIB Xw_get_text_attrib (void *awindow,int *color,int *type,int *font,XW_DRAWMODE *mode)
#else
XW_ATTRIB Xw_get_text_attrib (awindow,color,type,font,mode)
void *awindow;
int *color,*font ;
int *type ;
XW_DRAWMODE *mode ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_ATTRIB code;
int j,k;

  j = pwindow->textindex; k = j+1;

  code = pwindow->qgtext[j].code;

  *color = QGCOLOR(code);
  *type = QGTYPE(code);
  *font = QGFONT(code);
  *mode = (XW_DRAWMODE) QGMODE(code);

#ifdef  TRACE_GET_TEXT_ATTRIB
if( Xw_get_trace() > 1 ) {
    printf(" %d = Xw_get_text_attrib(%lx,%d,%d,%d,%d)\n",
                                k,(long ) pwindow,*color,*type,*font,*mode) ;
}
#endif

  return code;
}
