#define GG002   /*GG_020197
                Renvoyer la hauteur du texte et non de la font
*/

#define GG010   /*GG_150197
                Ameliorer la conformiter de l'underline vis a vis de WORLD
*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DRAW_POLYTEXT
#endif

/*
   STATUS Xw_draw_polytext (awindow,x,y,text,angle,marge,mode):
   XW_EXT_WINDOW *awindow
   float x,y		Position of the beginning of text in world coordinate
   char text[]		String to display
   float angle		Rotated text angle
   float marge		Margin text ratio
   int mode		1 si outline

	Display text in current QG set by set_text_attrib .

	returns ERROR if text length > MAXCHARS
	returns SUCCESS if successful

*/

#define MAXCOORD 32767
#define MINCOORD -32768

#define XROTATE(x,y) (x*cosa + y*sina)
#define YROTATE(x,y) (y*cosa - x*sina)

static XW_EXT_PTEXT *ptextlist ;
static XW_EXT_CHAR *ptextdesc ;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_draw_polytext (void* awindow,float x,float y,
				char* text,float angle,float marge,int mode)
#else
XW_STATUS Xw_draw_polytext (awindow,x,y,text,angle,marge,mode)
void *awindow;
float x,y ;
float angle ;
float marge ;
char *text ;
int mode ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
int ntext,nchar,length,bindex,ix,iy,textindex,textfont,texttype ;
XW_ATTRIB textcode;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_draw_polytext",pwindow) ;
            return (XW_ERROR) ;
        }

	if( marge < 0. || marge > 1. ) {
            /*ERROR*Bad poly text margin*/
            Xw_set_error(124,"Xw_draw_polytext",&marge) ;
            return (XW_ERROR) ;
	}

	length = strlen(text) ;
	if( length+1 > MAXCHARS ) {
	    length = MAXCHARS-1 ;
	    /*ERROR*Too big text string*/
	    Xw_set_error(38,"Xw_draw_polytext",&length) ;
	    return (XW_ERROR) ;
	}

	bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
        for( ptextlist = pbuffer->pptextlist ; ptextlist ;
                    		ptextlist = (XW_EXT_PTEXT*)ptextlist->link ) {
            if( ptextlist->ntext < MAXPTEXTS ) break ;
        }

        if( !ptextlist ) {
                ptextlist = Xw_add_polytext_structure(pbuffer) ;
        }

	if( !ptextlist ) return XW_ERROR ;

        for( ptextdesc = pbuffer->ptextdesc ; ptextdesc ;
                    		ptextdesc = (XW_EXT_CHAR*)ptextdesc->link ) {
            if( ptextdesc->nchar + length < MAXCHARS ) break ;
        }

        if( !ptextdesc ) {
                ptextdesc = Xw_add_text_desc_structure(pbuffer) ;
        }

	if( !ptextdesc ) return XW_ERROR ;

//OCC186
	ix = PXPOINT(x, pwindow->xratio) ;
	iy = PYPOINT(y, pwindow->attributes.height, pwindow->yratio) ;
