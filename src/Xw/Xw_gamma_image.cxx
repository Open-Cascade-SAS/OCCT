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

// JR 01.02.100 : convert of float to int : (int )

#include <Xw_Extension.h>

/* ifdef then trace on */
#ifdef TRACE
# define TRACE_GAMMA_IMAGE
#endif

/*
   XW_STATUS Xw_gamma_image (aimage,gamma):
   XW_EXT_IMAGEDATA *aimage     Image where apply the gamma correction
   float gamma                   gamma value to apply (>0.)

        Apply a GAMMA correction to an image

        returns  ERROR if bad gamma value
        returns  SUCCESS if successfull

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_gamma_image (void* aimage,float gamma)
#else
XW_STATUS Xw_gamma_image (aimage,gamma)
void *aimage;
float gamma ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*)aimage ;
XImage *pximage;
int wp,hp,xp,yp,ired,igreen,iblue;
unsigned long cmask,npixel,gpixel = 0,opixel = 0;
int sred,sgreen,sblue ;
float red,green,blue ;

    if( !Xw_isdefine_image(pimage) ) {
        /*ERROR*Bad EXT_IMAGE Address*/
        Xw_set_error(25,"Xw_gamma_image",pimage) ;
        return (XW_ERROR) ;
    }

    if( gamma <= 0. ) {
        /*ERROR*Bad Image GAMMA value*/
        return (XW_ERROR) ;
    }

    if( gamma == 1.0 ) return XW_SUCCESS;

    pximage = pimage->pximage;

    if( !pximage->red_mask || !pximage->green_mask || !pximage->blue_mask ) {
        printf(" *** Xw_gamma_image.Cann't apply the gamma correction to this image\n");
	return (XW_ERROR) ;
    }

    gamma = 1./gamma;

    wp = pximage->width; hp = pximage->height;

    sred = sgreen = sblue = 0 ;
    cmask = pximage->red_mask ;
    while ( !(cmask & 1) ) { cmask >>= 1 ; sred++ ; }
    cmask = pximage->green_mask ;
    while ( !(cmask & 1) ) { cmask >>= 1 ; sgreen++ ; }
    cmask = pximage->blue_mask ;
    while ( !(cmask & 1) ) { cmask >>= 1 ; sblue++ ; }

    for( yp=0 ; yp<hp ; yp++ ) {
      for( xp=0 ; xp<wp ; xp++ ) {
        npixel = XGetPixel(pximage,xp,yp) ;
	if( npixel != opixel ) {
	  opixel = npixel;
	  ired = (npixel >> sred) & cmask;
	  igreen = (npixel >> sgreen) & cmask;
	  iblue = (npixel >> sblue) & cmask;

	  red = (float)(ired)/(float)cmask;
	  green = (float)(igreen)/(float)cmask;
	  blue = (float)(iblue)/(float)cmask;

	  red = min(1.,pow(double(red),double(gamma)));
	  green = min(1.,pow(double(green),double(gamma)));
	  blue = min(1.,pow(double(blue),double(gamma)));

	  ired = (int )( red * cmask);
	  igreen = (int )( green * cmask);
	  iblue = (int )( blue * cmask);

	  gpixel = (ired << sred) | (igreen << sgreen) | (iblue << sblue);
/*
printf(" npixel %ld gpixel %ld cmask %ld sred %d sgreen %d sblue %d\n",
npixel,gpixel,cmask,sred,sgreen,sblue);
*/
	}
        XPutPixel(pximage,xp,yp,gpixel) ;
      }
    }

#ifdef  TRACE_GAMMA_IMAGE
if( Xw_get_trace() ) {
    printf (" Xw_gamma_image(%lx,%f)\n",(long ) pimage,gamma);
}
#endif

    return (XW_SUCCESS);
}
