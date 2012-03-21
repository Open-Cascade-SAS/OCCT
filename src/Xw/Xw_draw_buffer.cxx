// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#define CTS50025        /*GG_080997
Preinitialiser la police de caracteres du buffer sinon
                plantage au chargement
*/

#define PRO11005  /*GG_131197
//            La couleur de fond du paragraphe n'est pas respectee
//            En mode Highlight,on ne voit plus les textes.
*/


#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DRAW_BUFFER
#endif

#define XROTATE(x,y) (x*cosa + y*sina)
#define YROTATE(x,y) (y*cosa - x*sina)
#define UNKNOWN_BUFFER 0
#define MOVE_BUFFER 1
#define ROTATE_BUFFER 2
#define SCALE_BUFFER 3

/*
   STATUS Xw_draw_buffer (awindow,bufferid)
   XW_EXT_WINDOW *awindow
   int bufferid
				
	Draw one retain buffer of primitives at screen

	Returns ERROR if the extended window address is badly defined
		or Buffer is empty 
		or Buffer is already drawn at screen
		or Buffer is not opened
	Returns SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_draw_buffer (void* awindow,int bufferid)
#else
XW_STATUS Xw_draw_buffer (awindow,bufferid)
void *awindow;
int bufferid;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
XW_STATUS status ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_draw_buffer",pwindow) ;
            return (XW_ERROR) ;
        }

	if( bufferid > 0 ) {
	  pbuffer = Xw_get_buffer_structure(pwindow,bufferid) ;
	  if( !pbuffer ) {
            /*ERROR*BUFFER is not opened*/
            Xw_set_error(119,"Xw_draw_buffer",&bufferid) ;
            return (XW_ERROR) ;
	  }
	} else {
	  pbuffer = &_BUFFER(-bufferid) ;
	  if( !pbuffer->bufferid ) {
            /*ERROR*BUFFER is not opened*/
            return (XW_SUCCESS) ;
	  }
	}

	if( pbuffer->isempty ) return XW_ERROR ;

	if( pbuffer->isdrawn && (QGMODE(pbuffer->code) == XW_XORBACK) ) 
							return XW_ERROR ;

	pbuffer->isdrawn = True ;

	status = Xw_redraw_buffer(pwindow,pbuffer) ;

	if( !pbuffer->isretain && (QGTYPE(pbuffer->code) == XW_ENABLE) ) { 
	  int x,y,w,h ;
	  if( pbuffer->isupdated ) {
	    x = pbuffer->uxmin - 1 ;
	    y = pbuffer->uymin - 1 ;
	    w = pbuffer->uxmax - x + 1 ;
	    h = pbuffer->uymax - y + 1 ;
	  } else {
	    x = pbuffer->rxmin - 1 ;
	    y = pbuffer->rymin - 1 ;
	    w = pbuffer->rxmax - x + 1 ;
	    h = pbuffer->rymax - y + 1 ;
	  }

          if( x < 0 ) x = 0 ;
          if( y < 0 ) y = 0 ;
          if( x+w > _WIDTH ) w = _WIDTH - x ; 
          if( y+h > _HEIGHT ) h = _HEIGHT - y ;

	  if( _NWBUFFER > 0 ) {
            XCopyArea(_DISPLAY,_BWBUFFER,_FWBUFFER,
				pwindow->qgwind.gccopy,x,y,w,h,x,y) ;
	  } else if( _PIXMAP ) { 
            XCopyArea(_DISPLAY,_PIXMAP,_WINDOW,
				pwindow->qgwind.gccopy,x,y,w,h,x,y) ;
	  }
          XFlush(_DISPLAY) ;
	}

#ifdef  TRACE_DRAW_BUFFER
	if( Xw_get_trace() ) {
	  printf (" Xw_draw_buffer(%lx,%d)\n",(long ) pwindow,bufferid) ;
	}
#endif

	return status ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_redraw_buffer (XW_EXT_WINDOW *pwindow,XW_EXT_BUFFER *pbuffer)
#else
XW_STATUS Xw_redraw_buffer (pwindow,pbuffer)
XW_EXT_WINDOW *pwindow;
XW_EXT_BUFFER *pbuffer ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_IMAGE *pimaglist ;
XW_EXT_POLY *ppolylist ;
XW_EXT_LINE *plinelist ;
XW_EXT_SEGMENT *pseglist ;
XW_EXT_LTEXT *pltextlist ;
XW_EXT_PTEXT *pptextlist ;
XW_EXT_ARC *parclist ;
XW_EXT_PMARKER *ppmarklist ;
XW_EXT_LMARKER *plmarklist ;
XW_EXT_POINT *ppntlist ;

	if( QGTYPE(pbuffer->code) == XW_ENABLE ) {
	  _DRAWABLE = (_NWBUFFER > 0) ? _BWBUFFER : _PIXMAP ;
	} else {
	  _DRAWABLE = (_NWBUFFER > 0) ? _FWBUFFER : _WINDOW ;
	}

	for( pimaglist = pbuffer->pimaglist ; pimaglist ;
			pimaglist = (XW_EXT_IMAGE*)pimaglist->link ) {
	    if( pimaglist->nimage > 0 ) {
		Xw_draw_pixel_images(pwindow,pimaglist,
					pbuffer->gcf,pbuffer->gcf);
	    } else break ;
	}

	for( ppolylist = pbuffer->ppolylist ; ppolylist ;
			ppolylist = (XW_EXT_POLY*)ppolylist->link ) {
	    if( ppolylist->npoly > 0 ) {
#ifdef PRO11005
	        Xw_draw_pixel_polys(pwindow,ppolylist,NULL,pbuffer->gcf);
#else
	        Xw_draw_pixel_polys(pwindow,ppolylist,pbuffer->gcf,NULL);
#endif
	    } else break ;
	}

	for( parclist = pbuffer->pparclist ; parclist ;
			parclist = (XW_EXT_ARC*)parclist->link ) {
	    if( parclist->narc > 0 ) {
	        Xw_draw_pixel_polyarcs(pwindow,parclist,pbuffer->gcf,NULL);
	    } else break ;
	}

	for( pseglist = pbuffer->pseglist ; pseglist ;
			pseglist = (XW_EXT_SEGMENT*)pseglist->link ) {
	    if( pseglist->nseg > 0 ) {
		Xw_draw_pixel_segments(pwindow,pseglist,pbuffer->gcf);
	    } else break ;
	}

	for( plinelist = pbuffer->plinelist ; plinelist ;
			plinelist = (XW_EXT_LINE*)plinelist->link ) {
	    if( plinelist->nline > 0 ) {
		Xw_draw_pixel_lines(pwindow,plinelist,pbuffer->gcf);
	    } else break ;
	}

	for( parclist = pbuffer->plarclist ; parclist ;
			parclist = (XW_EXT_ARC*)parclist->link ) {
	    if( parclist->narc > 0 ) {
		Xw_draw_pixel_arcs(pwindow,parclist,pbuffer->gcf);
	    } else break ;
	}

	for( pltextlist = pbuffer->pltextlist ; pltextlist ;
			pltextlist = (XW_EXT_LTEXT*)pltextlist->link ) {
	    if( pltextlist->ntext > 0 ) {
		Xw_draw_pixel_texts(pwindow,pltextlist,
					pbuffer->gcf,pbuffer->code);
	    } else break ;
	}

	for( ppmarklist = pbuffer->ppmarklist ; ppmarklist ;
			ppmarklist = (XW_EXT_PMARKER*)ppmarklist->link ) {
	    if( ppmarklist->nmark > 0 ) {
		Xw_draw_pixel_pmarkers(pwindow,ppmarklist,pbuffer->gcf);
	    } else break ;
	}

	for( plmarklist = pbuffer->plmarklist ; plmarklist ;
			plmarklist = (XW_EXT_LMARKER*)plmarklist->link ) {
	    if( plmarklist->nmark > 0 ) {
		Xw_draw_pixel_lmarkers(pwindow,plmarklist,pbuffer->gcf);
	    } else break ;
	}

	for( pptextlist = pbuffer->pptextlist ; pptextlist ;
			pptextlist = (XW_EXT_PTEXT*)pptextlist->link ) {
	    if( pptextlist->ntext > 0 ) {
		Xw_draw_pixel_polytexts(pwindow,pptextlist,
			pbuffer->gcf,NULL,pbuffer->gcf,pbuffer->code);
	    } else break ;
	}

	for( ppntlist = pbuffer->ppntlist ; ppntlist ;
			ppntlist = (XW_EXT_POINT*)ppntlist->link ) {
	    if( ppntlist->npoint > 0 ) {
		Xw_draw_pixel_points(pwindow,ppntlist,pbuffer->gcf);
	    } else break ;
	}

	XFlush(_DISPLAY) ;

	return (XW_SUCCESS);
}

/*
   STATUS Xw_erase_buffer (awindow,bufferid)
   XW_EXT_WINDOW *awindow
   int bufferid
				
	Erase one retain buffer of primitives from screen

	Returns ERROR if the extended window address is badly defined
	        or Buffer is empty 
		or Buffer is already erase from screen
		or Buffer is not opened
	Returns SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_erase_buffer (void* awindow,int bufferid)
#else
XW_STATUS Xw_erase_buffer (awindow,bufferid)
void *awindow;
int bufferid;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
XW_STATUS status = XW_SUCCESS ;
int x,y,w,h ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_erase_buffer",pwindow) ;
            return (XW_ERROR) ;
        }

	if( bufferid > 0 ) {
	  pbuffer = Xw_get_buffer_structure(pwindow,bufferid) ;
	  if( !pbuffer ) {
            /*ERROR*BUFFER is not opened*/
            Xw_set_error(119,"Xw_erase_buffer",&bufferid) ;
            return (XW_ERROR) ;
	  }
	} else {
	  pbuffer = &_BUFFER(-bufferid) ;
	  if( !pbuffer->bufferid ) {
            /*ERROR*BUFFER is not opened*/
            return (XW_SUCCESS) ;
	  }
	}

	if( pbuffer->isempty ) return XW_ERROR ;

	if( !pbuffer->isdrawn && (QGMODE(pbuffer->code) == XW_XORBACK) ) 
							return XW_ERROR ;

	pbuffer->isdrawn = False ;
	if( pbuffer->isupdated ) {
	  x = pbuffer->uxmin - 1 ;
	  y = pbuffer->uymin - 1 ;
	  w = pbuffer->uxmax - x + 1 ;
	  h = pbuffer->uymax - y + 1 ;
	} else {
	  x = pbuffer->rxmin - 1 ;
	  y = pbuffer->rymin - 1 ;
	  w = pbuffer->rxmax - x + 1 ;
	  h = pbuffer->rymax - y + 1 ;
	}

        if( x < 0 ) x = 0 ;
        if( y < 0 ) y = 0 ;
        if( x+w > _WIDTH ) w = _WIDTH - x ; 
        if( y+h > _HEIGHT ) h = _HEIGHT - y ;

	if( QGMODE(pbuffer->code) == XW_REPLACE ) {
					/* Restore from MIN-MAX buffer pixmap */
          if( _NWBUFFER > 0 ) {
            XCopyArea(_DISPLAY,_BWBUFFER,_FWBUFFER,
                                pwindow->qgwind.gccopy,x,y,w,h,x,y) ;
	  } else if( _PIXMAP ) {
            XCopyArea(_DISPLAY,_PIXMAP,_WINDOW,
				pwindow->qgwind.gccopy,x,y,w,h,x,y) ;
	  } else {
	    XClearArea(_DISPLAY,_WINDOW,x,y,w,h,False);
	  }
          XFlush(_DISPLAY) ;
	} else if( QGMODE(pbuffer->code) == XW_XORBACK ) {	
	  status = Xw_redraw_buffer(pwindow,pbuffer) ;
	  if( !pbuffer->isretain && (QGTYPE(pbuffer->code) == XW_ENABLE) )  {	
            if( _NWBUFFER > 0 ) {
              XCopyArea(_DISPLAY,_BWBUFFER,_FWBUFFER,
                                pwindow->qgwind.gccopy,x,y,w,h,x,y) ;
	    } else if( _PIXMAP ) {
              XCopyArea(_DISPLAY,_PIXMAP,_WINDOW,
				pwindow->qgwind.gccopy,x,y,w,h,x,y) ;
	    }
            XFlush(_DISPLAY) ;
	  }
	}

