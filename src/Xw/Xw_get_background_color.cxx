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
#define TRACE_GET_BACKGROUND_COLOR
#endif

/*
   STATUS Xw_get_background_color(awindow,r,g,b):
   XW_EXT_WINDOW *awindow	Window extension structure
   float *r,*g,*b ;	Return Red,Green,Blue color value 0. >= x <= 1.

	Get R,G,B Background Color values from Default index .

	Returns ERROR if Extended Window Address is badly defined
	Returns SUCCESS if Successful      

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_background_color (void* awindow,float* r,float* g,float* b)
#else
XW_STATUS Xw_get_background_color (awindow,r,g,b)
void *awindow;
float *r,*g,*b ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
unsigned long pixel;

	if ( !Xw_isdefine_window(pwindow) ) {
	    /*ERROR*Bad EXTENDED Window ADDRESS*/
	    Xw_set_error(24,"Xw_get_background_color",pwindow) ;
	    return (XW_ERROR) ;
	}

	if( pwindow->backindex >= 0 ) {
	  switch ( _CLASS ) {
	    case PseudoColor :
	    case StaticColor :
	    case TrueColor :
	      Xw_get_color (_COLORMAP,pwindow->backindex,r,g,b,&pixel) ;
	      break ;

            default :
              /*Unmatchable Visual class*/
              Xw_set_error(67,"Xw_get_background_color",&_CLASS) ;
              return (XW_ERROR) ;
	  }
	} else {
	  *r = *g = *b = 1. ;
	}


#ifdef TRACE_GET_BACKGROUND_COLOR
if( Xw_get_trace() ) {
    printf(" Xw_get_background_color(%lx,%f,%f,%f)\n",(long ) pwindow,*r,*g,*b) ;
}
#endif

	return (XW_SUCCESS);
}
