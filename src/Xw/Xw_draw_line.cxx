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

#define S3593           /*GG_130398
OPTIMISATION MFT
                        Activer le clipping de maniere optionnelle
*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DRAW_LINE
#endif

/*
   STATUS Xw_draw_line (awindow,npoint,x,y):
   XW_EXT_WINDOW *awindow
   int npoint		Polyline point number
   float *x,*y		Points Arrays


	Display continuous line in current QG set by set_line_attrib .
        Note that lines can be buffered depending of the DisplayMode context
        and Flush at Xw_flush time .

	returns ERROR if npoint > MAXPOINTS
	returns SUCCESS always 

*/

static int BeginLine = -1 ;
static XW_EXT_LINE *plinelist ;
static XW_EXT_POINT *plinedesc ;
static XSegment segment;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_draw_line (void* awindow,int npoint,float* px,float* py) 
#else
XW_STATUS Xw_draw_line (awindow,npoint,px,py) 
void *awindow;
int npoint ;
float *px,*py ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) awindow ;
XW_EXT_BUFFER *pbuffer ;
int i,nline,ldesc,bindex;
int x,y,lx=0,ly=0 ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_draw_line",pwindow) ;
            return (XW_ERROR) ;
        }

        if( npoint > MAXPOINTS ) {
	    npoint = MAXPOINTS;
	    /*ERROR*Too many points in LINE*/
	    Xw_set_error(28,"Xw_draw_line",&npoint) ;
            return (XW_ERROR) ;
        }

	bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
        for( plinelist = pbuffer->plinelist ; plinelist ;
                       	plinelist = (XW_EXT_LINE*)plinelist->link ) {
            if( plinelist->nline < MAXLINES ) break ;
        }

        if( !plinelist ) {
            plinelist = Xw_add_polyline_structure(pbuffer) ;
        }

	if( !plinelist ) return XW_ERROR ;

        for( plinedesc = pbuffer->plinedesc ; plinedesc ;
                       	plinedesc = (XW_EXT_POINT*)plinedesc->link ) {
            if( plinedesc->npoint + npoint <= MAXPOINTS ) break ;
        }

        if( !plinedesc ) {
            plinedesc = Xw_add_line_desc_structure(pbuffer) ;
        }

	if( !plinedesc ) return XW_ERROR ;

	nline = plinelist->nline ;
	ldesc = plinedesc->npoint ;
	plinelist->plines[nline] = &plinedesc->rpoints[ldesc] ;
	for( i=0 ; i<npoint ; i++ ) {
//OCC186
	  x = PXPOINT(px[i], pwindow->xratio) ;
	  y = PYPOINT(py[i], pwindow->attributes.height, pwindow->yratio) ;
//OCC186
#ifdef S3593
	  if( pwindow->clipflag ) {  
#endif
	    if( i > 0 ) {
	      int status;
	      status = Xw_clip_segment(pwindow,lx,ly,x,y,&segment);
	      if( status >= 0 ) { 
	        if( (i < 2) || (status & 0xF ) ) {
                  plinedesc->rpoints[ldesc].x = segment.x1 ;
                  plinedesc->rpoints[ldesc].y = segment.y1 ;
	          ldesc++;
	          if( bindex > 0 ) {
		    int xx = segment.x1,yy = segment.y1;
                    pbuffer->rxmin = min(pbuffer->rxmin,xx) ;
                    pbuffer->rymin = min(pbuffer->rymin,yy) ;
                    pbuffer->rxmax = max(pbuffer->rxmax,xx) ;
                    pbuffer->rymax = max(pbuffer->rymax,yy) ;
	          }
	        }
                plinedesc->rpoints[ldesc].x = segment.x2 ;
                plinedesc->rpoints[ldesc].y = segment.y2 ;
	        ldesc++;
	        if( bindex > 0 ) {
	 	  int xx = segment.x2,yy = segment.y2;
                  pbuffer->rxmin = min(pbuffer->rxmin,xx) ;
                  pbuffer->rymin = min(pbuffer->rymin,yy) ;
                  pbuffer->rxmax = max(pbuffer->rxmax,xx) ;
                  pbuffer->rymax = max(pbuffer->rymax,yy) ;
	        }
	      }
	    }
	    lx = x; ly = y;
#ifdef S3593
	  } else {
            plinedesc->rpoints[ldesc].x = x ;
            plinedesc->rpoints[ldesc].y = y ;
	    ldesc++ ;
	    if( bindex > 0 ) {
              pbuffer->rxmin = min(pbuffer->rxmin,x) ;
              pbuffer->rymin = min(pbuffer->rymin,y) ;
              pbuffer->rxmax = max(pbuffer->rxmax,x) ;
              pbuffer->rymax = max(pbuffer->rymax,y) ;
	    }
	  }
#endif
	}
	plinelist->lines[nline] = ldesc - plinedesc->npoint ;
	if( plinelist->lines[nline] > 1 ) {
	  plinedesc->npoint = ldesc ;
	  plinelist->nline++ ;
	  if( bindex > 0 ) {
	    pbuffer->isempty = False ;
	  } else if( BeginLine < 0 ) {
	    int index = pwindow->lineindex ;
	    Xw_draw_pixel_lines(pwindow,plinelist,pwindow->qgline[index].gc);
	    plinelist->nline = 0 ;
	    plinedesc->npoint = 0 ;
	  }
	}

