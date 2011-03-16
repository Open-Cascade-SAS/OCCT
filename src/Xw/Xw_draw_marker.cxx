#define BUC0056         /*GG_300496
                        Protection contre zoom trop grand*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DRAW_MARKER
#endif

/*
   STATUS Xw_draw_marker (awindow,index,x,y,width,height,angle):
   XW_EXT_WINDOW *awindow
   int index		Marker index
   float x,y		Marker position in user space
   float width,height	Marker size in MM.
   float angle		Marker orientation in RAD


	Display polyline & polygones marker in current QG set 
	by set_marker_attrib .
        Note that markers can be buffered depending of the DisplayMode context
        and Flush at Xw_flush time .

	returns SUCCESS always 

*/

#ifdef BUC0056
#define MAXCOORD 32767
#define MINCOORD -32768
#endif

static int BeginMarkers = False ;
static XW_EXT_LMARKER *plmarklist ;
static XW_EXT_PMARKER *ppmarklist ;

#define XROTATE(x,y) (x*cosa - y*sina)
#define YROTATE(x,y) (x*sina + y*cosa)

#ifdef XW_PROTOTYPE
XW_STATUS Xw_draw_marker (void* awindow,int index,
			float x,float y,float width,float height,float angle)
#else
XW_STATUS Xw_draw_marker (awindow,index,x,y,width,height,angle)
void *awindow;
int index;
float x,y ;
float width,height;
float angle;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_DISPLAY *pdisplay = pwindow->connexion;
XW_EXT_BUFFER *pbuffer ;
int i,np,type,bindex,rotate = False ;
int ix,iy,ix1,iy1,ix2,iy2,sm,*ps;
int npoint,nseg,nmark ;
float xm,ym,cosa=0,sina=0,*px,*py;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_draw_marker",pwindow) ;
            return (XW_ERROR) ;
        }

        if( !Xw_isdefine_marker(_MARKMAP,index) ) {
            /*ERROR*Bad Defined Marker*/
            Xw_set_error(77,"Xw_draw_marker",&index) ;
            return (XW_ERROR) ;
        }

	type = QGTYPE(pwindow->qgmark[pwindow->markindex].code);
	np = _MARKMAP->npoint[index] ;
//OCC186
	ix = PXPOINT(x, pwindow->xratio) ;
	iy = PYPOINT(y, pwindow->attributes.height, pwindow->yratio) ;
//OCC186

#ifdef BUC0056
	if( ix < MINCOORD || ix > MAXCOORD ) return XW_SUCCESS;
	if( iy < MINCOORD || iy > MAXCOORD ) return XW_SUCCESS;
