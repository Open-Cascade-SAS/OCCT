
#define PRO4391         /*GG_040696
//                      Traitement des polygones transparents.
*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_POLY_ATTRIB
#define TRACE_GET_POLY_ATTRIB
#endif

/*
   XW_ATTRIB Xw_set_poly_attrib (awindow,color,type,tile,mode):
   XW_EXT_WINDOW *awindow
   int color ;		Color index to used 0 >= x < MAXCOLOR
   int type ;		Type can be TRUE if EDGES must be Draw with the
					set_line_attrib attributes
   int tile ;		Tile index to used x < MAXTILE
   DRAWMODE mode ;	Draw Mode must be one of DRAWMODE enum 

	Set Poly Color,Type and Tile current graphic attrib .
	NOTE than if tile is < 0 the poly background is transparente.

	Returns Selected Polygon Attribute Index if successful
	Returns 0 if Bad Color,Type or Tile Index

*/

#ifdef XW_PROTOTYPE
XW_ATTRIB Xw_set_poly_attrib (void *awindow,int color,int type,int tile,XW_DRAWMODE mode)
#else
XW_ATTRIB Xw_set_poly_attrib (awindow,color,type,tile,mode)
void *awindow;
int color,tile ;
int type ;
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
            Xw_set_error(24,"Xw_set_poly_attrib",pwindow) ;
            return (0) ;
        }

	if( _BINDEX > 0 ) return (1) ;

        if ( !Xw_isdefine_color(_COLORMAP,color) ) {
            /*ERROR*Bad Defined Color*/
            Xw_set_error(41,"Xw_set_poly_attrib",&color) ;
            return (0) ;
        }

	if( tile > 0 ) {
	  if ( !Xw_isdefine_tile(_TILEMAP,tile) ) {
	    /*WARNING*Bad Defined Tile*/
	    Xw_set_error(78,"Xw_set_poly_attrib",&tile) ;
#ifdef PRO4391
	    tile = 1 ;
	  }
	} else if( tile < 0 ) {
	  tile = 0;
	} else {
	  tile = 1;
	}
#else
	    tile = 0 ;
	  }
	}

	if( !_TILEMAP || !_TILEMAP->tiles[tile] ) tile = 0 ;
#endif

	code = QGCODE(color,type,tile,mode) ;
        if( code == pwindow->qgpoly[pwindow->polyindex].code ) {
                                /* The last index computed is already set*/
            pwindow->qgpoly[pwindow->polyindex].count++ ;
            return (pwindow->polyindex+1) ;
        }

        for( i=j=0,k=MAXQG ; i<MAXQG ; i++ ) {
             if( code == pwindow->qgpoly[i].code ) k = i ;
             if( pwindow->qgpoly[i].count < pwindow->qgpoly[j].count ) j = i;
        }

	if( k < MAXQG ) {
					/* A GC index already exist,use it */
	    pwindow->polyindex = k ;
	    pwindow->qgpoly[k].count++ ;
	    return (k+1) ; 	
	}

	pwindow->polyindex = j ;
	pwindow->qgpoly[j].count = 1 ;

        Xw_get_color_attrib(pwindow,mode,color,&hcolor,&function,&planemask);
        if( mode != QGMODE(pwindow->qgpoly[j].code) ) {
            mask |= GCFunction | GCPlaneMask | GCForeground ;
            gc_values.function = function ;
            gc_values.plane_mask = planemask ;
            gc_values.foreground = hcolor ;
        } else if( color != QGCOLOR(pwindow->qgpoly[j].code) ) {
            mask |= GCForeground;
            gc_values.foreground = hcolor ;
        }

	if( tile != QGTILE(pwindow->qgpoly[j].code) ) {
	    mask |= GCFillStyle ;
#ifdef PRO4391
	    if( (tile > 0) && _TILEMAP && _TILEMAP->tiles[tile] ) {
#else
	    if( tile > 0 ) {
#endif
	        mask |= GCStipple ;
		gc_values.fill_style = FillOpaqueStippled ; 
	        gc_values.stipple = _TILEMAP->tiles[tile] ; 
	    } else {
	        gc_values.fill_style = FillSolid ; 
	    }
	}

	k = j+1 ;

	if( mask ) {
	    XChangeGC(_DISPLAY,pwindow->qgpoly[j].gc,mask,&gc_values) ;
            pwindow->qgpoly[j].code = code ; 

#ifdef  TRACE_SET_POLY_ATTRIB
if( Xw_get_trace() > 1 ) {
    printf(" %d = Xw_set_poly_attrib(%lx,%d,%d,%d,%d)\n",
				k,(long ) pwindow,color,type,tile,mode) ;
}
#endif
	}

	return (k) ;	
}

/*
   XW_ATTRIB Xw_get_poly_attrib (awindow,color,type,tile,mode):
   XW_EXT_WINDOW *awindow
   int *color ;		Returns current Color index 
   int *type ;		Returns current Type
   int *tile ;		Returns current Tile index
   DRAWMODE *mode ;	Returns current Draw Mode

	Returns Selected Polygon Attribute Index

*/

#ifdef XW_PROTOTYPE
XW_ATTRIB Xw_get_poly_attrib (void *awindow,int *color,int *type,int *tile,XW_DRAWMODE *mode)
#else
XW_ATTRIB Xw_get_poly_attrib (awindow,color,type,tile,mode)
void *awindow;
int *color,*tile ;
int *type ;
XW_DRAWMODE *mode ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_ATTRIB code;
int j,k;

  j = pwindow->polyindex; k = j+1;

  code = pwindow->qgpoly[j].code;

  *color = QGCOLOR(code);
  *type = QGTYPE(code);
  *tile = QGTILE(code) > 1 ? QGTILE(code) : QGTILE(code) - 1; 
  *mode = (XW_DRAWMODE) QGMODE(code);

#ifdef  TRACE_GET_POLY_ATTRIB
if( Xw_get_trace() > 1 ) {
    printf(" %d = Xw_get_poly_attrib(%lx,%d,%d,%d,%d)\n",
				k,(long ) pwindow,*color,*type,*tile,*mode) ;
}
#endif

    return code;
}