#ifdef  TRACE_DRAW_BUFFER
	if( Xw_get_trace() ) {
	  printf (" Xw_erase_buffer(%lx,%d)\n",(long ) pwindow,bufferid) ;
	}
#endif
	return status ;
}

/*
   XW_STATUS Xw_clear_buffer (awindow,bufferid)
   XW_EXT_WINDOW *awindow
   int bufferid
				
	Erase and Clear one retain buffer of primitives from screen

	Returns ERROR if the extended window address is badly defined
	        or Buffer is empty 
		or Buffer is not opened
	Returns SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_clear_buffer (void* awindow,int bufferid)
#else
XW_STATUS Xw_clear_buffer (awindow,bufferid)
void *awindow;
int bufferid ;
#endif /*PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
XW_EXT_IMAGE *pimaglist ;
XW_EXT_POLY *ppolylist ;
XW_EXT_LINE *plinelist ;
XW_EXT_SEGMENT *pseglist ;
XW_EXT_LTEXT *pltextlist ;
XW_EXT_PTEXT *pptextlist ;
XW_EXT_ARC *parclist ;
XW_EXT_PMARKER *ppmarklist ;
XW_EXT_LMARKER *plmarklist ;
XW_EXT_POINT *ppntlist ;
XW_EXT_POINT *plinedesc ;
XW_EXT_CHAR *ptextdesc ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_clear_buffer",pwindow) ;
            return (XW_ERROR) ;
        }

	if( bufferid > 0 ) {
	  pbuffer = Xw_get_buffer_structure(pwindow,bufferid) ;
	  if( !pbuffer ) {
            /*ERROR*BUFFER is not opened*/
            Xw_set_error(119,"Xw_clear_buffer",&bufferid) ;
            return (XW_ERROR) ;
	  }
	} else {
	  pbuffer = &_BUFFER(-bufferid) ;
	  if( !pbuffer->bufferid ) {
            /*ERROR*BUFFER is not opened*/
            return (XW_SUCCESS) ;
	  }
	}

	if( pbuffer->isempty ) return XW_ERROR ;

	if( pbuffer->isdrawn ) Xw_erase_buffer(pwindow,bufferid) ;

	pbuffer->isempty = True ;
	pbuffer->isupdated = UNKNOWN_BUFFER ;
	pbuffer->rxmin = _WIDTH ;
	pbuffer->rymin = _HEIGHT ;
	pbuffer->rxmax = 0 ;
	pbuffer->rymax = 0 ;
	pbuffer->xscale = 1. ;
	pbuffer->dxscale = 1. ;
	pbuffer->yscale = 1. ;
	pbuffer->dyscale = 1. ;
	pbuffer->angle = 0. ;
	pbuffer->dangle = 0. ;
	pbuffer->dxpivot = 0 ;
	pbuffer->dypivot = 0 ;

	for( pimaglist = pbuffer->pimaglist ; pimaglist ;
			pimaglist = (XW_EXT_IMAGE*)pimaglist->link ) {
	    if( pimaglist->nimage > 0 ) {
		pimaglist->isupdated = False ;
		pimaglist->nimage = 0 ;
	    } else break ;
	}

	for( ppolylist = pbuffer->ppolylist ; ppolylist ;
			ppolylist = (XW_EXT_POLY*)ppolylist->link ) {
	    if( ppolylist->npoly > 0 ) {
		ppolylist->isupdated = False ;
		ppolylist->npoly = 0 ;
	    } else break ;
	}

	for( parclist = pbuffer->pparclist ; parclist ;
			parclist = (XW_EXT_ARC*)parclist->link ) {
	    if( parclist->narc > 0 ) {
		parclist->isupdated = False ;
		parclist->narc = 0 ;
	    } else break ;
	}

	for( pseglist = pbuffer->pseglist ; pseglist ;
			pseglist = (XW_EXT_SEGMENT*)pseglist->link ) {
	    if( pseglist->nseg > 0 ) {
		pseglist->isupdated = False ;
		pseglist->nseg = 0 ;
	    } else break ;
	}

	for( plinelist = pbuffer->plinelist ; plinelist ;
			plinelist = (XW_EXT_LINE*)plinelist->link ) {
	    if( plinelist->nline > 0 ) {
		plinelist->isupdated = False ;
		plinelist->nline = 0 ;
	    } else break ;
	}

	for( parclist = pbuffer->plarclist ; parclist ;
			parclist = (XW_EXT_ARC*)parclist->link ) {
	    if( parclist->narc > 0 ) {
		parclist->isupdated = False ;
		parclist->narc = 0 ;
	    } else break ;
	}

	for( pltextlist = pbuffer->pltextlist ; pltextlist ;
			pltextlist = (XW_EXT_LTEXT*)pltextlist->link ) {
	    if( pltextlist->ntext > 0 ) {
		pltextlist->isupdated = False ;
		pltextlist->ntext = 0 ;
	    } else break ;
	}

	for( pptextlist = pbuffer->pptextlist ; pptextlist ;
			pptextlist = (XW_EXT_PTEXT*)pptextlist->link ) {
	    if( pptextlist->ntext > 0 ) {
		pptextlist->isupdated = False ;
		pptextlist->ntext = 0 ;
	    } else break ;
	}

	for( ppmarklist = pbuffer->ppmarklist ; ppmarklist ;
			ppmarklist = (XW_EXT_PMARKER*)ppmarklist->link ) {
	    if( ppmarklist->nmark > 0 ) {
		ppmarklist->isupdated = False ;
		ppmarklist->nmark = 0 ;
		ppmarklist->npoint = 0 ;
	    } else break ;
	}

	for( plmarklist = pbuffer->plmarklist ; plmarklist ;
			plmarklist = (XW_EXT_LMARKER*)plmarklist->link ) {
	    if( plmarklist->nmark > 0 ) {
		plmarklist->isupdated = False ;
		plmarklist->nmark = 0 ;
		plmarklist->nseg = 0 ;
	    } else break ;
	}

	for( ppntlist = pbuffer->ppntlist ; ppntlist ;
			ppntlist = (XW_EXT_POINT*)ppntlist->link ) {
	    if( ppntlist->npoint > 0 ) {
		ppntlist->isupdated = False ;
		ppntlist->npoint = 0 ;
	    } else break ;
	}

	for( plinedesc = pbuffer->plinedesc ; plinedesc ;
			plinedesc = (XW_EXT_POINT*)plinedesc->link ) {
	    if( plinedesc->npoint > 0 ) {
		plinedesc->isupdated = False ;
		plinedesc->npoint = 0 ;
	    } else break ;
	}

	for( ptextdesc = pbuffer->ptextdesc ; ptextdesc ;
			ptextdesc = (XW_EXT_CHAR*)ptextdesc->link ) {
	    if( ptextdesc->nchar > 0 ) {
		ptextdesc->nchar = 0 ;
	    } else break ;
	}

#ifdef  TRACE_DRAW_BUFFER
	if( Xw_get_trace() ) {
	  printf (" Xw_clear_buffer(%lx,%d)\n",(long ) pwindow,bufferid) ;
	}
#endif

	return (XW_SUCCESS);
}

/*
   XW_STATUS Xw_open_buffer
	(awindow,bufferid,xpivot,ypivot,width,color,font,drawmode)
   XW_EXT_WINDOW *awindow
   int bufferid
   float xpivot,ypivot	buffer hit point
   int	 widthindex
   int	 colorindex
   int	 fontindex
   XW_DRAWMODE drawmode
				
	Open one retain buffer

	Returns ERROR if the extended window address is badly defined
		or Buffer identification is <= 0
	Returns SUCCESS if successful 

	Note that if the buffer is already opened,this is reset with the
	new hit point.

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_open_buffer
	(void* awindow,int bufferid,float xpivot,float ypivot,
	 int width,int color,int font,XW_DRAWMODE drawmode)
#else
XW_STATUS Xw_open_buffer
	(awindow,bufferid,xpivot,ypivot,width,color,font,drawmode)
void *awindow;
int bufferid ;
float xpivot,ypivot ;
int width ;
int color ;
int font ;
XW_DRAWMODE drawmode ;
#endif /*PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
XGCValues gc_values ;
//XFontStruct *pfontinfo ;
int i,mask = 0 ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_open_buffer",pwindow) ;
            return (XW_ERROR) ;
        }

	if( bufferid <= 0 ) {
            /*ERROR*Bad BUFFER identification*/
            Xw_set_error(122,"Xw_open_buffer",&bufferid) ;
            return (XW_ERROR) ;
	}

	if( drawmode != XW_REPLACE && drawmode != XW_XORBACK ) {
            /*ERROR*Bad Buffer drawing mode*/
            Xw_set_error(125,"Xw_open_buffer",&drawmode) ;
	    drawmode = XW_XORBACK ;
	}

        if ( !Xw_isdefine_width(_WIDTHMAP,width) ) {
            /*ERROR*Bad Defined Width*/
            Xw_set_error(52,"Xw_open_buffer",&width) ;
	    width = 0 ;
        }

        if ( !Xw_isdefine_color(_COLORMAP,color) ) {
            /*ERROR*Bad Defined Color*/
            Xw_set_error(41,"Xw_open_buffer",&color) ;
	    color = 0 ;
        }

        if ( !Xw_isdefine_font(_FONTMAP,font) ) {
	    /*WARNING*Bad Defined Font*/
            Xw_set_error(43,"Xw_open_buffer",&font) ;
            font = 0 ;
        }

	pbuffer = Xw_get_buffer_structure(pwindow,bufferid) ;
	if( pbuffer ) {
	    /*ERROR*BUFFER is already opened
            Xw_set_error(120,"Xw_open_buffer",&bufferid) ;
*/
	    if( pbuffer->isdrawn ) {
	      Xw_erase_buffer(pwindow,bufferid) ;
	    }
	} else {
	    for( i=1 ; i<MAXBUFFERS ; i++ ) {
	      if( _BUFFER(i).bufferid <= 0 ) break ;
	    }
	    if( i < MAXBUFFERS ) {
	      pbuffer = &_BUFFER(i) ;
	    } else {
              /*ERROR*Too much BUFFERS are opened,max is*/
              Xw_set_error(121,"Xw_open_buffer",&i) ;
	      return (XW_ERROR) ;
	    }
	    
	    pbuffer->bufferid = bufferid;
	    pbuffer->gcf = XCreateGC(_DISPLAY,_WINDOW, 0, NULL) ;
	    pbuffer->gcb = XCreateGC(_DISPLAY,_WINDOW, 0, NULL) ;
	}