//OCC186

        ix = max(min(ix,MAXCOORD),MINCOORD);
        iy = max(min(iy,MAXCOORD),MINCOORD);

        textindex = pwindow->textindex ;
        textcode = pwindow->qgtext[textindex].code ;
        textfont = QGFONT(textcode);
        texttype = QGTYPE(textcode);
        if( mode ) texttype |= XW_EXTENDEDTEXT_OUTLINE ;

	ntext = ptextlist->ntext ;
	nchar = ptextdesc->nchar ;
	ptextlist->modes[ntext] = texttype ;
	ptextlist->texts[ntext] = length ;
	ptextlist->ptexts[ntext] = &ptextdesc->chars[nchar] ;
	ptextlist->rpoints[ntext].x = ix ;
	ptextlist->rpoints[ntext].y = iy ;
	ptextlist->rscalex[ntext] = 1. ;
	ptextlist->rscaley[ntext] = 1. ;
	ptextlist->slants[ntext] = 0. ;
	strcpy(ptextlist->ptexts[ntext],text) ;
        if( angle > 0. ) {
            while( angle > 2.*PI ) angle -= 2.*PI ;
        } else if( angle < 0. ) {
            while( angle < -2.*PI ) angle += 2.*PI ;
        }
	ptextlist->rangles[ntext] = angle ;
	ptextlist->marges[ntext] = marge ;
	nchar += length+1 ;
	ptextlist->ntext++ ;
	ptextdesc->nchar = nchar ;

        if( bindex > 0 ) {
	    int dm ;
	    XFontStruct *pfontinfo = _FONTMAP->fonts[textfont] ;
#ifdef GG002
            int xmin,ymin,xmax,ymax,dir,fascent,fdescent;
            XCharStruct overall;
            XTextExtents(pfontinfo,text,length,
                                &dir,&fascent,&fdescent,&overall);
            xmin = overall.lbearing;
            xmax = overall.width;
            ymin = -overall.ascent;
            ymax = overall.descent;
#else
	    int xmin = 0 ;
            int xmax = XTextWidth(pfontinfo,text,length) ;
            int ymin = -pfontinfo->ascent ;
            int ymax = pfontinfo->descent ;
#endif
	    dm = (int )( marge*(ymax-ymin)+0.5 );
	    xmin -= dm ; ymin -= dm ; xmax += dm ; ymax  += dm ;

            pbuffer->isempty = False ;
            if( texttype & XW_EXTENDEDTEXT_UNDERLINE ) {
#ifdef GG010
              if( _FONTMAP->fratios[textfont] <= 0. )
#endif
              ymax += 2*max(2,(ymax-ymin)/8) ;
            }
//            if( abs(angle) > 0. ) {
            if( fabs(angle) > 0. ) {
              float cosa = cos((double)angle) ;
              float sina = sin((double)angle) ;
              int tx,ty ;
              tx = (int )( ix + XROTATE(xmin,ymin) );
              ty = (int )( iy + YROTATE(xmin,ymin) );
	      pbuffer->rxmin = min(pbuffer->rxmin,tx) ;	
	      pbuffer->rymin = min(pbuffer->rymin,ty) ;	
	      pbuffer->rxmax = max(pbuffer->rxmax,tx) ;	
	      pbuffer->rymax = max(pbuffer->rymax,ty) ;	
              tx = (int )( ix + XROTATE(xmax,ymax) );
              ty = (int )( iy + YROTATE(xmax,ymax) );
	      pbuffer->rxmin = min(pbuffer->rxmin,tx) ;	
	      pbuffer->rymin = min(pbuffer->rymin,ty) ;	
	      pbuffer->rxmax = max(pbuffer->rxmax,tx) ;	
	      pbuffer->rymax = max(pbuffer->rymax,ty) ;	
              tx = (int )( ix + XROTATE(xmax,ymin) );
	      ty = (int )( iy + YROTATE(xmax,ymin) );
	      pbuffer->rxmin = min(pbuffer->rxmin,tx) ;	
	      pbuffer->rymin = min(pbuffer->rymin,ty) ;	
	      pbuffer->rxmax = max(pbuffer->rxmax,tx) ;	
	      pbuffer->rymax = max(pbuffer->rymax,ty) ;	
              tx = (int )( ix + XROTATE(xmin,ymax) );
	      ty = (int )( iy + YROTATE(xmin,ymax) );
	      pbuffer->rxmin = min(pbuffer->rxmin,tx) ;	
	      pbuffer->rymin = min(pbuffer->rymin,ty) ;	
	      pbuffer->rxmax = max(pbuffer->rxmax,tx) ;	
	      pbuffer->rymax = max(pbuffer->rymax,ty) ;	
            } else {
              pbuffer->rxmin = min(pbuffer->rxmin,ix+xmin) ;
              pbuffer->rymin = min(pbuffer->rymin,iy+ymin) ;
              pbuffer->rxmax = max(pbuffer->rxmax,ix+xmax) ;
              pbuffer->rymax = max(pbuffer->rymax,iy+ymax) ;
	    }
	} else {
	    int polyindex = pwindow->polyindex ;
	    int lineindex = pwindow->lineindex ;
	    GC gctext = pwindow->qgtext[textindex].gc ;
	    GC gcpoly = (QGTILE(pwindow->qgpoly[polyindex].code)) ? 
				pwindow->qgpoly[polyindex].gc : NULL ;
	    GC gcline = (QGTYPE(pwindow->qgpoly[polyindex].code)) ? 
				pwindow->qgline[lineindex].gc : NULL ;
	    Xw_draw_pixel_polytexts(pwindow,ptextlist,
				gctext,gcpoly,gcline,textcode) ;
    	    ptextlist->ntext = 0 ;
	    ptextdesc->nchar = 0 ;
	}


#ifdef  TRACE_DRAW_POLYTEXT
if( Xw_get_trace() > 2 ) {
    printf(" Xw_draw_polytext(%lx,%f,%f,'%s',%f,%f)\n",
					(long ) pwindow,x,y,text,angle,marge) ;
}
#endif

	return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
