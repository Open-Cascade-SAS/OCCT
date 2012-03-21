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

/*
C<
C***********************************************************************
C
C     FONCTION :
C     ----------
C
C
C$    HISTORIQUE DES MODIFICATIONS   :
C     --------------------------------
C       21-02-97 : GG ; G1334-PERF 
C		   Ameliorer les performances de traduction
C		   des images XWD.
C		   >> Rempacer XGetPixel et XputPixel par des
C		   directes dans le buffer image.
C	07-05-97 : GG ; PRO7909
C		   Eviter de detruire l'image convertie lorsqu'il n'y a pas
C		   de conversion a faire !
*/

#define PRO7909
 
#include <Xw_Extension.h>

	/* ifdef then trace on */
#ifdef TRACE
#define TRACE_CONVERT_IMAGE
#endif

/*
   XW_STATUS Xw_convert_image (awindow,aimage,acolors,ncolors):
   XW_EXT_WINDOW *awindow
   XW_EXT_IMAGEDATA   *aimage
   XW_EXT_COLOR	*acolors	(can be NULL)
   int ncolors

	Convert an image depending of the window class

	returns  ERROR if image can't be converted.
	returns  SUCCESS if successfull

*/

#define CREATE_IMAGE(_fun,_out,_nbp) \
                                /* Allocate the new pixel buffer */	\
        if( !(_out = (unsigned char*) Xw_calloc(piimage->width * piimage->height,_nbp)) ) {\
          /*ERROR*Bad Image allocation*/	\
          Xw_set_error(60,_fun,0) ;	\
          return NULL ;	\
        }	\
                                /* Create the new Image structure */	\
        if( !(poimage = XCreateImage(_DISPLAY,_VISUAL,_DEPTH,ZPixmap,\
                0,(char*)_out,piimage->width,piimage->height,_nbp*8,0)) ) {\
          /* ERROR*XImage Creation failed */	\
          Xw_set_error(62,_fun,0) ;	\
          Xw_free((char*)_out) ;	\
          return NULL ;	\
        }

#define COPY_PSEUDO_TO_PSEUDO(_fun,_in,_out) \
        for( y=0 ; y<piimage->height ; y++ ) {	\
          for( x=0 ; x<piimage->width ; x++ ) {	\
            ipixel = *_in++;	\
            if( ipixel != lipixel ) {	\
              lipixel = ipixel;	\
              if( ipixel >= MAXCOLOR ) ipixel = 0 ;	\
              opixel = mapping[ipixel] ;	\
              if( !opixel ) {	\
                for( i=0,j=ipixel ; i<ncolors ; i++,j++) {	\
                  if( j >= ncolors ) j = 0 ;	\
                  if( ipixel == pcolors[j].pixel ) break;	\
                }	\
                red = (float)pcolors[j].red/0xFFFF ;	\
                green = (float)pcolors[j].green/0xFFFF ;	\
                blue = (float)pcolors[j].blue/0xFFFF ;	\
                if( !Xw_get_color_pixel(_COLORMAP,red,green,blue,&opixel,&isapproximate)) {\
                  /*ERROR*XImage Creation failed*/	\
                  Xw_set_error(63,_fun,&_COLORMAP->info.colormap) ;\
                }	\
                mapping[ipixel] = opixel ;	\
              }	\
            }	\
            *_out++ = opixel;	\
          }	\
          if( ipad > 0 ) pidata.cdata += ipad;	\
        }	

#define COPY_PSEUDO_TO_TRUE(_fun,_in,_out) \
        for( y=0 ; y<piimage->height ; y++ ) {	\
          for( x=0 ; x<piimage->width ; x++ ) {	\
            ipixel = *_in++;	\
            if( ipixel != lipixel ) {	\
              lipixel = ipixel;	\
              if( ipixel >= MAXCOLOR ) ipixel = 0 ;	\
              opixel = mapping[ipixel] ;	\
              if( !opixel ) {	\
                for( i=0,j=ipixel ; i<ncolors ; i++,j++) {	\
                  if( j >= ncolors ) j = 0 ;	\
                  if( ipixel == pcolors[j].pixel ) break;	\
                }	\
                red = (float)pcolors[j].red/0xFFFF ;	\
                green = (float)pcolors[j].green/0xFFFF ;	\
                blue = (float)pcolors[j].blue/0xFFFF ;	\
                if( !Xw_get_color_pixel(_COLORMAP,red,green,blue,&opixel,&isapproximate)) {\
                  /*ERROR*XImage Creation failed*/	\
                  Xw_set_error(63,_fun,&_COLORMAP->info.colormap) ;\
                }	\
                mapping[ipixel] = opixel ;	\
              }	\
            }	\
            *_out++ = opixel;	\
          }	\
          if( ipad > 0 ) pidata.cdata += ipad;	\
        }	

