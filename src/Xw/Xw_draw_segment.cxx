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


#define S3593		/*GG_130398
OPTIMISATION MFT
			Activer le clipping de maniere optionnelle
*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_DRAW_SEGMENT
#endif

static XW_EXT_SEGMENT *pseglist ;
static int BeginSegments = False ;

/*
   STATUS Xw_draw_segment (awindow,x1,y1,x2,y2):
   XW_EXT_WINDOW *awindow
   float x1,y1		First point Coordinates defined in User Space
   float x2,y2		Second point Coordinates defined in User Space

	Display segment in current QG set by set_line_attrib .
	Note that segments can be buffered depending of the DisplayMode context
	and Flush at Xw_flush time .

	returns SUCCESS always

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_draw_segment (void* awindow,float x1,float y1,float x2,float y2)
#else
XW_STATUS Xw_draw_segment (awindow,x1,y1,x2,y2)
void *awindow;
float x1,y1,x2,y2 ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_BUFFER *pbuffer ;
int status ;
int nseg,bindex;
int ix1,iy1,ix2,iy2 ;

	if( !Xw_isdefine_window(pwindow) ) {
	   /*ERROR*Bad EXT_WINDOW Address*/
	   Xw_set_error(24,"Xw_draw_segment",pwindow) ;
	   return (XW_ERROR) ;
	}

	bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
        for( pseglist = pbuffer->pseglist ; pseglist ;
                        pseglist = (XW_EXT_SEGMENT*)pseglist->link ) {
            if( pseglist->nseg < MAXSEGMENTS ) break ;
        }
             
        if( !pseglist ) {
            pseglist = Xw_add_segment_structure(pbuffer) ;
	}


	if( !pseglist ) return XW_ERROR ;

//OCC186
	ix1 = PXPOINT(x1, pwindow->xratio) ;
	iy1 = PYPOINT(y1, pwindow->attributes.height, pwindow->yratio) ;
	ix2 = PXPOINT(x2, pwindow->xratio) ;
	iy2 = PYPOINT(y2, pwindow->attributes.height, pwindow->yratio) ;
//OCC186

	nseg = pseglist->nseg ;

#ifdef S3593
	if( pwindow->clipflag ) {
#endif
	  status = Xw_clip_segment(pwindow,ix1,iy1,ix2,iy2,
					&pseglist->rsegments[nseg]);
	  if( status < 0 ) return (XW_SUCCESS);
#ifdef S3593
	} else {
	  pseglist->rsegments[nseg].x1 = ix1 ;
	  pseglist->rsegments[nseg].y1 = iy1 ;
	  pseglist->rsegments[nseg].x2 = ix2 ;
	  pseglist->rsegments[nseg].y2 = iy2 ;
	}
#endif
	pseglist->nseg++ ;

	if( bindex > 0 ) {
	    pbuffer->isempty = False ;
	    if( ix1 < ix2 ) {
	      pbuffer->rxmin = min(pbuffer->rxmin,ix1) ;
	      pbuffer->rxmax = max(pbuffer->rxmax,ix2) ;
	    } else {
	      pbuffer->rxmin = min(pbuffer->rxmin,ix2) ;
	      pbuffer->rxmax = max(pbuffer->rxmax,ix1) ;
	    }
	    if( iy1 < iy2 ) {
	      pbuffer->rymin = min(pbuffer->rymin,iy1) ;
	      pbuffer->rymax = max(pbuffer->rymax,iy2) ;
	    } else {
	      pbuffer->rymin = min(pbuffer->rymin,iy2) ;
	      pbuffer->rymax = max(pbuffer->rymax,iy1) ;
	    }
	} else if( !BeginSegments ) {
	    int index = pwindow->lineindex ;
	    Xw_draw_pixel_segments(pwindow,pseglist,pwindow->qgline[index].gc) ;
	    pseglist->nseg = 0 ;
	}

#ifdef  TRACE_DRAW_SEGMENT
if( Xw_get_trace() > 2 ) {
    printf(" Xw_draw_segment(%lx,%f,%f,%f,%f\n",(long ) pwindow,x1,y1,x2,y2) ;
}
#endif

	return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
