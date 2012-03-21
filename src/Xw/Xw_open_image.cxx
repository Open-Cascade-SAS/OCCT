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
#define TRACE_OPEN_IMAGE
#endif

/*
   XW_EXT_IMAGEDATA Xw_open_image (awindow,aimageinfo,width,height):
   XW_EXT_WINDOW *awindow ;
   XW_USERDATA *aimageinfo ;		Image identifier
   int width,height		Image size in pixels

	Open an Image in the same Window visual as defined
	NOTE than this image can be filled with Xw_put_pixel 

	Returns The image extension address if Successful      
	returns  NULL if BadAllocation

   XW_STATUS Xw_close_image (aimage):
   XW_EXT_IMAGEDATA *aimage     Extended Image address

        Close an Image

        returns  SUCCESS always

   XW_EXT_IMAGEDATA* Xw_get_image_handle (awindow,aimageinfo):
   XW_EXT_WINDOW *awindow ;
   XW_USERDATA	aimageinfo     Image identifier

	returns the Image handle associated to the image ID
		or NULL if no image exist.

*/

#ifdef XW_PROTOTYPE
void* Xw_open_image (void *awindow,void *aimageinfo,int width,int height)
#else
void* Xw_open_image (awindow,aimageinfo,width,height)
void *awindow ;
void *aimageinfo ;
int width,height ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_IMAGEDATA *pimage ;
XW_EXT_COLORMAP *pcolormap ;
//int i,pad ;
int pad ;
char *cdata ;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_open_image",pwindow) ;
        return (NULL) ;
    }

    switch (_DEPTH) {
    	case 8 :
	    pad = 1 ;
	    break ;
	case 12 :
	case 16 :
	    pad = 2 ;
	    break ;
	case 24 :
	    pad = 4 ;
	    break ;
	case 48 :
	case 64 :
	    pad = 8 ;
	    break ;
	default :
	    /*ERROR*Unimplemented Image Visual depth*/
	    Xw_set_error(64,"Xw_open_image",&_DEPTH) ;
	    return (NULL) ;
    }

    if( !(cdata = (char*) Xw_calloc(width*height,pad)) ) {
	/*ERROR*XImage Allocation failed*/
	Xw_set_error(60,"Xw_open_image",0) ;
        return (NULL) ;
    }

    if( !(pimage = Xw_add_imagedata_structure(sizeof(XW_EXT_IMAGEDATA))) ) 
								return (NULL) ;

    pimage->pimageinfo = aimageinfo ;
    pimage->pcolormap = pcolormap = _COLORMAP ;
    _IIMAGE = XCreateImage(_CDISPLAY,_CVISUAL,_DEPTH,
			ZPixmap,0,cdata,width,height,pad*8,width*pad) ;
    if( !_IIMAGE ) {
	/*ERROR*XImage Creation failed*/
	Xw_set_error(62,"Xw_open_image",0) ;
	Xw_del_imagedata_structure(pimage) ;
    }

#ifdef  TRACE_OPEN_IMAGE
if( Xw_get_trace() ) {
    printf (" %lx = Xw_open_image(%lx,%lx,%d,%d)\n",(long ) pimage,(long ) pwindow,(long ) aimageinfo,
							width,height);
}
#endif

    return (pimage);
}

static XW_EXT_IMAGEDATA *PimageList =NULL ;

#ifdef XW_PROTOTYPE
XW_EXT_IMAGEDATA* Xw_add_imagedata_structure(int size)
#else
XW_EXT_IMAGEDATA* Xw_add_imagedata_structure(size)
int size ;
#endif /*XW_PROTOTYPE*/
/*
        Create and Insert one Extended image structure in the
        EXtended image List

        returns Extended image address if successful
                or NULL if Bad Allocation
*/
{
XW_EXT_IMAGEDATA *pimage ;
//int i ;

	pimage = (XW_EXT_IMAGEDATA*) Xw_malloc(size) ;

        if( pimage ) {
	    pimage->type = IMAGE_TYPE ;
	    pimage->link = PimageList ;
	    PimageList = pimage ;
	    pimage->pcolormap = NULL ;
	    pimage->zoom = 1.;
            pimage->pximage = NULL ;
            pimage->zximage = NULL ;
	    pimage->maxwindow = 0 ;
        } else {
	    /*ERROR*EXT_IMAGE allocation failed*/
	    Xw_set_error(27,"Xw_open_image",0) ;
        }

        return (pimage) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_close_image (void* aimage)
#else
XW_STATUS Xw_close_image (aimage)
void *aimage;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*) aimage ;

        Xw_del_imagedata_structure(pimage) ;

#ifdef  TRACE_CLOSE_IMAGE
if( Xw_get_trace() ) {
        printf (" Xw_close_image(%x)\n",pimage);
}
#endif

        return (XW_SUCCESS);
}

#ifdef XW_PROTOTYPE
void* Xw_get_image_handle (void* awindow,void *aimageinfo)
#else
void* Xw_get_image_handle (awindow,aimageinfo)
void *awindow ;
void *aimageinfo ;
#endif /*XW_PROTOTYPE*/
{
#ifdef  TRACE_OPEN_IMAGE
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow ;
#endif
XW_EXT_IMAGEDATA *pimage ;

	for( pimage = PimageList ; pimage ; 
			pimage = (XW_EXT_IMAGEDATA*) pimage->link ) {
            if( pimage->pimageinfo == aimageinfo ) break ; 
        }

#ifdef  TRACE_OPEN_IMAGE
if( Xw_get_trace() ) {
    printf (" %lx = Xw_get_image_handle(%lx,%lx)\n",(long ) pimage,(long ) pwindow,(long ) aimageinfo);
}
#endif

	return (pimage) ;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_del_imagedata_structure(XW_EXT_IMAGEDATA *aimage)
#else
XW_STATUS Xw_del_imagedata_structure(aimage)
XW_EXT_IMAGEDATA *aimage;
#endif /*XW_PROTOTYPE*/
/*
        Remove the Extended image address from the Extended List and
        Free the Extended Image

        returns ERROR if the image address is not Found in the list
        returns XW_SUCCESS if successful
*/
{
XW_EXT_IMAGEDATA *pimage = aimage ;
//int i ;

    if( !pimage ) return (XW_ERROR) ;

    if( pimage->maxwindow ) --pimage->maxwindow ;

    if( pimage->maxwindow ) {
	return (XW_ERROR) ;
    } else {
	if( _IIMAGE ) XDestroyImage(_IIMAGE) ;
	if( _ZIMAGE ) XDestroyImage(_ZIMAGE) ;
        if( pimage == PimageList ) {
            PimageList = (XW_EXT_IMAGEDATA*) pimage->link ;
        } else {
            for( pimage = PimageList ; pimage ; 
			pimage = (XW_EXT_IMAGEDATA*) pimage->link ) {
                if( pimage->link == aimage ) {
                    pimage->link = aimage->link ;
                    break ;
                }
            }
        }
        Xw_free(aimage) ;
    }
    return (XW_SUCCESS) ;
}