#define COPY_TRUE_TO_PSEUDO(_fun,_in,_out) \
        for( y=0 ; y<piimage->height ; y++ ) {	\
          for( x=0 ; x<piimage->width ; x++ ) {	\
            ipixel = *_in++;	\
	    if( ipixel != lipixel ) {	\
	      lipixel = ipixel;	\
              red = (float)((ipixel >> sred) & cmask)/(float)cmask;	\
              green = (float)((ipixel >> sgreen) & cmask)/(float)cmask;	\
              blue = (float)((ipixel >> sblue) & cmask)/(float)cmask;	\
              if( !Xw_get_color_pixel(_COLORMAP,red,green,blue,&opixel,&isapproximate)) {\
                /*ERROR*XImage Creation failed*/	\
                Xw_set_error(63,_fun,&_COLORMAP->info.colormap) ;	\
              }	\
	    }	\
            *_out++ = opixel;	\
          }	\
          if( ipad > 0 ) pidata.cdata += ipad;	\
        }	

#define COPY_TRUE_TO_TRUE(_fun,_in,_out) \
        for( y=0 ; y<piimage->height ; y++ ) {	\
          for( x=0 ; x<piimage->width ; x++ ) {	\
            ipixel = *_in++;	\
	    if( ipixel != lipixel ) {	\
	      lipixel = ipixel;	\
              red = (float)((ipixel >> sred) & cmask)/(float)cmask;	\
              green = (float)((ipixel >> sgreen) & cmask)/(float)cmask;	\
              blue = (float)((ipixel >> sblue) & cmask)/(float)cmask;	\
              if( !Xw_get_color_pixel(_COLORMAP,red,green,blue,&opixel,&isapproximate)) {\
                /*ERROR*XImage Creation failed*/	\
                Xw_set_error(63,_fun,&_COLORMAP->info.colormap) ;	\
              }	\
	    }	\
            *_out++ = opixel;	\
          }	\
          if( ipad > 0 ) pidata.cdata += ipad;	\
        }	


#ifdef XW_PROTOTYPE
static XImage* ConvertPseudoToPseudo(XW_EXT_WINDOW *pwindow,XImage *piimage,XColor *pcolors,int ncolors) ;
static XImage* ConvertPseudoToTrue(XW_EXT_WINDOW *pwindow,XImage *piimage,XColor *pcolors,int ncolors) ;
static XImage* ConvertTrueToPseudo(XW_EXT_WINDOW *pwindow,XImage *piimage) ;
static XImage* ConvertTrueToTrue(XW_EXT_WINDOW *pwindow,XImage *piimage) ;
#else
static XImage* ConvertPseudoToPseudo() ;
static XImage* ConvertPseudoToTrue() ;
static XImage* ConvertTrueToPseudo() ;
static XImage* ConvertTrueToTrue() ;
#endif /*XW_PROTOTYPE*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_convert_image (void *awindow,void *aimage,void *acolors,int ncolors)
#else
XW_STATUS Xw_convert_image (awindow,aimage,acolors,ncolors)
void *awindow;
void *aimage;
void *acolors;
int ncolors;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*)aimage;
XColor *pcolors = (XColor*)acolors;
XImage *piimage,*poimage = NULL;
XW_STATUS status = XW_ERROR;
int i,iclass = 0;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_convert_image",pwindow) ;
        return (XW_ERROR) ;
    }

    if( !Xw_isdefine_image(pimage) ) {
         /*ERROR*Bad EXT_IMAGEDATA Address*/
         Xw_set_error(25,"Xw_convert_image",pimage) ;
         return (XW_ERROR) ;
    }

    piimage = (_ZIMAGE) ? _ZIMAGE : _IIMAGE;

    if( !pcolors && piimage->red_mask && 
		piimage->green_mask && piimage->blue_mask ) iclass = TrueColor;
    else iclass = PseudoColor;

    switch ( iclass ) {
      case PseudoColor :

        if( !pcolors ) {
          if( !_ICOLORMAP ) _ICOLORMAP = _COLORMAP;
          ncolors = min(_IVISUAL->map_entries,MAXCOLOR);
          pcolors = (XColor*) Xw_calloc(sizeof(XColor),ncolors);
          for( i=0 ; i<ncolors ; i++ ) pcolors[i].pixel = i;
          XQueryColors(_DISPLAY,_ICOLORMAP->info.colormap,pcolors,ncolors) ;
        }

	if( pcolors ) switch (_CLASS ) {
	 case PseudoColor :	/* Convert Pseudo to PseudoColor */
	   if( acolors || (_ICOLORMAP != _COLORMAP) ) {
	     poimage = ConvertPseudoToPseudo(pwindow,piimage,pcolors,ncolors) ;
	   } else {
	     poimage = piimage;
	   }
	   break ;
	 case TrueColor : 	/* Convert Pseudo to TrueColor */
	   poimage = ConvertPseudoToTrue(pwindow,piimage,pcolors,ncolors) ;
	   break ;
	 default:
           /*ERROR*Unimplemented Visual class*/
           Xw_set_error(5,"Xw_convert_image",&_CLASS) ;
        }
#ifdef BUG	/* Cette memoire est liberee par Xw_load_image !! */
	if( pcolors && !acolors ) Xw_free(pcolors);
#endif
        break ;

      case TrueColor :
	switch (_CLASS ) {
	 case PseudoColor :	/* Convert True to PseudoColor */
	   poimage = ConvertTrueToPseudo(pwindow,piimage) ;
	   break ;
	 case TrueColor :	/* Convert True to TrueColor */
	   poimage = ConvertTrueToTrue(pwindow,piimage) ;
	   break ;
	 default:
           /*ERROR*Unimplemented Visual class*/
           Xw_set_error(5,"Xw_convert_image",&_CLASS) ;
	}
	break ;

	default:
         /*ERROR*Unimplemented Visual class*/
         Xw_set_error(5,"Xw_convert_image",&iclass) ;
    }

    _ICOLORMAP = _COLORMAP;
    if( poimage ) {
      status = XW_SUCCESS;
                                /* Destroy the old image */
      if( _ZIMAGE && (_ZIMAGE != _IIMAGE) ) {
        XDestroyImage(_ZIMAGE);
        _ZIMAGE = NULL;
      }
                                /* Initialize the output image */
#ifdef PRO7909
      if( _IIMAGE != poimage ) {
	XDestroyImage(_IIMAGE);
        _IIMAGE = poimage;
      }
#else
      XDestroyImage(_IIMAGE);
      _IIMAGE = poimage;
#endif
      pimage->zoom = 1.;
    }