#endif

	if( angle != 0. ) {
	  rotate = True;
	  cosa = cos((double)angle);
	  sina = sin((double)angle);
	} 

        if( np >= MAXPOINTS ) {
	    np = MAXPOINTS-1;
            /*ERROR*Too many points in MARKER*/
            Xw_set_error(112,"Xw_draw_marker",&np) ;
	}

	bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
	if( bindex > 0 ) type = 0 ;	/* Poly markers will be disable */

	ppmarklist = NULL ;
	if( type > 0 ) {

          for( ppmarklist = pbuffer->ppmarklist ; ppmarklist ;
                       	ppmarklist = (XW_EXT_PMARKER*) ppmarklist->link ) {
            if( (ppmarklist->nmark < MAXPMARKERS) &&
			(ppmarklist->npoint + np < MAXPOINTS) ) break ;
          }

          if( !ppmarklist ) {
            ppmarklist = Xw_add_pmarker_structure(pbuffer) ;
          }

          if( !ppmarklist ) return XW_ERROR ;

	  npoint = ppmarklist->npoint ;
	  nmark = ppmarklist->nmark ;
	  ix1 = ix ; iy1 = iy ;
	  ps = _MARKMAP->spoint[index];
	  px = _MARKMAP->xpoint[index];
	  py = _MARKMAP->ypoint[index];
          for( i=0 ; i<np ; i++,ps++,px++,py++ ) {
	    sm = *ps; xm = *px * width/2. ; ym = *py * height/2.; 
	    if( rotate ) {
		float xr,yr;
		xr = XROTATE(xm,ym);
		yr = YROTATE(xm,ym);
	        ix2 = ix + PMMXVALUE(xr); iy2 = iy - PMMYVALUE(yr);
	    } else {
	        ix2 = ix + PMMXVALUE(xm); iy2 = iy - PMMYVALUE(ym);
	    }
            ppmarklist->rpoints[npoint].x = ix2 ;
            ppmarklist->rpoints[npoint].y = iy2 ;
	    if( sm || !i ) npoint++;
	    else break; 
	    ix1 = ix2; iy1 = iy2;
	  }
	  ppmarklist->marks[nmark] = npoint - ppmarklist->npoint;
          ppmarklist->npoint = npoint ;
	  ppmarklist->rcenters[nmark].x = ix ;
	  ppmarklist->rcenters[nmark].y = iy ;
	  ppmarklist->nmark++ ;

	}

        for( plmarklist = pbuffer->plmarklist ; plmarklist ;
                       	plmarklist = (XW_EXT_LMARKER*) plmarklist->link ) {
            if( (plmarklist->nmark < MAXLMARKERS) &&
			(plmarklist->nseg + np < MAXPOINTS) ) break ;
        }

        if( !plmarklist ) {
            plmarklist = Xw_add_lmarker_structure(pbuffer) ;
        }

        if( !plmarklist ) return XW_ERROR ;

	nseg = plmarklist->nseg ;
	nmark = plmarklist->nmark ;
	ix1 = ix ; iy1 = iy ;
	ps = _MARKMAP->spoint[index];
	px = _MARKMAP->xpoint[index];
	py = _MARKMAP->ypoint[index];
        for( i=0 ; i<np ; i++,ps++,px++,py++ ) {
	    sm = *ps; xm = *px * width/2. ; ym = *py * height/2.; 
	    if( rotate ) {
		float xr,yr;
		xr = xm*cosa - ym*sina;
		yr = xm*sina + ym*cosa;
	        ix2 = ix + PMMXVALUE(xr); iy2 = iy - PMMYVALUE(yr);
	    } else {
	        ix2 = ix + PMMXVALUE(xm); iy2 = iy - PMMYVALUE(ym);
	    }
	    if( sm ) {
              plmarklist->rsegments[nseg].x1 = ix1 ;
              plmarklist->rsegments[nseg].y1 = iy1 ;
              plmarklist->rsegments[nseg].x2 = ix2 ;
              plmarklist->rsegments[nseg].y2 = iy2 ;
	      nseg++;
	    }
	    ix1 = ix2; iy1 = iy2;
	}
	plmarklist->marks[nmark] = nseg - plmarklist->nseg;
        plmarklist->nseg = nseg ;
	plmarklist->rcenters[nmark].x = ix ;
	plmarklist->rcenters[nmark].y = iy ;
	plmarklist->nmark++ ;

        if( bindex > 0 ) {
           int w = (PMMXVALUE(width)+1)/2 ;
           int h = (PMMYVALUE(height)+1)/2 ;
           pbuffer->isempty = False ;
           pbuffer->rxmin = min(pbuffer->rxmin,ix-w) ;
           pbuffer->rymin = min(pbuffer->rymin,iy-h) ;
           pbuffer->rxmax = max(pbuffer->rxmax,ix+w) ;
           pbuffer->rymax = max(pbuffer->rymax,iy+h) ;
	} else if( !BeginMarkers ) {
	  if( ppmarklist ) {
	    int index = pwindow->polyindex ;
	    Xw_draw_pixel_pmarkers(pwindow,ppmarklist,
						pwindow->qgpoly[index].gc);
	    ppmarklist->nmark = 0 ;
	    ppmarklist->npoint = 0 ;
	  } 
	  if( plmarklist ) {
	    int index = pwindow->markindex ;
	    Xw_draw_pixel_lmarkers(pwindow,plmarklist,
						pwindow->qgmark[index].gc);
	    plmarklist->nmark = 0 ;
	    plmarklist->nseg = 0 ;
	  }
	}

