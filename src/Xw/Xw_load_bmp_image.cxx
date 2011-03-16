/*
 * Created:  zov : 23-Apr-1998  : Loads BMP image from file
 *

   XW_EXT_IMAGEDATA* Xw_load_bmp_image (awindow,aimageinfo,fimage,ppcolors,pncolors)
   XW_EXT_WINDOW *awindow
   XW_USERDATA   *aimageinfo
   XColor **ppcolors - address of pointer to array of used colors to return
   int *pncolors - address of a variable to return length of the array in

   Gets an image from a file.
   Note, that the file must be BMP formatted.

   Returns the image descriptor address if successful; or NULL if failed.
*/

#include <Xw_Extension.h>
#include <fcntl.h>


typedef unsigned int   DWORD; /* 32-bit signed */
typedef int            LONG;  /* 32-bit unsigned */
typedef unsigned short WORD;  /* 16-bit unsigned */

typedef struct tagBITMAPFILEHEADER {
/*        WORD    bfType;*/
        DWORD   bfSize;
        DWORD   bfReserved;
        DWORD   bfOffBits;
	DWORD   dwHeaderSize;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
/*   DWORD  biSize; */
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

/* constants for the biCompression field */
#define BI_RGB        0
#define BI_RLE8       1
#define BI_RLE4       2
#define BI_BITFIELDS  3


typedef struct tagBITMAPCOREHEADER {
/*        DWORD   bcSize; */
        WORD    bcWidth; 
        WORD    bcHeight; 
        WORD    bcPlanes;   /* 1 */
        WORD    bcBitCount; /* 1,4,8 or 24 */
} BITMAPCOREHEADER; 


typedef struct tagRGBQUAD { 
    BYTE    rgbBlue; 
    BYTE    rgbGreen; 
    BYTE    rgbRed; 
    BYTE    rgbReserved; 
} RGBQUAD; 


#define LOW_VAL_AT_LOW_ADDR (*(char*)&__swaptest)
static unsigned long __swaptest = 1;

static WORD _TestSwapWORD (WORD __w)
{
  return LOW_VAL_AT_LOW_ADDR ? __w:
  ((__w&0xFF) << 8) | ((__w&0xFF00) >> 8);
}

static DWORD _TestSwapDWORD (DWORD __dw)
{
  return LOW_VAL_AT_LOW_ADDR ? __dw:
  ((__dw&0x000000FF) << 24) | ((__dw&0x0000FF00) << 8)
    | ((__dw&0xFF0000) >> 8)  |  ((__dw&0xFF000000) >> 24);
}


XW_EXT_IMAGEDATA* Xw_load_bmp_image(void *awindow, void *aimageinfo, char *filename,
			int fimage, XColor **ppcolors, int *pncolors)
{
//  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW *)awindow;
  XW_EXT_IMAGEDATA *pimage = NULL;
//  XW_STATUS status;
  XImage *pximage = NULL;
  XColor *pcolors = NULL;
  char *wname = NULL, *pidata = NULL;
//  BYTE byte;
  int i, isize, ncolors, bytes_per_line, x, y, width, height;
  int hasColormap, hasMask, isInsideOut, isOS2Format;

  BITMAPFILEHEADER bmfh;     /* Standard BMP file header */
  BITMAPINFOHEADER bmih;     /* Windows BMP header */
  BITMAPCOREHEADER bmch;     /* OS/2 BMP header */
  RGBQUAD colors256 [256];   /* color table (up to 256 entries) */
  BYTE *pData, *pbData=NULL; /* pointer to bitmap pixels */
  BYTE *ptrByte;

  DWORD dwRMask, dwGMask, dwBMask;
  int   nRMult, nGMult, nBMult;
  int   nBitCount, iCompression;


  if (2 != lseek (fimage, 2, SEEK_SET) /* skip 'BM' signature */
   || sizeof (bmfh) != read (fimage, (char *)&bmfh, sizeof (bmfh)))
   goto _ExitReadError;


  /* Detect format using the size field (.biSize or .bcSize) */
  isOS2Format = _TestSwapDWORD (bmfh.dwHeaderSize) == sizeof (bmch) + 4;
  if (!isOS2Format && sizeof (bmih) != read (fimage, (char *)&bmih, sizeof (bmih))
    || isOS2Format && sizeof (bmch) != read (fimage, (char *)&bmch, sizeof (bmch)))
    goto _ExitReadError;


  nBitCount = _TestSwapWORD (isOS2Format? bmch.bcBitCount: bmih.biBitCount);
  if (nBitCount != 1 && nBitCount != 4 && nBitCount != 8
      && nBitCount != 16 && nBitCount != 24 && nBitCount != 32
      || isOS2Format && (nBitCount == 16 || nBitCount == 32)) {
#ifdef DEB
    fprintf (stderr, "\r\nXw_load_bmp_image: "
	   "Error: Wrong count of bits per pixel (%d) specified!", nBitCount);
#endif /*DEB*/
    goto _ExitReadError;
  }

  iCompression = isOS2Format? BI_RGB: _TestSwapDWORD (bmih.biCompression);
  hasColormap = nBitCount <= 8;
  hasMask = iCompression == BI_BITFIELDS;
  ncolors = hasColormap ? 1 << nBitCount: 0;
  width = isOS2Format ? _TestSwapWORD (bmch.bcWidth)
                      : _TestSwapDWORD (bmih.biWidth);
  height = isOS2Format ? _TestSwapWORD (bmch.bcHeight)
                      : _TestSwapDWORD (bmih.biHeight);
  isInsideOut = !isOS2Format && height < 0;
  height = isInsideOut? -height: height;
  bytes_per_line = width * nBitCount;
  bytes_per_line = (bytes_per_line>>3) + (bytes_per_line&0x7 ? 1: 0);
  bytes_per_line += (bytes_per_line&0x3)? (4-(bytes_per_line&0x3)): 0;

  if (hasColormap) {

    /* Retreive array of colors (as RGBQUAD sequence). */
    /* Note, that there can be less then ncolors colors. */
    isize = ncolors * sizeof (RGBQUAD);
    if (0 >= read (fimage, (char *)&colors256, isize))
      goto _ExitReadError;
   
    if (! (pcolors = (XColor*) Xw_calloc(ncolors, sizeof(XColor))))
      goto _ExitAllocError;

    for (i=0; i<ncolors; i++) { /* Fill in array of XColor's used */

      pcolors[i].red = ((unsigned) colors256[i].rgbRed) << 8; /*map to 0:ffff*/
      pcolors[i].green = ((unsigned) colors256[i].rgbGreen) << 8;
      pcolors[i].blue = ((unsigned) colors256[i].rgbBlue) << 8;

      pcolors[i].flags = DoRed|DoGreen|DoBlue;

      pcolors[i].pixel = i;
      pcolors[i].pad = 0;
    }
  }


  if (hasMask) {
    
    /* Retrieve three DWORD's that contain the masks for R, G and B respectively */
   if (4 != read (fimage, (char *)&dwRMask, sizeof (DWORD))
    || 4 != read (fimage, (char *)&dwGMask, sizeof (DWORD))
    || 4 != read (fimage, (char *)&dwBMask, sizeof (DWORD))
    || hasColormap || !dwRMask || !dwGMask || !dwBMask)
    goto _ExitReadError;

    
   dwRMask = _TestSwapDWORD (dwRMask);
   dwGMask = _TestSwapDWORD (dwGMask);
   dwBMask = _TestSwapDWORD (dwBMask);


   nRMult = nGMult = nBMult = 0;

   for (i=dwRMask; (i&1)^1; i >>= 1)
     nRMult++;
   for (i=dwGMask; (i&1)^1; i >>= 1)
     nGMult++;
   for (i=dwBMask; (i&1)^1; i >>= 1)
     nBMult++;
  }
  else
    dwBMask = 0x000000ff, dwGMask = 0x0000ff00, dwRMask = 0x00ff0000,
    nBMult = 0, nGMult = 8, nRMult = 16;


 
#ifdef DEBUG
    fprintf (stderr, "\r\nXw_load_bmp_image: Format: %s  Dimensions: %dx%d"
	     "  BitCount: %d  Compression: %d", isOS2Format? "OS/2": "Windows",
	     width, height, nBitCount, iCompression);
#endif /*DEBUG*/



  /* Allocate the pixel buffer and load raw data from file */
  i = _TestSwapDWORD (bmfh.bfOffBits);
  isize = _TestSwapDWORD (bmfh.bfSize) - i;
  if (! (pData = (BYTE *) Xw_malloc(isize)))
    goto _ExitAllocError;

  pbData = pData;


  if (i != lseek (fimage, i, SEEK_SET)
      || isize != read (fimage, (char *)pData, isize))
    goto _ExitReadError;



  if (iCompression == BI_RLE4 || iCompression == BI_RLE8) {

     if (! (pbData = (BYTE *) Xw_malloc(width*height))) {

       pbData = pData;
       goto _ExitAllocError;
     }

     /* Note, that it is possible that "spots" could appear
      *	after decompressing the data encoded using RLE algorith.
      * The method assumes that the image is drawn to an already
      * defined background. That's why I have to fill in these
      * probable spots with some color (I have chosen 0-indexed one).
      */
     memset (pbData, 0, width*height);


     /* Decompress the data to array of 8-bit color indices */
     x = 0;
     y = 0;

     ptrByte = pData;

     for (;;) {

       BYTE bPixCount = *ptrByte++;
       BYTE bCode, bColor;

       if (bPixCount) {
	 
	 bColor = *ptrByte++;
	 
	 /*fprintf (stderr, "[%02x S]", (int)bPixCount);*/
	 if (iCompression == BI_RLE8)
	   while (bPixCount--)
	     pbData [width*y + x++] = bColor;

	 else {
	   BYTE bColor0 = (bColor & 0xf0) >> 4;
	   BYTE bColor1 =  bColor & 0x0f;

	   i = 0;
	   while (bPixCount--)
	     pbData [width*y + x++] = i++ & 1 ? bColor1: bColor0;
	 }
	 
	 continue;
       }

       
       /* Zero code has been reached. */
       /* Do the command specified in the second byte. */
       bCode = *ptrByte++;

       if (!bCode) /* EOL */
	 x = 0, y++ /*,fprintf (stderr, "\r\n")*/;

       else if (bCode == 1) /* EOF */
	 break;

       else if (bCode == 2) {  /* Delta */

	 x += (unsigned) *ptrByte++;
	 y += (unsigned) *ptrByte++;
	 /*fprintf (stderr, "[POS=%d,%d]", x, y);*/
       }
       else { /* Absolute mode */

	 bPixCount = bCode;

	 /*fprintf (stderr, "[%02x D]", (int)bPixCount);*/
	 if (iCompression == BI_RLE8)
	   while (bPixCount--)
	     pbData [width*y + x++] = *ptrByte++;

	 else {
	   i = 0;
	   while (bPixCount--)
	     pbData [width*y + x++] = i++ & 1 ?  *ptrByte & 0x0f
		                              : (*ptrByte++ & 0xf0) >> 4;
	 }


	 /* each run must aligned on a word boundary */
	 if (iCompression == BI_RLE8 && (bCode & 1)
	     || iCompression == BI_RLE4 && (bCode & 3))
	   ptrByte ++;
       }
     }

     
     bytes_per_line = width;
     nBitCount = 8;

     Xw_free (pData);
  }
  /* else *  data isn't comressed. Can just use the raw data */


  /* Alloc image data (pidata) and fill in the buffer with RGBQUAD's */
  isize = width*height;
  if (! (pidata = (char *) Xw_malloc (hasColormap? isize: isize*4)))
    goto _ExitAllocError;

  x = 0;
  y = isInsideOut? 0: height-1;

  for (i=0; i<isize; i++) {
    
    DWORD dwValue=0;
    BYTE* pbLine = pbData + y*bytes_per_line;


    switch (nBitCount) {

    case 32:
      dwValue = *(((DWORD *) pbLine) + x); break; /* next double word value */
    case 16:
      dwValue = *(((WORD *) pbLine) + x); break; /* next word */
    case 8:
      dwValue = *(((BYTE *) pbLine) + x); break; /* next byte */
    case 24:
      { BYTE* pbTripple = pbLine + x+(x<<1);
	dwValue = *pbTripple + (*(pbTripple +1) <<8) + (*(pbTripple +2) <<16);
      }
      break;
    case 4:
      dwValue = x&1 ? *(pbLine+(x>>1))&0x0f: (*(pbLine+(x>>1))&0xf0) >> 4; break;
    case 1:
      {	int iBit = 7 - (x&7);
	dwValue = (*(pbLine+(x>>3)) & (1 <<iBit))  >>iBit;
      } break; /* next bit */
    }

    if (hasColormap)
      *(((BYTE *) pidata) + i) = (BYTE) dwValue; /* put index into colormap */
    else
      *(((DWORD *) pidata) + i) = (DWORD) dwValue; /* put RGB */


    if (++x == width) {

      x = 0;
      y += isInsideOut? 1: -1;
    }
  }
  

  /* Allocate the Image structure */
  if (! (pximage = (XImage*) Xw_malloc(sizeof(XImage))))
    goto _ExitAllocError;

  /* Allocate the Extended Image structure */
  if(! (pimage = Xw_add_imagedata_structure (sizeof(XW_EXT_IMAGEDATA))))
    goto _ExitError;


  /* Initialize the input image */
  pimage->pimageinfo = aimageinfo;
  _IIMAGE = pximage;
  _IIMAGE->data = pidata;
  _IIMAGE->width = (int) width;
  _IIMAGE->height = (int) height;
  _IIMAGE->xoffset = 0;
  _IIMAGE->format = (int) ZPixmap;
  _IIMAGE->byte_order = (int) LSBFirst;
  _IIMAGE->bitmap_unit = hasColormap? 8: 32;
  _IIMAGE->bitmap_bit_order = (int) LSBFirst;
  _IIMAGE->bitmap_pad = hasColormap? 8: 32;
  _IIMAGE->depth = (int) 24;
  _IIMAGE->bits_per_pixel = hasColormap? 8: 32;
  _IIMAGE->bytes_per_line = hasColormap? width: width*4;
  _IIMAGE->red_mask = hasColormap? 0: dwRMask;
  _IIMAGE->green_mask = hasColormap? 0: dwGMask;
  _IIMAGE->blue_mask = hasColormap? 0: dwBMask;
  _IIMAGE->obdata = NULL;
  _XInitImageFuncPtrs (_IIMAGE);


  /* Return array of colors */
  *ppcolors = pcolors;
  *pncolors = ncolors;

  if (wname) Xw_free (wname);
  if (pbData) Xw_free (pbData);


#ifdef DEBUG
  fprintf (stderr, "\r\nXw_load_bmp_image: Operation completed successfully.\r\n");
#endif /*DEBUG*/


  /* Successfully loaded */
  return (pimage);



_ExitReadError:
  fprintf (stderr, "\r\nXw_load_bmp_image: Error reading %s!", filename);
  goto _ExitError;


_ExitAllocError:
  fprintf (stderr, "\r\nXw_load_bmp_image: Error: Out of memory!");


_ExitError:

#ifdef DEB
  fprintf (stderr, "\r\nXw_load_bmp_image: An error occured! Returning NULL.");
#endif /*DEB*/

  if (pbData) Xw_free (pbData);
  if (pximage) Xw_free (pximage);
  if (pidata) Xw_free (pidata);
  if (pimage) Xw_free (pidata);
  if (pcolors) Xw_free (pcolors);
  
  return (NULL);
}
