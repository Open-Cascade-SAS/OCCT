#define BUC40223         /*GG_070797
                        Protection contre zoom trop grand*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DRAW_POLY
#endif

/*
   STATUS Xw_draw_poly (awindow,npoint,px,py):
   XW_EXT_WINDOW *awindow
   int npoint		Polygone point number
   float *px,*py	Polygone points


	Display continuous poly in current QG set by set_poly_attrib .
        Note that polys can be buffered depending of the DisplayMode context
        and Flush at Xw_flush time .

	returns ERROR if npoint > MAXPOINTS
	returns SUCCESS if successful 

*/

static int FirstPolyPoint = -1 ;
static int FirstPolyLine = -1 ;
static int FirstPathPoint = -1 ;
static XW_EXT_POLY *ppolylist ;
static XW_EXT_POINT *plinedesc ;
static XSegment segment;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_draw_poly (void* awindow,int npoint,float* px,float* py)
#else
XW_STATUS Xw_draw_poly (awindow,npoint,px,py)
void *awindow;
int npoint ;
float *px,*py ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
int i,npoly,ldesc,bindex;
int x=0,y=0,lx=0,ly=0 ;
XPoint *ppoint ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_draw_poly",pwindow) ;
            return (XW_ERROR) ;
        }

        if( npoint >= MAXPOINTS ) {
	    npoint = MAXPOINTS-1 ;
            /*ERROR*Too many points in POLYGONE*/
            Xw_set_error(28,"Xw_draw_poly",&npoint) ;
            return (XW_ERROR) ;
        }

	if( npoint < 3 ) {
	    /*WARNING*POLYGONE is Empty !!!*/
	    Xw_set_error(33,"Xw_draw_poly",&npoint) ;
	    return (XW_ERROR) ;
	}

	bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
        for( ppolylist = pbuffer->ppolylist ; ppolylist ;
                         ppolylist = (XW_EXT_POLY*)ppolylist->link ) {
            if( ppolylist->npoly < MAXPOLYS ) break ;
        }

        if( !ppolylist ) {
            ppolylist = Xw_add_polygone_structure(pbuffer) ;
        }

	if( !ppolylist ) return XW_ERROR ;

        for( plinedesc = pbuffer->plinedesc ; plinedesc ;
                        plinedesc = (XW_EXT_POINT*)plinedesc->link ) {
            if( plinedesc->npoint + npoint < MAXPOINTS ) break ;
        }
 
        if( !plinedesc ) {
            plinedesc = Xw_add_line_desc_structure(pbuffer) ;
        }
 
        if( !plinedesc ) return XW_ERROR ;
 
        npoly = ppolylist->npoly ;
        ldesc = plinedesc->npoint ;
        ppolylist->ppolys[npoly] = ppoint = &plinedesc->rpoints[ldesc] ;
        for( i=0 ; i<npoint ; i++ ) {
//OCC186
            x = PXPOINT(px[i], pwindow->xratio) ;
            y = PYPOINT(py[i], pwindow->attributes.height, pwindow->yratio) ;
//OCC186
#ifdef BUC40223
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
#else
            plinedesc->rpoints[ldesc].x = x ;
            plinedesc->rpoints[ldesc].y = y ;
            ldesc++ ;
            if( bindex > 0 ) {
              pbuffer->rxmin = min(pbuffer->rxmin,x) ;
              pbuffer->rymin = min(pbuffer->rymin,y) ;
              pbuffer->rxmax = max(pbuffer->rxmax,x) ;
              pbuffer->rymax = max(pbuffer->rymax,y) ;
            }
#endif
        }
	if( ppoint->x != x || ppoint->y != y ) {
            plinedesc->rpoints[ldesc].x = ppoint->x ;
            plinedesc->rpoints[ldesc].y = ppoint->y ;
	    ldesc++ ;
	}
        ppolylist->polys[npoly] = ldesc - plinedesc->npoint ;
        ppolylist->paths[npoly] = ppolylist->polys[npoly] ;
        if( ppolylist->polys[npoly] > 3 ) {
          ppolylist->npoly++ ;
          plinedesc->npoint = ldesc ;

	  if( bindex > 0 ) {
	    pbuffer->isempty = False ;
	  } else if( FirstPolyPoint < 0 ) {
            int polyindex = pwindow->polyindex ;
            int lineindex = pwindow->lineindex ;
	    GC gcpoly = (QGTILE(pwindow->qgpoly[polyindex].code)) ?
				pwindow->qgpoly[polyindex].gc : NULL ;
	    GC gcline = (QGTYPE(pwindow->qgpoly[polyindex].code)) ?
				pwindow->qgline[lineindex].gc : NULL ;
            Xw_draw_pixel_polys(pwindow,ppolylist,gcpoly,gcline);
            ppolylist->npoly = 0 ;
            plinedesc->npoint = 0 ;
	  }
        }

