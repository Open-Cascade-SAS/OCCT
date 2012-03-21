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
 * Created:  zov : 17-Apr-1998  : Loads XWD image from file
 * Note:     Adapted from Xw_load_image.c
 *

   XW_EXT_IMAGEDATA* Xw_load_xwd_image (awindow,aimageinfo,fimage,ppcolors,pncolors)
   XW_EXT_WINDOW *awindow
   XW_USERDATA   *aimageinfo
   XColor **ppcolors - address of pointer to array of used colors to return
   int *pncolors - address of a variable to return length of the array in

   Gets an image from a file.
   Note, that the file must be XWD formatted.

   Returns the image descriptor address if successful; or NULL if failed.
*/

#include <Xw_Extension.h>
#include <fcntl.h>


#define SWAPTEST (*(char*)&swaptest)
static unsigned long swaptest = 1;

static void SwapLong (register char* bp,register unsigned n)
/*
    Swap long ints depending on CPU
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

static void SwapShort (register char* bp, register unsigned n)
/*
    Swap short ints depending on CPU
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

XW_EXT_IMAGEDATA* Xw_load_xwd_image (void *awindow,void *aimageinfo,char *filename,int fimage,
                         XColor **ppcolors, int *pncolors)
{
//  XW_EXT_WINDOW *pwindow = (XW_EXT_WINDOW *)awindow;
  XW_EXT_IMAGEDATA *pimage;
//  XW_STATUS status;
  XWDFileHeader *pheader;
  XImage *pximage;
  XColor *pcolors = NULL;
  char *wname = NULL, *pidata;
  int i, isize;


  isize = sizeof (XWDFileHeader);
  if (!(pheader = (_xwd_file_header*) Xw_malloc (isize))) {
    /*ERROR*Bad Image allocation*/
    Xw_set_error (60,"Xw_load_xwd_image",0) ;
    return (NULL) ;
  }

  if (read(fimage,pheader,isize) != isize) {
    /*ERROR*Unable to read XWD file header*/
    Xw_set_error (56,"Xw_load_xwd_image",filename);
    Xw_free (pheader);
    return (NULL);
  }

  if (SWAPTEST) SwapLong ((char*) pheader, isize);

  /* Is it XWD file format ? */
  if (pheader->file_version != XWD_FILE_VERSION) {
    /*ERROR*Bad XWD file format*/
    Xw_set_error (57,"Xw_load_xwd_image",filename);
    Xw_free (pheader);
    return (NULL);
  }

  if (pheader->header_size < (unsigned int ) isize) {
    /*ERROR*Bad XWD file format*/
    Xw_set_error (57,"Xw_load_xwd_image",filename);
    Xw_free (pheader);
    return (NULL);
  }

  /* Check for image format */
  if (pheader->pixmap_format != ZPixmap) {
    /*ERROR*Bad XWD Pixmap format*/
    Xw_set_error (58,"Xw_load_xwd_image",filename);
    Xw_free (pheader);
    return (NULL);
  }

  /* Read a window name */
  isize = pheader->header_size - isize ;
  if (isize > 0) {

    if (! (wname = (char*) Xw_malloc(isize))) {
      /*ERROR*Bad Image allocation*/
      Xw_set_error(60,"Xw_load_xwd_image",0);
      Xw_free(pheader);
      return (NULL);
    }

    if(read (fimage, wname, isize) != isize) {
      /*ERROR*Unable to read Image data*/
      Xw_set_error(61,"Xw_load_xwd_image",filename) ;
      Xw_free (wname);
      Xw_free (pheader);
      return (NULL) ;
    }
  }

  /* Allocate the Image structure */
  if (! (pximage = (XImage*) Xw_malloc(sizeof(XImage)))) {
    /*ERROR*Bad Image allocation*/
    Xw_set_error(60,"Xw_load_xwd_image",0);
    if (wname) Xw_free(wname);
    Xw_free(pheader) ;
    return (NULL);
  }

  if (pheader->ncolors > 0) {
    /* Allocate the Color Table entries */
    if (! (pcolors = (XColor*) Xw_calloc(pheader->ncolors, sizeof(XColor)))) {
      /*ERROR*Bad Image allocation*/
      Xw_set_error (60,"Xw_load_xwd_image",0);
      if (wname) Xw_free(wname);
      Xw_free (pximage);
      Xw_free (pheader);
      return (NULL);
    }

    /* Read the Color Table entries */
    isize = sizeof(XWDColor) * pheader->ncolors;
    if (read (fimage, (char *)pcolors, isize) != isize) {
      /*ERROR*Unable to read Image data*/
      Xw_set_error(61,"Xw_load_xwd_image",filename);
      if (wname) Xw_free(wname);
      Xw_free (pximage);
      Xw_free (pcolors);
      Xw_free (pheader);
      return (NULL) ;
    }

    /* 32->64 bits conversion if any*/
    if (sizeof(XColor) > sizeof(XWDColor)) {
      XWDColor *qcolors = (XWDColor*) pcolors ;
      for( i=pheader->ncolors-1 ; i>=0 ; --i ) {
	pcolors[i].pad   = qcolors[i].pad;
	pcolors[i].flags = qcolors[i].flags;
	pcolors[i].blue  = qcolors[i].blue;
	pcolors[i].green = qcolors[i].green;
	pcolors[i].red   = qcolors[i].red;
	pcolors[i].pixel = qcolors[i].pixel;
      }
    }

    if (SWAPTEST) {
      for (i=0 ; (unsigned int ) i<pheader->ncolors ; i++) {
	SwapLong((char*) &pcolors[i].pixel,sizeof(long));
	SwapShort((char*) &pcolors[i].red,3*sizeof(short));
      }
    }
  } /* end if # colors > 0 */


  /* Allocate the pixel buffer */
  isize = pheader->bytes_per_line*pheader->pixmap_height;
  if (! (pidata = (char *) Xw_malloc(isize))) {
    /*ERROR*Bad Image allocation*/
    Xw_set_error (60,"Xw_load_xwd_image",0);
    if (wname) Xw_free(wname);
    Xw_free (pximage);
    if (pcolors) Xw_free(pcolors);
    Xw_free(pheader);
    return (NULL);
  }

  /* Read the pixel buffer */
  if (read (fimage,pidata,isize) != isize) {
    /*ERROR*Unable to read Image data*/
    Xw_set_error (61,"Xw_load_xwd_image",filename);
    if (wname) Xw_free(wname);
    Xw_free (pidata);
    Xw_free (pximage);
    if (pcolors) Xw_free (pcolors);
    Xw_free (pheader);
    return (NULL);
  }


  /* Allocate the Extended Image structure */
  if(! (pimage = Xw_add_imagedata_structure(sizeof(XW_EXT_IMAGEDATA)))) {
    if (wname) Xw_free(wname);
    Xw_free(pidata);
    Xw_free(pximage);
    if (pcolors) Xw_free(pcolors);
    Xw_free(pheader);
    return (NULL);
  }

  /* Initialize the input image */
  pimage->pimageinfo = aimageinfo;
  _IIMAGE = pximage;
  _IIMAGE->data = pidata;
  _IIMAGE->width = (int) pheader->pixmap_width;
  _IIMAGE->height = (int) pheader->pixmap_height;
  _IIMAGE->xoffset = (int) pheader->xoffset;
  _IIMAGE->format = (int) pheader->pixmap_format;
  _IIMAGE->byte_order = (int) pheader->byte_order;
  _IIMAGE->bitmap_unit = (int) pheader->bitmap_unit;
  _IIMAGE->bitmap_bit_order = (int) pheader->bitmap_bit_order;
  _IIMAGE->bitmap_pad = (int) pheader->bitmap_pad;
  _IIMAGE->depth = (int) pheader->pixmap_depth;
  _IIMAGE->bits_per_pixel = (int) pheader->bits_per_pixel;
  _IIMAGE->bytes_per_line = (int) pheader->bytes_per_line;
  _IIMAGE->red_mask = pheader->red_mask;
  _IIMAGE->green_mask = pheader->green_mask;
  _IIMAGE->blue_mask = pheader->blue_mask;
  _IIMAGE->obdata = NULL ;
  _XInitImageFuncPtrs (_IIMAGE);


  /* Return array of colors */
  *ppcolors = pcolors;
  *pncolors = pheader->ncolors;

  if (wname) Xw_free(wname);
  if (pheader) Xw_free(pheader);


  return (pimage);
}