#ifdef  TRACE_DRAW_MARKER
if( Xw_get_trace() > 2 ) {
    printf(" Xw_draw_marker(%lx,%d,%f,%f,%f,%f,%f)\n",
				(long ) pwindow,index,x,y,width,height,angle) ;
}
#endif

	return (XW_SUCCESS);
}

/*
   STATUS Xw_begin_markers (awindow,nmarker):
   XW_EXT_WINDOW *awindow
   int nmarker           Not used


        Begin a set of markers which must be filled by Xw_draw_marker and
                                     closed by Xw_close_markers

        returns ERROR if bad extended window address
        returns SUCCESS if successful

*/
 
#ifdef XW_PROTOTYPE
XW_STATUS Xw_begin_markers(void* awindow,int nmarker)
#else
XW_STATUS Xw_begin_markers(awindow,nmarker)
void *awindow ;
int nmarker ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
 
        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_begin_markers",pwindow) ;
            return (XW_ERROR) ;
        }    
 
        if( BeginMarkers ) Xw_close_markers(pwindow);
        
        BeginMarkers = True;
 
#ifdef  TRACE_DRAW_MARKER
if( Xw_get_trace() > 2 ) {
    printf(" Xw_begin_markers(%lx,%d)\n",(long ) pwindow,nmarker) ;
}
#endif
        return (XW_SUCCESS) ;
}
 
/*
   STATUS Xw_close_markers (awindow):
   XW_EXT_WINDOW *awindow
 
        Close the set of markers
 
        returns ERROR if bad extended window address
        returns SUCCESS successful
 
*/
 
#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_markers(void* awindow)
#else
XW_STATUS Xw_close_markers(awindow)
void *awindow ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_BUFFER *pbuffer ;
int bindex ;
 
        bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
        if( BeginMarkers && !bindex ) {
          int index = pwindow->polyindex ;
          for( ppmarklist = pbuffer->ppmarklist ; ppmarklist ;
                           ppmarklist = (XW_EXT_PMARKER*) ppmarklist->link ) {
            if( ppmarklist->nmark > 0 ) {
              Xw_draw_pixel_pmarkers(pwindow,ppmarklist,
					pwindow->qgpoly[index].gc) ;
	      ppmarklist->nmark = 0 ;
	      ppmarklist->npoint = 0 ;
            } else break ;
          }  
          index = pwindow->markindex ;
          for( plmarklist = pbuffer->plmarklist ; plmarklist ;
                           plmarklist = (XW_EXT_LMARKER*) plmarklist->link ) {
	    if( plmarklist->nseg > 0 ) {
	      Xw_draw_pixel_lmarkers(pwindow,plmarklist,
					pwindow->qgmark[index].gc) ;
	      plmarklist->nmark = 0 ;
	      plmarklist->nseg = 0 ;
	    } else break ;
	  }
        }   
          
        BeginMarkers = False;
 
#ifdef  TRACE_DRAW_MARKER
if( Xw_get_trace() > 2 ) {
    printf(" Xw_close_markers(%lx)\n",(long ) pwindow) ;
}
#endif
        return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
void Xw_draw_pixel_pmarkers (XW_EXT_WINDOW* pwindow,XW_EXT_PMARKER* pmarklist,GC gc)
#else
void Xw_draw_pixel_pmarkers (pwindow,pmarklist,gc)
XW_EXT_WINDOW *pwindow;
XW_EXT_PMARKER *pmarklist;
GC gc ;
#endif /*XW_PROTOTYPE*/
{
int i,j,npoint ;
XPoint *ppoint ;
 
        for( i=j=0 ; i<pmarklist->nmark ; i++,j += npoint ) {
          npoint = pmarklist->marks[i] ;
	  if( pmarklist->isupdated ) {
            ppoint = &pmarklist->upoints[j] ;
	  } else {
            ppoint = &pmarklist->rpoints[j] ;
	  }
          if( npoint > 2 ) {
            XFillPolygon(_DISPLAY,_DRAWABLE,gc,ppoint,npoint,
                                                Nonconvex,CoordModeOrigin) ;
          }
        }      
}

