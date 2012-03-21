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
 * Modified:  zov : 17-Apr-1998  : Saves an image as .BMP, .GIF or .XWD file
 *

   XW_STATUS Xw_save_image (awindow,aimage,filename):
   XW_EXT_WINDOW *awindow
   XW_EXT_IMAGEDATA   *aimage
   char *filename	XWD Image name

   Saves an image to an XWD, BMP or GIF file depending on given filename.

   returns  SUCCESS   if successfull;
   otherwise returns  ERROR
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Xw_Extension.h>
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#include <Image_PixMap.hxx>

#ifdef XW_PROTOTYPE
XW_STATUS Xw_save_image_adv (Display *aDisplay,Window aWindow,XWindowAttributes aWinAttr,XImage *aPximage,Colormap aColormap,int aNcolors,char *filename)
#else
XW_STATUS Xw_save_image_adv (aDisplay,aWindow,aWinAttr,aPximage,aColormap,aNcolors,filename)
Display *aDisplay;
Window aWindow;
XWindowAttributes aWinAttr;
XImage *aPximage;
Colormap aColormap;
int ncolors;
char *filename;
#endif /*XW_PROTOTYPE*/
{
  if (aWinAttr.visual->c_class == TrueColor)
  {
    Standard_Byte* aDataPtr = (Standard_Byte* )aPximage->data;
    Handle(Image_PixMap) anImagePixMap = new Image_PixMap (aDataPtr,
                                                           aPximage->width, aPximage->height,
                                                           aPximage->bytes_per_line,
                                                           aPximage->bits_per_pixel,
                                                           Standard_True);
    // save the image
    return anImagePixMap->Dump (filename) ? XW_SUCCESS : XW_ERROR;
  }
  else
  {
    std::cerr << "Visual Type not supported!";
    return XW_SUCCESS;
  }
}

#ifdef XW_PROTOTYPE
XW_STATUS Xw_save_image (void *awindow,void *aimage,char *filename)
#else
XW_STATUS Xw_save_image (awindow,aimage,filename)
void *awindow;
void *aimage;
char *filename ;
#endif /*XW_PROTOTYPE*/
{
  XW_EXT_WINDOW* pwindow = (XW_EXT_WINDOW* )awindow;
  if (!Xw_isdefine_window (pwindow))
  { // ERROR Bad EXT_WINDOW Address
    Xw_set_error (24, "Xw_save_image", pwindow);
    return (XW_ERROR);
  }

  XW_EXT_IMAGEDATA* pimage = (XW_EXT_IMAGEDATA* )aimage;
  if (!Xw_isdefine_image (pimage))
  { // ERROR Bad EXT_IMAGEDATA Address
    Xw_set_error (25, "Xw_save_image", pimage);
    return (XW_ERROR);
  }

  XImage* pximage = (_ZIMAGE) ? _ZIMAGE : _IIMAGE;
  return Xw_save_image_adv (_DISPLAY, _WINDOW, _ATTRIBUTES, pximage,
                            _COLORMAP->info.colormap, _COLORMAP->maxhcolor,
                            filename);
}
