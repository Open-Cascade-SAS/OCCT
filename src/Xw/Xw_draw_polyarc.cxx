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

#include <Xw_Extension.h>

/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DRAW_POLYARC
#endif

/*
   STATUS Xw_draw_polyarc (awindow,xc,yc,xradius,yradius,start,angle):
   XW_EXT_WINDOW *awindow
   float xc,yc		Arc center defined in User Space
   float xradius	Horizontal arc radius defined in User Space
   float xradius	Vertical Arc radius defined in User Space
   float start		Start angle defined in RADIAN
   float angle		Arc angle defined in RADIAN

	Display arc in current QG set by set_poly_attrib .
	or retain arcs in buffer.

	returns ERROR if bad parameter
	returns SUCCESS if successfull

*/

#define MAXCOORD 32767
#define MINCOORD -32768

static int BeginArcs = False;
static XW_EXT_ARC *parclist ;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_draw_polyarc (void* awindow,float xc,float yc,
			float xradius,float yradius,float start,float angle)
#else
XW_STATUS Xw_draw_polyarc (awindow,xc,yc,xradius,yradius,start,angle)
void *awindow;
float xc,yc,xradius,yradius,start,angle ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) awindow ;
XW_EXT_BUFFER *pbuffer ;
int narc,bindex,angle1,angle2,x,y,width,height ;

 	if( !Xw_isdefine_window(pwindow) ) {
	    /*ERROR*Bad EXT_WINDOW Address*/
	    Xw_set_error(24,"Xw_draw_polyarc",pwindow) ;
	    return (XW_ERROR) ;
	}

	if( xradius <= 0. ) {
	    /*ERROR*Bad arc radius*/
	    Xw_set_error(115,"Xw_draw_polyarc",&xradius) ;
	    return (XW_ERROR) ;
	}

	if( yradius <= 0. ) {
	    /*ERROR*Bad arc radius*/
	    Xw_set_error(115,"Xw_draw_polyarc",&yradius) ;
	    return (XW_ERROR) ;
	}

	bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
        for( parclist = pbuffer->pparclist ; parclist ;
                           parclist = (XW_EXT_ARC*) parclist->link ) {
            if( parclist->narc < MAXARCS ) break ;
        }

	if( !parclist ) {
	    parclist = Xw_add_polyarc_structure(pbuffer) ;
	}

	if( !parclist ) return XW_ERROR ;

	angle1 = (int )( start*64./DRAD );
	if( angle1 > 0 ) {
	    while( angle1 > MAXANGLE ) angle1 -= MAXANGLE ;
	} else if( angle1 < 0 ) {
	    while( angle1 < -MAXANGLE ) angle1 += MAXANGLE ;
	}
	angle2 = (int )( angle*64./DRAD );
	if( angle2 > 0 ) {
	    while( angle2 > MAXANGLE ) angle2 -= MAXANGLE ;
	} else if( angle2 < 0 ) {
	    while( angle2 < -MAXANGLE ) angle2 += MAXANGLE ;
	}
//OCC186
	width = 2*PVALUE(xradius, pwindow->xratio, pwindow->yratio) ;
	height = 2*PVALUE(yradius, pwindow->xratio, pwindow->yratio) ;
        x = PXPOINT(xc, pwindow->xratio) ; 
        y = PYPOINT(yc, pwindow->attributes.height, pwindow->yratio) ; 
//OCC186
        x = max(min(x,MAXCOORD),MINCOORD);
        y = max(min(y,MAXCOORD),MINCOORD);
	if( width < 0xFFFF && height < 0xFFFF ) {
	    narc = parclist->narc ;
	    parclist->rarcs[narc].width = width ;
	    parclist->rarcs[narc].height = height ;
	    parclist->rarcs[narc].x = x - width/2 ;
	    parclist->rarcs[narc].y = y - height/2 ;
	    parclist->rarcs[narc].angle1 = angle1 ;
	    parclist->rarcs[narc].angle2 = angle2 ;
	    parclist->narc++ ;
            if( bindex > 0 ) {
              pbuffer->isempty = False ;
	      width = (width+1)/2 ;
	      height = (height+1)/2 ;
              pbuffer->rxmin = min(pbuffer->rxmin,x-width) ;
              pbuffer->rymin = min(pbuffer->rymin,y-height) ;
              pbuffer->rxmax = max(pbuffer->rxmax,x+width) ;
              pbuffer->rymax = max(pbuffer->rymax,y+height) ;
            } else if( !BeginArcs ) {
              int polyindex = pwindow->polyindex ;
              int lineindex = pwindow->lineindex ;
              GC gcpoly = pwindow->qgpoly[polyindex].gc ;
              GC gcline = (QGTYPE(pwindow->qgpoly[polyindex].code)) ?
                                pwindow->qgline[lineindex].gc : NULL ;
              Xw_draw_pixel_polyarcs(pwindow,parclist,gcpoly,gcline);
	      parclist->narc = 0 ;
	    }
	} else {
	    /*ERROR*Too big arc radius*/
	    Xw_set_error(116,"Xw_draw_polyarc",0) ;
	    return (XW_ERROR) ;
	}