#ifdef  TRACE_DRAW_LINE
if( Xw_get_trace() > 2 ) {
    printf(" Xw_draw_line(%lx,%d)\n",(long ) pwindow,npoint) ;
    for( i=0 ; i<npoint ; i++ ) {
	printf("	Point(%d) = {%f,%f}\n",i,px[i],py[i]) ;
    }
}
#endif

	return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
void Xw_draw_pixel_lines (XW_EXT_WINDOW* pwindow,XW_EXT_LINE *plinelist,GC gc) 
#else
void Xw_draw_pixel_lines (pwindow,plinelist,gc) 
XW_EXT_WINDOW *pwindow;
XW_EXT_LINE *plinelist;
GC gc ;
#endif /*XW_PROTOTYPE*/
{
int i,npoint;
XPoint *ppoint ;

    for( i=0 ; i<plinelist->nline ; i++ ) {
        npoint = plinelist->lines[i] ;
	ppoint = plinelist->plines[i] ;
	if( plinelist->isupdated ) {
	  ppoint += MAXPOINTS ;
	}
	if( ppoint && npoint > 1 ) {
	  XDrawLines(_DISPLAY,_DRAWABLE,gc,ppoint,npoint,CoordModeOrigin) ;
	}
    }
}

/*
   STATUS Xw_begin_line (awindow,npoint):
   XW_EXT_WINDOW *awindow
   int npoint           Polyline point number


        Begin Polyline which must be filled by Xw_line_point and
                                     closed by Xw_close_line

        returns ERROR if npoint > MAXPOINTS
        returns SUCCESS if successful

*/

static int Npoint = 0;
static int Lx,Ly;
#ifdef XW_PROTOTYPE
XW_STATUS Xw_begin_line(void* awindow,int npoint)
#else
XW_STATUS Xw_begin_line(awindow,npoint)
void *awindow ;
int npoint ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_BUFFER *pbuffer ;
int nline,ldesc,bindex ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_begin_line",pwindow) ;
            return (XW_ERROR) ;
        }

        if( npoint > MAXPOINTS ) {
	    npoint = MAXPOINTS ;
	    /*ERROR*Too many points in LINE*/
	    Xw_set_error(28,"Xw_begin_line",&npoint) ;
            return (XW_ERROR) ;
        }

	if( BeginLine >= 0 ) Xw_close_line(pwindow);

	bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
        for( plinelist = pbuffer->plinelist ; plinelist ;
                       	plinelist = (XW_EXT_LINE*)plinelist->link ) {
            if( plinelist->nline < MAXLINES ) break ;
        }

        if( !plinelist ) {
            plinelist = Xw_add_polyline_structure(pbuffer) ;
        }

	if( !plinelist ) return XW_ERROR ;

        for( plinedesc = pbuffer->plinedesc ; plinedesc ;
                       	plinedesc = (XW_EXT_POINT*)plinedesc->link ) {
            if( plinedesc->npoint + npoint <= MAXPOINTS ) break ;
        }

        if( !plinedesc ) {
            plinedesc = Xw_add_line_desc_structure(pbuffer) ;
        }

	if( !plinedesc ) return XW_ERROR ;

	nline = plinelist->nline ;
	ldesc = plinedesc->npoint ;
	plinelist->lines[nline] = Npoint = 0 ;
	plinelist->plines[nline] = &plinedesc->rpoints[ldesc] ;
	BeginLine = ldesc ;

