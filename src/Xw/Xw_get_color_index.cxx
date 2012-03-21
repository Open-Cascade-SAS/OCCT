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

#define PRO9517	/*GG_280897
// Ameliorer le critere de comparaison de 2 couleurs
// sinon ROUGE = VERT = BLEU !!!
*/

#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_GET_COLOR_INDEX
#endif

/*
   STATUS Xw_get_color_index(acolormap,r,g,b,index):
   XW_EXT_COLORMAP *acolormap	Colormap extension structure
   float r,g,b ;	Red,Green,Blue color value 0. >= x <= 1.
   int *index ;		Return Color index 0 >= x < MAXCOLOR

	Gets the nearest color index from R,G,B Color values.

	Returns ERROR if BadColor Index or color is not defined
	Returns SUCCESS if Successful

*/

#define XTEST 1 
#ifdef TEST	/* Optim. No more necessary with XQueryColors() */
#define MAX_COLRS 256
static int indice = 0;
static int indice_max = 0;
static XW_EXT_COLORMAP *qcolormap = NULL;
static float tab_ind[MAX_COLRS][4];
#endif

#ifdef XW_PROTOTYPE
XW_STATUS Xw_get_color_index(void* acolormap,float r,float g,float b,int* index)
#else
XW_STATUS Xw_get_color_index(acolormap,r,g,b,index)
void *acolormap ;
float r,g,b ;
int *index ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_COLORMAP *pcolormap = (XW_EXT_COLORMAP*)acolormap ;
XW_STATUS status = XW_ERROR ;
XColor colors[MAXCOLOR] ;
int color_indexs[MAXCOLOR] ;
register int i,j,n ;
int curind = -1,freeind = -1,isapproximate ;
#ifdef PRO9517
int drmin,dgmin,dbmin;
#else
float curdist,newdist ;
float dr,dg,db ;
#endif

    if( !pcolormap ) {
	/*ERROR*Bad EXT_COLORMAP Address*/
	Xw_set_error( 42,"Xw_get_color_index",pcolormap ) ;
	return( XW_ERROR ) ;
    }

    if( _CCLASS != TrueColor ) {
      if( pcolormap->mapping == Xw_TOM_COLORCUBE ) {
        if( (_CGINFO.red_mult > 0) && (fabs(r-g) < 0.01) && (fabs(r-b) < 0.01) ) {
          curind = (int)(0.5+r*_CGINFO.red_max)*_CGINFO.red_mult;
	  if( _CINFO.red_max > 0 ) curind += (_CINFO.red_max+1)*(_CINFO.green_max+1)*(_CINFO.blue_max+1);
        } else if( _CINFO.red_mult > 0 ) {
          curind = ((int)(0.5+r*_CINFO.red_max))*_CINFO.red_mult+
                            ((int)(0.5+g*_CINFO.green_max))*_CINFO.green_mult+
                            ((int)(0.5+b*_CINFO.blue_max))*_CINFO.blue_mult;
        } else if( _CGINFO.red_mult > 0 ) {
	  float l = (r+g+b)/3.;
          curind = (int)(0.5+l*_CGINFO.red_max)*_CGINFO.red_mult;
	} else {
	  curind = 0;
	}
	if( pcolormap->pixels[curind] == curind +_CINFO.base_pixel )
							status = XW_SUCCESS ;
	else curind = -1 ;
      }
    } 

    if( curind < 0 ) {
#ifdef TEST
	if (pcolormap != qcolormap) {
		for (i=0; i<MAX_COLRS; i++) tab_ind[i][0] = -1.;
		qcolormap = pcolormap;
		indice_max = 0;
		indice = 0;
	}

	for (i=0; i<indice_max; i++) {
		if ( tab_ind[i][0] >= 0. ) {
			if ((tab_ind[i][1] == r) &&
			    (tab_ind[i][2] == g) &&
			    (tab_ind[i][3] == b)) {
				*index = curind	= (int) tab_ind[i][0];
				status	= XW_SUCCESS;
#ifdef TRACE_GET_COLOR_INDEX
if( Xw_get_trace() > 3 ) {
    printf(" %d = Xw_get_color_index(%x,%f,%f,%f,=%d/0x%x)\n",
		status,pcolormap,r,g,b,curind,pcolormap->pixels[curind]) ;
}
#endif
				return (status);
			}
		}
	}
#endif

#ifdef PRO9517
      	drmin = dgmin = dbmin = 65536;
#else
	newdist	= 4.0 ;
	curdist	= 4.0 ;
#endif
	curind	= -1 ;

	for( i=n=0 ; i<pcolormap->maxcolor ; i++ ) {
	    if( Xw_isdefine_color( pcolormap,i ) == XW_SUCCESS ) {
	      if( n < pcolormap->maxhcolor ) {
	        colors[n].pixel = pcolormap->pixels[i]; 
	        color_indexs[n] = i;
	        n++;
	      } else {
	        for( j=0 ; j<n ; j++ ) {
		  if( colors[j].pixel == pcolormap->pixels[i] ) break;
	        }
		if( j >= n ) {
		  /* Too many entrys in the virtual colormap */
		  Xw_set_error(129,"Xw_get_color_index",pcolormap ) ;
	 	}
	      }
	    } else {
	      if( freeind < 0 ) freeind = i ;
	    } 
	}

	if( _CCLASS != TrueColor ) {
	  if( n > 0 ) {
	    XQueryColors(_CDISPLAY,_CINFO.colormap,colors,n) ;
#ifdef PRO9517
	    {
	      unsigned long rred = (long unsigned int )( r * 0xFFFF);
	      unsigned long rgreen = (long unsigned int )( g * 0xFFFF);
	      unsigned long rblue = (long unsigned int )( b * 0xFFFF);
	      int dr,dg,db;
	      for( i=0 ; i<n ; i++ ){
		//#if defined(_AIX) || defined(__hpux)
                int aLocalInt =  rred - colors[i].red  ; dr = abs( aLocalInt ) >> 8;
                aLocalInt = rgreen - colors[i].green   ; dg = abs( aLocalInt ) >> 8;
                aLocalInt = rblue - colors[i].blue     ; db = abs( aLocalInt ) >> 8;
		//#else
		//                dr = abs( rred - colors[i].red ) >> 8;
		//                dg = abs( rgreen - colors[i].green ) >> 8;
		//                db = abs( rblue - colors[i].blue ) >> 8;
		//#endif
                if( (dr <= drmin) && (dg <= dgmin) && (db <= dbmin) ) {
                  drmin = dr; dgmin = dg; dbmin = db;
		  curind  = color_indexs[i];
		  if( !dr && !dg && !db ) break;
                }
	      }
	    }
#else
	    for( i=0 ; ((i<n) && (newdist != 0.0)) ; i++ ){
	      dr	= (float)colors[i].red/0xFFFF - r ;
	      dg	= (float)colors[i].green/0xFFFF - g ;
	      db	= (float)colors[i].blue/0xFFFF - b ;
	      newdist	= dr*dr + dg*dg + db*db ;
	      if( newdist < curdist ) {
		curind	= color_indexs[i] ;
		curdist	= newdist ;
	      }
	    }
#endif
	  }
	} else {
	  if( n > 0 ) {
//	    unsigned long mask = _CVISUAL->map_entries-1 ;
	    unsigned long red,green,blue,rmask,gmask,bmask ;
#ifdef PRO9517
	    {
	      unsigned long rred = (long unsigned int )( r * (1 << _CVISUAL->bits_per_rgb) - 1);
	      unsigned long rgreen = (long unsigned int )( g * (1 << _CVISUAL->bits_per_rgb) - 1);
	      unsigned long rblue = (long unsigned int )( b * (1 << _CVISUAL->bits_per_rgb) - 1);
	      int dr,dg,db;
	      for( i=0 ; i<n ; i++ ){
	        rmask = _CVISUAL->red_mask;
	        red = colors[i].pixel & rmask ;
                while ( !(rmask & 0x01) ) { rmask >>= 1; red >>= 1; }
 
	        gmask = _CVISUAL->green_mask;
	        green = colors[i].pixel & gmask ;
                while ( !(gmask & 0x01) ) { gmask >>= 1; green >>= 1; }

	        bmask = _CVISUAL->blue_mask;
	        blue = colors[i].pixel & bmask ;
                while ( !(bmask & 0x01) ) { bmask >>= 1; blue >>= 1; }
	
		//#if defined(_AIX) || defined(__hpux)
		int aLocalInt = rred - red ; dr = abs(aLocalInt);
		aLocalInt = rgreen - green ; dg = abs(aLocalInt);
		aLocalInt = rblue - blue   ; db = abs(aLocalInt);
		//#else
		//                dr = abs( rred - red );
		//                dg = abs( rgreen - green );
		//                db = abs( rblue - blue );
		//#endif
                if( (dr <= drmin) && (dg <= dgmin) && (db <= dbmin) ) {
                  drmin = dr; dgmin = dg; dbmin = db;
		  curind  = color_indexs[i];
		  if( !dr && !dg && !db ) break;
                }
	      }
	    }
#else
	    for( i=0 ; ((i<n) && (newdist != 0.0)) ; i++ ){
	      rmask = _CVISUAL->red_mask;
	      red = colors[i].pixel & rmask ;
              while ( !(rmask & 0x01) ) { rmask >>= 1; red >>= 1; }
 
	      gmask = _CVISUAL->green_mask;
	      green = colors[i].pixel & gmask ;
              while ( !(gmask & 0x01) ) { gmask >>= 1; green >>= 1; }

	      bmask = _CVISUAL->blue_mask;
	      blue = colors[i].pixel & bmask ;
              while ( !(bmask & 0x01) ) { bmask >>= 1; blue >>= 1; }
	
	      dr	= (float)red/mask - r ;
	      dg	= (float)green/mask - g ;
	      db	= (float)blue/mask - b ;
	      newdist	= dr*dr + dg*dg + db*db ;

	      if( newdist < curdist ) {
		curind	= color_indexs[i] ;
		curdist	= newdist ;
	      }
	    }
#endif
	  }
	}

#ifdef PRO9517
	if( drmin > 0 || dgmin > 0 || dbmin > 0 ) {
#else
	if( curdist > 0.0 ) {
#endif
	  if( freeind < 0 ) {
	    unsigned long pixel;
	    if( Xw_alloc_color(pcolormap,r,g,b,&pixel,&isapproximate) ) {
	      for( i=0 ; i<pcolormap->maxcolor ; i++ ) {
	        if( pixel ==  pcolormap->pixels[i] ) break; 
	      }
	      if( i < pcolormap->maxcolor ) {
	        curind = i;
	      }
	    }
	  } else {
	    if( Xw_def_color(pcolormap,freeind,r,g,b) ) {
	      curind = freeind ;
	    }
	  }
	}

	if( curind >= 0 ) {
	    status = XW_SUCCESS ;
#ifdef TEST
	    tab_ind[indice][0] = curind ;
	    tab_ind[indice][1] = r ;
	    tab_ind[indice][2] = g ;
	    tab_ind[indice][3] = b ;
	/*
	 * Une fois que le tableau est  plein, il faut le
	 * parcourir completement, donc on ne touche plus
	 * a indice_max.
	 */
	    indice ++ ;
	    if( indice < MAX_COLRS ) {
	        indice_max = max(indice_max,indice);
	    } else {
		indice = 0 ; /* on repart a 0 */
#ifdef TRACE_GET_COLOR_INDEX
			if( Xw_get_trace() > 1 ) {
			    printf(" ************************\n") ;
			    printf(" Xw_get_color_index, full array\n") ;
			    printf(" ************************\n") ;
			}
#endif
	    }
#endif
	}

    }

    *index = curind ;

#ifdef TRACE_GET_COLOR_INDEX
if( Xw_get_trace() > 3 ) {
    printf(" %d = Xw_get_color_index(%lx,%f,%f,%f,#%d/0x%lx)\n",
		status,(long ) pcolormap,r,g,b,curind,(long ) pcolormap->pixels[curind]) ;
}
#endif

	return( status ) ;
}
