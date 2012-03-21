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
#define TRACE_GET_WINDOW_PIXELVALUE
#endif

/*
   float Xw_get_window_pixelvalue (awindow,pv):
   XW_EXT_WINDOW *awindow
   int pv	Pixel Values 

	Returns DWU value from Pixel value

*/

#ifdef XW_PROTOTYPE
float Xw_get_window_pixelvalue(void *awindow,int pv)
#else
float Xw_get_window_pixelvalue(awindow,pv)
void *awindow;
int pv ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
float uv ;

        if( !Xw_isdefine_window(pwindow) ) {
            /*ERROR*Bad EXT_WINDOW Address*/
            Xw_set_error(24,"Xw_get_window_pixelvalue",pwindow) ;
            return (XW_ERROR) ;
        }

	uv = UVALUE(pv) ;
	
#ifdef  TRACE_GET_WINDOW_PIXELVALUE
if( Xw_get_trace() > 1 ) {
    printf (" %f = Xw_get_window_pixelvalue(%lx,%d)\n",uv,(long ) pwindow,pv) ;
}
#endif

	return (uv);
}