#ifdef  TRACE_DRAW_LINE
if( Xw_get_trace() > 2 ) {
    printf(" Xw_begin_line(%lx,%d)\n",(long ) pwindow,npoint) ;
}
#endif
        return (XW_SUCCESS) ;
}

/*
   STATUS Xw_line_point (awindow,x,y):
   XW_EXT_WINDOW *awindow
   float x,y    New point to add in polyline in user-space coordinates

        Fill Polyline with one point more

        returns ERROR if Too Many Points in polylines
        returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_line_point(void* awindow,float x,float y)
#else
XW_STATUS Xw_line_point(awindow,x,y)
void *awindow ;
float x,y ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_BUFFER *pbuffer ;
int bindex,xi,yi ;

      if( BeginLine >= 0 ) {
	int ldesc = plinedesc->npoint ;
        if( ldesc >= MAXPOINTS ) {
	  /*ERROR*Too many points in LINE*/
	  Xw_set_error(28,"Xw_line_point",&ldesc) ;
          Xw_close_line(pwindow) ;
          return (XW_ERROR) ;
        }

	bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
//OCC186
	xi = PXPOINT(x, pwindow->xratio) ;
	yi = PYPOINT(y, pwindow->attributes.height, pwindow->yratio) ;
//OCC186
#ifdef S3593
	if( pwindow->clipflag ) {  
#endif
	  if( Npoint > 0 ) {
	    int status;
	    status = Xw_clip_segment(pwindow,Lx,Ly,xi,yi,&segment);
	    if( status >= 0 ) { 
	      if( (Npoint < 2) || (status & 0xF ) ) {
                plinedesc->rpoints[ldesc].x = segment.x1 ;
                plinedesc->rpoints[ldesc].y = segment.y1 ;
	        ldesc++;
	        if( bindex > 0 ) {
		  int xx = segment.x1,yy = segment.y1;
	          pbuffer->isempty = False ;
                  pbuffer->rxmin = min(pbuffer->rxmin,xx) ;
                  pbuffer->rymin = min(pbuffer->rymin,yy) ;
                  pbuffer->rxmax = max(pbuffer->rxmax,xx) ;
                  pbuffer->rymax = max(pbuffer->rymax,yy) ;
	        }
	      }
              plinedesc->rpoints[ldesc].x = segment.x2 ;
              plinedesc->rpoints[ldesc].y = segment.y2 ;
	      ldesc++;
	      if( bindex > 0 ) {
	 	int xx = segment.x2,yy = segment.y2;
	        pbuffer->isempty = False ;
                pbuffer->rxmin = min(pbuffer->rxmin,xx) ;
                pbuffer->rymin = min(pbuffer->rymin,yy) ;
                pbuffer->rxmax = max(pbuffer->rxmax,xx) ;
                pbuffer->rymax = max(pbuffer->rymax,yy) ;
	      }
	    }
	  }
	  Lx = xi; Ly = yi;
#ifdef S3593
	} else {
          plinedesc->rpoints[ldesc].x = xi ;
          plinedesc->rpoints[ldesc].y = yi ;
	  ldesc++;
	  if( bindex > 0 ) {
	    pbuffer->isempty = False ;
            pbuffer->rxmin = min(pbuffer->rxmin,xi) ;
            pbuffer->rymin = min(pbuffer->rymin,yi) ;
            pbuffer->rxmax = max(pbuffer->rxmax,xi) ;
            pbuffer->rymax = max(pbuffer->rymax,yi) ;
	  }
	}