#ifdef  TRACE_DRAW_POLY
if( Xw_get_trace() > 2 ) {
    printf(" Xw_draw_poly(%lx,%d)\n",(long ) pwindow,npoint) ;
    for( i=0 ; i<npoint ; i++ ) {
	printf("        Point(%d) = {%f,%f}\n",i,px[i],py[i]) ;
    }
}
#endif

	return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
void Xw_draw_pixel_polys(XW_EXT_WINDOW* pwindow,XW_EXT_POLY* ppolylist,
							GC gcpoly,GC gcline)
#else
void Xw_draw_pixel_polys(pwindow,ppolylist,gcpoly,gcline)
XW_EXT_WINDOW *pwindow ;
XW_EXT_POLY *ppolylist ;
GC gcpoly,gcline ;
#endif
{
int i,npolypoint,npathpoint,shape,npoint=0,count=0;
Region chr_region=NULL;
XPoint *ppoint ;

	for( i=0 ; i<ppolylist->npoly ; i++ ) {	
	    npolypoint = ppolylist->polys[i] ;
	    npathpoint = ppolylist->paths[i] ;
	    ppoint = ppolylist->ppolys[i] ;
	    if( ppolylist->isupdated ) {
	      ppoint += MAXPOINTS ;
	    }
	    if( gcpoly ) {
	      if( npolypoint > 0 ) {
		npoint = npolypoint; count = 0;
		chr_region = 0;
	      }
	      if( npoint > npathpoint ) {
	        if( npathpoint > 3 ) {
                  if( chr_region == 0 ) {
		    chr_region = 
			XPolygonRegion(ppoint,npathpoint-1,EvenOddRule);
                  } else {
		    Region chr_pathreg = 
			XPolygonRegion(ppoint,npathpoint-1,EvenOddRule);
                    Region chr_xorreg = XCreateRegion ();
                    XXorRegion (chr_region, chr_pathreg, chr_xorreg);
                    XDestroyRegion (chr_region);
                    chr_region = chr_xorreg;
                  }
                }
		count += npathpoint;
		if( count >= npoint && chr_region ) {
                  XRectangle rect;
                  XSetRegion(_DISPLAY,gcpoly,chr_region);
                  XClipBox(chr_region,&rect);
                  XFillRectangles(_DISPLAY,_DRAWABLE,gcpoly,&rect,1);
                  XDestroyRegion(chr_region);
                  XSetClipMask(_DISPLAY,gcpoly,None);
		}
	      } else if( npoint > 3 ) { 
	        shape = (npoint > 4) ? Nonconvex : Convex;
                XFillPolygon(_DISPLAY,_DRAWABLE,gcpoly,ppoint,npoint-1,
						shape,CoordModeOrigin) ;
	
	      }
	    }
	    if( gcline && (gcpoly != gcline) && (npathpoint > 3) ) {
              XDrawLines(_DISPLAY,_DRAWABLE,gcline,
					ppoint,npathpoint,CoordModeOrigin) ;
	    }
        }
}