void Xw_draw_pixel_segments(XW_EXT_WINDOW* pwindow,XW_EXT_SEGMENT *pseglist,GC gc)
#else
void Xw_draw_pixel_segments(pwindow,pseglist,gc)
XW_EXT_WINDOW *pwindow;
XW_EXT_SEGMENT *pseglist;
GC gc ;
#endif /*XW_PROTOTYPE*/
{

	if( pseglist->isupdated ) {
	  XDrawSegments(_DISPLAY,_DRAWABLE,gc,
			pseglist->usegments,pseglist->nseg) ;
	} else {
	  XDrawSegments(_DISPLAY,_DRAWABLE,gc,
			pseglist->rsegments,pseglist->nseg) ;
	}

#ifdef  TRACE_DRAW_SEGMENT
if( Xw_get_trace() > 2 ) {
    printf(" %d = Xw_draw_pixel_segments(%lx,%lx,%lx\n",pseglist->nseg,(long ) pwindow,(long ) pseglist,(long ) gc) ;
}
#endif
}

/*
   STATUS Xw_begin_segments (awindow,nsegment):
   XW_EXT_WINDOW *awindow
   int nsegment           Not used


        Begin a set of segments which must be filled by Xw_draw_segment and
                                     closed by Xw_close_segments 

        returns ERROR if bad extended window address 
        returns SUCCESS if successful

*/
 
#ifdef XW_PROTOTYPE
XW_STATUS Xw_begin_segments(void* awindow,int nsegment)
#else
XW_STATUS Xw_begin_segments(awindow,nsegment)
void *awindow ;
int nsegment ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
 
        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_begin_segments",pwindow) ;
            return (XW_ERROR) ;
        }    

        if( BeginSegments ) Xw_close_segments(pwindow);
	
	BeginSegments = True;
 
#ifdef  TRACE_DRAW_LINE
if( Xw_get_trace() > 2 ) {
    printf(" Xw_begin_segments(%x,%d)\n",pwindow,nsegment) ;
}
#endif
        return (XW_SUCCESS) ;
}

/*
   STATUS Xw_close_segments (awindow):
   XW_EXT_WINDOW *awindow

        Close the set of segments 

        returns ERROR if bad extended window address 
        returns SUCCESS successful

*/
 
#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_segments(void* awindow)
#else
XW_STATUS Xw_close_segments(awindow)
void *awindow ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
XW_EXT_BUFFER *pbuffer ;
int bindex ;
 
	bindex = _BINDEX ; 
	pbuffer = &_BUFFER(bindex) ;
        if( BeginSegments && !bindex ) {
	  int index = pwindow->lineindex ;
	  for( pseglist = pbuffer->pseglist ; pseglist ;
				pseglist = (XW_EXT_SEGMENT*)pseglist->link ) {
	    if( pseglist->nseg ) {
              Xw_draw_pixel_segments(pwindow,pseglist,
						pwindow->qgline[index].gc) ;
	      pseglist->nseg = 0 ;
	    } else break ;
	  }
        }
 
	BeginSegments = False; 

#ifdef  TRACE_DRAW_SEGMENT
if( Xw_get_trace() > 2 ) {
    printf(" Xw_close_segments(%lx)\n",(long ) pwindow) ;
}
#endif
        return (XW_SUCCESS) ;
}
 
#ifdef XW_PROTOTYPE
XW_EXT_SEGMENT* Xw_add_segment_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_SEGMENT* Xw_add_segment_structure(pbuflist )
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert at end one Extended segment structure in the
        segment List

        returns Extended segment address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_SEGMENT *pseg ;

	pseg = (XW_EXT_SEGMENT*) Xw_malloc(sizeof(XW_EXT_SEGMENT)) ;
        if( pseg ) {
            pseg->link = pbuflist->pseglist ;
            pseg->isupdated = False ;
            pseg->nseg = 0 ;
	    pbuflist->pseglist = pseg ;
        } else {
	    /*ERROR*EXT_SEGMENT Allocation failed*/
	    Xw_set_error(35,"Xw_add_segment_structure",NULL) ;
        }

        return (pseg) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_segment_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_segment_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended segment structure in the
        segment List

        SUCCESS always
