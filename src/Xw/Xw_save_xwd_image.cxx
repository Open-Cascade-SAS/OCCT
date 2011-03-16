/*
  Created:  zov : 1-Apr-1998  : Saves an image as .XWD file
  Note:     Adapted from Xw_save_image.c
*/

#define PRO5356	/* GG_130996: Compatibillite vis a vis de XReadDisplay (IRIX)
		   GG_240898  Compatibillite vis a vis de XReadScreen (SOLARIS)
*/

#define PRO16753       /* GG 261198
//              Don't free pname because this pointer is a
//              static address in Xw_get_filename() space.
*/

#include <Xw_Extension.h>

/* ifdef then trace on */
#ifdef TRACE
#define TRACE_SAVE_IMAGE
#endif

#define DECOSF1_COMPATIBILITY

/*

   XW_STATUS Xw_save_xwd_image (awindow,aimage,filename):
   XW_EXT_WINDOW *awindow
   XW_EXT_IMAGEDATA   *aimage
   char *filename	XWD Image name

	Save an image to an XWD file.

	returns  SUCCESS if successfull
	returns  ERROR if something is wrong

*/

#define SWAPTEST (*(char*)&swaptest)
#define LOWBIT(x) ((x) & (~(x) + 1))

static unsigned long swaptest = 1;
#ifdef XW_PROTOTYPE
static void SwapLong ( char*, unsigned);
static void SwapShort ( char* bp, unsigned);
#else
static void SwapLong();
static void SwapShort();
#endif
static XWDFileHeader header;