/*
   STATUS Xw_begin_poly (awindow,npoint,npath):
   XW_EXT_WINDOW *awindow
   int npoint		Estimated polygone point number
   int npath		Estimated polygone path number


	Begin Polygone which must be filled by Xw_poly_point and
				     closed by Xw_close_poly

	returns ERROR if npoint > MAXPOINTS
		      or npath > MAXPOLYS
	returns SUCCESS if successful 

*/

static int Npoint = 0;
static int Lx,Ly;
#ifdef XW_PROTOTYPE
XW_STATUS Xw_begin_poly(void* awindow,int npoint,int npath)
#else
XW_STATUS Xw_begin_poly(awindow,npoint,npath)
void *awindow ;
int npoint,npath ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
int npoly,ldesc,bindex ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_begin_poly",pwindow) ;
            return (XW_ERROR) ;
        }

	if( npoint >= MAXPOINTS ) {
	    npoint = MAXPOINTS-1 ;
	    /*ERROR*Too many points in POLYGONE*/
	    Xw_set_error(32,"Xw_begin_poly",&npoint) ;
	    return (XW_ERROR) ;
	}

	if( npath >= MAXPOLYS ) {
	    npath = MAXPOLYS-1 ;
	    /*ERROR*Too many paths in POLYGONE*/
	    Xw_set_error(32,"Xw_begin_poly",&npath) ;
	    return (XW_ERROR) ;
	}

	if( FirstPolyPoint >= 0 ) Xw_close_poly(pwindow) ;

        bindex = _BINDEX ;
        for( ppolylist = _BUFFER(bindex).ppolylist ; ppolylist ;
                        ppolylist = (XW_EXT_POLY*)ppolylist->link ) {
            if( (ppolylist->npoly + npath) < MAXPOLYS ) break ;
        }    
 
        if( !ppolylist ) {
            ppolylist = Xw_add_polygone_structure(&_BUFFER(bindex)) ;
        }    
 
        if( !ppolylist ) return XW_ERROR ;
 
        for( plinedesc = _BUFFER(bindex).plinedesc ; plinedesc ;
                        plinedesc = (XW_EXT_POINT*)plinedesc->link ) {
            if( (plinedesc->npoint + npoint) < MAXPOINTS ) break ;
        }    
 
        if( !plinedesc ) {
            plinedesc = Xw_add_line_desc_structure(&_BUFFER(bindex)) ;
        }    
 
        if( !plinedesc ) return XW_ERROR ;
 
        npoly = ppolylist->npoly ;
        ldesc = plinedesc->npoint ;
        ppolylist->polys[npoly] = 0 ;
        ppolylist->paths[npoly] = Npoint = 0 ;
        ppolylist->ppolys[npoly] = &plinedesc->rpoints[ldesc] ;
        FirstPolyPoint = FirstPathPoint = ldesc ;
	FirstPolyLine = npoly;


#ifdef  TRACE_DRAW_POLY
if( Xw_get_trace() > 2 ) {
    printf(" Xw_begin_poly(%lx,%d,%d)\n",(long ) pwindow,npoint,npath) ;
}
#endif
	return (XW_SUCCESS) ;
}

/*
   STATUS Xw_poly_point (awindow,x,y):
   XW_EXT_WINDOW *awindow
   float x,y	New point to add in polygone in user-space coordinates

	Fill Polygone with one point more

	returns ERROR if Too Many Points in polygones
	returns SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_poly_point(void* awindow,float x,float y)
#else
XW_STATUS Xw_poly_point(awindow,x,y)
void *awindow ;
float x,y ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_BUFFER *pbuffer ;
int bindex,xi,yi ;

        if( FirstPolyPoint >= 0 ) {
          int ldesc = plinedesc->npoint ;
	  if( ldesc >= MAXPOINTS ) {
	    /*ERROR*Too many points in POLYGONE*/
	    Xw_set_error(32,"Xw_poly_point",&ldesc) ;
	    Xw_close_poly(pwindow) ;
	    return (XW_ERROR) ;
	  }

          bindex = _BINDEX ;
