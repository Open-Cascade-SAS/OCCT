/*
  Created:  zov : 1-Apr-1998  : Saves an image as .BMP file
  Modified: zov : 10-Apr-1998 : First release
  Modified: zov : 17-Apr-1998
*/

#define PRO5356 /* GG_130996: Compatibillite vis a vis de XReadDisplay (IRIX)
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

   XW_STATUS Xw_save_bmp_image (awindow,aimage,filename):
   XW_EXT_WINDOW *awindow
   XW_EXT_IMAGEDATA   *aimage
   char *filename	BMP Image name

	Save an image to a BMP file.

	returns  SUCCESS if successfull
	returns  ERROR if something is wrong

*/

typedef unsigned int   DWORD; /* 32-bit signed */
typedef int            LONG;  /* 32-bit unsigned */
typedef unsigned short WORD;  /* 16-bit unsigned */


/* NOTE, different platforms store WORDs in different ways */
/* You have to select from the given macros those that you need */
#if  defined(__osf__) || defined(DECOSF1)

#define SWAP_WORD(__w)  (__w)
#define SWAP_DWORD(__w)  (__w)

#else

#define SWAP_WORD(__w) ((((__w)&0xFF) << 8) | (((__w)&0xFF00) >> 8))
#define SWAP_DWORD(__w) ((((__w)&0x000000FF) << 24) | (((__w)&0x0000FF00) << 8) \
 | (((__w)&0xFF0000) >> 8) | (((__w)&0xFF000000) >> 24)  )

#endif


typedef struct tagBITMAPFILEHEADER {
/*        WORD    bfType;*/
        DWORD   bfSize;
        DWORD   bfReserved;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
   DWORD  biSize; 
   LONG   biWidth; 
   LONG   biHeight; 
   WORD   biPlanes; 
   WORD   biBitCount; 
   DWORD  biCompression; 
   DWORD  biSizeImage; 
   LONG   biXPelsPerMeter; 
   LONG   biYPelsPerMeter; 
   DWORD  biClrUsed; 
   DWORD  biClrImportant; 
} BITMAPINFOHEADER; 

typedef struct tagRGBQUAD { 
    BYTE    rgbBlue; 
    BYTE    rgbGreen; 
    BYTE    rgbRed; 
    BYTE    rgbReserved; 
} RGBQUAD; 

