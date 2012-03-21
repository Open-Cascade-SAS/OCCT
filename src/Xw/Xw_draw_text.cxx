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

#define GG002   /*GG_020197
Renvoyer la hauteur du texte et non de la font
*/

#define GG010   /*GG_150197
                Ameliorer la conformiter de l'underline vis a vis de WORLD
*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DRAW_TEXT
#endif

/*
   STATUS Xw_draw_text (awindow,x,y,text,angle,mode):
   XW_EXT_WINDOW *awindow
   float x,y		Position of the beginning of text in world coordinate
   char text[]		String to display
   float angle		Rotated text angle
   mode			1 si outline

	Display text in current QG set by set_text_attrib .

	returns ERROR if text length > MAXCHARS
	returns SUCCESS if successful

*/

#define MAXCOORD 32767
#define MINCOORD -32768

#define XROTATE(x,y) (x*cosa + y*sina)
#define YROTATE(x,y) (y*cosa - x*sina)

static XW_EXT_LTEXT *ptextlist ;
static XW_EXT_CHAR *ptextdesc ;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_draw_text (void* awindow,float x,float y,char* text,float angle,int mode)
#else
XW_STATUS Xw_draw_text (awindow,x,y,text,angle,mode)
void *awindow;
float x,y ;
float angle ;
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
            Xw_set_error(24,"Xw_draw_text",pwindow) ;
            return (XW_ERROR) ;
        }

	length = strlen(text) ;
	if( length+1 > MAXCHARS ) {
	    length = MAXCHARS-1 ;
	    /*ERROR*Too big text string*/
	    Xw_set_error(38,"Xw_draw_text",&length) ;
	    return (XW_ERROR) ;
	}

	bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
        for( ptextlist = pbuffer->pltextlist ; ptextlist ;
                    		ptextlist = (XW_EXT_LTEXT*)ptextlist->link ) {
            if( ptextlist->ntext < MAXLTEXTS ) break ;
        }

        if( !ptextlist ) {
                ptextlist = Xw_add_text_structure(pbuffer) ;
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
            while( angle > 2.*M_PI ) angle -= 2.*M_PI ;
        } else if( angle < 0. ) {
            while( angle < -2.*M_PI ) angle += 2.*M_PI ;
        }
	ptextlist->rangles[ntext] = angle ;
 
	nchar += length+1 ;
	ptextlist->ntext++ ;
	ptextdesc->nchar = nchar ;

	if( bindex > 0 ) {
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
	    int index = pwindow->textindex ;
	    Xw_draw_pixel_texts(pwindow,ptextlist,
			pwindow->qgtext[index].gc,
			pwindow->qgtext[index].code) ;
    	    ptextlist->ntext = 0 ;
	    ptextdesc->nchar = 0 ;
	}


#ifdef  TRACE_DRAW_TEXT
if( Xw_get_trace() > 2 ) {
    printf(" Xw_draw_text(%lx,%f,%f,'%s',%f)\n",(long ) pwindow,x,y,text,angle) ;
}
#endif

	return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
void Xw_draw_pixel_texts(XW_EXT_WINDOW* pwindow,XW_EXT_LTEXT* ptextlist,
							GC gctext,XW_ATTRIB code)
