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
#define TRACE_ALLOC_COLOR
#endif

/*
   XW_STATUS Xw_alloc_color(pcolormap,r,g,b,pixel,isapproximate):
   XW_EXT_COLORMAP *pcolormap
   float r,g,b ;	Red,Green,Blue color value 0. >= x <= 1.
   unsigned long pixel ;Returned Color pixel value
   bool isapproximate

	Get the color pixel value from an {r,g,b} color definition.
	Returned the existing color pixel or create an other if it don't exist.

	Returns ERROR if Bad Color pixel
	Returns SUCCESS if Successful      

*/

#define OCC38      /* SAV 30/11/01 correcred: gamma correction formula */

static double theGammaCorrection = 1.0;
static Colormap theColormap;
static XColor theColors[MAXCOLOR];
static unsigned char theFilters[MAXCOLOR];

#ifdef XW_PROTOTYPE
XW_STATUS Xw_alloc_color (XW_EXT_COLORMAP* pcolormap,
			float r,float g,float b,unsigned long *pixel,int *isapproximate)
#else
XW_STATUS Xw_alloc_color (pcolormap,r,g,b,pixel,isapproximate)
XW_EXT_COLORMAP *pcolormap;
float r,g,b ;
unsigned long *pixel;
int *isapproximate;
#endif /*XW_PROTOTYPE*/
{
int status = False;
int drmin = 65536;
int dgmin = 65536;
int dbmin = 65536;
XColor color;
unsigned char filter='\0';
char svalue[6];

        if( !Xw_isdefine_colormap(pcolormap) ) {
          /*ERROR*Bad EXT_COLORMAP Address*/
          Xw_set_error(42,"Xw_alloc_color",pcolormap) ;
          return (XW_ERROR) ;
        }

	*isapproximate = False;
        if( _CCLASS == TrueColor ) {
	  if( theColormap != _CINFO.colormap ) {
            theColormap = _CINFO.colormap;
            if( Xw_get_env("Xw_SET_GAMMA_CORRECTION",svalue,sizeof(svalue)) ) {
              if( strlen(svalue) > 0 ) {
                float gamma; 
                sscanf(svalue,"%f",&gamma);
#ifdef OCC38
                if( gamma >  0. )
		  theGammaCorrection = 1. / gamma;
#else
                if( gamma >  0. ) theGammaCorrection = gamma;
#endif
              }
              printf(" Xw_SET_GAMMA_CORRECTION is %f\n",theGammaCorrection) ;
            }
	  }
	  color.pixel = 0;
	  if( theGammaCorrection != 1.0 ) {
            color.red = (unsigned short) (pow((double)r,theGammaCorrection)*65535.);
            color.green = (unsigned short) (pow((double)g,theGammaCorrection)*65535.);
            color.blue = (unsigned short) (pow((double)b,theGammaCorrection)*65535.);
	  } else {
	    color.red = (unsigned short) (r*65535.);
	    color.green = (unsigned short) (g*65535.);
	    color.blue = (unsigned short) (b*65535.);
	  }
	  status = XAllocColor(_CDISPLAY,_CINFO.colormap,&color) ;
	  if( !status ) {
            unsigned long mask = _CVISUAL->map_entries-1 ;
            unsigned long red   = (unsigned long) (r * mask) ;
            unsigned long green = (unsigned long) (g * mask) ;
            unsigned long blue  = (unsigned long) (b * mask) ;

            mask = _CVISUAL->red_mask;
            while ( !(mask & 0x01) ) { mask >>= 1; red <<= 1; }
            mask = _CVISUAL->green_mask;
            while ( !(mask & 0x01) ) { mask >>= 1; green <<= 1; }
            mask = _CVISUAL->blue_mask;
            while ( !(mask & 0x01) ) { mask >>= 1; blue <<= 1; }
            color.pixel = red|green|blue ;
	  }
	} else { 
	  color.pixel = 0;
	  color.red = (unsigned short) (r*65535.);
	  color.green = (unsigned short) (g*65535.);
	  color.blue = (unsigned short) (b*65535.);
	  status = XAllocColor(_CDISPLAY,_CINFO.colormap,&color) ;
	  if( !status ) {
	    int i,j,ncolor = min(MAXCOLOR,_CVISUAL->map_entries);
	    int dr,dg,db;

	    if( theColormap != _CINFO.colormap ) {
	      theColormap = _CINFO.colormap;
	      for( i=0 ; i<ncolor ; i++ ) {
		theColors[i].pixel = i;
	        theFilters[i] = 0;
	      }
	      XQueryColors(_CDISPLAY,_CINFO.colormap,theColors,ncolor);
	      for( i=0 ; i<ncolor ; i++ ) {
		filter = 0;
	        if( theColors[i].red > theColors[i].blue ) filter |= 1;
	        else if( theColors[i].blue > theColors[i].red ) filter |= 4;
	        if( theColors[i].red > theColors[i].green ) filter |= 2;
	        else if( theColors[i].green > theColors[i].red ) filter |= 4;
	        if( theColors[i].blue > theColors[i].green ) filter |= 2;
	        else if( theColors[i].green > theColors[i].blue ) filter |= 1;
		theFilters[i] = filter;
	      }
	    }

	    filter = 0;
	    if( color.red > color.blue ) filter |= 1;
	    else if( color.blue > color.red ) filter |= 4;
	    if( color.red > color.green ) filter |= 2;
	    else if( color.green > color.red ) filter |= 4;
	    if( color.blue > color.green ) filter |= 2;
	    else if( color.green > color.blue ) filter |= 1;

	    for( i=j=0 ; i<ncolor ; i++ ) {
	      if( filter == theFilters[i] ) {
	        if( filter ) {			/* This is a color */
		  dr = abs( color.red - theColors[i].red ) >> 8;
		  dg = abs( color.green - theColors[i].green ) >> 8;
		  db = abs( color.blue - theColors[i].blue ) >> 8;
                  if( (dr <= drmin) && (dg <= dgmin) && (db <= dbmin) ) {
                    j = i; drmin = dr; dgmin = dg; dbmin = db;
                  }  
		} else {			/* This is a gray */
		  dr = abs( color.red - theColors[i].red ) >> 8;
                  if( dr <= drmin ) {
                    j = i; drmin = dr;
                  }  
		}
	      }
	    }

	    if( filter ) {
	      if( (drmin > 0) || (dgmin > 0) || (dbmin > 0) ) *isapproximate = True;
	    } else {
	      if( drmin > 0 ) *isapproximate = True;
	    }

	    color.pixel = theColors[j].pixel;
	  }
	}
	*pixel = color.pixel;
	status = XW_SUCCESS;

#ifdef TRACE_ALLOC_COLOR
if( Xw_get_trace() ) {
    printf(" %d = Xw_alloc_color(%lx,%f,%f,%f,%ld,%d)\n",
           status,(long ) pcolormap,r,g,b,*pixel,*isapproximate) ;
    if( *isapproximate ) {
      if( !filter ) {
        printf("      Is an approximate color of delta-GRAY (%f)\n",(float)drmin/65535.);
      } else {
        printf("      Is an approximate color of delta-COLOR (%f,%f,%f)\n",
							(float)drmin/65535.,
							(float)dgmin/65535.,
							(float)dbmin/65535.);
      }
    }
}
#endif
    return (XW_STATUS)status;
}