//OCC186
	pbuffer->xpivot = PXPOINT(xpivot, pwindow->xratio) ;
	pbuffer->ypivot = PYPOINT(ypivot, pwindow->attributes.height, pwindow->yratio) ;
//OCC186

	mask = GCFont | GCFunction | GCForeground | GCBackground ;
	mask |= GCLineWidth ;
	XGetGCValues(_DISPLAY,pwindow->qgwind.gccopy,mask,&gc_values) ;
	pbuffer->code = 0 ;
	if( width > 0 ) {
#ifdef BUG	/* Conflicting with SETFONT */
	    pbuffer->code = QGSETWIDTH(pbuffer->code,width) ;
#endif
	    gc_values.line_width = _WIDTHMAP->widths[width] ;
	    /* gc_values.line_style = LineSolid ; */
	}
	if( color > 0 ) {
	    pbuffer->code = QGSETCOLOR(pbuffer->code,color) ;
	    gc_values.foreground = _COLORMAP->pixels[color] ;
	}
	if( font >= 0 ) {
	    pbuffer->code = QGSETFONT(pbuffer->code,font) ;
	    gc_values.font = _FONTMAP->fonts[font]->fid ;
	}
	if( drawmode == XW_REPLACE ) {
	    pbuffer->code = QGSETMODE(pbuffer->code,XW_REPLACE) ;
	    gc_values.function = GXcopy ;
	} else if( drawmode == XW_XORBACK ) {
	    pbuffer->code = QGSETMODE(pbuffer->code,XW_XORBACK) ;
	    gc_values.function = GXxor ;
	    gc_values.foreground ^= gc_values.background ;
	}
	XChangeGC(_DISPLAY,pbuffer->gcf,mask,&gc_values) ;
	gc_values.function = GXcopy ;
	gc_values.foreground = gc_values.background ;
	XChangeGC(_DISPLAY,pbuffer->gcb,mask,&gc_values) ;

#ifdef CTS50025
        Xw_set_text_attrib (pwindow,color,0,font,XW_REPLACE);
#endif

#ifdef  TRACE_DRAW_BUFFER
	if( Xw_get_trace() ) {
	  printf (" Xw_open_buffer(%lx,%d,%f,%f)\n",
				(long ) pwindow,bufferid,xpivot,ypivot) ;
	}
#endif
	return (XW_SUCCESS);
}

/*
   XW_STATUS Xw_close_buffer (awindow,bufferid)
   XW_EXT_WINDOW *awindow
   int bufferid
				
	Close the retain buffer

	Returns ERROR if the extended window address is badly defined
		or Buffer is not opened 
	Returns SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_buffer (void* awindow,int bufferid)
#else
XW_STATUS Xw_close_buffer (awindow,bufferid)
void *awindow;
int bufferid ;
#endif /*PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
//int i ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_close_buffer",pwindow) ;
            return (XW_ERROR) ;
        }

	if( bufferid > 0 ) {
	  pbuffer = Xw_get_buffer_structure(pwindow,bufferid) ;
	  if( !pbuffer ) {
            /*ERROR*BUFFER is not opened*/
            Xw_set_error(119,"Xw_close_buffer",&bufferid) ;
            return (XW_ERROR) ;
	  }
	} else if( bufferid < 0 ) {
	  pbuffer = &_BUFFER(-bufferid) ;
	  if( !pbuffer->bufferid ) {
            /*ERROR*BUFFER is not opened*/
            return (XW_SUCCESS) ;
	  }
	} else {
	  pbuffer = &_BUFFER(0) ;
	}

	if( !pbuffer->isempty ) {
	  Xw_clear_buffer(pwindow,bufferid) ;
	}

	if( pbuffer->gcf ) {
	  XFreeGC(_DISPLAY,pbuffer->gcf) ;
	  pbuffer->gcf = NULL ;
	}
	if( pbuffer->gcb ) {
	  XFreeGC(_DISPLAY,pbuffer->gcb) ;
	  pbuffer->gcb = NULL ;
	}
	pbuffer->code = 0 ;
	pbuffer->bufferid = 0;
	Xw_del_text_desc_structure(pbuffer) ;
	Xw_del_line_desc_structure(pbuffer) ;
	Xw_del_image_structure(pbuffer) ;
	Xw_del_point_structure(pbuffer) ;
	Xw_del_segment_structure(pbuffer) ;
	Xw_del_polyline_structure(pbuffer) ;
	Xw_del_arc_structure(pbuffer) ;
	Xw_del_polyarc_structure(pbuffer) ;
	Xw_del_polygone_structure(pbuffer) ;
	Xw_del_text_structure(pbuffer) ;
	Xw_del_polytext_structure(pbuffer) ;
	Xw_del_lmarker_structure(pbuffer) ;
	Xw_del_pmarker_structure(pbuffer) ;
	
#ifdef  TRACE_DRAW_BUFFER
	if( Xw_get_trace() ) {
	  printf (" Xw_close_buffer(%lx,%d)\n",(long ) pwindow,bufferid) ;
	}
#endif
	return (XW_SUCCESS);
}

/*
   XW_STATUS Xw_set_buffer (awindow,bufferid)
   XW_EXT_WINDOW *awindow
   int bufferid
				
	Activate the retain buffer for drawing

	Returns ERROR if the extended window address is badly defined
		or Buffer is not opened 
	Returns SUCCESS if successful 

*/
#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_buffer (void* awindow,int bufferid)
#else
XW_STATUS Xw_set_buffer (awindow,bufferid)
void *awindow;
int bufferid ;
#endif /*PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
int i ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_set_buffer",pwindow) ;
            return (XW_ERROR) ;
        }

	if( bufferid > 0 ) {
	  for( i=1 ; i<MAXBUFFERS ; i++ ) {
	    pbuffer = &_BUFFER(i) ;
	    if( bufferid == pbuffer->bufferid ) break ;
	  }
	  if( i < MAXBUFFERS ) {
	    _BINDEX = i ;
	    if( QGMODE(pbuffer->code) == XW_REPLACE ) {
	      pbuffer->code = QGSETTYPE(pbuffer->code,XW_DISABLE) ;
	    } else {
	      if( _PIXMAP || _NWBUFFER > 0 ) {
	        pbuffer->code = QGSETTYPE(pbuffer->code,XW_ENABLE) ;
	      } else {
	        pbuffer->code = QGSETTYPE(pbuffer->code,XW_DISABLE) ;
	      }
	    }
	  } else {
            /*ERROR*BUFFER is not opened*/
            Xw_set_error(119,"Xw_set_buffer",&bufferid) ;
            return (XW_ERROR) ;
	  }
	} else {
	  _BINDEX = 0 ;
	}

#ifdef  TRACE_DRAW_BUFFER
	if( Xw_get_trace() ) {
	  printf (" Xw_set_buffer(%lx,%d)\n",(long ) pwindow,bufferid) ;
	}
#endif
	return (XW_SUCCESS);
}

/*
   XW_STATUS Xw_get_buffer_status (awindow,bufferid,isopen,isdrawn,isempty)
   XW_EXT_WINDOW *awindow
   int	bufferid
   int  *isopen
   int  *isdrawn
   int  *isempty

	Get the status info of one retain buffer

	Returns ERROR if the extended window address is badly defined
	Returns SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_buffer_status (void* awindow,int bufferid,int* isopen,int* isdrawn,int* isempty)
#else
XW_STATUS Xw_get_buffer_status (awindow,bufferid,isopen,isdrawn,isempty)
void *awindow;
int bufferid ;
int* isopen;
int* isdrawn;
int* isempty;
#endif /*PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_BUFFER *pbuffer ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_buffer_status",pwindow) ;
            return (XW_ERROR) ;
        }

	pbuffer = Xw_get_buffer_structure(pwindow,bufferid) ;

	*isopen = *isdrawn = *isempty = False ;

	if( pbuffer ) {
	  *isopen = True ;
	  *isdrawn = pbuffer->isdrawn ;
	  *isempty = pbuffer->isempty ;
	}

#ifdef  TRACE_DRAW_BUFFER
        if( Xw_get_trace() ) {
          printf (" Xw_get_buffer_status(%lx,%d,%d,%d,%d)\n",
                              (long ) pwindow,bufferid,*isopen,*isdrawn,*isempty) ;
        }
#endif
	return (XW_SUCCESS);
 
}

/*
   XW_STATUS Xw_get_buffer_info (awindow,bufferid,xpivot,ypivot,xscale,yscale,angle)
   XW_EXT_WINDOW *awindow
   int	bufferid
   float *xpivot	returned buffer position
   float *ypivot	returned buffer position
   float *xscale	returned buffer scale 
   float *yscale	returned buffer scale 
   float *angle		returned buffer orientation 

	Get the transform infos of one retain buffer

	Returns ERROR if the extended window address is badly defined
	Returns SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_buffer_info (void* awindow,int bufferid,float *xpivot,float *ypivot,float *xscale,float *yscale,float *angle)
#else
XW_STATUS Xw_get_buffer_info (awindow,bufferid,xpivot,ypivot,xscale,yscale,angle)
void *awindow;
int bufferid ;
float *xpivot,*ypivot ;
float *xscale,*yscale ;
float *angle ;
#endif /*PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_BUFFER *pbuffer ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_buffer_info",pwindow) ;
            return (XW_ERROR) ;
        }

	pbuffer = Xw_get_buffer_structure(pwindow,bufferid) ;
	if( !pbuffer ) {
            /*ERROR*BUFFER is not opened*/
            Xw_set_error(119,"Xw_get_buffer_info",&bufferid) ;
            return (XW_ERROR) ;
	}

	*xpivot = UXPOINT(pbuffer->xpivot+pbuffer->dxpivot) ;
	*ypivot = UYPOINT(pbuffer->ypivot+pbuffer->dypivot) ;
	*xscale = pbuffer->xscale*pbuffer->dxscale ;
	*yscale = pbuffer->yscale*pbuffer->dyscale ;
	*angle = pbuffer->angle+pbuffer->dangle ;

#ifdef  TRACE_DRAW_BUFFER
        if( Xw_get_trace() ) {
          printf (" Xw_get_buffer_info(%lx,%d,%f,%f,%f,%f,%f)\n",
                     (long ) pwindow,bufferid,*xpivot,*ypivot,*xscale,*yscale,*angle) ;
        }
#endif
	return (XW_SUCCESS);
 
}

