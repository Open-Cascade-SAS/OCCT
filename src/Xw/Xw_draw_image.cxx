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
#define TRACE_DRAW_IMAGE
#endif

/*
   STATUS Xw_draw_image (awindow,aimage,xc,yc):
   XW_EXT_WINDOW *awindow
   XW_EXT_IMAGEDATA *aimage		Image structure
   float xc,yc			Image center location in user space 

	Draw an image to the window at the required Center location 
	NOTE than current Window position and Size is taken

	returns ERROR if NO Image is defined or Image is outside of the window
	returns  SUCCESS if successful

*/

#ifdef BUC0056
#define MAXCOORD 32767
#define MINCOORD -32768
#endif

static XW_EXT_IMAGE *pimaglist ;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_draw_image (void* awindow,void* aimage,float xc,float yc)
#else
XW_STATUS Xw_draw_image (awindow,aimage,xc,yc)
void *awindow;
void *aimage ;
float xc,yc ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*)aimage ;
XW_EXT_BUFFER *pbuffer ;
//int color,nimage,bindex,x,y,width,height ;
int nimage,bindex,x,y,width,height ;

	if( !Xw_isdefine_window(pwindow) ) {
	    /*ERROR*Bad EXT_WINDOW Address*/
	    Xw_set_error(24,"Xw_draw_image",pwindow) ;
	    return (XW_ERROR) ;
	}

	if( !Xw_isdefine_image(pimage) ) {
	    /*ERROR*Bad EXT_IMAGEDATA Address*/
	    Xw_set_error(25,"Xw_draw_image",pimage) ;
	    return (XW_ERROR) ;
	}

	bindex = _BINDEX ;
	pbuffer = &_BUFFER(bindex) ;
        for( pimaglist = pbuffer->pimaglist ; pimaglist ;
                               	pimaglist = (XW_EXT_IMAGE*) pimaglist->link ) {
            if( pimaglist->nimage < MAXIMAGES ) break ;
        }

	if( !pimaglist ) {
            pimaglist = Xw_add_image_structure(pbuffer) ;
	}

//OCC186
	x = PXPOINT(xc, pwindow->xratio) ;
	y = PYPOINT(yc, pwindow->attributes.height, pwindow->yratio) ;
//OCC186

#ifdef BUC0056
        x = max(min(x,MAXCOORD),MINCOORD);
        y = max(min(y,MAXCOORD),MINCOORD);
#endif
	width = (_ZIMAGE) ? _ZIMAGE->width : _IIMAGE->width ;
	height = (_ZIMAGE) ? _ZIMAGE->height : _IIMAGE->height ;
	nimage = pimaglist->nimage ;
	pimaglist->pimagdata[nimage] = pimage ;
	pimaglist->rpoints[nimage].x = x ;
	pimaglist->rpoints[nimage].y = y ;
	pimaglist->nimage++ ;
	if( bindex ) {
	    pbuffer->isempty = False ;
	    width = (width+3)/2 ;
	    height = (height+3)/2 ;
            pbuffer->rxmin = min(pbuffer->rxmin,x-width) ;
            pbuffer->rymin = min(pbuffer->rymin,y-height) ;
            pbuffer->rxmax = max(pbuffer->rxmax,x+width) ;
            pbuffer->rymax = max(pbuffer->rymax,y+height) ;
	} else {
	    Xw_draw_pixel_images(pwindow,pimaglist,pwindow->qgwind.gccopy,NULL);
	    pimaglist->nimage = 0 ;
        }

#ifdef  TRACE_DRAW_IMAGE
if( Xw_get_trace() ) {
    printf (" Xw_draw_image(%lx,%lx,%f,%f)\n",(long ) pwindow,(long ) pimage,xc,yc) ;
}
#endif

	return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
void Xw_draw_pixel_images (XW_EXT_WINDOW* pwindow,XW_EXT_IMAGE* pimaglist,
							GC gcimag,GC gcline)
#else
void Xw_draw_pixel_images (pwindow,pimaglist,gcimag,gcline)
XW_EXT_WINDOW *pwindow;
XW_EXT_IMAGE *pimaglist ;
GC gcimag,gcline ;
#endif
{
XW_EXT_IMAGEDATA *pimage;
unsigned int width,height;
XImage *pximage;
int i,x,y;

	for( i=0 ; i<pimaglist->nimage ; i++ ) {
	    pimage = pimaglist->pimagdata[i] ;
	    pximage = (_ZIMAGE) ? _ZIMAGE : _IIMAGE;
	    width = pximage->width;
	    height = pximage->height;
	    if( pimaglist->isupdated ) {
	      x = pimaglist->upoints[i].x - width/2 ;
	      y = pimaglist->upoints[i].y - height/2 ;
	    } else {
	      x = pimaglist->rpoints[i].x - width/2 ;
	      y = pimaglist->rpoints[i].y - height/2 ;
	    }
	    if( gcimag ) {
	      XPutImage(_DISPLAY,_DRAWABLE,gcimag,pximage,0,0,x,y,width,height) ;
	    }
	    if( gcline ) {
              XDrawRectangle(_DISPLAY,_DRAWABLE,gcline,x-1,y-1,width+2,height+2) ;
	    }
	}
}

#ifdef XW_PROTOTYPE
XW_EXT_IMAGE* Xw_add_image_structure(XW_EXT_BUFFER* pbuflist )
#else
XW_EXT_IMAGE* Xw_add_image_structure(pbuflist )
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert at end one Extended image structure in the
        image List

        returns Extended image address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_IMAGE *pimage ;
int i ;

        pimage = (XW_EXT_IMAGE*) Xw_malloc(sizeof(XW_EXT_IMAGE)) ;
        if( pimage ) {
            pimage->link = pbuflist->pimaglist ;
	    pimage->isupdated = False ;
            pimage->nimage = 0 ;
	    for( i=0 ; i<MAXIMAGES ; i++ ) {
		pimage->pimagdata[i] = NULL ; 
	    }
	    pbuflist->pimaglist = pimage ;
        } else {
	    /*ERROR*EXT_IMAGE Allocation failed*/
	    Xw_set_error(27,"Xw_add_image_structure",NULL) ;
        }

        return (pimage) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_image_structure(XW_EXT_BUFFER* pbuflist)
#else
XW_STATUS Xw_del_image_structure(pbuflist)
XW_EXT_BUFFER *pbuflist ;
#endif /*XW_PROTOTYPE*/
/*
        Remove ALL Extended image structure in the
        image List

        SUCCESS always
*/
{
XW_EXT_IMAGE *pimag,*qimag ;
int i ;

        for( pimag = pbuflist->pimaglist ; pimag ; pimag = qimag ) {
            qimag = (XW_EXT_IMAGE*)pimag->link ;
	    for( i=0 ; i<pimag->nimage ; i++ ) {
		if( pimag->pimagdata[i] ) 
			Xw_del_imagedata_structure(pimag->pimagdata[i]) ;
	    }
            Xw_free(pimag) ;
        }
	pbuflist->pimaglist = NULL ;

        return (XW_SUCCESS) ;
}
