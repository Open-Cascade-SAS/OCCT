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

#ifdef TRACE
# define TRACE_DRAW_ZOOMED_IMAGE
#endif

/*
  STATUS Xw_draw_zoomed_image (awindow,aimage,xc,yc,zoom):
  XW_EXT_WINDOW *awindow       Drawable
  XW_EXT_IMAGEDATA *aimage     Image structure
  float xc,yc                  Image center location in user space
  float zoom                   Image zoom factor

    Draw an image to the window at the required Center location
    with required zoom factor.

    returns ERROR if NO Image is defined or Image is outside of the window
    returns SUCCESS if successful
*/

#ifdef XW_PROTOTYPE
XW_STATUS Xw_draw_zoomed_image (void* awindow, void* aimage, float xc, float yc, float zoom)
#else
XW_STATUS Xw_draw_zoomed_image (awindow, aimage, xc, yc, zoom)
void *awindow;
void *aimage;
float xc, yc, zoom;
#endif // XW_PROTOTYPE
{
  XW_EXT_WINDOW    *pwindow = (XW_EXT_WINDOW*)awindow;
  XW_EXT_IMAGEDATA *pimage  = (XW_EXT_IMAGEDATA*)aimage;
//  XW_EXT_BUFFER    *pbuffer;
  XImage           *pximage, *qximage;
  int              wWidth, wHeight, xC, yC, wp, hp, dxp, dyp, isize;
  int              x, y, xp, yp, xn, yn, xq, yq, nxC, nyC;
  unsigned long    pixel;
  float            ratio;

#ifdef TRACE_DRAW_ZOOMED_IMAGE
  if (Xw_get_trace ())
    printf ("Xw_draw_zoomed_image(%lx, %lx, xc=%f, yc=%f, zoom=%f)\n",(long ) pwindow,(long ) pimage,xc,yc,zoom);
#endif // TRACE_DRAW_ZOOMED_IMAGE

  if (!Xw_isdefine_window(pwindow)) {
    // ERROR*Bad EXT_WINDOW Address
    Xw_set_error(24,"Xw_draw_zoomed_image",pwindow);
    return (XW_ERROR);
  }

  if (!Xw_isdefine_image(pimage)) {
    // ERROR*Bad EXT_IMAGEDATA Address
    Xw_set_error(25,"Xw_draw_zoomed_image",pimage);
    return (XW_ERROR);
  }

  if (zoom <= 0.) {
    // ERROR*Bad Image Zoom factor
    Xw_set_error(113,"Xw_draw_zoomed_image",&zoom) ;
    return (XW_ERROR);
  }

  if (Xw_get_window_size(pwindow, &wWidth, &wHeight) != XW_SUCCESS) {
    // ERROR*Get Window Size
    printf (" ***Xw_draw_zoomed_image : BAD call to Xw_get_window_size()\n");
    return (XW_ERROR);
  }
  printf ("\nXw_draw_zoomed_image info -> window size (%d x %d)\n", wWidth, wHeight);

//OCC186
  xC = PXPOINT (xc, pwindow->xratio);
  yC = PYPOINT (yc, pwindow->attributes.height, pwindow->yratio);
//OCC186
  printf ("Xw_draw_zoomed_image info -> image center (%d, %d)\n", xC, yC);

  pximage = pimage->pximage ;
  qximage = (XImage*) Xw_malloc(sizeof(XImage));

  if (!qximage) {
    // ERROR*XImage Allocation failed
    Xw_set_error (60, "Xw_draw_zoomed_image",NULL);
    return (XW_ERROR);
  }

  Xw_bytecopy ((char*)pximage, (char*)qximage, sizeof(XImage));

  wp = pximage->width;
  hp = pximage->height;
  dxp = dyp = 0;
  ratio = (float) wp/hp;
  qximage->width  = (int )( wp * zoom );
  qximage->height = (int )( hp * zoom );
  nxC = wWidth  / 2;
  nyC = wHeight / 2;

  if (qximage->width > wWidth || qximage->height > wHeight) {
//    int rw = qximage->width, rh = qximage->height;

    // Set new image size
#ifdef DEBUG
    printf (" ***Xw_draw_zoomed_image (%f). Image size is too big (%d x %d)\n",zoom,qximage->width,qximage->height);
#endif
    if (qximage->width  > wWidth ) qximage->width  = wWidth;
    if (qximage->height > wHeight) qximage->height = wHeight;
#ifdef DEBUG
    printf ("Xw_draw_zoomed_image info -> image will be truncated to (%d x %d)\n",qximage->width,qximage->height);
#endif

    // Part of an original image to be zoomed (size of it)
    wp = (int )( qximage->width  / zoom + 1);
    hp = (int )( qximage->height / zoom + 1);

    // Offset inside the original image
    dxp = (int )( (pximage->width -  wp) / 2 - (xC - qximage->width  / 2) / zoom );
    dyp = (int )( (pximage->height - hp) / 2 - (yC - qximage->height / 2) / zoom );

    // Correct image size, offset and center
    if (dxp < 0) {
      nxC            -= (int )( dxp*zoom/2);
      qximage->width += (int )( dxp*zoom);
      dxp             = 0;
    } else if (dxp + wp > pximage->width) {
      nxC            -= (int )( (dxp + wp - pximage->width)*zoom/2);
      qximage->width -= (int )( (dxp + wp - pximage->width)*zoom);
      wp              = pximage->width - dxp;
    }

    if (dyp < 0) {
      nyC             -= (int )( dyp*zoom/2);
      qximage->height += (int )( dyp*zoom);
      dyp              = 0;
    } else if (dyp + hp > pximage->height) {
      nyC             -= (int )( (dyp + hp - pximage->height)*zoom/2);
      qximage->height -= (int )( (dyp + hp - pximage->height)*zoom);
      hp               = pximage->height - dyp;
    }
#ifdef DEBUG
    printf ("Xw_draw_zoomed_image info -> WP = %d, HP = %d\n", wp, hp);
    printf ("Xw_draw_zoomed_image info -> DX = %d, DY = %d\n", dxp, dyp);
    printf ("Xw_draw_zoomed_image info -> New point (%d, %d)\n", nxC, nyC);
    printf ("Xw_draw_zoomed_image info -> final image size is (%d x %d)\n\n",qximage->width,qximage->height);
#endif
  }

  // Truncate image if it is visible (i.e. part of it)
  if (qximage->width > 0. && qximage->height > 0.) {
    qximage->bytes_per_line = qximage->width * (pximage->bitmap_pad/8);
    isize = qximage->bytes_per_line * qximage->height;
    qximage->data = (char*) Xw_malloc (isize);

    if (!qximage->data) {
      // ERROR*XImage Allocation failed
      Xw_set_error (60,"Xw_draw_zoomed_image",NULL);
      return (XW_ERROR);
    }

    // Zoom image
    for (yp = yq = 0; yp < hp; yq = yn, yp++) {
      yn = (int) ((yp+1)*zoom+0.5);
      if (yn >= qximage->height) yn = qximage->height-1;
      for (xp = xq = 0; xp < wp; xq = xn, xp++) {
        xn = (int) ((xp+1)*zoom+0.5);
        if (xn >= qximage->width) xn = qximage->width-1;
        pixel = XGetPixel(pximage,xp+dxp,yp+dyp) ;
        if (((xn-xq) > 1) || ((yn-yq) > 1)) {
          for (y = yq; y < yn; y++)
            for (x = xq; x < xn; x++)
              XPutPixel (qximage, x, y, pixel);
        } else {
          XPutPixel (qximage, xq, yq, pixel);
        }
      }
    }

    if (_ZIMAGE && (_ZIMAGE != _IIMAGE))
      XDestroyImage (_ZIMAGE);

    _ZIMAGE = qximage;
    pimage->zoom = zoom;

    // Draw zoomed image at the new location
    return Xw_draw_image (awindow, pimage, UXPOINT(nxC), UYPOINT(nyC));
  }

  return XW_SUCCESS;
}
