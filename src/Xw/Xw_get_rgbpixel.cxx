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
#define TRACE_GET_RGBPIXEL
#endif

/*
   STATUS Xw_get_rgbpixel (aimage,x,y,r,g,b,npixel):
   XW_EXT_IMAGEDATA 		*aimage
   int x,y			Pixel position
   float *r,*g,*b		return R,G,B Pixel values      
   int *npixel			return Pixel number filled with 
				       the same color index

	Extract RGB pixels from an existing IMAGE created by Xw_get_image

	returns ERROR if Extended Image is not defined
		      or VISUAL Image type is not TRUECOLOR
		      or Pixel position is wrong (Outside of image)
	returns SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_rgbpixel (void* aimage,
		int x,int y,float* r,float* g,float* b,int* npixel)
#else
XW_STATUS Xw_get_rgbpixel (aimage,x,y,r,g,b,npixel)
void *aimage ;
int x,y ;
float *r,*g,*b ;
int *npixel ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*) aimage ;
XW_EXT_COLORMAP *pcolormap = pimage->pcolormap ;
union {
    char  *data ;
    unsigned char *cdata ;
    unsigned short *sdata ;
    unsigned long  *ldata ;
} data ;
int i=0,fpixel,simage ;
unsigned long pixel=0 ;
XColor color ;
XImage *pximage;

        if( !Xw_isdefine_image(pimage) ) {
	    /*ERROR*Bad EXT_IMAGEDATA Address*/
	    Xw_set_error(25,"Xw_get_rgbpixel",pimage) ;
            return (XW_ERROR) ;
        }

 	pximage = (_ZIMAGE) ? _ZIMAGE : _IIMAGE ;
	fpixel = x*pximage->width + y ;
	simage = pximage->height*pximage->width ;

	if( x < 0 || y < 0 || (fpixel >= simage) ) {
	    /*ERROR*Bad PIXEL position*/
	    Xw_set_error(47,"Xw_get_rgbpixel",&simage) ;
	    return (XW_ERROR) ;
	}

	switch (pximage->bitmap_pad) {
	    case 8 :
		data.data = pximage->data + (fpixel) ; simage -= fpixel ;
		pixel = (unsigned long) *data.cdata ;
		for( i=1 ; i<simage ; i++ ) {
		    data.cdata++ ; 
		    if( *data.cdata != (unsigned char) pixel ) break ;
		}
		break ;
	    case 16 :
		data.data = pximage->data + (fpixel<<1) ; simage -= fpixel ;
		pixel = (unsigned long) *data.sdata ;
		for( i=1 ; i<simage ; i++ ) {
		    data.sdata++ ; 
		    if( *data.sdata != (unsigned short) pixel ) break ;
		}
		break ;
	    case 32 :
		data.data = pximage->data + (fpixel<<2) ; simage -= fpixel ;
		pixel = (unsigned long) *data.ldata ;
		for( i=1 ; i<simage ; i++ ) {
		    data.ldata++ ; 
		    if( *data.ldata != (unsigned long) pixel ) break ;
		}
	}

	*npixel = i ;
	switch ( _CCLASS ) {
	    case TrueColor :
                { unsigned long mask = _CVISUAL->map_entries-1 ;
                unsigned long red,green,blue ;
                unsigned long rmask = _CVISUAL->red_mask ;
                unsigned long gmask = _CVISUAL->green_mask ;
                unsigned long bmask = _CVISUAL->blue_mask ;
 
                red = pixel & rmask ;
                while ( !(rmask & 0x01) ) { rmask >>= 1; red >>= 1; }

                green = pixel & gmask ;
                while ( !(gmask & 0x01) ) { gmask >>= 1; green >>= 1; }

                blue = pixel & bmask ;
                while ( !(bmask & 0x01) ) { bmask >>= 1; blue >>= 1; }

                *r = (float)red/mask ;
                *g = (float)green/mask ;
                *b = (float)blue/mask ;
    	        } 
		break ;

	    case PseudoColor :
		color.pixel = pixel ;
		XQueryColor(_CDISPLAY,_CINFO.colormap,&color) ;
		*r = (float) color.red/0xFFFF ;
		*g = (float) color.green/0xFFFF ;
		*b = (float) color.blue/0xFFFF ;
		break ;
	    default :
	    /*ERROR*Unimplemented Visual Class*/
	    	Xw_set_error(5,"Xw_get_rgbpixel",&_CCLASS) ;
	        return (XW_ERROR) ;
	}

#ifdef  TRACE_GET_RGBPIXEL
	printf (" Xw_get_rgbpixel(%lx,%d,%d,%f,%f,%f,%d)\n",
					(long ) pimage,x,y,*r,*g,*b,*npixel) ;
#endif

	return (XW_SUCCESS);
}
