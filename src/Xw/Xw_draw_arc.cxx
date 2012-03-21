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


#include <Standard_Integer.hxx>

#include <Xw_Extension.h>

/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DRAW_ARC
#endif

/*
   STATUS Xw_draw_arc (awindow,xc,yc,xradius,yradius,start,angle):
   XW_EXT_WINDOW *awindow
   float xc,yc		Arc center defined in User Space
   float xradius	Horizontal arc radius defined in User Space
   float xradius	Vertical Arc radius defined in User Space
   float start		Start angle defined in RADIAN
   float angle		Arc angle defined in RADIAN

	Display arc in current QG set by set_line_attrib .
	or retain arc in buffer.

	return ERROR if bad parameter
	returns SUCCESS if successful

*/

#define MAXCOORD 32767
#define MINCOORD -32768

static int BeginArcs = False;
static XW_EXT_ARC *parclist ;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_draw_arc (void* awindow,float xc,float yc,
			float xradius,float yradius,float start,float angle)
#else
XW_STATUS Xw_draw_arc (awindow,xc,yc,xradius,yradius,start,angle)
void *awindow;
float xc,yc,xradius,yradius,start,angle ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) awindow ;
XW_EXT_BUFFER *pbuffer ;
int narc,bindex,angle1,angle2,x,y,width,height ;

 	if( !Xw_isdefine_window(pwindow) ) {
	    /*ERROR*Bad EXT_WINDOW Address*/
	    Xw_set_error(24,"Xw_draw_arc",pwindow) ;
	    return (XW_ERROR) ;
	}

	if( xradius <= 0. ) {
	    /*ERROR*Bad arc radius*/
	    Xw_set_error(115,"Xw_draw_arc",&xradius) ;
	    return (XW_ERROR) ;
	}

	if( yradius <= 0. ) {
	    /*ERROR*Bad arc radius*/
	    Xw_set_error(115,"Xw_draw_arc",&yradius) ;
	    return (XW_ERROR) ;
	}

	bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
        for( parclist = pbuffer->plarclist ; parclist ;
                           parclist = (XW_EXT_ARC*) parclist->link ) {
            if( parclist->narc < MAXARCS ) break ;
        }

	if( !parclist ) {
	    parclist = Xw_add_arc_structure(pbuffer) ;
	}

	if( !parclist ) return XW_ERROR ;

        angle1 = (Standard_Integer )( start*64./DRAD );
        if( angle1 > 0 ) {
            while( angle1 > MAXANGLE ) angle1 -= MAXANGLE ;
        } else if( angle1 < 0 ) {
            while( angle1 < -MAXANGLE ) angle1 += MAXANGLE ;
        }
        angle2 = (Standard_Integer ) ( angle*64./DRAD );
        if( angle2 > 0 ) {
            while( angle2 > MAXANGLE ) angle2 -= MAXANGLE ;
        } else if( angle2 < 0 ) {
            while( angle2 < -MAXANGLE ) angle2 += MAXANGLE ;
        }
//OCC186
        width = 2*PVALUE(xradius, pwindow->xratio, pwindow->yratio);
        height = 2*PVALUE(yradius, pwindow->xratio, pwindow->yratio);
	x = PXPOINT(xc, pwindow->xratio);
	y = PYPOINT(yc, pwindow->attributes.height, pwindow->yratio);
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
	      int index = pwindow->lineindex ;
              Xw_draw_pixel_arcs(pwindow,parclist,pwindow->qgline[index].gc) ;
	      parclist->narc = 0 ;
	    }
	} else {
	    /*ERROR*Too big arc radius*/
	    Xw_set_error(116,"Xw_draw_arc",0) ;
	    return (XW_ERROR) ;
	}

#ifdef  TRACE_DRAW_ARC
if( Xw_get_trace() > 2 ) {
    printf(" Xw_draw_arc(%lx,%f,%f,%f,%f,%f,%f\n",
			(long ) pwindow,xc,yc,xradius,yradius,start,angle);
}
#endif

	return (XW_SUCCESS);
}