#ifdef XW_PROTOTYPE
XW_EXT_BUFFER* Xw_get_buffer_structure(XW_EXT_WINDOW *pwindow,int bufferid)
#else
XW_EXT_BUFFER* Xw_get_buffer_structure(pwindow,bufferid)
XW_EXT_WINDOW *pwindow;
int bufferid;
#endif /*XW_PROTOTYPE*/
{
int i ;

	if( bufferid <= 0 ) return NULL ;

	for( i=1 ; i<MAXBUFFERS ; i++ ) {
	   if( _BUFFER(i).bufferid == bufferid ) break ;
	}

	if( i < MAXBUFFERS ) return &_BUFFER(i) ;
	else return NULL ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_update_buffer (XW_EXT_WINDOW* pwindow,XW_EXT_BUFFER* pbuffer)
#else
XW_STATUS Xw_update_buffer (pwindow,pbuffer)
XW_EXT_WINDOW *pwindow;
XW_EXT_BUFFER *pbuffer;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_IMAGE *pimaglist ;
XW_EXT_POLY *ppolylist ;
XW_EXT_LINE *plinelist ;
XW_EXT_SEGMENT *pseglist ;
XW_EXT_LTEXT *pltextlist ;
XW_EXT_PTEXT *pptextlist ;
XW_EXT_ARC *parclist ;
XW_EXT_PMARKER *ppmarklist ;
XW_EXT_LMARKER *plmarklist ;
XW_EXT_POINT *ppntlist ;
XW_EXT_POINT *plinedesc ;
int i ;

	if( !pbuffer->isupdated ) return XW_ERROR ;

	for( pimaglist = pbuffer->pimaglist ; pimaglist ;
			pimaglist = (XW_EXT_IMAGE*)pimaglist->link ) {
	    if( pimaglist->nimage > 0 ) {
	      pimaglist->isupdated = False ;
	      for( i=0 ; i<pimaglist->nimage ; i++ ) {
		pimaglist->rpoints[i].x = pimaglist->upoints[i].x ;
		pimaglist->rpoints[i].y = pimaglist->upoints[i].y ;
	      }
	    } else break ;
	}

	for( parclist = pbuffer->pparclist ; parclist ;
			parclist = (XW_EXT_ARC*)parclist->link ) {
	    if( parclist->narc > 0 ) {
	      parclist->isupdated = False ;
	      for( i=0 ; i<parclist->narc ; i++ ) {
		parclist->rarcs[i].x = parclist->uarcs[i].x ;
		parclist->rarcs[i].y = parclist->uarcs[i].y ;
		parclist->rarcs[i].width = parclist->uarcs[i].width ;
		parclist->rarcs[i].height = parclist->uarcs[i].height ;
		parclist->rarcs[i].angle1 = parclist->uarcs[i].angle1 ;
		parclist->rarcs[i].angle2 = parclist->uarcs[i].angle2 ;
	
	      }
	    } else break ;
	}

	for( pseglist = pbuffer->pseglist ; pseglist ;
			pseglist = (XW_EXT_SEGMENT*)pseglist->link ) {
	    if( pseglist->nseg > 0 ) {
	      pseglist->isupdated = False ;
	      for( i=0 ; i<pseglist->nseg ; i++ ) {
	        pseglist->rsegments[i].x1 = pseglist->usegments[i].x1 ;
	        pseglist->rsegments[i].y1 = pseglist->usegments[i].y1 ;
	        pseglist->rsegments[i].x2 = pseglist->usegments[i].x2 ;
	        pseglist->rsegments[i].y2 = pseglist->usegments[i].y2 ;
	      }
	    } else break ;
	}

	for( parclist = pbuffer->plarclist ; parclist ;
			parclist = (XW_EXT_ARC*)parclist->link ) {
	    if( parclist->narc > 0 ) {
	      parclist->isupdated = False ;
	      for( i=0 ; i<parclist->narc ; i++ ) {
		parclist->rarcs[i].x = parclist->uarcs[i].x ;
		parclist->rarcs[i].y = parclist->uarcs[i].y ;
		parclist->rarcs[i].width = parclist->uarcs[i].width ;
		parclist->rarcs[i].height = parclist->uarcs[i].height ;
		parclist->rarcs[i].angle1 = parclist->uarcs[i].angle1 ;
		parclist->rarcs[i].angle2 = parclist->uarcs[i].angle2 ;
	      }
	    } else break ;
	}

	for( pltextlist = pbuffer->pltextlist ; pltextlist ;
			pltextlist = (XW_EXT_LTEXT*)pltextlist->link ) {
	    if( pltextlist->ntext > 0 ) {
	      pltextlist->isupdated = False ;
	      for( i=0 ; i<pltextlist->ntext ; i++ ) {
		pltextlist->rpoints[i].x = pltextlist->upoints[i].x ;
		pltextlist->rpoints[i].y = pltextlist->upoints[i].y ;
		pltextlist->rangles[i] = pltextlist->uangles[i] ;
		pltextlist->rscalex[i] = pltextlist->uscalex[i] ;
		pltextlist->rscaley[i] = pltextlist->uscaley[i] ;
	      }
	    } else break ;
	}

	for( pptextlist = pbuffer->pptextlist ; pptextlist ;
			pptextlist = (XW_EXT_PTEXT*)pptextlist->link ) {
	    if( pptextlist->ntext > 0 ) {
	      pptextlist->isupdated = False ;
	      for( i=0 ; i<pptextlist->ntext ; i++ ) {
		pptextlist->rpoints[i].x = pptextlist->upoints[i].x ;
		pptextlist->rpoints[i].y = pptextlist->upoints[i].y ;
		pptextlist->rangles[i] = pptextlist->uangles[i] ;
		pptextlist->rscalex[i] = pptextlist->uscalex[i] ;
		pptextlist->rscaley[i] = pptextlist->uscaley[i] ;
	      }
	    } else break ;
	}

	for( ppmarklist = pbuffer->ppmarklist ; ppmarklist ;
			ppmarklist = (XW_EXT_PMARKER*)ppmarklist->link ) {
	    if( ppmarklist->nmark > 0 ) {
	      ppmarklist->isupdated = False ;
	      for( i=0 ; i<ppmarklist->nmark ; i++ ) {
		ppmarklist->rcenters[i].x = ppmarklist->ucenters[i].x ;
		ppmarklist->rcenters[i].y = ppmarklist->ucenters[i].y ;
	      }
	      for( i=0 ; i<ppmarklist->npoint ; i++ ) {
		ppmarklist->rpoints[i].x = ppmarklist->upoints[i].x ;
		ppmarklist->rpoints[i].y = ppmarklist->upoints[i].y ;
	      }
	    } else break ;
	}

	for( plmarklist = pbuffer->plmarklist ; plmarklist ;
			plmarklist = (XW_EXT_LMARKER*)plmarklist->link ) {
	    if( plmarklist->nmark > 0 ) {
	      plmarklist->isupdated = False ;
	      for( i=0 ; i<plmarklist->nmark ; i++ ) {
		plmarklist->rcenters[i].x = plmarklist->ucenters[i].x ;
		plmarklist->rcenters[i].y = plmarklist->ucenters[i].y ;
	      }
	      for( i=0 ; i<plmarklist->nseg ; i++ ) {
		plmarklist->rsegments[i].x1 = plmarklist->usegments[i].x1 ;
		plmarklist->rsegments[i].y1 = plmarklist->usegments[i].y1 ;
		plmarklist->rsegments[i].x2 = plmarklist->usegments[i].x2 ;
		plmarklist->rsegments[i].y2 = plmarklist->usegments[i].y2 ;
	      }
	    } else break ;
	}

	for( ppntlist = pbuffer->ppntlist ; ppntlist ;
			ppntlist = (XW_EXT_POINT*)ppntlist->link ) {
	    if( ppntlist->npoint > 0 ) {
	      ppntlist->isupdated = False ;
	      for( i=0 ; i<ppntlist->npoint ; i++ ) {
		ppntlist->rpoints[i].x = ppntlist->upoints[i].x ;
		ppntlist->rpoints[i].y = ppntlist->upoints[i].y ;
	      }
	    } else break ;
	}

	for( ppolylist = pbuffer->ppolylist ; ppolylist ;
			ppolylist = (XW_EXT_POLY*)ppolylist->link ) {
	    if( ppolylist->npoly > 0 ) {
		ppolylist->isupdated = False ;
	    } else break ;
	}

	for( plinelist = pbuffer->plinelist ; plinelist ;
			plinelist = (XW_EXT_LINE*)plinelist->link ) {
	    if( plinelist->nline > 0 ) {
		plinelist->isupdated = False ;
	    } else break ;
	}

	for( plinedesc = pbuffer->plinedesc ; plinedesc ;
			plinedesc = (XW_EXT_POINT*)plinedesc->link ) {
	    if( plinedesc->npoint > 0 ) {
	      plinedesc->isupdated = False ;
	      for( i=0 ; i<plinedesc->npoint ; i++ ) {
		plinedesc->rpoints[i].x = plinedesc->upoints[i].x ;
		plinedesc->rpoints[i].y = plinedesc->upoints[i].y ;
	      }
	    } else break ;
	}

	pbuffer->xpivot += pbuffer->dxpivot ;
	pbuffer->ypivot += pbuffer->dypivot ;
	pbuffer->dxpivot = pbuffer->dypivot = 0 ;

	pbuffer->angle += pbuffer->dangle ;
	pbuffer->dangle = 0. ;

	pbuffer->xscale *= pbuffer->dxscale ;
	pbuffer->yscale *= pbuffer->dyscale ;
	pbuffer->dxscale = pbuffer->dyscale = 1. ;

	pbuffer->rxmin = pbuffer->uxmin ;
	pbuffer->rymin = pbuffer->uymin ;
	pbuffer->rxmax = pbuffer->uxmax ;
	pbuffer->rymax = pbuffer->uymax ;

	pbuffer->isupdated = UNKNOWN_BUFFER ;

#ifdef  TRACE_DRAW_BUFFER
        if( Xw_get_trace() ) {
          printf (" Xw_update_buffer(%lx,%d)\n",(long ) pwindow,pbuffer->bufferid) ;
        }
#endif

	return (XW_SUCCESS);
}

/*
   STATUS Xw_move_buffer (awindow,bufferid,xpivot,ypivot)
   XW_EXT_WINDOW *awindow
   int bufferid
   float xpivot,ypivot	new DWU pivot point location
				
	Move & display one retain buffer of primitives at screen

	Returns ERROR if the extended window address is badly defined
		or Buffer is empty 
		or Buffer is not opened
	Returns SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_move_buffer (void* awindow,int bufferid,float pivotx,float pivoty)
#else
XW_STATUS Xw_move_buffer (awindow,bufferid,pivotx,pivoty)
void *awindow;
int bufferid;
float pivotx,pivoty;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
XW_EXT_IMAGE *pimaglist ;
XW_EXT_POLY *ppolylist ;
XW_EXT_LINE *plinelist ;
XW_EXT_SEGMENT *pseglist ;
XW_EXT_LTEXT *pltextlist ;
XW_EXT_PTEXT *pptextlist ;
XW_EXT_ARC *parclist ;
XW_EXT_PMARKER *ppmarklist ;
XW_EXT_LMARKER *plmarklist ;
XW_EXT_POINT *ppntlist ;
XW_EXT_POINT *plinedesc ;
int i,dx,dy,xpivot,ypivot ;
int rxmin,rymin,rxmax,rymax ;
int uxmin,uymin,uxmax,uymax ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_move_buffer",pwindow) ;
            return (XW_ERROR) ;
        }

	pbuffer = Xw_get_buffer_structure(pwindow,bufferid) ;
	if( !pbuffer ) {
            /*ERROR*BUFFER is not opened*/
            Xw_set_error(119,"Xw_move_buffer",&bufferid) ;
            return (XW_ERROR) ;
	}

	if( pbuffer->isempty ) return XW_ERROR ;

	pbuffer->isretain = True ;
	if( pbuffer->isdrawn ) {
	  Xw_erase_buffer(pwindow,bufferid) ;
	}

	if( pbuffer->isupdated != MOVE_BUFFER ) {
	  if( pbuffer->isupdated ) Xw_update_buffer(pwindow,pbuffer) ;
	}
 
