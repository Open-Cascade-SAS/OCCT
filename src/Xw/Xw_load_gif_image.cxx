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
 * Created:  zov : 22-Apr-1998  : Loads GIF image from file
 *

   XW_EXT_IMAGEDATA* Xw_load_gif_image (awindow,aimageinfo,fimage,ppcolors,pncolors)
   XW_EXT_WINDOW *awindow
   XW_USERDATA   *aimageinfo
   XColor **ppcolors - address of pointer to array of used colors to return
   int *pncolors - address of a variable to return length of the array in

   Gets an image from a file.
   Note, that the file must be GIF formatted.

   Returns the image descriptor address if successful; or NULL if failed.
*/

#define PURIFY 	/*GG+STT 11/01/99
//		Avoid Free memory leak.
*/

#include <Xw_Extension.h>
#include <fcntl.h>


#define         _ADD_PIXEL(idx)                                      \
{                                                                    \
  if (y < height)  *(pidata + y*width + x) = ((unsigned char)(idx)); \
  if (++x == width) {                                                \
    x = 0;                                                           \
    if (!isInterlace)  ++y;                                          \
    else  switch (pass) {                                            \
    case 0:  y += 8; if (y >= height) ++pass, y = 4; break;          \
    case 1:  y += 8; if (y >= height) ++pass, y = 2; break;          \
    case 2:  y += 4; if (y >= height) ++pass, y = 1; break;          \
    default: y += 2;                                                 \
    }                                                                \
  }                                                                  \
}

/* Note,
** Portions Copyright (c) of CISIGRAPH SOFTWARE.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

XW_EXT_IMAGEDATA *Xw_load_gif_image (void *awindow,void *aimageinfo,char *filename,int fimage,
                         XColor **ppcolors, int *pncolors)
{
//  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW *)awindow;
  XW_EXT_IMAGEDATA *pimage = NULL;
//  XW_STATUS status;
  XImage *pximage = NULL;
  XColor *pcolors = NULL;
  char *wname = NULL, *pidata = NULL;
  unsigned char byte, byte1;
  unsigned *OutCode=NULL, *Prefix=NULL, *Suffix=NULL,
    BitMask, CodeSize, ClearCode, EOFCode, FreeCode, x, y, pass, width, height,
    InitCodeSize, MaxCode, ReadMask, FirstFree, OutCount, BitOffset,
    ByteOffset, Code, CurCode, OldCode=0, FinChar=0, InCode;
  long lFileSize;
//  int i, isize, dataSize, ncolors;
  int i, ncolors;
static  unsigned char *pchFileStream_start = NULL,
                *pchFileStream, *ptr1, *rasterPtr = NULL;
  int isInterlace, hasColormap;


  if (-1 == (lFileSize = lseek (fimage, 0, SEEK_END))
      || NULL == (pchFileStream_start = (unsigned char *) Xw_malloc (lFileSize))
      || 0 != lseek (fimage, 0, SEEK_SET)
      || lFileSize != read (fimage, (char *)pchFileStream_start, lFileSize)
      ) 
    goto _ExitReadError;

  pchFileStream = pchFileStream_start + 10;


  byte = *pchFileStream++; /* Color descriptor byte (M#CR0#BP) */
  hasColormap = byte & 0x80;
  ncolors = hasColormap ? 1<<((byte & 0x07) + 1): 1<<8;
  BitMask = ncolors - 1;

#ifdef DEBUG
  fprintf (stderr, "\r\nXw_load_gif_image: GIF contains %d colors.",
	   hasColormap? ncolors: 0);
#endif /*DEBUG*/

  pchFileStream += 2; /* Skip background byte and following zero byte */

  if (ncolors > 0) { /* Allocate Color Table entries */

    if (! (pcolors = (XColor*) Xw_calloc(ncolors, sizeof(XColor))))
      goto _ExitAllocError;

    for (i=0; i<ncolors; i++) { /* Fill in array of XColor's used */

      /* Note, that if there's no colormap specified then I use green scale */
      pcolors[i].red = ((int)(hasColormap ? *pchFileStream++: i)) << 8; /*map to 0:ffff*/
      pcolors[i].green = ((int)(hasColormap ? *pchFileStream++: i)) << 8;
      pcolors[i].blue = ((int)(hasColormap ? *pchFileStream++: i)) << 8;

      pcolors[i].flags = DoRed|DoGreen|DoBlue;

      pcolors[i].pixel = i;
      pcolors[i].pad = 0;
    }
  }


