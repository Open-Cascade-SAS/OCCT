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


#define GG012		/*GG_150796
//			Installer correctement la colormap meme sur une fenetre
//			dont le visual est TrueColor.
*/

#define GG013		/*GG_151096
//			Ne pas chercher a installer la colormap demande
//			lorsque les visualid sont differents.
*/

#define PRO14951        /*GG_200798
//              Ne pas modifier la couleur du background de la fenetre
*/


#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SET_COLORMAP
#endif

/*
   XW_STATUS Xw_set_colormap(awindow,acolormap):
   XW_EXT_WINDOW *awindow
   XW_EXT_COLORMAP *acolormap

	Set a colormap to an existing window 

	Returns XW_ERROR if Bad Visual matching between Window and Colormap
	Returns XW_SUCCESS if successful

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_set_colormap (void *awindow,void *acolormap)
#else
XW_STATUS Xw_set_colormap (awindow,acolormap)
void *awindow;
void *acolormap ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;

    if( !Xw_isdefine_window(pwindow) ) {
	/*ERROR*Bad EXT_WINDOW Address*/
	Xw_set_error(24,"Xw_set_colormap",pwindow) ;
	return (XW_ERROR) ;
    }

    if( !Xw_isdefine_colormap(pcolormap) ) {
	/*ERROR*Bad EXT_COLORMAP Address*/
	Xw_set_error(42,"Xw_set_colormap",pcolormap) ;
	return (XW_ERROR) ;
    }

    if( _CLASS != _CCLASS ) {
	/*ERROR*Unmatchable Visual class*/
	Xw_set_error(67,"Xw_set_colormap",&_CCLASS) ;
	return (XW_ERROR) ;
    }

    _COLORMAP = pcolormap ;
    _COLORMAP->maxwindow++ ;

    if( _COLORMAP->info.colormap ) {
#ifdef GG012
      XW_EXT_DISPLAY *pdisplay = pwindow->connexion;
      Window *ptree,root,parent,shell = _WINDOW;
      unsigned int tcount ;
      Colormap colormap = _COLORMAP->info.colormap;

#ifdef GG013
      if( _COLORMAP->info.visualid != _VISUAL->visualid ) {
	colormap = _ATTRIBUTES.colormap;
      }
#endif
      XSetWindowColormap(_DISPLAY,_WINDOW,colormap) ;
      if( _VISUAL->visualid != _DVISUAL->visualid ) {
	XInstallColormap(_DISPLAY,colormap);
        while( shell ) {	/* Find the top level shell window */
          if( XQueryTree(_DISPLAY,shell,&root,&parent,&ptree,&tcount)  ) {
	    if( tcount ) XFree((char*)ptree);
            if( root == parent ) break;
            shell = parent;
	  } else break;
        }
	if( shell ) {
	  Window *pgetwindows,*psetwindows;
	  int wcount;
	  if( XGetWMColormapWindows(_DISPLAY,shell,&pgetwindows,&wcount) ) {
            if(( psetwindows = (Window*) Xw_calloc(wcount+1,sizeof(Window)) )) {
	      int i,j;
              for( i=j=0; i<wcount ; i++ ) {
                if( pgetwindows[i] != _WINDOW ) {
                  psetwindows[j++] = pgetwindows[i];
                }
              }
              psetwindows[j++] = _WINDOW;
	      XSetWMColormapWindows(_DISPLAY,shell,psetwindows,j);
	      XFree((char*)psetwindows);
	    }
            XFree((char*)pgetwindows);
	  } else {
	    XSetWMColormapWindows(_DISPLAY,shell,&_WINDOW,1) ;
	  }
	}
      }
#else
	if( _CCLASS != TrueColor ) {
	  Window root,parent,*childs;
	  unsigned int nchilds ;

	  XQueryTree(_DISPLAY,_WINDOW,&root,&parent,&childs,&nchilds) ;
          XSetWindowColormap(_DISPLAY,_WINDOW,_COLORMAP->info.colormap) ;
	  XSetWMColormapWindows(_DISPLAY,parent,&_WINDOW,1) ;
	}
#endif

#ifndef PRO14951
        Xw_set_background_index(pwindow,pwindow->backindex);
#endif
    }

    XFlush(_DISPLAY) ;

#ifdef TRACE_SET_COLORMAP
if( Xw_get_trace() ) {
    printf(" Xw_set_colormap(%lx,%lx)\n",(long ) pwindow,(long ) pcolormap) ;
}
#endif

	return (XW_SUCCESS);
}