//OCC186
	xpivot = PXPOINT(pivotx, pwindow->xratio) ;
	ypivot = PYPOINT(pivoty, pwindow->attributes.height, pwindow->yratio) ;
//OCC186

	dx = xpivot - pbuffer->xpivot ;
	dy = ypivot - pbuffer->ypivot ;

	rxmin = pbuffer->rxmin ;
	rymin = pbuffer->rymin ;
	rxmax = pbuffer->rxmax ;
	rymax = pbuffer->rymax ;

	if( pbuffer->isupdated ) {
	  uxmin = pbuffer->uxmin ;
	  uymin = pbuffer->uymin ;
	  uxmax = pbuffer->uxmax ;
	  uymax = pbuffer->uymax ;
	} else {
	  uxmin = rxmin ; uymin = rymin ;
	  uxmax = rxmax ; uymax = rymax ;
	}

	if( dx || dy ) {

	  for( pimaglist = pbuffer->pimaglist ; pimaglist ;
			pimaglist = (XW_EXT_IMAGE*)pimaglist->link ) {
	    if( pimaglist->nimage > 0 ) {
	      pimaglist->isupdated = True ;
	      for( i=0 ; i<pimaglist->nimage ; i++ ) {
		pimaglist->upoints[i].x = pimaglist->rpoints[i].x + dx ;
		pimaglist->upoints[i].y = pimaglist->rpoints[i].y + dy ;
	      }
	    } else break ;
	  }

	  for( parclist = pbuffer->pparclist ; parclist ;
			parclist = (XW_EXT_ARC*)parclist->link ) {
	    if( parclist->narc > 0 ) {
	      parclist->isupdated = True ;
	      for( i=0 ; i<parclist->narc ; i++ ) {
		parclist->uarcs[i].x = parclist->rarcs[i].x + dx ;
		parclist->uarcs[i].y = parclist->rarcs[i].y + dy ;
		parclist->uarcs[i].width = parclist->rarcs[i].width ;
		parclist->uarcs[i].height = parclist->rarcs[i].height ;
		parclist->uarcs[i].angle1 = parclist->rarcs[i].angle1 ;
		parclist->uarcs[i].angle2 = parclist->rarcs[i].angle2 ;
	
	      }
	    } else break ;
	  }

	  for( pseglist = pbuffer->pseglist ; pseglist ;
			pseglist = (XW_EXT_SEGMENT*)pseglist->link ) {
	    if( pseglist->nseg > 0 ) {
	      pseglist->isupdated = True ;
	      for( i=0 ; i<pseglist->nseg ; i++ ) {
	        pseglist->usegments[i].x1 = pseglist->rsegments[i].x1 + dx ;
	        pseglist->usegments[i].y1 = pseglist->rsegments[i].y1 + dy ;
	        pseglist->usegments[i].x2 = pseglist->rsegments[i].x2 + dx ;
	        pseglist->usegments[i].y2 = pseglist->rsegments[i].y2 + dy ;
	      }
	    } else break ;
	  }

	  for( parclist = pbuffer->plarclist ; parclist ;
			parclist = (XW_EXT_ARC*)parclist->link ) {
	    if( parclist->narc > 0 ) {
	      parclist->isupdated = True ;
	      for( i=0 ; i<parclist->narc ; i++ ) {
		parclist->uarcs[i].x = parclist->rarcs[i].x + dx ;
		parclist->uarcs[i].y = parclist->rarcs[i].y + dy ;
		parclist->uarcs[i].width = parclist->rarcs[i].width ;
		parclist->uarcs[i].height = parclist->rarcs[i].height ;
		parclist->uarcs[i].angle1 = parclist->rarcs[i].angle1 ;
		parclist->uarcs[i].angle2 = parclist->rarcs[i].angle2 ;
	      }
	    } else break ;
	  }

	  for( pltextlist = pbuffer->pltextlist ; pltextlist ;
			pltextlist = (XW_EXT_LTEXT*)pltextlist->link ) {
	    if( pltextlist->ntext > 0 ) {
	      pltextlist->isupdated = True ;
	      for( i=0 ; i<pltextlist->ntext ; i++ ) {
		pltextlist->upoints[i].x = pltextlist->rpoints[i].x + dx ;
		pltextlist->upoints[i].y = pltextlist->rpoints[i].y + dy ;
		pltextlist->uangles[i] = pltextlist->rangles[i] ;
		pltextlist->uscalex[i] = pltextlist->rscalex[i] ;
		pltextlist->uscaley[i] = pltextlist->rscaley[i] ;
	      }
	    } else break ;
	  }

	  for( pptextlist = pbuffer->pptextlist ; pptextlist ;
			pptextlist = (XW_EXT_PTEXT*)pptextlist->link ) {
	    if( pptextlist->ntext > 0 ) {
	      pptextlist->isupdated = True ;
	      for( i=0 ; i<pptextlist->ntext ; i++ ) {
		pptextlist->upoints[i].x = pptextlist->rpoints[i].x + dx ;
		pptextlist->upoints[i].y = pptextlist->rpoints[i].y + dy ;
		pptextlist->uangles[i] = pptextlist->rangles[i] ;
		pptextlist->uscalex[i] = pptextlist->rscalex[i] ;
		pptextlist->uscaley[i] = pptextlist->rscaley[i] ;
	      }
	    } else break ;
	  }

	  for( ppmarklist = pbuffer->ppmarklist ; ppmarklist ;
			ppmarklist = (XW_EXT_PMARKER*)ppmarklist->link ) {
	    if( ppmarklist->nmark > 0 ) {
	      ppmarklist->isupdated = True ;
	      for( i=0 ; i<ppmarklist->nmark ; i++ ) {
		ppmarklist->ucenters[i].x = ppmarklist->rcenters[i].x + dx ;
		ppmarklist->ucenters[i].y = ppmarklist->rcenters[i].y + dy ;
	      }
	      for( i=0 ; i<ppmarklist->npoint ; i++ ) {
		ppmarklist->upoints[i].x = ppmarklist->rpoints[i].x + dx ;
		ppmarklist->upoints[i].y = ppmarklist->rpoints[i].y + dy ;
	      }
	    } else break ;
	  }

	  for( plmarklist = pbuffer->plmarklist ; plmarklist ;
			plmarklist = (XW_EXT_LMARKER*)plmarklist->link ) {
	    if( plmarklist->nmark > 0 ) {
	      plmarklist->isupdated = True ;
	      for( i=0 ; i<plmarklist->nmark ; i++ ) {
		plmarklist->ucenters[i].x = plmarklist->rcenters[i].x + dx ;
		plmarklist->ucenters[i].y = plmarklist->rcenters[i].y + dy ;
	      }
	      for( i=0 ; i<plmarklist->nseg ; i++ ) {
		plmarklist->usegments[i].x1 = plmarklist->rsegments[i].x1 + dx ;
		plmarklist->usegments[i].y1 = plmarklist->rsegments[i].y1 + dy ;
		plmarklist->usegments[i].x2 = plmarklist->rsegments[i].x2 + dx ;
		plmarklist->usegments[i].y2 = plmarklist->rsegments[i].y2 + dy ;
	      }
	    } else break ;
	  }

	  for( ppntlist = pbuffer->ppntlist ; ppntlist ;
			ppntlist = (XW_EXT_POINT*)ppntlist->link ) {
	    if( ppntlist->npoint > 0 ) {
	      ppntlist->isupdated = True ;
	      for( i=0 ; i<ppntlist->npoint ; i++ ) {
		ppntlist->upoints[i].x = ppntlist->rpoints[i].x + dx ;
		ppntlist->upoints[i].y = ppntlist->rpoints[i].y + dy ;
	      }
	    } else break ;
	  }

	  for( ppolylist = pbuffer->ppolylist ; ppolylist ;
			ppolylist = (XW_EXT_POLY*)ppolylist->link ) {
	    if( ppolylist->npoly > 0 ) {
		ppolylist->isupdated = True ;
	    } else break ;
	  }

	  for( plinelist = pbuffer->plinelist ; plinelist ;
			plinelist = (XW_EXT_LINE*)plinelist->link ) {
	    if( plinelist->nline > 0 ) {
		plinelist->isupdated = True ;
	    } else break ;
	  }

	  for( plinedesc = pbuffer->plinedesc ; plinedesc ;
			plinedesc = (XW_EXT_POINT*)plinedesc->link ) {
	    if( plinedesc->npoint > 0 ) {
	      plinedesc->isupdated = True ;
	      for( i=0 ; i<plinedesc->npoint ; i++ ) {
		plinedesc->upoints[i].x = plinedesc->rpoints[i].x + dx ;
		plinedesc->upoints[i].y = plinedesc->rpoints[i].y + dy ;
	      }
	    } else break ;
	  }

	  pbuffer->dxpivot = dx ;
	  pbuffer->dypivot = dy ;

	  pbuffer->uxmin = pbuffer->rxmin + dx ;
	  pbuffer->uymin = pbuffer->rymin + dy ;
	  pbuffer->uxmax = pbuffer->rxmax + dx ;
	  pbuffer->uymax = pbuffer->rymax + dy ;

	  uxmin = min(uxmin,pbuffer->uxmin) ;
	  uymin = min(uymin,pbuffer->uymin) ;
	  uxmax = max(uxmax,pbuffer->uxmax) ;
	  uymax = max(uymax,pbuffer->uymax) ;

	  pbuffer->isupdated = MOVE_BUFFER ;

	}

	Xw_draw_buffer(pwindow,bufferid) ;

	if( QGTYPE(pbuffer->code) == XW_ENABLE ) {
	  int x = uxmin - 1 ;
	  int y = uymin - 1 ;
	  int w = uxmax - x + 1 ;
	  int h = uymax - y + 1 ;

          if( x < 0 ) x = 0 ;
          if( y < 0 ) y = 0 ;
          if( x+w > _WIDTH ) w = _WIDTH - x ; 
          if( y+h > _HEIGHT ) h = _HEIGHT - y ;

	  if( _NWBUFFER > 0 ) {
            XCopyArea(_DISPLAY,_BWBUFFER,_FWBUFFER,
				pwindow->qgwind.gccopy,x,y,w,h,x,y) ;
	  } else if( _PIXMAP ) {
            XCopyArea(_DISPLAY,_PIXMAP,_WINDOW,
				pwindow->qgwind.gccopy,x,y,w,h,x,y) ;
	  }
          XFlush(_DISPLAY) ;
	}
	pbuffer->isretain = False ;