#ifdef DEBUG
  if (*pchFileStream == '!')
    fprintf (stderr, "\r\nXw_load_gif_image: GIF contains extension blocks"
	     " (code: %x).", (int)*(pchFileStream + 1));
  else 
    fprintf (stderr, "\r\nXw_load_gif_image: GIF has no extension blocks.");
#endif /*DEBUG*/


  /* Skip extension blocks if any.
   * Format:  <'!'><size><...><size><...><0>
   */
  while (*pchFileStream == '!') {

    pchFileStream += 2;  /* skip code byte followed '!' sign */

    while (*pchFileStream)
      pchFileStream += (unsigned)(1 + *(unsigned char *)pchFileStream);

    pchFileStream ++;
  }
 


  if (*pchFileStream++ != ',') { /* must be an image descriptor */

#ifdef DEB
    fprintf (stderr, "\r\nXw_load_gif_image: Error: There's no separator"
	     " following the colormap");
#endif /*DEB*/

    goto _ExitReadError;
  }


  pchFileStream += 2*2; /* Skip image left & top offsets*/
  width = (unsigned) *pchFileStream++;
  width += ((unsigned)*pchFileStream++) << 8;
  height = (unsigned) *pchFileStream++;
  height += ((unsigned)*pchFileStream++) << 8;

    
  byte = *pchFileStream++;

  isInterlace = byte & 0x40;

  if (byte & 0x80) {

    fprintf (stderr, "\r\nXw_load_gif_image:"
	     " Can't read GIF image with locally defined colormap!\r\n");
    goto _ExitReadError;
  }

  
#ifdef DEBUG
    fprintf (stderr, "\r\nXw_load_gif_image: Dimensions: %dx%d"
	     "  Interlace mode: %s",
	     width, height, isInterlace? "ON": "OFF");
