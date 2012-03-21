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
/*
#define TRACE_PUT_PIXEL
*/
#endif

/*
   XW_STATUS Xw_put_pixel (aimage,x,y,index,npixel):
   XW_EXT_IMAGEDATA 		*aimage
   int x,y			Pixel position
   int index			Pixel color index 
   int npixel			Pixel number to fill with the same color index

	Fill an existing IMAGE created by Xw_open_image from Pixel index 

	returns XW_ERROR if Extended Image is not defined
			 or Color is not defined
			 or Pixel position is wrong (Outside of image)
	returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_put_pixel (void *aimage,int x,int y,int index,int npixel)
#else
XW_STATUS Xw_put_pixel (aimage,x,y,index,npixel)
void *aimage ;
int x,y,index,npixel ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*) aimage ;
register union {
    char  *data ;
    unsigned char *cdata ;
    unsigned short *sdata ;
    unsigned long  *ldata ;
} data ;
unsigned long lpixel ;
register int np ;
int fpixel,epixel,simage ;
XImage *pximage;

	if( !Xw_isdefine_image(pimage) ) {
	    /*ERROR*Bad EXT_IMAGEDATA Address*/
	    Xw_set_error(25,"Xw_put_pixel",pimage) ;
            return (XW_ERROR) ;
        }

	if( !Xw_isdefine_color(_ICOLORMAP,index) ) {
	    /*ERROR*Bad Defined Color at index*/
	    Xw_set_error(41,"Xw_put_pixel",&index) ;
	    return (XW_ERROR) ;
	}

	pximage = (_ZIMAGE) ? _ZIMAGE : _IIMAGE;
	simage =  pximage->height*pximage->width ;
	fpixel = x*pximage->width + y ;
	epixel = fpixel + npixel - 1 ;

	if( x < 0 || y < 0 || (epixel >= simage) ) {
	    /*WARNING*Bad PIXEL position*/
	    Xw_set_error(47,"Xw_put_pixel",&simage) ;
	    return (XW_ERROR) ;
	}

	lpixel = _ICOLORMAP->pixels[index] ;
	switch (pximage->bitmap_pad) {
	    case 8 :
		{ register unsigned char cpixel = (unsigned char) lpixel ;
		data.data = pximage->data + (fpixel) ;
		for( np=npixel ; np ; --np ) *data.cdata++ = cpixel ;
		}
		break ;
	    case 16 :
		{ register unsigned short spixel = (unsigned short) lpixel ;
		data.data = pximage->data + (fpixel<<1) ;
		for( np=npixel ; np ; --np ) *data.sdata++ = spixel ;
		}
		break ;
	    case 32 :
		data.data = pximage->data + (fpixel<<2) ;
		for( np=npixel ; np ; --np ) *data.ldata++ = lpixel ;
	}

#ifdef  TRACE_PUT_PIXEL
	printf (" Xw_put_pixel(%x,%d,%d,%d,%d)\n",
					pimage,x,y,index,npixel) ;
#endif

	return (XW_SUCCESS);
}
