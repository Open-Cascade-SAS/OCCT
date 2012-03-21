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
#define TRACE_GET_COLORMAP_XID
#endif

/*
   Aspect_Handle Xw_get_colormap_xid (acolormap):
   XW_EXT_COLORMAP *acolormap

	Returns the colormap  XId 

*/

#ifdef XW_PROTOTYPE
Aspect_Handle Xw_get_colormap_xid (void* acolormap)
#else
Aspect_Handle Xw_get_colormap_xid (acolormap)
void *acolormap;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap;

        if( !Xw_isdefine_colormap(pcolormap) ) {
            /*ERROR*Bad EXT_COLORMAP Address*/
            Xw_set_error(42,"Xw_get_colormap_xid",pcolormap) ;
            return (0) ;
        }

#ifdef  TRACE_GET_COLORMAP_XID
if( Xw_get_trace() > 1 ) {
    printf (" %lx = Xw_get_colormap_xid(%lx)\n",(long ) _CINFO.colormap,(long ) pcolormap) ;
}
#endif

	return (_CINFO.colormap) ;
}

#ifdef TRACE
#define TRACE_GET_COLORMAP_COLORCUBE
#endif

/*
   XW_STATUS Xw_get_colormap_colorcube (acolormap,
		colormap,visualid,base_pixel,red_max,red_mult,
			green_max,green_mult,blue_max,blue_mult):
   XW_EXT_COLORMAP *acolormap
   Aspect_Handle *colormap
   int *visualid
   int *base_pixel,*red_max,*red_mult,*green_max,*green_mult,*blue_max,*blue_mult

	Returns  TRUE if the colormap provide a color-cube definition 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_colormap_colorcube (void* acolormap,
		Aspect_Handle *colormap,int *visualid,int *base_pixel,
			int *red_max,int *red_mult,int *green_max,int *green_mult,
				int *blue_max,int *blue_mult)
#else
XW_STATUS Xw_get_colormap_colorcube (acolormap,
		colormap,visualid,base_pixel,red_max,red_mult,green_max,green_mult,
				blue_max,blue_mult)
void *acolormap;
Aspect_Handle *colormap;
int *visualid;
int *base_pixel,*red_max,*red_mult,*green_max,*green_mult,*blue_max,*blue_mult;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap;
XW_STATUS status = XW_SUCCESS;

        if( !Xw_isdefine_colormap(pcolormap) ) {
            /*ERROR*Bad EXT_COLORMAP Address*/
            Xw_set_error(42,"Xw_get_colormap_colorcube",pcolormap) ;
            return (XW_ERROR) ;
        }

        *colormap = _CINFO.colormap;
        *visualid = _CINFO.visualid;
        *base_pixel = _CINFO.base_pixel;
	*red_max = _CINFO.red_max;
	*red_mult = _CINFO.red_mult;
	*green_max = _CINFO.green_max;
	*green_mult = _CINFO.green_mult;
	*blue_max = _CINFO.blue_max;
	*blue_mult = _CINFO.blue_mult;

	if( _CINFO.red_mult <= 0 ) {
	  status = XW_ERROR;
	}

#ifdef  TRACE_GET_COLORMAP_COLORCUBE
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_colormap_colorcube(%lx,%lx,%d,%d,%d,%d,%d,%d,%d,%d)\n",
	status,(long ) pcolormap,*colormap,*visualid,*base_pixel,*red_max,*red_mult,
			*green_max,*green_mult,*blue_max,*blue_mult) ;
}
#endif

	return (status) ;
}

#ifdef TRACE
#define TRACE_GET_COLORMAP_GRAYRAMP
#endif

/*
   XW_STATUS Xw_get_colormap_grayramp (acolormap,
		colormap,visualid,base_pixel,gray_max,gray_mult):
   XW_EXT_COLORMAP *acolormap
   Aspect_Handle *colormap
   int *visualid
   int *base_pixel,*gray_max,*gray_mult

	Returns  TRUE if the colormap provide a gray_ramp definition 

*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_colormap_grayramp (void* acolormap,
		Aspect_Handle *colormap,int *visualid,int *base_pixel,
			int *gray_max,int *gray_mult)
#else
XW_STATUS Xw_get_colormap_grayramp (acolormap,
		colormap,visualid,base_pixel,gray_max,gray_mult)
void *acolormap;
Aspect_Handle *colormap;
int *visualid;
int *base_pixel,*gray_max,*gray_mult;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap;
XW_STATUS status = XW_SUCCESS;

        if( !Xw_isdefine_colormap(pcolormap) ) {
            /*ERROR*Bad EXT_COLORMAP Address*/
            Xw_set_error(42,"Xw_get_colormap_grayramp",pcolormap) ;
            return (XW_ERROR) ;
        }

        *colormap = _CGINFO.colormap;
        *visualid = _CGINFO.visualid;
        *base_pixel = _CGINFO.base_pixel;
	*gray_max = _CGINFO.red_max;
	*gray_mult = _CGINFO.red_mult;

	if( _CGINFO.red_mult <= 0 ) {
	  status = XW_ERROR;
	}

#ifdef  TRACE_GET_COLORMAP_GRAYRAMP
if( Xw_get_trace() > 1 ) {
    printf (" %d = Xw_get_colormap_grayramp(%lx,%lx,%d,%d,%d,%d)\n",
	status,(long ) pcolormap,*colormap,*visualid,*base_pixel,*gray_max,*gray_mult);
}
#endif

	return (status) ;
}
