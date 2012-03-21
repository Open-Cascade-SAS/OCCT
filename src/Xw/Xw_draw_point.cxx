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


#define BUC0056         /*GG_300496
Protection contre zoom trop grand*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DRAW_POINT
#endif

/*
   STATUS Xw_draw_point (awindow,x,y):
   XW_EXT_WINDOW *awindow
   float x,y	Point position in user space


	Display point in current QG set by set_marker_attrib .
        Note that points can be buffered depending of the DisplayMode context
        and Flush at Xw_flush time .

	returns SUCCESS always 

*/

#ifdef BUC0056
#define MAXCOORD 32767
#define MINCOORD -32768
#endif

static int BeginPoints = False ;
static XW_EXT_POINT *ppntlist = NULL ;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_draw_point (void* awindow,float x,float y)
#else
XW_STATUS Xw_draw_point (awindow,x,y)
void *awindow;
float x,y ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
int npnt,bindex,ix,iy ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_draw_point",pwindow) ;
            return (XW_ERROR) ;
        }

	bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
        for( ppntlist = pbuffer->ppntlist ; ppntlist ;
                        	ppntlist = (XW_EXT_POINT*) ppntlist->link ) {
             if( ppntlist->npoint < MAXPOINTS ) break ;
        }

        if( !ppntlist ) {
            ppntlist = Xw_add_point_structure(pbuffer) ;
        }

	if( !ppntlist ) return XW_ERROR ;

//OCC186
	ix = PXPOINT(x, pwindow->xratio) ;
	iy = PYPOINT(y, pwindow->attributes.height, pwindow->yratio) ;
//OCC186

#ifdef BUC0056
        if( ix < MINCOORD || ix > MAXCOORD ) return XW_SUCCESS;
        if( iy < MINCOORD || iy > MAXCOORD ) return XW_SUCCESS;
#endif 
	npnt = ppntlist->npoint ;
	ppntlist->rpoints[npnt].x = ix ; 
	ppntlist->rpoints[npnt].y = iy ; 
	ppntlist->npoint++ ;

        if( bindex > 0 ) {
            pbuffer->isempty = False ;
            pbuffer->rxmin = min(pbuffer->rxmin,ix) ;
            pbuffer->rymin = min(pbuffer->rymin,iy) ;
            pbuffer->rxmax = max(pbuffer->rxmax,ix) ;
            pbuffer->rymax = max(pbuffer->rymax,iy) ;
        } else if( !BeginPoints ) {
	    int index = pwindow->markindex ;
	    Xw_draw_pixel_points(pwindow,ppntlist,pwindow->qgmark[index].gc) ;
	    ppntlist->npoint = 0 ;
	}
	
#ifdef  TRACE_DRAW_POINT
if( Xw_get_trace() > 2 ) {
    printf(" Xw_draw_point(%lx,%f,%f)\n",(long ) pwindow,x,y) ;
}
#endif

	return (XW_SUCCESS);
}

/*
   STATUS Xw_begin_points (awindow,npoint):
   XW_EXT_WINDOW *awindow
   int npoint           Not used


        Begin a set of points which must be filled by Xw_draw_point and
                                     closed by Xw_close_points

        returns ERROR if bad extended window address
        returns SUCCESS if successful

*/
 
#ifdef XW_PROTOTYPE
XW_STATUS Xw_begin_points(void* awindow,int npoint)
#else
XW_STATUS Xw_begin_points(awindow,npoint)
void *awindow ;
int npoint ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
 
        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_begin_points",pwindow) ;
            return (XW_ERROR) ;
        }    
 
        if( BeginPoints ) Xw_close_points(pwindow);
        
        BeginPoints = True;

#ifdef  TRACE_DRAW_POINT
if( Xw_get_trace() > 2 ) {
    printf(" Xw_begin_points(%lx,%d)\n",(long ) pwindow,npoint) ;
}
#endif
        return (XW_SUCCESS) ;
}
 
/*
   STATUS Xw_close_points (awindow):
   XW_EXT_WINDOW *awindow
 
        Close the set of points
 
        returns ERROR if bad extended window address
        returns SUCCESS successful
 
*/
 
#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_points(void* awindow)
#else
XW_STATUS Xw_close_points(awindow)
void *awindow ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_BUFFER *pbuffer ;
int bindex ;
  
        bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
        if( BeginPoints && !bindex ) {
          int index = pwindow->lineindex ;
          for( ppntlist = pbuffer->ppntlist ; ppntlist ;
                           ppntlist = (XW_EXT_POINT*) ppntlist->link ) {
            if( ppntlist->npoint > 0 ) {
              Xw_draw_pixel_points(pwindow,ppntlist,pwindow->qgmark[index].gc) ;
	      ppntlist->npoint = 0 ;
            } else break ;
          }  
        }   
          
        BeginPoints = False;
 
#ifdef  TRACE_DRAW_POINT
if( Xw_get_trace() > 2 ) {
    printf(" Xw_close_points(%lx)\n",(long ) pwindow) ;
}
#endif
        return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
void Xw_draw_pixel_points(XW_EXT_WINDOW *pwindow,XW_EXT_POINT* ppntlist,GC gc)
#else
void Xw_draw_pixel_points(pwindow,ppntlist,gc)
XW_EXT_WINDOW *pwindow ;
XW_EXT_POINT *ppntlist ;
GC gc ;
#endif /*XW_PROTOTYPE*/
{

	if( ppntlist->isupdated ) {
          XDrawPoints(_DISPLAY,_DRAWABLE,gc,ppntlist->upoints,
				ppntlist->npoint,CoordModeOrigin) ;
	} else {
          XDrawPoints(_DISPLAY,_DRAWABLE,gc,ppntlist->rpoints,
				ppntlist->npoint,CoordModeOrigin) ;
	}
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_point_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_point_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended point structure in the
        point List

        SUCCESS always
*/
{
XW_EXT_POINT *ppoint,*qpoint ;

        for( ppoint = pbuflist->ppntlist ; ppoint ; ppoint = qpoint ) {
            qpoint = (XW_EXT_POINT*)ppoint->link ;
            Xw_free(ppoint) ;
        }
	pbuflist->ppntlist = NULL ;

        return (XW_SUCCESS) ;
}

#ifdef XW_PROTOTYPE
XW_EXT_POINT* Xw_add_point_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_POINT* Xw_add_point_structure(pbuflist )
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert at end one Extended point structure in the
        point List

        returns Extended point address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_POINT *ppoint ;

        ppoint = (XW_EXT_POINT*) Xw_malloc(sizeof(XW_EXT_POINT)) ;
        if( ppoint ) {
            ppoint->link = pbuflist->ppntlist ;
            ppoint->isupdated = False ;
            ppoint->npoint = 0 ;
            pbuflist->ppntlist = ppoint ;
        } else {
	    /*ERROR*EXT_POINT Allocation failed*/
	    Xw_set_error(117,"Xw_add_point_structure",NULL) ;
        }

        return (ppoint) ;
}