*/
{
XW_EXT_SEGMENT *pseg,*qseg ;

      	for( pseg = pbuflist->pseglist ; pseg ; pseg = qseg ) {
            qseg = (XW_EXT_SEGMENT*)pseg->link ;
            Xw_free(pseg) ;
        }
	pbuflist->pseglist = NULL ;
	
	return (XW_SUCCESS) ;
}

#define MINXCOORD -32768
#define MAXXCOORD 32767 
#define MINYCOORD -32768
#define MAXYCOORD 32767 

#ifdef XW_PROTOTYPE
int Xw_clip_segment (XW_EXT_WINDOW* pwindow,int x1,int y1,int x2,int y2,XSegment* pseg)
#else
int Xw_clip_segment (pwindow,x1,y1,x2,y2,pseg)
XW_EXT_WINDOW *pwindow;
int x1,y1,x2,y2;
XSegment *pseg;	
#endif /*XW_PROTOTYPE*/
/*
	Clip a segment when one coord is < MINCOORD or > MAXCOORD.
	Returns the clipped segment and a clip bit-mask status :

	  0   Nothing is clipped
	  1   X first point is clipped on MAX
	  2   X first point is clipped on MIN
	  4   Y first point is clipped on MAX
	  8   Y first point is clipped on MIN
	  16  X second point is clipped on MAX
	  32  X second point is clipped on MIN
	  64  Y second point is clipped on MAX
	  128 Y second point is clipped on MIN
	  -1 segment is out of space.
*/
{
int xx1 = x1,yy1 = y1,xx2 = x2,yy2 = y2;
int status = 0;

	if( xx1 > MAXXCOORD ) {
	  if( xx2 < MAXXCOORD ) {
	    float rap = (float)(yy2 - yy1)/(xx2 - xx1);
	    yy1 += (int)((MAXXCOORD - xx1) * rap); 
	    xx1 = MAXXCOORD;
	    status |= 1;
	  } else return -1;
	} else if( xx1 < MINXCOORD ) { 
	  if( xx2 > MINXCOORD ) {
	    float rap = (float)(yy2 - yy1)/(xx2 - xx1);
	    yy1 += (int)((MINXCOORD - xx1) * rap); 
	    xx1 = MINXCOORD;
	    status |= 2;
	  } else return -1;
	} 

        if( yy1 > MAXYCOORD ) {
	  if( yy2 < MAXYCOORD ) {
	    float rap = (float)(xx2 - xx1)/(yy2 - yy1);
	    xx1 += (int)((MAXYCOORD - yy1) * rap); 
	    yy1 = MAXYCOORD;
	    status |= 4;
	  } else return -1;
	} else if( yy1 < MINYCOORD ) { 
	  if( yy2 > MINYCOORD ) {
	    float rap = (float)(xx2 - xx1)/(yy2 - yy1);
	    xx1 += (int)((MINYCOORD - yy1) * rap); 
	    yy1 = MINYCOORD;
	    status |= 8;
	  } else return -1;
	}

	if( xx2 > MAXXCOORD ) {
	  float rap = (float)(yy2 - yy1)/(xx2 - xx1);
	  yy2 = yy1 + (int)((MAXXCOORD - xx1) * rap); 
	  xx2 = MAXXCOORD;
	  status |= 16;
	} else if( xx2 < MINXCOORD ) { 
	  float rap = (float)(yy2 - yy1)/(xx2 - xx1);
	  yy2 = yy1 + (int)((MINXCOORD - xx1) * rap); 
	  xx2 = MINXCOORD;
	  status |= 32;
	} 

        if( yy2 > MAXYCOORD ) {
	  float rap = (float)(xx2 - xx1)/(yy2 - yy1);
	  xx2 = xx1 + (int)((MAXYCOORD - yy1) * rap); 
	  yy2 = MAXYCOORD;
	  status |= 64;
	} else if( yy2 < MINYCOORD ) { 
	  float rap = (float)(xx2 - xx1)/(yy2 - yy1);
	  xx2 = xx1 + (int)((MINYCOORD - yy1) * rap); 
	  yy2 = MINYCOORD;
	  status |= 128;
	}

	pseg->x1 = xx1;
	pseg->y1 = yy1;
	pseg->x2 = xx2;
	pseg->y2 = yy2;

  	return status;
}