#ifdef  TRACE_DRAW_POLYARC
if( Xw_get_trace() > 2 ) {
    printf(" Xw_draw_polyarc(%lx,%f,%f,%f,%f,%f,%f\n",
			(long ) pwindow,xc,yc,xradius,yradius,start,angle);
}
#endif

	return (XW_SUCCESS);
}

/*
   STATUS Xw_begin_polyarcs (awindow,narc):
   XW_EXT_WINDOW *awindow
   int narc           Not used


        Begin a set of arcs which must be filled by Xw_draw_polyarc and
                                     closed by Xw_close_polyarcs

        returns ERROR if bad extended window address
        returns SUCCESS if successful

*/
 
#ifdef XW_PROTOTYPE
XW_STATUS Xw_begin_polyarcs(void* awindow,int narc)
#else
XW_STATUS Xw_begin_polyarcs(awindow,narc)
void *awindow ;
int narc ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
 
        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_begin_polyarcs",pwindow) ;
            return (XW_ERROR) ;
        }    
 
        if( BeginArcs ) Xw_close_polyarcs(pwindow);
        
        BeginArcs = True;
 
#ifdef  TRACE_DRAW_POLYARC
if( Xw_get_trace() > 2 ) {
    printf(" Xw_begin_polyarcs(%lx,%d)\n",(long ) pwindow,narc) ;
}
#endif
        return (XW_SUCCESS) ;
}
 
/*
   STATUS Xw_close_polyarcs (awindow):
   XW_EXT_WINDOW *awindow
 
        Close the set of arcs
 
        returns ERROR if bad extended window address
        returns SUCCESS successful
 
*/
 
#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_polyarcs(void* awindow)
#else
XW_STATUS Xw_close_polyarcs(awindow)
void *awindow ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
int bindex ;
 
        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_close_polyarcs",pwindow) ;
            return (XW_ERROR) ;
        }    

	bindex = _BINDEX ; 
        if( BeginArcs && !bindex ) {
          int polyindex = pwindow->polyindex ;
          int lineindex = pwindow->lineindex ;
          GC gcpoly = pwindow->qgpoly[polyindex].gc ;
          GC gcline = (QGTYPE(pwindow->qgpoly[polyindex].code)) ?
                                pwindow->qgline[lineindex].gc : NULL ;
          for( parclist = _BUFFER(bindex).pparclist ; parclist ;
                           parclist = (XW_EXT_ARC*) parclist->link ) {
            if( parclist->narc > 0 ) {
              Xw_draw_pixel_polyarcs(pwindow,parclist,gcpoly,gcline);
              parclist->narc = 0 ;
            } else break ;
          }
        }    
 
        BeginArcs = False;
 
#ifdef  TRACE_DRAW_POLYARC
if( Xw_get_trace() > 2 ) {
    printf(" Xw_close_polyarcs(%lx)\n",(long ) pwindow) ;
}
#endif
        return (XW_SUCCESS) ;
}


#ifdef XW_PROTOTYPE
void Xw_draw_pixel_polyarcs (XW_EXT_WINDOW* pwindow,XW_EXT_ARC* parclist,
							GC gcpoly,GC gcline)
#else
void Xw_draw_pixel_polyarcs (pwindow,parclist,gcpoly,gcline)
XW_EXT_WINDOW *pwindow;
XW_EXT_ARC *parclist;
GC gcpoly,gcline;
#endif /*XW_PROTOTYPE*/
{

   if( parclist->isupdated ) {
     XFillArcs(_DISPLAY,_DRAWABLE,gcpoly,parclist->uarcs,parclist->narc) ;
     if( gcline && (gcpoly != gcline) ) {
       XDrawArcs(_DISPLAY,_DRAWABLE,gcline,parclist->uarcs,parclist->narc) ;
     }
   } else {
     XFillArcs(_DISPLAY,_DRAWABLE,gcpoly,parclist->rarcs,parclist->narc) ;
     if( gcline && (gcpoly != gcline) ) {
       XDrawArcs(_DISPLAY,_DRAWABLE,gcline,parclist->rarcs,parclist->narc) ;
     }
   }
}

#ifdef XW_PROTOTYPE
XW_EXT_ARC* Xw_add_polyarc_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_ARC* Xw_add_polyarc_structure(pbuflist )
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert at end one Extended arc structure in the
        arc List

        returns Extended arc address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_ARC *parc ;
 
        parc = (XW_EXT_ARC*) Xw_malloc(sizeof(XW_EXT_ARC)) ;
        if( parc ) {
            parc->link = pbuflist->pparclist ;
            parc->isupdated = 0 ;
            parc->narc = 0 ;
            pbuflist->pparclist = parc ;
        } else {
	    /*ERROR*EXT_ARC Allocation failed*/
	    Xw_set_error(35,"Xw_add_polyarc_structure",NULL) ;
        }

        return (parc) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_polyarc_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_polyarc_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended arc structure in the
        arc List

        SUCCESS always
*/
{
XW_EXT_ARC *parc,*qarc ;

      	for( parc = pbuflist->pparclist ; parc ; parc = qarc ) {
            qarc = (XW_EXT_ARC*)parc->link ;
            Xw_free(parc) ;
        }
	pbuflist->pparclist = NULL ;
	
	return (XW_SUCCESS) ;
}