#ifdef  TRACE_CONVERT_IMAGE
if( Xw_get_trace() ) {
    printf (" %d = Xw_convert_image(%lx,%lx,%lx,%d)\n",
	    status,(long ) pwindow,(long ) pimage,(long ) pcolors,ncolors);
}
#endif

    return (status);
}

#ifdef XW_PROTOTYPE
static XImage* ConvertTrueToPseudo(XW_EXT_WINDOW *pwindow,XImage *piimage)
#else
static XImage* ConvertTrueToPseudo(pwindow,piimage)
XW_EXT_WINDOW *pwindow ;
XImage *piimage ;
#endif /*XW_PROTOTYPE*/
/*
	Convert TrueColor to PseudoColor Image
*/
{
unsigned long ipixel,opixel,cmask ;
unsigned long lipixel = 0xFFFFFFFF ;
//int i,j,ipad,isapproximate ;
int ipad,isapproximate ;
int sred,sgreen,sblue ;
float red,green,blue ;
register union {
  unsigned char *cdata ;
  unsigned short *sdata ;
  unsigned int *idata ;
} pidata,podata;
register int x,y;
XImage *poimage = NULL ;

#ifdef TRACE_CONVERT_IMAGE
	if( Xw_get_trace() ) {
	    printf(" Xw_convert_image.TrueToPseudo()\n") ;
	}
#endif

    ipad =  piimage->bytes_per_line - 
			piimage->width * piimage->bits_per_pixel/8 ;
    pidata.cdata = (unsigned char*)piimage->data + piimage->xoffset;

    sred = sgreen = sblue = 0 ;
    cmask = piimage->red_mask ;
    while ( !(cmask & 1) ) { cmask >>= 1 ; sred++ ; }
    cmask = piimage->green_mask ;
    while ( !(cmask & 1) ) { cmask >>= 1 ; sgreen++ ; }
    cmask = piimage->blue_mask ;
    while ( !(cmask & 1) ) { cmask >>= 1 ; sblue++ ; }

    CREATE_IMAGE("ConvertTrueToPseudo",podata.cdata,1)
    switch (piimage->bits_per_pixel) {
      case 8 :
         COPY_TRUE_TO_PSEUDO("ConvertTrueToPseudo",pidata.cdata,podata.cdata);
	 break;
      case 16 :
        COPY_TRUE_TO_PSEUDO("ConvertTrueToPseudo",pidata.sdata,podata.cdata);
        break;
      case 32 :
        COPY_TRUE_TO_PSEUDO("ConvertTrueToPseudo",pidata.idata,podata.cdata);
        break;
      default:
        /*ERROR*Unimplemented Image Visual depth*/
        Xw_set_error(64,"ConvertTrueToPseudo",&piimage->bits_per_pixel);
    }

    return (poimage) ;
}