#else
void Xw_draw_pixel_texts(pwindow,ptextlist,gctext,code)
XW_EXT_WINDOW *pwindow ;
XW_EXT_LTEXT *ptextlist ;
GC gctext ;
XW_ATTRIB code ;
#endif /*XW_PROTOTYPE*/
{
int i,ix,iy,mode,length,font = QGFONT(code),type = QGTYPE(code) ;
float angle,cosa,sina ;
char *pchar ;

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
              int xmin,ymin,xmax,ymax ;
	      if( angle == 0. ) {
                if( type & XW_EXTENDEDTEXT_UNDERLINE ) {
#ifdef GG002
	    	  int dir,fascent,fdescent;
	    	  XCharStruct overall;
	    	  XTextExtents(pfontinfo,pchar,length,
                                &dir,&fascent,&fdescent,&overall);
            	  xmin = overall.lbearing;
	    	  xmax = overall.width;
            	  ymin = -overall.ascent;
            	  ymax = overall.descent;
#else
                  xmin = 0 ;
                  xmax = XTextWidth(pfontinfo,pchar,length) ;
                  ymin = -pfontinfo->ascent ;
                  ymax = pfontinfo->descent ;
#endif
#ifdef GG010
                  if( _FONTMAP->fratios[font] > 0. ) {
//OCC186
                    ymax = PVALUE(0.6*_FONTMAP->fratios[font]*
                                        FROMMILLIMETER(_FONTMAP->ssizey[font]),
				  pwindow->xratio, pwindow->yratio);
//OCC186
                  } else {
                    ymax = max(2,(ymax-ymin)/8) ;
                  }
#else
                  ymax += max(2,(ymax-ymin)/8) ;
#endif
                  XDrawLine(_DISPLAY,_DRAWABLE,gctext,
					ix+xmin,iy+ymax,ix+xmax,iy+ymax) ;
                }
	        XDrawString(_DISPLAY,_DRAWABLE,gctext,ix,iy,pchar,length) ;
	      } else {
		int w,j,tx,ty ;
		cosa = cos((double)angle) ;
		sina = sin((double)angle) ;

                if( type & XW_EXTENDEDTEXT_UNDERLINE ) {
		  int x1,y1,x2,y2 ;
#ifdef GG002
	    	  int dir,fascent,fdescent;
	    	  XCharStruct overall;
	    	  XTextExtents(pfontinfo,pchar,length,
                                &dir,&fascent,&fdescent,&overall);
            	  xmin = overall.lbearing;
	    	  xmax = overall.width;
            	  ymin = -overall.ascent;
            	  ymax = overall.descent;
#else
                  xmin = 0 ;
                  xmax = XTextWidth(pfontinfo,pchar,length) ;
                  ymin = -pfontinfo->ascent ;
                  ymax = pfontinfo->descent ;
#endif
#ifdef GG010
                  if( _FONTMAP->fratios[font] > 0. ) {
//OCC186
                    ymax = PVALUE(0.6*_FONTMAP->fratios[font]*
                                        FROMMILLIMETER(_FONTMAP->ssizey[font]), 
				  pwindow->xratio, pwindow->yratio);
//OCC186
                  } else {
                    ymax = max(2,(ymax-ymin)/8) ;
                  }
#else
                  ymax += max(2,(ymax-ymin)/8) ;
#endif
                  x1 = (int )( XROTATE(xmin,ymax) );
                  y1 = (int )( YROTATE(xmin,ymax) );
                  x2 = (int )( XROTATE(xmax,ymax) );
                  y2 = (int )( YROTATE(xmax,ymax) );
                  XDrawLine(_DISPLAY,_DRAWABLE,gctext,
					ix+x1,iy+y1,ix+x2,iy+y2) ;
		}
                for( j=tx=ty=0 ; j<length ; j++) {
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
XW_EXT_LTEXT* Xw_add_text_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_LTEXT* Xw_add_text_structure(pbuflist )
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert at end one Extended text structure in the
        text List

        returns Extended text address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_LTEXT *ptext ;

        ptext = (XW_EXT_LTEXT*) Xw_malloc(sizeof(XW_EXT_LTEXT)) ;
        if( ptext ) {
            ptext->link = pbuflist->pltextlist ;
            ptext->isupdated = False ;
            ptext->ntext = 0 ;
	    pbuflist->pltextlist = ptext ;
        } else {
	    /*ERROR*EXT_TEXT Allocation failed*/
	    Xw_set_error(39,"Xw_add_text_structure",NULL) ;
        }

        return (ptext) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_text_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_text_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended text structure in the
        text List

        SUCCESS always
*/
{
XW_EXT_LTEXT *ptext,*qtext ;

        for( ptext = pbuflist->pltextlist ; ptext ; ptext = qtext ) {
            qtext = (XW_EXT_LTEXT*)ptext->link ;
            Xw_free(ptext) ;
        }
	pbuflist->pltextlist = NULL ;

        return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
XW_EXT_CHAR* Xw_add_text_desc_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_CHAR* Xw_add_text_desc_structure(pbuflist )
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert at end one Extended char structure in the
        char List

        returns Extended char address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_CHAR *pchar ;

        pchar = (XW_EXT_CHAR*) Xw_malloc(sizeof(XW_EXT_CHAR)) ;
        if( pchar ) {
            pchar->link = pbuflist->ptextdesc ;
            pchar->nchar = 0 ;
	    pbuflist->ptextdesc = pchar ;
        } else {
	    /*ERROR*EXT_CHAR Allocation failed*/
	    Xw_set_error(118,"Xw_add_text_desc_structure",NULL) ;
        }

        return (pchar) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_text_desc_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_text_desc_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended char structure in the
        char List

        SUCCESS always
*/
{
XW_EXT_CHAR *pchar,*qchar ;

        for( pchar = pbuflist->ptextdesc ; pchar ; pchar = qchar ) {
            qchar = (XW_EXT_CHAR*)pchar->link ;
            Xw_free(pchar) ;
        }
	pbuflist->ptextdesc = NULL ;

        return (XW_SUCCESS) ;
}