#ifdef XW_PROTOTYPE
void Xw_draw_pixel_lmarkers (XW_EXT_WINDOW* pwindow,XW_EXT_LMARKER* pmarklist,GC gc)
#else
void Xw_draw_pixel_lmarkers (pwindow,pmarklist,gc)
XW_EXT_WINDOW *pwindow;
XW_EXT_LMARKER *pmarklist;
GC gc ;
#endif /*XW_PROTOTYPE*/
{

	if( pmarklist->isupdated ) {
          XDrawSegments(_DISPLAY,_DRAWABLE,gc,
			pmarklist->usegments,pmarklist->nseg) ;
	} else {
          XDrawSegments(_DISPLAY,_DRAWABLE,gc,
			pmarklist->rsegments,pmarklist->nseg) ;
	}
 
}

#ifdef XW_PROTOTYPE
XW_EXT_LMARKER* Xw_add_lmarker_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_LMARKER* Xw_add_lmarker_structure(pbuflist )
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert at end one Extended marker structure in the
        marker List

        returns Extended marker address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_LMARKER *pmarker ;

        pmarker = (XW_EXT_LMARKER*) Xw_malloc(sizeof(XW_EXT_LMARKER)) ;
        if( pmarker ) {
            pmarker->link = pbuflist->plmarklist ;
            pmarker->isupdated = False ;
            pmarker->nmark = 0 ;
	    pmarker->nseg = 0 ;
            pbuflist->plmarklist = pmarker ;
        } else {
            /*ERROR*EXT_MARKER Allocation failed*/
            Xw_set_error(31,"Xw_add_marker_structure",NULL) ;
        }      
 
        return (pmarker) ;
}
 
#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_lmarker_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_lmarker_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended marker structure in the
        marker List
 
        SUCCESS always
*/
{
XW_EXT_LMARKER *pmarker,*qmarker ;
 
        for( pmarker = pbuflist->plmarklist ; pmarker ; pmarker = qmarker ) {
            qmarker = (XW_EXT_LMARKER*)pmarker->link ;
            Xw_free(pmarker) ;
        }      
        pbuflist->plmarklist = NULL ;
        
        return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
XW_EXT_PMARKER* Xw_add_pmarker_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_PMARKER* Xw_add_pmarker_structure(pbuflist )
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert at end one Extended marker structure in the
        marker List

        returns Extended marker address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_PMARKER *pmarker ;

        pmarker = (XW_EXT_PMARKER*) Xw_malloc(sizeof(XW_EXT_PMARKER)) ;
        if( pmarker ) {
            pmarker->link = pbuflist->ppmarklist ;
            pmarker->isupdated = False ;
            pmarker->nmark = 0 ;
	    pmarker->npoint = 0 ;
            pbuflist->ppmarklist = pmarker ;
        } else {
            /*ERROR*EXT_MARKER Allocation failed*/
            Xw_set_error(31,"Xw_add_marker_structure",NULL) ;
        }      
 
        return (pmarker) ;
}
 
#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_pmarker_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_pmarker_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended marker structure in the
        marker List
 
        SUCCESS always
*/
{
XW_EXT_PMARKER *pmarker,*qmarker ;
 
        for( pmarker = pbuflist->ppmarklist ; pmarker ; pmarker = qmarker ) {
            qmarker = (XW_EXT_PMARKER*)pmarker->link ;
            Xw_free(pmarker) ;
        }      
        pbuflist->ppmarklist = NULL ;
        
        return (XW_SUCCESS) ;
}