#ifdef XW_PROTOTYPE
static XImage* ConvertPseudoToTrue(XW_EXT_WINDOW *pwindow,XImage *piimage,XColor *pcolors,int ncolors)
#else
static XImage* ConvertPseudoToTrue(pwindow,piimage,pcolors,ncolors)
XW_EXT_WINDOW *pwindow ;
XImage *piimage ;
XColor *pcolors;
int ncolors;
#endif /*XW_PROTOTYPE*/
/*
	Convert PseudoColor to TrueColor Image
*/
{
unsigned long ipixel,opixel ;
unsigned long lipixel = 0xFFFFFFFF ;
unsigned long mapping[MAXCOLOR];
int i,j,ipad,isapproximate ;
float red,green,blue ;
register union {
  unsigned char *cdata ;
  unsigned short *sdata ;
  unsigned int *idata ;
} pidata,podata;
register int x,y;
XImage *poimage = NULL ;

#ifdef TRACE_CONVERT_IMAGE
	if( Xw_get_trace() ) {
	    printf(" Xw_convert_image.PseudoToTrue()\n") ;
	}
#endif

    for( i=0 ; i<MAXCOLOR ; i++ ) mapping[i] = 0;
    ipad =  piimage->bytes_per_line - 
			piimage->width * piimage->bits_per_pixel/8 ;
    pidata.cdata = (unsigned char*)piimage->data + piimage->xoffset;

    switch (_DEPTH) {
      case 4 :
      case 8 :
        CREATE_IMAGE("ConvertPseudoToTrue",podata.cdata,1)
	COPY_PSEUDO_TO_TRUE("ConvertPseudoToTrue",pidata.cdata,podata.cdata);
	break;
      case 12 :
        CREATE_IMAGE("ConvertPseudoToTrue",podata.cdata,2)
	COPY_PSEUDO_TO_TRUE("ConvertPseudoToTrue",pidata.cdata,podata.sdata);
	break;
      case 24 :
        CREATE_IMAGE("ConvertPseudoToTrue",podata.cdata,4)
	COPY_PSEUDO_TO_TRUE("ConvertPseudoToTrue",pidata.cdata,podata.idata);
	break;
      default:
        /*ERROR*Unimplemented Image Visual depth*/
        Xw_set_error(64,"ConvertPseudoToTrue",&_DEPTH);
    }


    return (poimage) ;
}

#ifdef XW_PROTOTYPE
static XImage* ConvertPseudoToPseudo(XW_EXT_WINDOW *pwindow,XImage *piimage,XColor *pcolors,int ncolors)
#else
static XImage* ConvertPseudoToPseudo(pwindow,piimage,pcolors,ncolors)
XW_EXT_WINDOW *pwindow ;
XImage *piimage ;
XColor *pcolors;
int ncolors;
#endif /*XW_PROTOTYPE*/
/*
	Convert PseudoColor to PseudoColor Image
*/
{
unsigned long ipixel,opixel ;
unsigned long lipixel = 0xFFFFFFFF ;
unsigned long mapping[MAXCOLOR];
int i,j,ipad,isapproximate ;
float red,green,blue ;
register union {
  unsigned char *cdata ;
  unsigned short *sdata ;
  unsigned int *idata ;
} pidata,podata;
register int x,y;
XImage *poimage = NULL ;

#ifdef TRACE_CONVERT_IMAGE
	if( Xw_get_trace() ) {
	    printf(" Xw_convert_image.PseudoToPseudo()\n") ;
	}
#endif

    for( i=0 ; i<MAXCOLOR ; i++ ) mapping[i] = 0;
    ipad =  piimage->bytes_per_line - 
			piimage->width * piimage->bits_per_pixel/8 ;
    pidata.cdata = (unsigned char*)piimage->data + piimage->xoffset;

    switch (_DEPTH) {
      case 4 :
      case 8 :
        CREATE_IMAGE("ConvertPseudoToPseudo",podata.cdata,1)
	COPY_PSEUDO_TO_PSEUDO("ConvertPseudoToPseudo",pidata.cdata,podata.cdata);
	break;
      default:
        /*ERROR*Unimplemented Image Visual depth*/
        Xw_set_error(64,"ConvertPseudoToPseudo",&_DEPTH);
    }


    return (poimage) ;
}