#ifdef XW_PROTOTYPE
XW_STATUS Xw_save_bmp_image_adv (Display *aDisplay,Window aWindow,XWindowAttributes aWinAttr,XImage *aPximage,Colormap aColormap,int aNcolors,char *filename)
#else
XW_STATUS Xw_save_bmp_image_adv (aDisplay,aWindow,aWinAttr,aPximage,aColormap,aNcolors,filename)
Display *aDisplay;
Window aWindow;
XWindowAttributes aWinAttr;
XImage *aPximage;
Colormap aColormap;
int ncolors;
char *filename;
#endif /*XW_PROTOTYPE*/
{
  BYTE* pbData = NULL, *pbCell;
  char *wname = NULL,*pname = NULL ;
  int i,isize,lname,iclass,ncolors,x,y,mult_r=0,mult_g=0,mult_b=0,blue_mask,red_mask,green_mask;
  Visual *pvisual = aWinAttr.visual;
  XColor *pcolors = NULL;
  XWDColor *qcolors = NULL;
  FILE *fimage = NULL ;
  XW_STATUS status = XW_SUCCESS;
#if defined(LIN) || defined(linux)
  char sign[2];
  memcpy(sign,"BM",2);// BM = 0x4D42
  BITMAPFILEHEADER*  bfh=NULL;
  BITMAPINFOHEADER*  bih=NULL;
#else
  WORD sign = SWAP_WORD(0x4D42); /* .BMP file signature: 'MB' */
  BITMAPFILEHEADER  bfh;
  BITMAPINFOHEADER  bih;
#endif

	/* Open BMP File */
  pname = Xw_get_filename (filename, "bmp");
  if (pname)
    fimage = fopen(pname,"w");
  
  if( !fimage ) {	/*ERROR*Bad Filename*/
    Xw_set_error(55,"Xw_save_bmp_image",filename) ;
    return (XW_ERROR) ;
  }

	/* Get Window name */	
  XFetchName(aDisplay,aWindow,&wname);
  
  if (!wname || !strlen (wname))
    wname = (char*) strdup(pname);
  lname = strlen (wname) + 1;
  
  blue_mask = aPximage->blue_mask;
  green_mask = aPximage->green_mask;
  red_mask = aPximage->red_mask;

  iclass = pvisual->c_class;
#ifdef PRO5356
  if( (iclass == PseudoColor) && (aPximage->red_mask > 0) ) iclass = TrueColor;
/*    iclass = (aPximage->depth > 8) ? TrueColor : pvisual->c_class;  GG_240898 */
#endif

  /* Get color table */ 
  switch (iclass) {
    
  case PseudoColor :
    ncolors = aNcolors;
#ifdef DEBUG
    fprintf (stderr, "Xw_save_bmp_file: using pseudocolors (%d)\r\n", ncolors);
#endif
    if ((pcolors = (XColor*) Xw_calloc(ncolors, sizeof(XColor)))) {
      for( i=0 ; i<ncolors ; i++) {
	pcolors[i].pixel = i;
	pcolors[i].pad = 0;
      }
      XQueryColors(aDisplay,aColormap,pcolors,ncolors);
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
    break;


  case TrueColor :  
  case DirectColor :
    if (red_mask && green_mask && blue_mask) {
      
      ncolors = 0;
      mult_r = mult_g = mult_b = 0;
      
      for (i=red_mask; (i&1)^1; i >>= 1)
	mult_r++;
      for (i=green_mask; (i&1)^1; i >>= 1)
	mult_g++;
      for (i=blue_mask; (i&1)^1; i >>= 1)
	mult_b++;
      
      break;
    }
#ifdef DEB    
    fprintf (stderr, "Xw_save_bmp_file: wrong RGB mask! Exiting.\r\n");
#endif
    
    default :  /*ERROR*Unimplemented Image Visual class*/
      Xw_set_error(59,"Xw_save_bmp_image",&iclass) ;
#ifndef PRO16753
    Xw_free(pname) ;
#endif
    Xw_free(wname);
    fclose(fimage) ;
    return (XW_ERROR) ;
  }

  /* Get the size of array of pixels */
  isize = aPximage->width*3; /* actual size of data in each scan line */
  isize += (isize & 3 ? 4 - (isize & 3): 0); /* plus pad size */
  isize *= aPximage->height;

  if (! (pbData = (BYTE*) Xw_malloc(isize))) {

      /*ERROR*Bad Image allocation*/
    Xw_set_error(60,"Xw_save_bmp_image",0) ;
    Xw_free(wname) ;
#ifndef PRO16753
    Xw_free(pname) ;
#endif
    fclose(fimage) ;
    return (XW_ERROR) ;
  }
  
  /* Build and write File Header. */

#if defined(LIN) || defined(linux)
  bfh = (BITMAPFILEHEADER *) Xw_malloc(12); 
  bfh->bfSize      = (DWORD) (2+sizeof (BITMAPFILEHEADER) +
			      sizeof (BITMAPINFOHEADER) +
			      sizeof (RGBQUAD)*0 +
			      isize);        
  bfh->bfReserved = (DWORD) 0;
  bfh->bfOffBits   = (DWORD) (2+sizeof (BITMAPFILEHEADER) +
			      sizeof (BITMAPINFOHEADER) +
			      sizeof (RGBQUAD)*0);
  /*Write file signature */
  if (status && (int)fwrite (sign, 2, 1, fimage) < 1)
    status = XW_ERROR;
  if (status && (int)fwrite (bfh, sizeof(BITMAPFILEHEADER), 1, fimage) < 1)
    status = XW_ERROR;

  Xw_free(bfh);
#else

  bfh.bfSize      = SWAP_DWORD(2+sizeof (BITMAPFILEHEADER) +
			       sizeof (BITMAPINFOHEADER) +
			       sizeof (RGBQUAD)*0 +
			       isize);
  bfh.bfReserved = 0;
  bfh.bfOffBits   = SWAP_DWORD(2+sizeof (BITMAPFILEHEADER) +
                               sizeof (BITMAPINFOHEADER) +
			       sizeof (RGBQUAD)*0);
  /* Write file signature */
  if (status && (int)fwrite ((char *)&sign, 2, 1, fimage) < 1)
    status = XW_ERROR;
  
  if (status && (int)fwrite ((char *)&bfh, sizeof(bfh), 1, fimage) < 1)
    status = XW_ERROR;
#endif
  
#ifdef DEBUG  
  fprintf (stderr, "Xw_save_bmp_file: image depth = %d\r\n", (int)aPximage->depth);
  fprintf (stderr, "Xw_save_bmp_file: bits per pixel = %d\r\n",
	   (int)aPximage->bits_per_pixel);
  fprintf (stderr, "Xw_save_bmp_file: width = %d\r\n", (int)aPximage->width);
  fprintf (stderr, "Xw_save_bmp_file: height = %d\r\n", (int)aPximage->height);
  fprintf (stderr, "Xw_save_bmp_file: raw data = %d bytes\r\n", (int)isize);
#endif

#if defined(LIN) || defined(linux)
  /* Build and write Info Header. */
  bih = (BITMAPINFOHEADER *) Xw_malloc( sizeof(BITMAPINFOHEADER));
  bih->biSize          = sizeof (BITMAPINFOHEADER);
  bih->biWidth	       = aPximage->width;
  bih->biHeight        = aPximage->height;
  bih->biPlanes        = 1;
  bih->biBitCount      = 24; /*8 bits per each of R,G and B value*/
  bih->biCompression   = 0; /*BI_RGB*/
  bih->biSizeImage     = 0; /*can be zero for BI_RGB*/
  bih->biXPelsPerMeter = 0;
  bih->biYPelsPerMeter = 0; 
  bih->biClrUsed       = 0; /*all colors used*/
  bih->biClrImportant  = 0; /*all colors imortant*/
  if (status && (int)fwrite (bih, sizeof(BITMAPINFOHEADER), 1, fimage) < 1)
    status = XW_ERROR;
  Xw_free(bih);
#else
  /* Build and write Info Header. */
  bih.biSize          = SWAP_DWORD(sizeof (BITMAPINFOHEADER));
  bih.biWidth	      = SWAP_DWORD(aPximage->width);
  bih.biHeight        = SWAP_DWORD(aPximage->height);
  bih.biPlanes        = SWAP_WORD(1);
  bih.biBitCount      = SWAP_WORD(24); /*8 bits per each of R,G and B value*/
  bih.biCompression   = 0; /*BI_RGB*/
  bih.biSizeImage     = 0; /*can be zero for BI_RGB*/
  bih.biXPelsPerMeter = 0;
  bih.biYPelsPerMeter = 0; 
  bih.biClrUsed       = 0; /*all colors used*/
  bih.biClrImportant  = 0; /*all colors imortant*/
	
  if (status && (int)fwrite ((char *)&bih, sizeof(bih), 1, fimage) < 1)
    status = XW_ERROR;
#endif	
	
  /* Fill in array of pixels and write out the buffer. */
  pbCell = pbData;
  for (y=aPximage->height-1; y>=0; y--) {
    for (x=0; x<aPximage->width; x++) {
	  
      unsigned long pixel = XGetPixel (aPximage, x, y);
	  
      if (iclass == PseudoColor) {
	*pbCell++ = pcolors[pixel].flags &DoBlue? pcolors[pixel].blue>>8: 0;
	*pbCell++ = pcolors[pixel].flags &DoGreen? pcolors[pixel].green>>8: 0;
	*pbCell++ = pcolors[pixel].flags &DoRed? pcolors[pixel].red>>8: 0;
	/**pbCell++ = 0;*/
      }
      else {
	*pbCell++ = (pixel & blue_mask) >> mult_b;
	*pbCell++ = (pixel & green_mask) >> mult_g;
	*pbCell++ = (pixel & red_mask) >> mult_r;
	/**pbCell++ = 0;*/
      }
    }
      
    x *= 3;
    /* Pad each scan line with zeroes to end on a LONG data-type boundary */
    while (x++ & 0x03) /* i.e. until 4 devides x */
      *pbCell++ = 0;
  }
  
/*
 Here is the long way to get RGB (perhaps, the longest way!)
 that always works! So use this way if any problem occured:

 XColor col;
 RGBQUAD rgbqColor;

 col.pixel = XGetPixel (aPximage, x, y);

 XQueryColor (aDisplay, aColormap, &col);
 *pbCell++ =  col.flags & DoBlue ? col.blue>>8: 0;
 *pbCell++ =  col.flags & DoGreen ? col.green>>8: 0;
 *pbCell++ =  col.flags & DoRed ? col.red>>8: 0;
 *pbCell++ =  0;
*/

  if (status && (int)fwrite(pbData, isize, 1, fimage) < 1)
    status = XW_ERROR;

  /* Finally, store a name of the window. */
  if (status && (int)fwrite(wname, lname, 1, fimage) < 1)
    status = XW_ERROR;
	
  /* close the image file */
  fclose(fimage);
  
#ifndef PRO16753
  Xw_free(pname);
#endif
  Xw_free(wname);
  Xw_free(pbData);
  if (pcolors)
      Xw_free(pcolors);

  return status;
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_save_bmp_image (void *awindow,void *aimage,char *filename)
#else
XW_STATUS Xw_save_bmp_image (awindow,aimage,filename)
void *awindow;
void *aimage;
char *filename ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW*)awindow;
  XW_EXT_IMAGEDATA *pimage = (XW_EXT_IMAGEDATA*)aimage;
  XW_STATUS status = XW_SUCCESS;
  XImage *pximage = NULL;  

  if (!Xw_isdefine_window(pwindow)) { /*ERROR*Bad EXT_WINDOW Address*/
    Xw_set_error(24,"Xw_save_bmp_image",pwindow) ;
    return (XW_ERROR) ;
  }

  if( !Xw_isdefine_image(pimage) ) { /*ERROR*Bad EXT_IMAGEDATA Address*/
   Xw_set_error(25,"Xw_save_bmp_image",pimage) ;
   return (XW_ERROR) ;
  }

  pximage = (_ZIMAGE) ? _ZIMAGE : _IIMAGE;

  status = Xw_save_bmp_image_adv(_DISPLAY,_WINDOW,_ATTRIBUTES,pximage,_COLORMAP->info.colormap,_COLORMAP->maxhcolor,filename);
	
#ifdef  TRACE_SAVE_IMAGE
  if (Xw_get_trace ())
      printf (" %d = Xw_save_bmp_image(%lx,%lx,'%s')\n", 
	      status,(long ) pwindow,(long ) pimage, filename);
#endif
		
  return (status);
}