//OCC186
          xi = PXPOINT(x, pwindow->xratio) ;
          yi = PYPOINT(y, pwindow->attributes.height, pwindow->yratio) ;
//OCC186
#ifdef BUC40223
          if( Npoint > 0 ) {
            int status;
            status = Xw_clip_segment(pwindow,Lx,Ly,xi,yi,&segment);
            if( status >= 0 ) {
              if( (Npoint < 2) || (status & 0xF ) ) {
                plinedesc->rpoints[ldesc].x = segment.x1 ;
                plinedesc->rpoints[ldesc].y = segment.y1 ;
                ldesc++; plinedesc->npoint++ ;
                if( bindex > 0 ) {
                  int xx = segment.x1,yy = segment.y1;
                  pbuffer = &_BUFFER(bindex) ;
                  pbuffer->isempty = False ;
                  pbuffer->rxmin = min(pbuffer->rxmin,xx) ;
                  pbuffer->rymin = min(pbuffer->rymin,yy) ;
                  pbuffer->rxmax = max(pbuffer->rxmax,xx) ;
                  pbuffer->rymax = max(pbuffer->rymax,yy) ;
                }
              }
              plinedesc->rpoints[ldesc].x = segment.x2 ;
              plinedesc->rpoints[ldesc].y = segment.y2 ;
              plinedesc->npoint++ ;
              if( bindex > 0 ) {
                int xx = segment.x2,yy = segment.y2;
                pbuffer = &_BUFFER(bindex) ;
                pbuffer->isempty = False ;
                pbuffer->rxmin = min(pbuffer->rxmin,xx) ;
                pbuffer->rymin = min(pbuffer->rymin,yy) ;
                pbuffer->rxmax = max(pbuffer->rxmax,xx) ;
                pbuffer->rymax = max(pbuffer->rymax,yy) ;
              }
            }
          }
          Lx = xi; Ly = yi;
          Npoint++;
#else
          plinedesc->rpoints[ldesc].x = xi ;
          plinedesc->rpoints[ldesc].y = yi ;
          plinedesc->npoint++ ;
          if( bindex > 0 ) {
            pbuffer = &_BUFFER(bindex) ;
	    pbuffer->isempty = False ;
            pbuffer->rxmin = min(pbuffer->rxmin,xi) ;
            pbuffer->rymin = min(pbuffer->rymin,yi) ;
            pbuffer->rxmax = max(pbuffer->rxmax,xi) ;
            pbuffer->rymax = max(pbuffer->rymax,yi) ;
          }
#endif
        }

#ifdef  TRACE_DRAW_POLY
if( Xw_get_trace() > 3 ) {
    printf(" Xw_poly_point(%lx,%f,%f)\n",(long ) pwindow,x,y) ;
}
#endif
	return (XW_SUCCESS) ;
}