#endif
	Npoint++;
	plinedesc->npoint = ldesc ;
      }

#ifdef  TRACE_DRAW_LINE
if( Xw_get_trace() > 3 ) {
    printf(" Xw_line_point(%lx,%f,%f)\n",(long ) pwindow,x,y) ;
}
#endif
        return (XW_SUCCESS) ;
}

/*
   STATUS Xw_close_line (awindow):
   XW_EXT_WINDOW *awindow

        Close the Polyline

        returns ERROR if Polyline is empty
        returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_line(void* awindow)
#else
XW_STATUS Xw_close_line(awindow)
void *awindow ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;

        if( BeginLine >= 0 ) {
	  int nline = plinelist->nline ;
	  int ldesc = plinedesc->npoint ;
	  int bindex = _BINDEX ;
	  plinelist->lines[nline] = ldesc - BeginLine ;
          plinelist->nline++ ;

	  if( !bindex ) {	
	    int index = pwindow->lineindex ;
	    Xw_draw_pixel_lines(pwindow,plinelist,pwindow->qgline[index].gc) ;
	    plinelist->nline = 0 ;
	    plinedesc->npoint = 0 ;
          }
	  BeginLine = -1 ;
	}

#ifdef  TRACE_DRAW_LINE
if( Xw_get_trace() > 2 ) {
    printf(" Xw_close_line(%lx)\n",(long ) pwindow) ;
}
#endif
        return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
XW_EXT_LINE* Xw_add_polyline_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_LINE* Xw_add_polyline_structure(pbuflist )
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert at end one Extended polyline structure in the
        polyline List

        returns Extended polyline address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_LINE *pline ;

        pline = (XW_EXT_LINE*) Xw_malloc(sizeof(XW_EXT_LINE)) ;
        if( pline ) {
            pline->link = pbuflist->plinelist ;
	    pline->isupdated = False ;
            pline->nline = 0 ;
            pbuflist->plinelist = pline ;
        } else {
	    /*ERROR*EXT_LINE allocation failed*/
	    Xw_set_error(30,"Xw_add_polyline_structure",NULL) ;
        }

        return (pline) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_polyline_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_polyline_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended polyline structure in the
        polyline List

        SUCCESS always
*/
{
XW_EXT_LINE *pline,*qline ;

        for( pline = pbuflist->plinelist ; pline ; pline = qline ) {
            qline = (XW_EXT_LINE*)pline->link ;
            Xw_free(pline) ;
        }
	pbuflist->plinelist = NULL ;

        return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
XW_EXT_POINT* Xw_add_line_desc_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_POINT* Xw_add_line_desc_structure(pbuflist )
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert at end one Extended line_desc structure in the
        line_desc List

        returns Extended line_desc address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_POINT *pdesc ;

        pdesc = (XW_EXT_POINT*) Xw_malloc(sizeof(XW_EXT_POINT)) ;
        if( pdesc ) {
            pdesc->link = pbuflist->plinedesc ;
            pdesc->npoint = 0 ;
            pbuflist->plinedesc = pdesc ;
        } else {
	    /*ERROR*EXT_POINT allocation failed*/
	    Xw_set_error(117,"Xw_add_line_desc_structure",NULL) ;
        }

        return (pdesc) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_line_desc_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_line_desc_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended line_desc structure in the
        line_desc List

        SUCCESS always
*/
{
XW_EXT_POINT *pdesc,*qdesc ;

        for( pdesc = pbuflist->plinedesc ; pdesc ; pdesc = qdesc ) {
            qdesc = (XW_EXT_POINT*)pdesc->link ;
            Xw_free(pdesc) ;
        }
	pbuflist->plinedesc = NULL ;

        return (XW_SUCCESS) ;
}