#ifdef XW_PROTOTYPE
XW_STATUS Xw_save_xwd_image_adv (Display *aDisplay,Window aWindow,XWindowAttributes aWinAttr,XImage *aPximage,Colormap aColormap,int aNcolors,char *filename)
#else
XW_STATUS Xw_save_xwd_image_adv (aDisplay,aWindow,aWinAttr,aPximage,aColormap,aNcolors,filename)
Display *aDisplay;
Window aWindow;
XWindowAttributes aWinAttr;
XImage *aPximage;
Colormap aColormap;
int ncolors;
char *filename;
#endif /*XW_PROTOTYPE*/
{
char *wname = NULL,*pname = NULL ;
int i,isize,lname,iclass,ncolors;
Visual *pvisual = aWinAttr.visual;
XColor *pcolors = NULL;
XWDColor *qcolors = NULL;
FILE *fimage = NULL ;
XW_STATUS status = XW_SUCCESS;

    pname = Xw_get_filename(filename,"xwd") ;
				/* Open XWD File */
    if( pname ) fimage = fopen(pname,"w") ;

    if( !fimage ) {
	/*ERROR*Bad Filename*/
	Xw_set_error(55,"Xw_save_xwd_image",filename) ;
	return (XW_ERROR) ;
    }
				/* Get Window name */	
    XFetchName(aDisplay,aWindow,&wname);

    if( !wname || !strlen(wname) ) {
        wname = (char*) strdup(pname);
    }
    lname = strlen(wname) + 1;

    iclass = pvisual->c_class;
#ifdef PRO5356
    if( (iclass == PseudoColor) && (aPximage->red_mask > 0) ) iclass = TrueColor;

/*    iclass = (pximage->depth > 8) ? TrueColor : pvisual->c_class;  GG_240898 */
#endif
				/* Get color table */ 
    switch ( iclass ) {
        case PseudoColor :     
          ncolors = aNcolors;
    	  if(( pcolors = (XColor*) Xw_calloc(ncolors,sizeof(XColor)) )) {
            for( i=0 ; i<ncolors ; i++) {
              pcolors[i].pixel = i;
              pcolors[i].pad = 0;
	    }
    	    XQueryColors(aDisplay,aColormap,pcolors,ncolors);
#ifdef DECOSF1_COMPATIBILITY
	    if( sizeof(XColor) != sizeof(XWDColor) ) {
    	      if(( qcolors = (XWDColor*) Xw_calloc(ncolors,sizeof(XWDColor)) )) {
                for( i=0 ; i<ncolors ; i++) {
                  qcolors[i].pixel = pcolors[i].pixel;
                  qcolors[i].red = pcolors[i].red;
                  qcolors[i].green = pcolors[i].green;
                  qcolors[i].blue = pcolors[i].blue;
                  qcolors[i].flags = pcolors[i].flags;
		  qcolors[i].pad = 0;
	        }
		Xw_free(pcolors);
	      } else {
	        /*ERROR*Bad Image allocation*/
	        Xw_set_error(60,"Xw_save_xwd_image",0) ;
	        Xw_free(wname) ;
#ifndef PRO16753 
	        Xw_free(pname) ;
#endif
	        Xw_free(pcolors) ;
    	        fclose(fimage) ;
	        return (XW_ERROR) ;
	      }
	    } else {
	      qcolors = (XWDColor*) pcolors;
	    }
#endif
	  } else {
	    /*ERROR*Bad Image allocation*/
	    Xw_set_error(60,"Xw_save_xwd_image",0) ;
	    Xw_free(wname) ;
#ifndef PRO16753 
	    Xw_free(pname) ;
#endif
    	    fclose(fimage) ;
	    return (XW_ERROR) ;
          } 
          break ;
        case TrueColor :  
	  ncolors = 0;
          break ;
        case DirectColor :
	  ncolors = aNcolors;
    	  if(( pcolors = (XColor*) Xw_calloc(ncolors,sizeof(XColor)) )) {
            unsigned long red = 0,lred = LOWBIT(pvisual->red_mask);
            unsigned long green = 0,lgreen = LOWBIT(pvisual->green_mask);
            unsigned long blue = 0,lblue = LOWBIT(pvisual->blue_mask);
            for( i=0 ; i<ncolors ; i++) {
              pcolors[i].pixel = red|green|blue;
              pcolors[i].pad = 0;
              red += lred;
              if( red > pvisual->red_mask ) red = 0;
              green += lgreen;
              if( green > pvisual->green_mask ) green = 0;
              blue += lblue;
              if( blue > pvisual->blue_mask ) blue = 0;
            }
    	    XQueryColors(aDisplay,aColormap,pcolors,ncolors);
#ifdef DECOSF1_COMPATIBILITY
	    if( sizeof(XColor) != sizeof(XWDColor) ) {
    	      if(( qcolors = (XWDColor*) Xw_calloc(ncolors,sizeof(XWDColor)) )) {
                for( i=0 ; i<ncolors ; i++) {
                  qcolors[i].pixel = pcolors[i].pixel;
                  qcolors[i].red = pcolors[i].red;
                  qcolors[i].green = pcolors[i].green;
                  qcolors[i].blue = pcolors[i].blue;
                  qcolors[i].flags = pcolors[i].flags;
                  qcolors[i].pad = 0;
	        }
		Xw_free(pcolors);
	      } else {
	        /*ERROR*Bad Image allocation*/
	        Xw_set_error(60,"Xw_save_xwd_image",0) ;
	        Xw_free(wname) ;
#ifndef PRO16753 
	        Xw_free(pname) ;
#endif
	        Xw_free(pcolors) ;
    	        fclose(fimage) ;
	        return (XW_ERROR) ;
	      }
	    } else {
	      qcolors = (XWDColor*) pcolors;
	    }
#endif
	  } else {
	    /*ERROR*Bad Image allocation*/
	    Xw_set_error(60,"Xw_save_xwd_image",0) ;
	    Xw_free(wname) ;
#ifndef PRO16753 
	    Xw_free(pname) ;
#endif
    	    fclose(fimage) ;
	    return (XW_ERROR) ;
          } 
          break ;
	default :
	    /*ERROR*Unimplemented Image Visual class*/
	    Xw_set_error(59,"Xw_save_xwd_image",&header.visual_class) ;
    	    fclose(fimage) ;
	    return (XW_ERROR) ;
    }

 				/* Build Header */
    header.ncolors = ncolors;
    header.header_size = (CARD32) (sizeof(header) + lname);
    header.file_version = (CARD32) XWD_FILE_VERSION;
    header.pixmap_format = (CARD32) ZPixmap;
    header.pixmap_depth = (CARD32) aPximage->depth;
    header.pixmap_width = (CARD32) aPximage->width;
    header.pixmap_height = (CARD32) aPximage->height;
    header.xoffset = (CARD32) aPximage->xoffset;
    header.byte_order = (CARD32) aPximage->byte_order;
    header.bitmap_unit = (CARD32) aPximage->bitmap_unit;
    header.bitmap_bit_order = (CARD32) aPximage->bitmap_bit_order;
    header.bitmap_pad = (CARD32) aPximage->bitmap_pad;
    header.bits_per_pixel = (CARD32) aPximage->bits_per_pixel;
    header.bytes_per_line = (CARD32) aPximage->bytes_per_line;
    header.visual_class = (CARD32) iclass;
#ifdef PRO5356
    header.red_mask = (CARD32) aPximage->red_mask;
    header.green_mask = (CARD32) aPximage->green_mask;
    header.blue_mask = (CARD32) aPximage->blue_mask;
#else
    header.red_mask = (CARD32) pvisual->red_mask;
    header.green_mask = (CARD32) pvisual->green_mask;
    header.blue_mask = (CARD32) pvisual->blue_mask;
#endif
    header.bits_per_rgb = (CARD32) pvisual->bits_per_rgb;
    header.colormap_entries = (CARD32) pvisual->map_entries;
    header.window_width = (CARD32) aWinAttr.width;
    header.window_height = (CARD32) aWinAttr.height;
    header.window_x = aWinAttr.x;
    header.window_y = aWinAttr.y;
    header.window_bdrwidth = (CARD32) aWinAttr.border_width;
    isize = header.bytes_per_line*header.pixmap_height ;

    if( SWAPTEST ) {
      SwapLong((char*) &header,sizeof(header)) ;
#ifdef DECOSF1_COMPATIBILITY
      for( i=0 ; i<ncolors ; i++) {
          SwapLong((char *) &qcolors[i].pixel,4);
          SwapShort((char *) &qcolors[i].red, 3*sizeof(short));
      }
#else
      for( i=0 ; i<ncolors ; i++) {
          SwapLong((char *) &pcolors[i].pixel,sizeof(long));
          SwapShort((char *) &pcolors[i].red, 3*sizeof(short));
      }
#endif
    }
 
    if( status && (int)fwrite((char *)&header, sizeof(header), 1, fimage) < 1 ) 
							status = XW_ERROR;
    if( status && (int)fwrite(wname, lname, 1, fimage) < 1 ) status = XW_ERROR;
 
    /*
     * Write out the color maps, if any
     */
    
#ifdef DECOSF1_COMPATIBILITY
    if( ncolors > 0 ) {    
      if( status && 
	 (int)fwrite((char *) qcolors, sizeof(XWDColor), ncolors, fimage) < ncolors ) 
								status = XW_ERROR;
    }
#else
    if( ncolors > 0 ) {    
      if( status && 
	 fwrite((char *) pcolors, sizeof(XColor), ncolors, fimage) < ncolors ) 
								status = XW_ERROR;
    }
#endif
 
    /*
     * Write out the buffer.
     */
      
    /*
     *    This copying of the bit stream (data) to a file is to be replaced
     *  by an Xlib call which hasn't been written yet.  It is not clear
     *  what other functions of xwd will be taken over by this (as yet)
     *  non-existant X function.
     */ 
    if( status && (int)fwrite(aPximage->data, 1, isize, fimage) < isize ) 
							status = XW_ERROR;
       
				/* Close the image file */
    fclose(fimage) ;

#ifndef PRO16753 
    Xw_free(pname) ;
#endif

    Xw_free(wname) ;

#ifdef DECOSF1_COMPATIBILITY
    if( qcolors ) Xw_free(qcolors) ;
#else
    if( pcolors ) Xw_free(pcolors) ;
#endif

    return (status);
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_save_xwd_image (void *awindow,void *aimage,char *filename)
#else
XW_STATUS Xw_save_xwd_image (awindow,aimage,filename)
void *awindow;
void *aimage;
char *filename ;
#endif /*XW_PROTOTYPE*/
{
XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*) awindow;
XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*) aimage;
XImage *pximage = NULL;
XW_STATUS status = XW_SUCCESS;

    if( !Xw_isdefine_window(pwindow) ) {
        /*ERROR*Bad EXT_WINDOW Address*/
        Xw_set_error(24,"Xw_save_xwd_image",pwindow) ;
        return (XW_ERROR) ;
    }

    if( !Xw_isdefine_image(pimage) ) {
         /*ERROR*Bad EXT_IMAGEDATA Address*/
         Xw_set_error(25,"Xw_save_xwd_image",pimage) ;
         return (XW_ERROR) ;
    }

    pximage = (_ZIMAGE) ? _ZIMAGE : _IIMAGE;

    status = Xw_save_xwd_image_adv(_DISPLAY,_WINDOW,_ATTRIBUTES,pximage,_COLORMAP->info.colormap,_COLORMAP->maxhcolor,filename);
#ifdef  TRACE_SAVE_IMAGE
    if( Xw_get_trace() ) {
      printf (" %d = Xw_save_xwd_image(%lx,%lx,'%s')\n",status,(long ) pwindow,(long ) pimage,filename);
    }
#endif
    return status;
}

#ifdef XW_PROTOTYPE
static void SwapLong (register char* bp, register unsigned n)
#else
static void SwapLong (bp, n)
    register char *bp;
    register unsigned n;
#endif
/*
    Swap long ints depending of CPU
*/
{
    register char c;
    register char *ep = bp + n;
    register char *sp;

    while (bp < ep) {
        sp = bp + 3;
        c = *sp;
        *sp = *bp;
        *bp++ = c;
        sp = bp + 1;
        c = *sp;
        *sp = *bp;
        *bp++ = c;
        bp += 2;
    }
}

#ifdef XW_PROTOTYPE
static void SwapShort (register char* bp, register unsigned n)
#else
static void SwapShort (bp, n)
    register char *bp;
    register unsigned n;
#endif
/*
    Swap short ints depending of CPU
*/
{
    register char c;
    register char *ep = bp + n;

    while (bp < ep) {
        c = *bp;
        *bp = *(bp + 1);
        bp++;
        *bp++ = c;
    }
}