/*
   STATUS Xw_close_path (awindow):
   XW_EXT_WINDOW *awindow

	Close the Polygone path

	returns ERROR if TOO many path 
	returns SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_path(void* awindow)
#else
XW_STATUS Xw_close_path(awindow)
void *awindow ;
#endif /*XW_PROTOTYPE*/
{
#ifdef  TRACE_DRAW_POLY
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
#endif

        if( FirstPolyPoint >= 0 ) {
          int npoly = ppolylist->npoly ;
	  int ldesc = plinedesc->npoint ;
            
	  if( plinedesc->rpoints[FirstPathPoint].x != plinedesc->rpoints[ldesc-1].x ||
	      plinedesc->rpoints[FirstPathPoint].y != plinedesc->rpoints[ldesc-1].y ) {
	    plinedesc->rpoints[ldesc].x = plinedesc->rpoints[FirstPathPoint].x ; 
	    plinedesc->rpoints[ldesc].y = plinedesc->rpoints[FirstPathPoint].y ; 
	    plinedesc->npoint++ ; ldesc++ ;
	  } 
          ppolylist->polys[FirstPolyLine] = ldesc - FirstPolyPoint ;
          ppolylist->paths[npoly] = ldesc - FirstPathPoint ;
	  FirstPathPoint = ldesc;
          if( ppolylist->npoly < MAXPOLYS ) {
	    ppolylist->npoly = ++npoly ;
            ppolylist->polys[npoly] = 0 ;
            ppolylist->paths[npoly] = Npoint = 0 ;
            ppolylist->ppolys[npoly] = &plinedesc->rpoints[ldesc] ;
	  } else {
	    /*ERROR*Too many paths in POLYGONE*/
	    Xw_set_error(32,"Xw_close_path",&ppolylist->npoly) ;
	    return (XW_ERROR) ;
	  }
        }    
          
#ifdef  TRACE_DRAW_POLY
if( Xw_get_trace() > 2 ) {
    printf(" Xw_close_path(%lx)\n",(long ) pwindow) ;
}
#endif
	return (XW_SUCCESS) ;
}

/*
   STATUS Xw_close_poly (awindow):
   XW_EXT_WINDOW *awindow

	Close the Polygone

	returns ERROR if Polygone is empty
	returns SUCCESS if successful 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_poly(void* awindow)
#else
XW_STATUS Xw_close_poly(awindow)
void *awindow ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;

        if( FirstPolyPoint >= 0 ) {
	
 	  if( plinedesc->npoint > FirstPathPoint ) Xw_close_path(pwindow); 
 
          if( !_BINDEX ) {
            int polyindex = pwindow->polyindex ;
            int lineindex = pwindow->lineindex ;
	    GC gcpoly = (QGTILE(pwindow->qgpoly[polyindex].code)) ?
				pwindow->qgpoly[polyindex].gc : NULL ;
	    GC gcline = (QGTYPE(pwindow->qgpoly[polyindex].code)) ?
				pwindow->qgline[lineindex].gc : NULL ;
            Xw_draw_pixel_polys(pwindow,ppolylist,gcpoly,gcline);
            ppolylist->npoly = 0 ;
            plinedesc->npoint = 0 ;
          }  
          FirstPolyPoint = -1 ;
        }    
          
#ifdef  TRACE_DRAW_POLY
if( Xw_get_trace() > 2 ) {
    printf(" Xw_close_poly(%lx)\n",(long ) pwindow) ;
}
#endif
	return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
XW_EXT_POLY* Xw_add_polygone_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_POLY* Xw_add_polygone_structure(pbuflist )
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert at end one Extended polygone structure in the
        polygone List

        returns Extended polygone address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_POLY *ppoly ;

        ppoly = (XW_EXT_POLY*) Xw_malloc(sizeof(XW_EXT_POLY)) ;
        if( ppoly ) {
            ppoly->link = pbuflist->ppolylist ;
            ppoly->isupdated = False ;
            ppoly->npoly = 0 ;
	    pbuflist->ppolylist = ppoly ;
        } else {
	    /*ERROR*EXT_POLYGONE allocation failed*/
	    Xw_set_error(34,"Xw_add_polygone_structure",NULL) ;
        }

        return (ppoly) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_polygone_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_polygone_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended polygone structure in the
        polygone List

        SUCCESS always
*/
{
XW_EXT_POLY *ppoly,*qpoly ;

        for( ppoly = pbuflist->ppolylist ; ppoly ; ppoly = qpoly ) {
            qpoly = (XW_EXT_POLY*)ppoly->link ;
            Xw_free(ppoly) ;
        }
	pbuflist->ppolylist = NULL ;

        return (XW_SUCCESS) ;
}