#endif /*DEBUG*/



  /* Allocate the pixel buffer */
  if (! (rasterPtr = (unsigned char *) Xw_malloc(lFileSize)))
    goto _ExitAllocError;


  OutCode = (unsigned *)Xw_malloc(1025 * sizeof (unsigned));
  Prefix  = (unsigned *)Xw_malloc(4096 * sizeof (unsigned));
  Suffix  = (unsigned *)Xw_malloc(4096 * sizeof (unsigned));

  if (!OutCode || !Prefix || !Suffix)
    goto _ExitAllocError;



  /* Decode compressed raster data. */
  CodeSize  = *pchFileStream++;
  ClearCode = 1 << CodeSize;
  EOFCode   = ClearCode + 1;
  FreeCode  = FirstFree = EOFCode + 1;

  ++CodeSize;

  InitCodeSize = CodeSize;
  MaxCode      = 1 << CodeSize;
  ReadMask     = MaxCode - 1;

  ptr1 = rasterPtr;

  /* Read encoded data to a continuous array pointed to by rsterPtr */
  do {
    byte = byte1 = *pchFileStream++;

    while (byte--)
      *ptr1++ = *pchFileStream++;

    if ((long) (ptr1 - rasterPtr) > lFileSize)  /* Currupt file */
      goto _ExitReadError;
  }
  while (byte1);

  Xw_free (pchFileStream_start); /* The file data has been already read */
  pchFileStream_start = NULL;



  if (! (pidata = (char *) Xw_malloc (width*height)))
    goto _ExitAllocError;


  x = y = pass = OutCount = BitOffset = ByteOffset = 0;

  /* Fetch the next code (3 to 12 bits) from the raster data stream */
  Code = rasterPtr[0] + (((unsigned) rasterPtr[1]) << 8);
  if (CodeSize >= 8)
    Code += ((unsigned) rasterPtr[2]) << 16;

  Code >>= BitOffset & 0x7;
  BitOffset += CodeSize;
  Code      &= ReadMask;


  while (Code != EOFCode) {

  
    if (Code == ClearCode) {
   
      /* Read the next code */
      CodeSize   = InitCodeSize;
      MaxCode    = 1 << CodeSize;
      ReadMask   = MaxCode - 1;
      FreeCode   = FirstFree;
      ByteOffset = BitOffset >> 3;
      Code       = rasterPtr[ByteOffset];

      Code       = rasterPtr[ByteOffset]
	           + (((unsigned) rasterPtr[ByteOffset + 1]) << 8);
      if (CodeSize >= 8)
	Code += ((unsigned) rasterPtr[ByteOffset + 2]) << 16;

      Code      >>= BitOffset & 0x7;
      BitOffset +=  CodeSize;
      Code      &=  ReadMask;

      CurCode = OldCode = Code;
      FinChar = CurCode & BitMask;

      _ADD_PIXEL (FinChar);
    }
    else {
   
      CurCode = InCode = Code;

      if (CurCode >= FreeCode) {
	
	CurCode = OldCode;
	OutCode[OutCount++] = FinChar;
      }

      while (CurCode > BitMask) {
	
	if (OutCount > 1024)
	  goto _ExitReadError;

	OutCode [OutCount++] = Suffix [CurCode];
	CurCode = Prefix [CurCode];
      }

      FinChar = CurCode & BitMask;
      OutCode [OutCount++] = FinChar;

      for (i = OutCount - 1; i >= 0; --i)
	_ADD_PIXEL (OutCode [i]);

      OutCount = 0;
      Prefix [FreeCode] = OldCode;
      Suffix [FreeCode] = FinChar;
      OldCode = InCode;
      ++FreeCode;

      if (FreeCode >= MaxCode) {
	
	if (CodeSize < 12) {
     
	  ++CodeSize;
	  MaxCode <<= 1;
	  ReadMask = (1 << CodeSize) - 1;
    	}
      }
    }

    ByteOffset = BitOffset >> 3;
    Code = (unsigned) rasterPtr[ByteOffset];
    Code = (unsigned) rasterPtr[ByteOffset]
           + (((unsigned) rasterPtr[ByteOffset + 1]) << 8);
    if (CodeSize >= 8)
      Code += ((unsigned) rasterPtr[ByteOffset + 2]) << 16;

    Code >>= (BitOffset & 0x7);
    BitOffset += CodeSize;
    Code &=  ReadMask;
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
  _IIMAGE->xoffset = (int) 0;
  _IIMAGE->format = (int) ZPixmap;
  _IIMAGE->byte_order = (int) LSBFirst;
  _IIMAGE->bitmap_unit = (int) 8;
  _IIMAGE->bitmap_bit_order = (int) LSBFirst;
  _IIMAGE->bitmap_pad = (int) 8;
  _IIMAGE->depth = (int) 24;
  _IIMAGE->bits_per_pixel = (int) 8;
  _IIMAGE->bytes_per_line = (int) width;
  _IIMAGE->red_mask = 0;
  _IIMAGE->green_mask = 0;
  _IIMAGE->blue_mask = 0;
  _IIMAGE->obdata = NULL ;
  _XInitImageFuncPtrs (_IIMAGE);


  /* Return array of colors */
  *ppcolors = pcolors;
  *pncolors = ncolors;

  if (wname) Xw_free(wname);


#ifdef DEBUG
  fprintf (stderr, "\r\nXw_load_gif_image: Operation completed successfully.\r\n");
#endif /*DEBUG*/


  /* Successfully loaded */

#ifdef PURIFY
  if (rasterPtr) Xw_free (rasterPtr);
  if (OutCode) Xw_free (OutCode);
  if (Prefix) Xw_free (Prefix);
  if (Suffix) Xw_free (Suffix);
#endif
  return (pimage);



_ExitReadError:

  /*ERROR*Unable to read Image data
  Xw_set_error(61,"Xw_load_gif_image",filename);
  */
  fprintf (stderr, "\r\nXw_load_gif_image: Error reading %s!", filename);
  goto _ExitError;


_ExitAllocError:
  /*ERROR*Bad Image allocation
  Xw_set_error(60,"Xw_load_gif_image",0);
  */
  fprintf (stderr, "\r\nXw_load_gif_image: Error: Out of memory!");


_ExitError:

#ifdef DEB
  fprintf (stderr, "\r\nXw_load_gif_image: An error occured! Returning NULL.");
#endif /*DEB*/

  if (pchFileStream_start) Xw_free (pchFileStream_start);
  if (pximage) Xw_free (pximage);
  if (rasterPtr) Xw_free (rasterPtr);
  if (pidata) Xw_free (pidata);
  if (pimage) Xw_free (pidata);
  if (pcolors) Xw_free (pcolors);
  if (OutCode) Xw_free (OutCode);
  if (Prefix) Xw_free (Prefix);
  if (Suffix) Xw_free (Suffix);
  
  return (NULL);
}

#undef _ADD_PIXEL