#ifdef  TRACE_DRAW_BUFFER
	if( Xw_get_trace() ) {
	  printf (" Xw_move_buffer(%lx,%d,%d,%d)\n",
				(long ) pwindow,bufferid,xpivot,ypivot) ;
	}
#endif

	return (XW_SUCCESS);
}

/*
   STATUS Xw_rotate_buffer (awindow,bufferid,angle)
   XW_EXT_WINDOW *awindow
   int bufferid
   float angle	Last orientation since the creation time
				
	Rotate & display one retain buffer of primitives at screen

	Returns ERROR if the extended window address is badly defined
		or Buffer is empty 
		or Buffer is not opened
	Returns SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_rotate_buffer (void* awindow,int bufferid,float angle)
#else
XW_STATUS Xw_rotate_buffer (awindow,bufferid,angle)
void *awindow;
int bufferid;
float angle;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
XW_EXT_IMAGE *pimaglist ;
XW_EXT_POLY *ppolylist ;
XW_EXT_LINE *plinelist ;
XW_EXT_SEGMENT *pseglist ;
XW_EXT_LTEXT *pltextlist ;
XW_EXT_PTEXT *pptextlist ;
XW_EXT_ARC *parclist ;
XW_EXT_PMARKER *ppmarklist ;
XW_EXT_LMARKER *plmarklist ;
XW_EXT_POINT *ppntlist ;
XW_EXT_POINT *plinedesc ;
int i,j,n,x,y,dx,dy,xp,yp,angle1 ;
int rxmin,rymin,rxmax,rymax ;
int uxmin,uymin,uxmax,uymax ;
float a,cosa,sina ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_rotate_buffer",pwindow) ;
            return (XW_ERROR) ;
        }

	pbuffer = Xw_get_buffer_structure(pwindow,bufferid) ;
	if( !pbuffer ) {
            /*ERROR*BUFFER is not opened*/
            Xw_set_error(119,"Xw_rotate_buffer",&bufferid) ;
            return (XW_ERROR) ;
	}

	if( pbuffer->isempty ) return XW_ERROR ;

	pbuffer->isretain = True ;
	if( pbuffer->isdrawn ) {
	  Xw_erase_buffer(pwindow,bufferid) ;
	}

	if( pbuffer->isupdated != ROTATE_BUFFER ) {
	  if( pbuffer->isupdated ) Xw_update_buffer(pwindow,pbuffer) ;
	}

	xp = pbuffer->xpivot ;
	yp = pbuffer->ypivot ;

	rxmin = pbuffer->rxmin ;
	rymin = pbuffer->rymin ;
	rxmax = pbuffer->rxmax ;
	rymax = pbuffer->rymax ;

	if( pbuffer->isupdated ) {
	  uxmin = pbuffer->uxmin ;
	  uymin = pbuffer->uymin ;
	  uxmax = pbuffer->uxmax ;
	  uymax = pbuffer->uymax ;
	} else {
	  uxmin = rxmin ; uymin = rymin ;
	  uxmax = rxmax ; uymax = rymax ;
	}

	if( angle != pbuffer->dangle ) {
	  int dxmin = rxmin - xp ;
	  int dymin = rymin - yp ;
	  int dxmax = rxmax - xp ;
	  int dymax = rymax - yp ;

	  cosa = cos((double)angle) ;
	  sina = sin((double)angle) ;

	  for( pimaglist = pbuffer->pimaglist ; pimaglist ;
			pimaglist = (XW_EXT_IMAGE*)pimaglist->link ) {
	    if( pimaglist->nimage > 0 ) {
	      pimaglist->isupdated = True ;
	      for( i=0 ; i<pimaglist->nimage ; i++ ) {
		x = pimaglist->rpoints[i].x - xp ;
		y = pimaglist->rpoints[i].y - yp ;
		pimaglist->upoints[i].x = (short int )( xp + XROTATE(x,y) );
		pimaglist->upoints[i].y = (short int )( yp + YROTATE(x,y) );
	      }
	    } else break ;
	  }

	  for( parclist = pbuffer->pparclist ; parclist ;
			parclist = (XW_EXT_ARC*)parclist->link ) {
	    if( parclist->narc > 0 ) {
	      parclist->isupdated = True ;
	      for( i=0 ; i<parclist->narc ; i++ ) {
		x = parclist->rarcs[i].x + parclist->rarcs[i].width/2 - xp ;
		y = parclist->rarcs[i].y + parclist->rarcs[i].height/2 - yp ;
		parclist->uarcs[i].x = (short int )( xp + XROTATE(x,y) - 
					(int)parclist->rarcs[i].width/2 );
		parclist->uarcs[i].y = (short int )( yp + YROTATE(x,y) - 
					(int)parclist->rarcs[i].height/2 );
	        angle1 = (int )( parclist->rarcs[i].angle1 + angle*64./DRAD );
        	if( angle1 > 0 ) {
            	  while( angle1 > MAXANGLE ) angle1 -= MAXANGLE ;
        	} else if( angle1 < 0 ) {
            	  while( angle1 < -MAXANGLE ) angle1 += MAXANGLE ;
        	}
		parclist->uarcs[i].angle1 = angle1 ;
		parclist->uarcs[i].angle2 = parclist->rarcs[i].angle2 ;
	      }
	    } else break ;
	  }

	  for( pseglist = pbuffer->pseglist ; pseglist ;
			pseglist = (XW_EXT_SEGMENT*)pseglist->link ) {
	    if( pseglist->nseg > 0 ) {
	      pseglist->isupdated = True ;
	      for( i=0 ; i<pseglist->nseg ; i++ ) {
	        x = pseglist->rsegments[i].x1 - xp ;
	        y = pseglist->rsegments[i].y1 - yp ;
	        pseglist->usegments[i].x1 = (short int )( xp + XROTATE(x,y) );
	        pseglist->usegments[i].y1 = (short int )( yp + YROTATE(x,y) );
	        x = pseglist->rsegments[i].x2 - xp ;
	        y = pseglist->rsegments[i].y2 - yp ;
	        pseglist->usegments[i].x2 = (short int )( xp + XROTATE(x,y) );
	        pseglist->usegments[i].y2 = (short int )( yp + YROTATE(x,y) );
	      }
	    } else break ;
	  }

	  for( parclist = pbuffer->plarclist ; parclist ;
			parclist = (XW_EXT_ARC*)parclist->link ) {
	    if( parclist->narc > 0 ) {
	      parclist->isupdated = True ;
	      for( i=0 ; i<parclist->narc ; i++ ) {
		x = parclist->rarcs[i].x + parclist->rarcs[i].width/2 - xp ;
		y = parclist->rarcs[i].y + parclist->rarcs[i].height/2 - yp ;
		parclist->uarcs[i].x = (short int )( xp + XROTATE(x,y) - 
					(int)parclist->rarcs[i].width/2 );
		parclist->uarcs[i].y = (short int )( yp + YROTATE(x,y) - 
					(int)parclist->rarcs[i].height/2 );
	        angle1 = (int )( parclist->rarcs[i].angle1 + angle*64./DRAD );
        	if( angle1 > 0 ) {
            	  while( angle1 > MAXANGLE ) angle1 -= MAXANGLE ;
        	} else if( angle1 < 0 ) {
            	  while( angle1 < -MAXANGLE ) angle1 += MAXANGLE ;
        	}
		parclist->uarcs[i].angle1 = angle1 ;
		parclist->uarcs[i].angle2 = parclist->rarcs[i].angle2 ;
	      }
	    } else break ;
	  }

	  for( pltextlist = pbuffer->pltextlist ; pltextlist ;
			pltextlist = (XW_EXT_LTEXT*)pltextlist->link ) {
	    if( pltextlist->ntext > 0 ) {
	      pltextlist->isupdated = True ;
	      for( i=0 ; i<pltextlist->ntext ; i++ ) {
		x = pltextlist->rpoints[i].x - xp ;
		y = pltextlist->rpoints[i].y - yp ;
		pltextlist->upoints[i].x = (short int )( xp + XROTATE(x,y) );
		pltextlist->upoints[i].y = (short int )( yp + YROTATE(x,y) );
	        a = pltextlist->rangles[i] + angle ;
        	if( a > 0. ) {
            	  while( a > 2.*M_PI ) a -= 2.*M_PI ;
        	} else if( a < 0. ) {
            	  while( a < -2.*M_PI ) a += 2.*M_PI ;
        	}
		pltextlist->uangles[i] = a ;
		pltextlist->uscalex[i] = pltextlist->rscalex[i] ;
		pltextlist->uscaley[i] = pltextlist->rscaley[i] ;
	      }
	    } else break ;
	  }

	  for( pptextlist = pbuffer->pptextlist ; pptextlist ;
			pptextlist = (XW_EXT_PTEXT*)pptextlist->link ) {
	    if( pptextlist->ntext > 0 ) {
	      pptextlist->isupdated = True ;
	      for( i=0 ; i<pptextlist->ntext ; i++ ) {
		x = pptextlist->rpoints[i].x - xp ;
		y = pptextlist->rpoints[i].y - yp ;
		pptextlist->upoints[i].x = (short int )( xp + XROTATE(x,y) );
		pptextlist->upoints[i].y = (short int )( yp + YROTATE(x,y) );
	        a = pptextlist->rangles[i] + angle ;
        	if( a > 0. ) {
            	  while( a > 2.*M_PI ) a -= 2.*M_PI ;
        	} else if( a < 0. ) {
            	  while( a < -2.*M_PI ) a += 2.*M_PI ;
        	}
		pptextlist->uangles[i] = a ;
		pptextlist->uscalex[i] = pptextlist->rscalex[i] ;
		pptextlist->uscaley[i] = pptextlist->rscaley[i] ;
	      }
	    } else break ;
	  }

	  for( ppmarklist = pbuffer->ppmarklist ; ppmarklist ;
			ppmarklist = (XW_EXT_PMARKER*)ppmarklist->link ) {
	    if( ppmarklist->nmark > 0 ) {
	      ppmarklist->isupdated = True ;
	      for( i=n=0 ; i<ppmarklist->nmark ; i++ ) {
		x = ppmarklist->rcenters[i].x - xp ;
		y = ppmarklist->rcenters[i].y - yp ;
		dx = (int )( XROTATE(x,y) - x );
                dy = (int )( YROTATE(x,y) - y );
		ppmarklist->ucenters[i].x = ppmarklist->rcenters[i].x + dx ;
		ppmarklist->ucenters[i].y = ppmarklist->rcenters[i].y + dy ;
		for( j=0 ; j<ppmarklist->marks[i] ; j++,n++ ) {
		  ppmarklist->upoints[n].x = ppmarklist->rpoints[n].x + dx ;
		  ppmarklist->upoints[n].y = ppmarklist->rpoints[n].y + dy ;
		}
	      }
	    } else break ;
	  }

	  for( plmarklist = pbuffer->plmarklist ; plmarklist ;
			plmarklist = (XW_EXT_LMARKER*)plmarklist->link ) {
	    if( plmarklist->nmark > 0 ) {
	      plmarklist->isupdated = True ;
	      for( i=n=0 ; i<plmarklist->nmark ; i++ ) {
		x = plmarklist->rcenters[i].x - xp ;
		y = plmarklist->rcenters[i].y - yp ;
		dx = (int )( XROTATE(x,y) - x );
                dy = (int )( YROTATE(x,y) - y );
		plmarklist->ucenters[i].x = plmarklist->rcenters[i].x + dx ;
		plmarklist->ucenters[i].y = plmarklist->rcenters[i].y + dy ;
		for( j=0 ; j<plmarklist->marks[i] ; j++,n++ ) {
		  plmarklist->usegments[n].x1 = plmarklist->rsegments[n].x1 + dx ;
		  plmarklist->usegments[n].y1 = plmarklist->rsegments[n].y1 + dy ;
		  plmarklist->usegments[n].x2 = plmarklist->rsegments[n].x2 + dx ;
		  plmarklist->usegments[n].y2 = plmarklist->rsegments[n].y2 + dy ;
		}
	      }
	    } else break ;
	  }

	  for( ppntlist = pbuffer->ppntlist ; ppntlist ;
			ppntlist = (XW_EXT_POINT*)ppntlist->link ) {
	    if( ppntlist->npoint > 0 ) {
	      ppntlist->isupdated = True ;
	      for( i=0 ; i<ppntlist->npoint ; i++ ) {
		x = ppntlist->rpoints[i].x - xp ;
		y = ppntlist->rpoints[i].y - yp ;
		ppntlist->upoints[i].x = (short int )( xp + XROTATE(x,y) );
		ppntlist->upoints[i].y = (short int )( yp + YROTATE(x,y) );
	      }
	    } else break ;
  	  }

	  for( ppolylist = pbuffer->ppolylist ; ppolylist ;
			ppolylist = (XW_EXT_POLY*)ppolylist->link ) {
	    if( ppolylist->npoly > 0 ) {
		ppolylist->isupdated = True ;
	    } else break ;
	  }

	  for( plinelist = pbuffer->plinelist ; plinelist ;
			plinelist = (XW_EXT_LINE*)plinelist->link ) {
	    if( plinelist->nline > 0 ) {
		plinelist->isupdated = True ;
	    } else break ;
	  }
  
	  for( plinedesc = pbuffer->plinedesc ; plinedesc ;
			plinedesc = (XW_EXT_POINT*)plinedesc->link ) {
	    if( plinedesc->npoint > 0 ) {
	      plinedesc->isupdated = True ;
	      for( i=0 ; i<plinedesc->npoint ; i++ ) {
		x = plinedesc->rpoints[i].x - xp ;
		y = plinedesc->rpoints[i].y - yp ;
		plinedesc->upoints[i].x = (short int )( xp + XROTATE(x,y) );
		plinedesc->upoints[i].y = (short int )( yp + YROTATE(x,y) );
	      }
	    } else break ;
	  }

	  pbuffer->dangle = angle ;
	  pbuffer->uxmin = _WIDTH ;
	  pbuffer->uymin = _HEIGHT ;
	  pbuffer->uxmax = 0 ;
	  pbuffer->uymax = 0 ;

	  x = (int )( xp + XROTATE(dxmin,dymin) );
	  pbuffer->uxmin = min(pbuffer->uxmin,x) ;
	  pbuffer->uxmax = max(pbuffer->uxmax,x) ;
	  y = (int )( yp + YROTATE(dxmin,dymin) );
	  pbuffer->uymin = min(pbuffer->uymin,y) ;
	  pbuffer->uymax = max(pbuffer->uymax,y) ;

	  x = (int )( xp + XROTATE(dxmax,dymin) );
	  pbuffer->uxmin = min(pbuffer->uxmin,x) ;
	  pbuffer->uxmax = max(pbuffer->uxmax,x) ;
	  y = (int )( yp + YROTATE(dxmax,dymin) );
	  pbuffer->uymin = min(pbuffer->uymin,y) ;
	  pbuffer->uymax = max(pbuffer->uymax,y) ;

	  x = (int )( xp + XROTATE(dxmin,dymax) );
	  pbuffer->uxmin = min(pbuffer->uxmin,x) ;
	  pbuffer->uxmax = max(pbuffer->uxmax,x) ;
	  y = (int )( yp + YROTATE(dxmin,dymax) );
	  pbuffer->uymin = min(pbuffer->uymin,y) ;
	  pbuffer->uymax = max(pbuffer->uymax,y) ;

	  x = (int )( xp + XROTATE(dxmax,dymax) );
	  pbuffer->uxmin = min(pbuffer->uxmin,x) ;
	  pbuffer->uxmax = max(pbuffer->uxmax,x) ;
	  y = (int )( yp + YROTATE(dxmax,dymax) );
	  pbuffer->uymin = min(pbuffer->uymin,y) ;
	  pbuffer->uymax = max(pbuffer->uymax,y) ;

	  uxmin = min(uxmin,pbuffer->uxmin) ;
	  uymin = min(uymin,pbuffer->uymin) ;
	  uxmax = max(uxmax,pbuffer->uxmax) ;
	  uymax = max(uymax,pbuffer->uymax) ;

	  pbuffer->isupdated = ROTATE_BUFFER ;

	}

	Xw_draw_buffer(pwindow,bufferid) ;

	if( QGTYPE(pbuffer->code) == XW_ENABLE ) {
	  int x = uxmin - 1 ;
	  int y = uymin - 1 ;
	  int w = uxmax - x + 1 ;
	  int h = uymax - y + 1 ;

          if( x < 0 ) x = 0 ;
          if( y < 0 ) y = 0 ;
          if( x+w > _WIDTH ) w = _WIDTH - x ; 
          if( y+h > _HEIGHT ) h = _HEIGHT - y ;

	  if( _NWBUFFER > 0 ) {
            XCopyArea(_DISPLAY,_BWBUFFER,_FWBUFFER,
				pwindow->qgwind.gccopy,x,y,w,h,x,y) ;
	  } else if( _PIXMAP ) {
            XCopyArea(_DISPLAY,_PIXMAP,_WINDOW,
				pwindow->qgwind.gccopy,x,y,w,h,x,y) ;
	  }
          XFlush(_DISPLAY) ;
	}
	pbuffer->isretain = False ;