void Xw_draw_pixel_polytexts(XW_EXT_WINDOW* pwindow,XW_EXT_PTEXT* ptextlist,
				GC gctext,GC gcpoly,GC gcline,XW_ATTRIB code)
#else
void Xw_draw_pixel_polytexts(pwindow,ptextlist,gctext,gcpoly,gcline,code)
XW_EXT_WINDOW *pwindow ;
XW_EXT_PTEXT *ptextlist ;
GC gctext,gcpoly,gcline ;
XW_ATTRIB code ;
#endif /*XW_PROTOTYPE*/
{
int i,ix,iy,mode,length,font = QGFONT(code),type = QGTYPE(code) ;
float angle,marge,cosa,sina ;
char *pchar ;
XPoint points[5] ;

	for( i=0 ; i<ptextlist->ntext ; i++ ) {
	    length = ptextlist->texts[i] ;
	    if( length <= 0 ) continue ;
	    if( ptextlist->isupdated ) {
	      ix = ptextlist->upoints[i].x ;
	      iy = ptextlist->upoints[i].y ;
	      angle = ptextlist->uangles[i] ;
	    } else {
	      ix = ptextlist->rpoints[i].x ;
	      iy = ptextlist->rpoints[i].y ;
	      angle = ptextlist->rangles[i] ;
	    }
	    mode = ptextlist->modes[i] ;
	    marge = ptextlist->marges[i] ;
	    pchar = ptextlist->ptexts[i] ;
            if( type & XW_EXTENDEDTEXT_MINHEIGHT ) {
//OCC186
              int rcx = max(4,PVALUE(FROMMILLIMETER(_FONTMAP->ssizex[font]),
				     pwindow->xratio, pwindow->yratio)/3);
              int rcy = max(4,PVALUE(FROMMILLIMETER(_FONTMAP->ssizey[font]),
				     pwindow->xratio, pwindow->yratio)/3);
//OCC186
              if( angle == 0. ) {
                XFillArc(_DISPLAY,_DRAWABLE,gctext,
                                        ix-rcx,iy-rcy,2*rcx,2*rcy,0,360*64) ;
              } else {
                int dx,dy ;
                cosa = cos((double)angle) ;
                sina = sin((double)angle) ;
                dx = (int )( XROTATE(0,rcx) );
                dy = (int )( YROTATE(0,rcy) );
                XFillArc(_DISPLAY,_DRAWABLE,gctext,
                                        ix-dx,iy-dy,2*rcx,2*rcy,0,360*64) ;
              }  
            } else {
	      XFontStruct *pfontinfo = _FONTMAP->fonts[font] ;
	      int dm,du = 0;
#ifdef GG002
              int xmin,ymin,xmax,ymax,dir,fascent,fdescent;
              XCharStruct overall;
              XTextExtents(pfontinfo,pchar,length,
                                &dir,&fascent,&fdescent,&overall);
              xmin = overall.lbearing;
              xmax = overall.width;
              ymin = -overall.ascent;
              ymax = overall.descent;
#else
	      int xmin = 0 ;
              int xmax = XTextWidth(pfontinfo,pchar,length) ;
              int ymin = -pfontinfo->ascent ;
              int ymax = pfontinfo->descent ;
#endif
              if( type & XW_EXTENDEDTEXT_UNDERLINE ) {
#ifdef GG010
                if( _FONTMAP->fratios[font] > 0. ) {
//OCC186
                  du = PVALUE(0.6*_FONTMAP->fratios[font]*
                                        FROMMILLIMETER(_FONTMAP->ssizey[font]),
			      pwindow->xratio, pwindow->yratio);
//OCC186
                } else {
                  du = max(2,(ymax-ymin)/8) ;
                  ymax += 2*du ;
                }
#else
                du = max(2,(ymax-ymin)/8) ;
                ymax += 2*du ;
#endif
              }  
	      dm = (int )( marge*(ymax-ymin)+0.5 );
	      xmin -= dm ; ymin -= dm ; xmax += dm ; ymax  += dm ;
	      if( angle == 0. ) {
                if( gcpoly ) {
	          XFillRectangle(_DISPLAY,_DRAWABLE,gcpoly,
					ix+xmin,iy+ymin,xmax-xmin,ymax-ymin) ;
		}
		if( gcline && (gcline != gcpoly) ) {
	          XDrawRectangle(_DISPLAY,_DRAWABLE,gcline,
					ix+xmin,iy+ymin,xmax-xmin,ymax-ymin) ;
		}
                if( type & XW_EXTENDEDTEXT_UNDERLINE ) {
#ifdef GG010
                  xmin += dm ; xmax -= dm ; ymax  = du ;
#else
                  xmin += dm ; xmax -= dm ; ymax  -= dm + du ;
#endif
                  XDrawLine(_DISPLAY,_DRAWABLE,gctext,
                                        ix+xmin,iy+ymax,ix+xmax,iy+ymax) ;
                } 
	        XDrawString(_DISPLAY,_DRAWABLE,gctext,ix,iy,pchar,length) ;
	      } else {
		int j,tx,ty,w ;
                float dx,dy ;

		XFontStruct *pfontinfo = _FONTMAP->fonts[font] ;
		cosa = cos((double)angle) ;
		sina = sin((double)angle) ;
                dx = XROTATE(xmin,ymin) ;
                dy = YROTATE(xmin,ymin) ;
                points[0].x = ix + ROUND(dx) ;
                points[0].y = iy + ROUND(dy) ;
                dx = XROTATE(xmax,ymin) ;
                dy = YROTATE(xmax,ymin) ;
                points[1].x = ix + ROUND(dx) ;
                points[1].y = iy + ROUND(dy) ;
                dx = XROTATE(xmax,ymax) ;
                dy = YROTATE(xmax,ymax) ;
                points[2].x = ix + ROUND(dx) ;
                points[2].y = iy + ROUND(dy) ;
                dx = XROTATE(xmin,ymax) ;
                dy = YROTATE(xmin,ymax) ;
                points[3].x = ix + ROUND(dx) ;
                points[3].y = iy + ROUND(dy) ;
                points[4].x = points[0].x ;
                points[4].y = points[0].y ;
                if( gcpoly ) {
		  XFillPolygon(_DISPLAY,_DRAWABLE,gcpoly,points,4,
						Convex,CoordModeOrigin) ;
		}
		if( gcline && (gcline != gcpoly) ) {
		  XDrawLines(_DISPLAY,_DRAWABLE,gcline,points,5,
						CoordModeOrigin) ;
		}
                if( type & XW_EXTENDEDTEXT_UNDERLINE ) {
                  int x1,y1,x2,y2 ;
#ifdef GG010
                  xmin += dm ; xmax -= dm ; ymax  = du ;
#else
                  xmin += dm ; xmax -= dm ; ymax  -= dm + du ;
#endif
                  x1 = (int )( XROTATE(xmin,ymax) );
                  y1 = (int )( YROTATE(xmin,ymax) );
                  x2 = (int )( XROTATE(xmax,ymax) );
                  y2 = (int )( YROTATE(xmax,ymax) );
                  XDrawLine(_DISPLAY,_DRAWABLE,gctext,ix+x1,iy+y1,ix+x2,iy+y2) ;                } 
		for( j=tx=ty=0 ; j<length ; j++ ) {
        	    XDrawString(_DISPLAY,_DRAWABLE,gctext,
						ix+tx,iy-ty,&pchar[j],1) ;
                    w = XTextWidth(pfontinfo,pchar,j);
		    tx = ROUND(w*cosa) ;
		    ty = ROUND(w*sina) ;
    	 	}
	      }
	    }
	}
}

#ifdef XW_PROTOTYPE
XW_EXT_PTEXT* Xw_add_polytext_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_PTEXT* Xw_add_polytext_structure(pbuflist )
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert at end one Extended polytext structure in the
        text List

        returns Extended text address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_PTEXT *ptext ;

        ptext = (XW_EXT_PTEXT*) Xw_malloc(sizeof(XW_EXT_PTEXT)) ;
        if( ptext ) {
            ptext->link = pbuflist->pptextlist ;
            ptext->isupdated = 0 ;
            ptext->ntext = 0 ;
	    pbuflist->pptextlist = ptext ;
        } else {
	    /*ERROR*EXT_TEXT Allocation failed*/
	    Xw_set_error(39,"Xw_add_polytext_structure",NULL) ;
        }

        return (ptext) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_polytext_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_polytext_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended polytext structure in the
        text List

        SUCCESS always
*/
{
XW_EXT_PTEXT *ptext,*qtext ;

        for( ptext = pbuflist->pptextlist ; ptext ; ptext = qtext ) {
            qtext = (XW_EXT_PTEXT*)ptext->link ;
            Xw_free(ptext) ;
        }
	pbuflist->pptextlist = NULL ;

        return (XW_SUCCESS) ;
}