#ifdef XW_PROTOTYPE
static XImage* ConvertTrueToTrue(XW_EXT_WINDOW *pwindow,XImage *piimage)
#else
static XImage* ConvertTrueToTrue(pwindow,piimage)
XW_EXT_WINDOW *pwindow ;
XImage *piimage ;
#endif /*XW_PROTOTYPE*/
/*
	Convert TrueColor to TrueColor Image
*/
{
unsigned long ipixel,opixel,cmask ;
unsigned long lipixel = 0xFFFFFFFF ;
//int i,j,ipad,isapproximate ;
int ipad,isapproximate ;
int sred,sgreen,sblue ;
float red,green,blue ;
register union {
  unsigned char *cdata ;
  unsigned short *sdata ;
  unsigned int *idata ;
} pidata,podata;
register int x,y;
XImage *poimage = NULL ;

#ifdef TRACE_CONVERT_IMAGE
	if( Xw_get_trace() ) {
	    printf(" Xw_convert_image.TrueToTrue()\n") ;
	}
#endif

    if( (_DEPTH == piimage->depth) &&
	  (_VISUAL->red_mask == piimage->red_mask) &&
	    (_VISUAL->green_mask == piimage->green_mask) &&
	      (_VISUAL->blue_mask == piimage->blue_mask) ) return piimage;

    ipad =  piimage->bytes_per_line - 
			piimage->width * piimage->bits_per_pixel/8 ;
    pidata.cdata = (unsigned char*)piimage->data + piimage->xoffset;

    sred = sgreen = sblue = 0 ;
    cmask = piimage->red_mask ;
    while ( !(cmask & 1) ) { cmask >>= 1 ; sred++ ; }
    cmask = piimage->green_mask ;
    while ( !(cmask & 1) ) { cmask >>= 1 ; sgreen++ ; }
    cmask = piimage->blue_mask ;
    while ( !(cmask & 1) ) { cmask >>= 1 ; sblue++ ; }

    switch (_DEPTH) {
      case 4 :
      case 8 :
        CREATE_IMAGE("ConvertTrueToTrue",podata.cdata,1)
        switch (piimage->bits_per_pixel) {
	  case 8 :	
            COPY_TRUE_TO_TRUE("ConvertTrueToTrue",pidata.cdata,podata.cdata);
	    break;
	  case 16 :
            COPY_TRUE_TO_TRUE("ConvertTrueToTrue",pidata.sdata,podata.cdata);
	    break;
	  case 32 :
            COPY_TRUE_TO_TRUE("ConvertTrueToTrue",pidata.idata,podata.cdata);
	    break;
          default:
            /*ERROR*Unimplemented Image Visual depth*/
            Xw_set_error(64,"ConvertTrueToTrue",&piimage->bits_per_pixel);
	}
	break;
      case 12 :
        CREATE_IMAGE("ConvertTrueToTrue",podata.cdata,2)
        switch (piimage->bits_per_pixel) {
	  case 8 :	
            COPY_TRUE_TO_TRUE("ConvertTrueToTrue",pidata.cdata,podata.sdata);
	    break;
	  case 16 :
            COPY_TRUE_TO_TRUE("ConvertTrueToTrue",pidata.sdata,podata.sdata);
	    break;
	  case 32 :
            COPY_TRUE_TO_TRUE("ConvertTrueToTrue",pidata.idata,podata.sdata);
	    break;
          default:
            /*ERROR*Unimplemented Image Visual depth*/
            Xw_set_error(64,"ConvertTrueToTrue",&piimage->bits_per_pixel);
	}
        break;
      case 24 :
        CREATE_IMAGE("ConvertTrueToTrue",podata.cdata,4)
        switch (piimage->bits_per_pixel) {
	  case 8 :	
            COPY_TRUE_TO_TRUE("ConvertTrueToTrue",pidata.cdata,podata.idata);
	    break;
	  case 16 :
            COPY_TRUE_TO_TRUE("ConvertTrueToTrue",pidata.sdata,podata.idata);
	    break;
	  case 32 :
            COPY_TRUE_TO_TRUE("ConvertTrueToTrue",pidata.idata,podata.idata);
	    break;
          default:
            /*ERROR*Unimplemented Image Visual depth*/
            Xw_set_error(64,"ConvertTrueToTrue",&piimage->bits_per_pixel);
	}
        break;
      default:
        /*ERROR*Unimplemented Image Visual depth*/
        Xw_set_error(64,"ConvertTrueToTrue",&_DEPTH);
    }

    return (poimage) ;
}