#ifdef  TRACE_DRAW_BUFFER
	if( Xw_get_trace() ) {
	  printf (" Xw_rotate_buffer(%lx,%d,%f)\n",(long ) pwindow,bufferid,angle) ;
	}
#endif

	return (XW_SUCCESS);
}

/*
   STATUS Xw_scale_buffer (awindow,bufferid,xscale,yscale)
   XW_EXT_WINDOW *awindow
   int bufferid
   float xscale,yscale	Last scale factors since the creation time
				
	scale & display one retain buffer of primitives at screen

	Returns ERROR if the extended window address is badly defined
		or one of scale factor is <= 0.
		or Buffer is empty 
		or Buffer is not opened
	Returns SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_scale_buffer (void* awindow,int bufferid,float xscale,float yscale)
#else
XW_STATUS Xw_scale_buffer (awindow,bufferid,xscale,yscale)
void *awindow;
int bufferid;
float xscale,yscale;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
XW_EXT_IMAGE *pimaglist ;
XW_EXT_POLY *ppolylist ;
XW_EXT_LINE *plinelist ;
XW_EXT_SEGMENT *pseglist ;
XW_EXT_LTEXT *pltextlist ;
XW_EXT_PTEXT *pptextlist ;
XW_EXT_ARC *parclist ;
XW_EXT_PMARKER *ppmarklist ;
XW_EXT_LMARKER *plmarklist ;
XW_EXT_POINT *ppntlist ;
XW_EXT_POINT *plinedesc ;
int i,j,n,x,y,dx,dy,xp,yp ;
int rxmin,rymin,rxmax,rymax ;
int uxmin,uymin,uxmax,uymax ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_scale_buffer",pwindow) ;
            return (XW_ERROR) ;
        }

	pbuffer = Xw_get_buffer_structure(pwindow,bufferid) ;
	if( !pbuffer ) {
            /*ERROR*BUFFER is not opened*/
            Xw_set_error(119,"Xw_scale_buffer",&bufferid) ;
            return (XW_ERROR) ;
	}

	if( xscale <= 0. ) { ;
            /*ERROR*Bad BUFFER scale factor*/
            Xw_set_error(123,"Xw_scale_buffer",&xscale) ;
            return (XW_ERROR) ;
	}

	if( yscale <= 0. ) { ;
            /*ERROR*Bad BUFFER scale factor*/
            Xw_set_error(123,"Xw_scale_buffer",&yscale) ;
            return (XW_ERROR) ;
	}

	if( pbuffer->isempty ) return XW_ERROR ;

	pbuffer->isretain = True ;
	if( pbuffer->isdrawn ) {
	  Xw_erase_buffer(pwindow,bufferid) ;
	}

	if( pbuffer->isupdated != SCALE_BUFFER ) {
	  if( pbuffer->isupdated ) Xw_update_buffer(pwindow,pbuffer) ;
	}

	xp = pbuffer->xpivot ;
	yp = pbuffer->ypivot ;

	rxmin = pbuffer->rxmin ;
	rymin = pbuffer->rymin ;
	rxmax = pbuffer->rxmax ;
	rymax = pbuffer->rymax ;

	if( pbuffer->isupdated ) {
	  uxmin = pbuffer->uxmin ;
	  uymin = pbuffer->uymin ;
	  uxmax = pbuffer->uxmax ;
	  uymax = pbuffer->uymax ;
	} else {
	  uxmin = rxmin ; uymin = rymin ;
	  uxmax = rxmax ; uymax = rymax ;
	}

	if( xscale != pbuffer->xscale || yscale != pbuffer->yscale ) {
	  int dxmin = rxmin - xp ;
	  int dymin = rymin - yp ;
	  int dxmax = rxmax - xp ;
	  int dymax = rymax - yp ;

	  for( pimaglist = pbuffer->pimaglist ; pimaglist ;
			pimaglist = (XW_EXT_IMAGE*)pimaglist->link ) {
	    if( pimaglist->nimage > 0 ) {
	      pimaglist->isupdated = True ;
	      for( i=0 ; i<pimaglist->nimage ; i++ ) {
		x = pimaglist->rpoints[i].x - xp ;
		y = pimaglist->rpoints[i].y - yp ;
		dx = ROUND(xscale*x) - x ; dy = ROUND(yscale*y) - y ;
		pimaglist->upoints[i].x = pimaglist->rpoints[i].x + dx ;
		pimaglist->upoints[i].y = pimaglist->rpoints[i].y + dy ;
	      }
	    } else break ;
	  }

	  for( parclist = pbuffer->pparclist ; parclist ;
			parclist = (XW_EXT_ARC*)parclist->link ) {
	    if( parclist->narc > 0 ) {
	      parclist->isupdated = True ;
	      for( i=0 ; i<parclist->narc ; i++ ) {
		dx = parclist->rarcs[i].width/2 ;
		dy = parclist->rarcs[i].height/2 ;
		x = parclist->rarcs[i].x + dx - xp ;
		y = parclist->rarcs[i].y + dy - yp ;
		x = ROUND(xscale*x) ; y = ROUND(yscale*y) ; 
		dx = ROUND(xscale*dx) ; dy = ROUND(yscale*dy) ;
		parclist->uarcs[i].width = 2*dx ;
		parclist->uarcs[i].height = 2*dy ;
		parclist->uarcs[i].x = xp + x - dx ;
		parclist->uarcs[i].y = yp + y - dy ;
		parclist->uarcs[i].angle1 = parclist->rarcs[i].angle1 ;
		parclist->uarcs[i].angle2 = parclist->rarcs[i].angle2 ;
	      }
	    } else break ;
	  }

	  for( pseglist = pbuffer->pseglist ; pseglist ;
			pseglist = (XW_EXT_SEGMENT*)pseglist->link ) {
	    if( pseglist->nseg > 0 ) {
	      pseglist->isupdated = True ;
	      for( i=0 ; i<pseglist->nseg ; i++ ) {
	        x = pseglist->rsegments[i].x1 - xp ;
	        y = pseglist->rsegments[i].y1 - yp ;
		dx = ROUND(xscale*x) - x ; dy = ROUND(yscale*y) - y ;
	        pseglist->usegments[i].x1 = pseglist->rsegments[i].x1 + dx ;
	        pseglist->usegments[i].y1 = pseglist->rsegments[i].y1 + dy ;
	        x = pseglist->rsegments[i].x2 - xp ;
	        y = pseglist->rsegments[i].y2 - yp ;
		dx = ROUND(xscale*x) - x ; dy = ROUND(yscale*y) - y ;
	        pseglist->usegments[i].x2 = pseglist->rsegments[i].x2 + dx ;
	        pseglist->usegments[i].y2 = pseglist->rsegments[i].y2 + dy ;
	      }
	    } else break ;
	  }

	  for( parclist = pbuffer->plarclist ; parclist ;
			parclist = (XW_EXT_ARC*)parclist->link ) {
	    if( parclist->narc > 0 ) {
	      parclist->isupdated = True ;
	      for( i=0 ; i<parclist->narc ; i++ ) {
		dx = parclist->rarcs[i].width/2 ;
		dy = parclist->rarcs[i].height/2 ;
		x = parclist->rarcs[i].x + dx - xp ;
		y = parclist->rarcs[i].y + dy - yp ;
		x = ROUND(xscale*x) ; y = ROUND(yscale*y) ; 
		dx = ROUND(xscale*dx) ; dy = ROUND(yscale*dy) ;
		parclist->uarcs[i].width = 2*dx ;
		parclist->uarcs[i].height = 2*dy ;
		parclist->uarcs[i].x = xp + x - dx ;
		parclist->uarcs[i].y = yp + y - dy ;
		parclist->uarcs[i].angle1 = parclist->rarcs[i].angle1 ;
		parclist->uarcs[i].angle2 = parclist->rarcs[i].angle2 ;
	      }
	    } else break ;
	  }

	  for( pltextlist = pbuffer->pltextlist ; pltextlist ;
			pltextlist = (XW_EXT_LTEXT*)pltextlist->link ) {
	    if( pltextlist->ntext > 0 ) {
	      pltextlist->isupdated = True ;
	      for( i=0 ; i<pltextlist->ntext ; i++ ) {
		x = pltextlist->rpoints[i].x - xp ;
		y = pltextlist->rpoints[i].y - yp ;
		dx = ROUND(xscale*x) - x ; dy = ROUND(yscale*y) - y ;
		pltextlist->upoints[i].x = pltextlist->rpoints[i].x + dx ;
		pltextlist->upoints[i].y = pltextlist->rpoints[i].y + dy ;
		pltextlist->uangles[i] = pltextlist->rangles[i] ;
		pltextlist->uscalex[i] = pltextlist->rscalex[i] * xscale;
		pltextlist->uscaley[i] = pltextlist->rscaley[i] * yscale; 
	      }
	    } else break ;
	  }

	  for( pptextlist = pbuffer->pptextlist ; pptextlist ;
			pptextlist = (XW_EXT_PTEXT*)pptextlist->link ) {
	    if( pptextlist->ntext > 0 ) {
	      pptextlist->isupdated = True ;
	      for( i=0 ; i<pptextlist->ntext ; i++ ) {
		x = pptextlist->rpoints[i].x - xp ;
		y = pptextlist->rpoints[i].y - yp ;
		dx = ROUND(xscale*x) - x ; dy = ROUND(yscale*y) - y ;
		pptextlist->upoints[i].x = pptextlist->rpoints[i].x + dx ;
		pptextlist->upoints[i].y = pptextlist->rpoints[i].y + dy ;
		pptextlist->uangles[i] = pptextlist->rangles[i] ;
		pptextlist->uscalex[i] = pptextlist->rscalex[i] * xscale;
		pptextlist->uscaley[i] = pptextlist->rscaley[i] * yscale;
	      }
	    } else break ;
	  }

	  for( ppmarklist = pbuffer->ppmarklist ; ppmarklist ;
			ppmarklist = (XW_EXT_PMARKER*)ppmarklist->link ) {
	    if( ppmarklist->nmark > 0 ) {
	      ppmarklist->isupdated = True ;
	      for( i=n=0 ; i<ppmarklist->nmark ; i++ ) {
		x = ppmarklist->rcenters[i].x - xp ;
		y = ppmarklist->rcenters[i].y - yp ;
		dx = ROUND(xscale*x) - x ; dy = ROUND(yscale*y) - y ;
		ppmarklist->ucenters[i].x = ppmarklist->rcenters[i].x + dx ;
		ppmarklist->ucenters[i].y = ppmarklist->rcenters[i].y + dy ;
		for( j=0 ; j<ppmarklist->marks[i] ; j++,n++ ) {
		  ppmarklist->upoints[n].x = ppmarklist->rpoints[n].x + dx ;
		  ppmarklist->upoints[n].y = ppmarklist->rpoints[n].y + dy ;
		}
	      }
	    } else break ;
	  }

	  for( plmarklist = pbuffer->plmarklist ; plmarklist ;
			plmarklist = (XW_EXT_LMARKER*)plmarklist->link ) {
	    if( plmarklist->nmark > 0 ) {
	      plmarklist->isupdated = True ;
	      for( i=n=0 ; i<plmarklist->nmark ; i++ ) {
		x = plmarklist->rcenters[i].x - xp ;
		y = plmarklist->rcenters[i].y - yp ;
		dx = ROUND(xscale*x) - x ; dy = ROUND(yscale*y) - y ;
		plmarklist->ucenters[i].x = plmarklist->rcenters[i].x + dx ;
		plmarklist->ucenters[i].y = plmarklist->rcenters[i].y + dy ;
		for( j=0 ; j<plmarklist->marks[i] ; j++,n++ ) {
		  plmarklist->usegments[n].x1 = plmarklist->rsegments[n].x1 + dx ;
		  plmarklist->usegments[n].y1 = plmarklist->rsegments[n].y1 + dy ;
		  plmarklist->usegments[n].x2 = plmarklist->rsegments[n].x2 + dx ;
		  plmarklist->usegments[n].y2 = plmarklist->rsegments[n].y2 + dy ;
		}
	      }
	    } else break ;
	  }

	  for( ppntlist = pbuffer->ppntlist ; ppntlist ;
			ppntlist = (XW_EXT_POINT*)ppntlist->link ) {
	    if( ppntlist->npoint > 0 ) {
	      ppntlist->isupdated = True ;
	      for( i=0 ; i<ppntlist->npoint ; i++ ) {
		x = ppntlist->rpoints[i].x - xp ;
		y = ppntlist->rpoints[i].y - yp ;
		dx = ROUND(xscale*x) - x ; dy = ROUND(yscale*y) - y ;
		ppntlist->upoints[i].x = ppntlist->rpoints[i].x + dx ;
		ppntlist->upoints[i].y = ppntlist->rpoints[i].y + dy ;
	      }
	    } else break ;
  	  }

	  for( ppolylist = pbuffer->ppolylist ; ppolylist ;
			ppolylist = (XW_EXT_POLY*)ppolylist->link ) {
	    if( ppolylist->npoly > 0 ) {
		ppolylist->isupdated = True ;
	    } else break ;
	  }

	  for( plinelist = pbuffer->plinelist ; plinelist ;
			plinelist = (XW_EXT_LINE*)plinelist->link ) {
	    if( plinelist->nline > 0 ) {
		plinelist->isupdated = True ;
	    } else break ;
	  }
  
	  for( plinedesc = pbuffer->plinedesc ; plinedesc ;
			plinedesc = (XW_EXT_POINT*)plinedesc->link ) {
	    if( plinedesc->npoint > 0 ) {
	      plinedesc->isupdated = True ;
	      for( i=0 ; i<plinedesc->npoint ; i++ ) {
		x = plinedesc->rpoints[i].x - xp ;
		y = plinedesc->rpoints[i].y - yp ;
		dx = ROUND(xscale*x) - x ; dy = ROUND(yscale*y) - y ;
		plinedesc->upoints[i].x = plinedesc->rpoints[i].x + dx ;
		plinedesc->upoints[i].y = plinedesc->rpoints[i].y + dy ;
	      }
	    } else break ;
	  }

	  pbuffer->dxscale = xscale ;
	  pbuffer->dyscale = yscale ;
	  x = ROUND(xscale*dxmin) ; y = ROUND(yscale*dymin) ;
	  pbuffer->uxmin = xp + x ; 
	  pbuffer->uymin = yp + y ;
	  x = ROUND(xscale*dxmax) ; y = ROUND(yscale*dymax) ;
	  pbuffer->uxmax = xp + x ;
	  pbuffer->uymax = yp + y ;

	  uxmin = min(uxmin,pbuffer->uxmin) ;
	  uymin = min(uymin,pbuffer->uymin) ;
	  uxmax = max(uxmax,pbuffer->uxmax) ;
	  uymax = max(uymax,pbuffer->uymax) ;

	  pbuffer->isupdated = SCALE_BUFFER ;

	}

	Xw_draw_buffer(pwindow,bufferid) ;

	if( QGTYPE(pbuffer->code) == XW_ENABLE ) {
	  int x = uxmin - 1 ;
	  int y = uymin - 1 ;
	  int w = uxmax - x + 1 ;
	  int h = uymax - y + 1 ;

          if( x < 0 ) x = 0 ;
          if( y < 0 ) y = 0 ;
          if( x+w > _WIDTH ) w = _WIDTH - x ; 
          if( y+h > _HEIGHT ) h = _HEIGHT - y ;

	  if( _NWBUFFER > 0 ) {
            XCopyArea(_DISPLAY,_BWBUFFER,_FWBUFFER,
				pwindow->qgwind.gccopy,x,y,w,h,x,y) ;
	  } else if( _PIXMAP ) {
            XCopyArea(_DISPLAY,_PIXMAP,_WINDOW,
				pwindow->qgwind.gccopy,x,y,w,h,x,y) ;
	  }
          XFlush(_DISPLAY) ;
	}
	pbuffer->isretain = False ;

#ifdef  TRACE_DRAW_BUFFER
	if( Xw_get_trace() ) {
	  printf (" Xw_scale_buffer(%lx,%d,%f,%f)\n",
				(long ) pwindow,bufferid,xscale,yscale) ;
	}
#endif

	return (XW_SUCCESS);
}