/*
   STATUS Xw_begin_arcs (awindow,narc):
   XW_EXT_WINDOW *awindow
   int narc           Not used


        Begin a set of arcs which must be filled by Xw_draw_arc and
                                     closed by Xw_close_arcs

        returns ERROR if bad extended window address
        returns SUCCESS if successful

*/
 
#ifdef XW_PROTOTYPE
XW_STATUS Xw_begin_arcs(void* awindow,int narc)
#else
XW_STATUS Xw_begin_arcs(awindow,narc)
void *awindow ;
int narc ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
 
        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_begin_arcs",pwindow) ;
            return (XW_ERROR) ;
        }    
 
        if( BeginArcs ) Xw_close_arcs(pwindow);
        
        BeginArcs = True;
 
#ifdef  TRACE_DRAW_ARC
if( Xw_get_trace() > 2 ) {
    printf(" Xw_begin_arcs(%lx,%d)\n",(long ) pwindow,narc) ;
}
#endif
        return (XW_SUCCESS) ;
}
 
/*
   STATUS Xw_close_arcs (awindow):
   XW_EXT_WINDOW *awindow
 
        Close the set of arcs
 
        returns ERROR if bad extended window address
        returns SUCCESS successful
 
*/
 
#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_arcs(void* awindow)
#else
XW_STATUS Xw_close_arcs(awindow)
void *awindow ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
int bindex ;
 
        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_close_arcs",pwindow) ;
            return (XW_ERROR) ;
        }    

	bindex = _BINDEX ;
        if( BeginArcs && !bindex ) {
	  int index = pwindow->lineindex ;
          for( parclist = _BUFFER(bindex).plarclist ; parclist ;
                           parclist = (XW_EXT_ARC*) parclist->link ) {
	    if( parclist->narc > 0 ) {
              Xw_draw_pixel_arcs(pwindow,parclist,pwindow->qgline[index].gc) ;
	      parclist->narc = 0 ;
	    } else break ;
	  }
        }    
 
        BeginArcs = False;
 
#ifdef  TRACE_DRAW_ARC
if( Xw_get_trace() > 2 ) {
    printf(" Xw_close_arcs(%lx)\n",(long ) pwindow) ;
}
#endif
        return (XW_SUCCESS) ;
}


#ifdef XW_PROTOTYPE
void Xw_draw_pixel_arcs (XW_EXT_WINDOW* pwindow,XW_EXT_ARC* parclist,GC gc)
#else
void Xw_draw_pixel_arcs (pwindow,parclist,gc)
XW_EXT_WINDOW *pwindow;
XW_EXT_ARC *parclist;
GC gc;
#endif /*XW_PROTOTYPE*/
{
	if( parclist->isupdated ) {
   	  XDrawArcs(_DISPLAY,_DRAWABLE,gc,parclist->uarcs,parclist->narc) ;
	} else {
   	  XDrawArcs(_DISPLAY,_DRAWABLE,gc,parclist->rarcs,parclist->narc) ;
	}

}

#ifdef XW_PROTOTYPE
XW_EXT_ARC* Xw_add_arc_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_ARC* Xw_add_arc_structure(pbuflist )
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
            parc->link = pbuflist->plarclist ;
	    parc->isupdated = False ;
            parc->narc = 0 ;
            pbuflist->plarclist = parc ;
        } else {
	    /*ERROR*EXT_ARC Allocation failed*/
	    Xw_set_error(35,"Xw_add_arc_structure",NULL) ;
        }

        return (parc) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_arc_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_arc_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended arc structure in the
        arc List

        SUCCESS always
*/
{
XW_EXT_ARC *parc,*qarc ;

      	for( parc = pbuflist->plarclist ; parc ; parc = qarc ) {
            qarc = (XW_EXT_ARC*)parc->link ;
            Xw_free(parc) ;
        }
	pbuflist->plarclist = NULL ;
	
	return (XW_SUCCESS) ;
}
