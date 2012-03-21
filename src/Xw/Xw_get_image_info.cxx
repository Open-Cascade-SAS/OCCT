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
#define TRACE_GET_IMAGE_INFO
#endif

/*
   XW_STATUS Xw_get_image_info (aimage,zoom,width,height,depth):
   XW_EXT_IMAGEDATA *aimage     Image
   float *zoom                  returned Zoom factor 
   int *width,*height		returned Image pixel size
   int *depth			returned Image planes depth.

	Get image size and zoom factor.

        returns  ERROR if bad image extended address 
        returns  SUCCESS if successfull

*

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_image_info (void* aimage,float *zoom,int* width,int* height,int *depth)
#else
XW_STATUS Xw_get_image_info (aimage,zoom,width,height,depth)
void *aimage;
float *zoom;
int *width;
int *height;
int *depth;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*)aimage;

    if( !Xw_isdefine_image(pimage) ) {
        /*ERROR*Bad EXT_IMAGE Address*/
        Xw_set_error(25,"Xw_get_image_info",pimage) ;
        return (XW_ERROR) ;
    }

    *zoom = pimage->zoom;
    *width = _IIMAGE->width;
    *height = _IIMAGE->height;
    *depth = _IIMAGE->depth;

#ifdef  TRACE_GET_IMAGE_INFO
if( Xw_get_trace() ) {
    printf (" Xw_get_image_info('%lx',%f,%d,%d,%d)\n",(long ) aimage,
				*zoom,*width,*height,*depth);
}
#endif

    return (XW_SUCCESS);
}
