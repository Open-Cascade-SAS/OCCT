// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#include <OpenGl_GlCore11.hxx>

#include <OpenGl_FrameBuffer.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <OpenGl_telem_util.hxx>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_FREEIMAGE
  #include <NCollection_Handle.hxx>
  #include <FreeImagePlus.h>
  #ifdef _MSC_VER
  #pragma comment( lib, "FreeImage.lib" )
  #pragma comment( lib, "FreeImagePlus.lib" )
  #endif
  typedef NCollection_Handle<fipImage> FipHandle;
#endif

#include <OpenGl_PrinterContext.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Display.hxx>

#include <GL/glu.h> // gluOrtho2D()

//10-05-96 : CAL ; Ajout d'un nouveau delta dans les copies de pixels (voir CALL_DEF_DELTA)
#define CALL_DEF_DELTA 10

// ---------------------------------------------------------------
// Function: getNearestPowOfTwo
// Purpose:  get the nearest power of two for theNumber
// ---------------------------------------------------------------
static GLsizei getNearestPowOfTwo (const GLsizei theNumber)
{
  GLsizei aLast = 1;
  for (GLsizei p2 = 1; p2 <= theNumber; aLast = p2, p2 <<= 1);
  return aLast;
}

// ---------------------------------------------------------------
// Function: getMaxFrameSize
// Purpose:  get the maximum possible frame size
// ---------------------------------------------------------------
static void getMaxFrameSize(Standard_Integer& theWidth,
                            Standard_Integer& theHeight)
{
  GLsizei aMaxX, aMaxY;
  GLint aVpDim[2];
  GLint aTexDim = 2048;
  glGetIntegerv (GL_MAX_VIEWPORT_DIMS, (GLint*) &aVpDim);
  glGetIntegerv (GL_MAX_TEXTURE_SIZE, &aTexDim);
  (aVpDim[0] >= aTexDim) ? aMaxX = (GLsizei) aTexDim : 
                           aMaxX = getNearestPowOfTwo((GLsizei)aVpDim[0]);
  (aVpDim[1] >= aTexDim) ? aMaxY = (GLsizei) aTexDim :
                           aMaxY = getNearestPowOfTwo((GLsizei)aVpDim[1]);

  theWidth  = (Standard_Integer)aMaxX;
  theHeight = (Standard_Integer)aMaxY;
}

// ---------------------------------------------------------------
// Function: fitDimensionsRatio
// Purpose:  calculate correct width/height ratio for theWidth and
//           theHeight parameters
// ---------------------------------------------------------------
static void fitDimensionsRatio (Standard_Integer& theWidth,
                                Standard_Integer& theHeight,
                                const Standard_Real theViewRatio)
{
  // set dimensions in accordance with the viewratio
  if (theHeight <  theWidth/theViewRatio)
      theWidth  = (Standard_Integer)(theHeight*theViewRatio);

  if (theWidth  <  theHeight*theViewRatio)
      theHeight = (Standard_Integer)(theWidth/theViewRatio);
}

// ---------------------------------------------------------------
// Function: getDimensionsTiling
// Purpose:  calculate maximum possible dimensions for framebuffer 
//           in tiling mode according to the view size
// ---------------------------------------------------------------
static void getDimensionsTiling (Standard_Integer& theFrameWidth,
                                 Standard_Integer& theFrameHeight,
                                 const int theViewWidth,
                                 const int theViewHeight)
{
  // fit the maximum dimensions into the printing area
  if (theFrameWidth > theViewWidth)
      theFrameWidth = theViewWidth;

  if (theFrameHeight > theViewHeight)
      theFrameHeight = theViewHeight;
}

// ---------------------------------------------------------------
// Function: initBufferStretch
// Purpose:  calculate initialization sizes for frame buffer
//           when the stretch algorithm is selected
// ---------------------------------------------------------------
static void initBufferStretch (Standard_Integer& theFrameWidth,
                               Standard_Integer& theFrameHeight,
                               const int theViewWidth,
                               const int theViewHeight)
{

  // Calculate correct width/height for framebuffer
  Standard_Real aViewRatio = (Standard_Real)theViewWidth/theViewHeight;
  fitDimensionsRatio (theFrameWidth, theFrameHeight, aViewRatio);

  // downscale the framebuffer if it is too large
  Standard_Real aWidthRate  = (Standard_Real)theFrameWidth /theViewWidth;
  Standard_Real aHeightRate = (Standard_Real)theFrameHeight/theViewHeight;

  if ((aWidthRate > 1 && aHeightRate > 1 && aWidthRate >= aHeightRate) || 
      (aWidthRate > 1 && aHeightRate <= 1))
  {
    theFrameWidth  = (Standard_Integer)(theFrameWidth /aWidthRate);
    theFrameHeight = (Standard_Integer)(theFrameHeight/aWidthRate);
  }
  else if ((aWidthRate  > 1 && aHeightRate > 1 && aWidthRate < aHeightRate) ||
           (aWidthRate <= 1 && aHeightRate > 1))
  {
    theFrameWidth  = (Standard_Integer)(theFrameWidth /aHeightRate);
    theFrameHeight = (Standard_Integer)(theFrameHeight/aHeightRate);
  }

}

// ---------------------------------------------------------------
// Function: initBufferTiling
// Purpose:  calculate initialization sizes for frame buffer
//           when the tile algorithm is selected
// ---------------------------------------------------------------
static void initBufferTiling (Standard_Integer& theFrameWidth,
                              Standard_Integer &theFrameHeight,
                              const int theViewWidth,
                              const int theViewHeight)
{
  // fit framebuffer into the printing area
  if (theFrameWidth > theViewWidth)
      theFrameWidth = theViewWidth;

  if (theFrameHeight > theViewHeight)
      theFrameHeight = theViewHeight;
}

// ---------------------------------------------------------------
// Function: initBitmapBuffer
// Purpose:  init device independent bitmap to hold printing data
// ---------------------------------------------------------------
#ifdef WNT
#ifndef HAVE_FREEIMAGE
static void initBitmapBuffer (const HDC theMemoryDC,
                              HBITMAP &theMemoryBmp,
                              const   Standard_Integer theBmpWidth,
                              const   Standard_Integer theBmpHeight,
                              VOID*   &theBufferPtr)
{
  // define compatible bitmap
  BITMAPINFO aBitmapData;
  memset (&aBitmapData, 0, sizeof (BITMAPINFOHEADER));
  aBitmapData.bmiHeader.biSize          = sizeof (BITMAPINFOHEADER);
  aBitmapData.bmiHeader.biWidth         = theBmpWidth;
  aBitmapData.bmiHeader.biHeight        = theBmpHeight;
  aBitmapData.bmiHeader.biPlanes        = 1;
  aBitmapData.bmiHeader.biBitCount      = 24;
  aBitmapData.bmiHeader.biXPelsPerMeter = 0;
  aBitmapData.bmiHeader.biYPelsPerMeter = 0;
  aBitmapData.bmiHeader.biClrUsed       = 0;
  aBitmapData.bmiHeader.biClrImportant  = 0;
  aBitmapData.bmiHeader.biCompression   = BI_RGB;
  aBitmapData.bmiHeader.biSizeImage     = 0;

  // Create Device Independent Bitmap
  theMemoryBmp = CreateDIBSection (theMemoryDC, &aBitmapData, DIB_RGB_COLORS,
                                   &theBufferPtr, NULL, 0);
}
#else
// ---------------------------------------------------------------
// Function: imagePasteDC
// Purpose:  copy the data from image buffer to the device context
// ---------------------------------------------------------------
static bool imagePasteDC(HDC theDstDC,    FipHandle theImage, int theOffsetX,
                         int theOffsetY,  int theWidth, int theHeight, 
                         int theLeft = 0, int theTop = 0)
{
  // get image parameters
  BITMAPINFO* aBitmapData = theImage->getInfo ();
  SetStretchBltMode (theDstDC, STRETCH_HALFTONE);
 
  // organize blocks data passing if memory isn't enough to pass all the data
  // at once
  int aLinesComplete = 0, aMaxBlockWidth = theHeight, aBlockWidth = 0,
      aPassed        = 0, aInverseLine   = 0, aScan = 0;
  BYTE *aDataPtr = 0;
  while (aMaxBlockWidth >= 1 && aLinesComplete < theHeight)
  {
    // how much lines still to pass
    aBlockWidth = theHeight - aLinesComplete;

    // normalize count of lines to pass to maximum lines count at one pass.
    if (aBlockWidth > aMaxBlockWidth)
      aBlockWidth = aMaxBlockWidth;

    // access image data at the start scan line, we need to calculate scan from
    // the bottom of image (image is bottom-left, the src coord is top-left)
    aInverseLine = theTop + aBlockWidth + aLinesComplete;
    aScan = theImage->getHeight() - aInverseLine;
    aDataPtr = theImage->getScanLine (aScan);
    if (!aDataPtr)
      return false;

    // try to pass block to the device
    if (aBlockWidth > 0)
    {
      // instead of banded output we provide blocked as it isn't always passed
      // to printer as it is expected
      aPassed = SetDIBitsToDevice (theDstDC, theOffsetX,
                                   theOffsetY + aLinesComplete,
                                   theWidth, aBlockWidth, theLeft, 0,
                                   0, aBlockWidth,
                                   aDataPtr, aBitmapData, DIB_RGB_COLORS);

      // if result is bad, try to decrease band width
      if (aPassed != aBlockWidth)
      {
        aMaxBlockWidth = aMaxBlockWidth >> 1;
        aLinesComplete = 0;
      }
      else
        aLinesComplete += aBlockWidth;
    }
  }

  // check for total failure
  if (aMaxBlockWidth < 1)
    return false;

  return true;
}

// ---------------------------------------------------------------
// Function: imageStretchDC
// Purpose:  copy pixels from image to dc by stretching them
// ---------------------------------------------------------------
static bool imageStretchDC(HDC theDstDC,   FipHandle theImage, int theOffsetX,
                           int theOffsetY, int theWidth, int theHeight)
{
  // access to raw image data
  BYTE *aDataPtr = theImage->accessPixels ();
  if (!aDataPtr)
    return false;

  // get image parameters
  unsigned int widthPx    = theImage->getWidth ();
  unsigned int heightPx   = theImage->getHeight ();
  BITMAPINFO* aBitmapData = theImage->getInfo ();
  SetStretchBltMode (theDstDC, STRETCH_HALFTONE);
  
  // pass lines and check if operation is succesfull
  int aPassed = 0;
  aPassed = StretchDIBits (theDstDC, theOffsetX, theOffsetY, theWidth,
                           theHeight, 0, 0, widthPx, heightPx, aDataPtr,
                           aBitmapData, DIB_RGB_COLORS, SRCCOPY);

  if (aPassed != heightPx)
    return false;
 
  return true;
}
#endif
#endif

// ---------------------------------------------------------------
// ---------------------------------------------------------------

//call_togl_print
Standard_Boolean OpenGl_Workspace::Print
  (const Graphic3d_CView& ACView, 
   const Aspect_CLayer2d& ACUnderLayer, 
   const Aspect_CLayer2d& ACOverLayer,
   const Aspect_Handle    hPrintDC,// const Aspect_Drawable hPrintDC,
   const Standard_Boolean showBackground,
   const Standard_CString filename,
   const Aspect_PrintAlgo printAlgorithm,
   const Standard_Real theScaleFactor)
{
#ifdef WNT

  if (!Activate())
  {
    //MessageBox (NULL, "Print failed: can't setup the view for printing.",
    //            "The operation couldn't be completed.", MB_OK);
    return Standard_False;
  }

  // printer page dimensions
  HDC hPrnDC = (HDC) hPrintDC;
  int devWidth  = GetDeviceCaps (hPrnDC, HORZRES);
  int devHeight = GetDeviceCaps (hPrnDC, VERTRES);

  // if context is actually a memory dc, try to retrieve bitmap dimensions
  // (memory dc could be used for testing purposes)
  if (GetObjectType (hPrnDC) == OBJ_MEMDC)
  {
    // memory dc dimensions
    BITMAP aBitmapInfo;
    HBITMAP aMemoryBitmap = (HBITMAP) GetCurrentObject (hPrnDC, OBJ_BITMAP);
    if (aMemoryBitmap)
      if (GetObject (aMemoryBitmap, sizeof (BITMAP), &aBitmapInfo))
      {
        devWidth  = aBitmapInfo.bmWidth;
        devHeight = aBitmapInfo.bmHeight;
      }
  }

  Standard_Integer tempWidth  = (Standard_Integer) devWidth;
  Standard_Integer tempHeight = (Standard_Integer) devHeight;

  // view dimensions
  int viewWidth  = myWidth;
  int viewHeight = myHeight;
  if (viewWidth == 0 || viewHeight == 0)
  {
    //MessageBox (NULL, "Print failed: can't setup the view for printing.",
    //            "The operation couldn't be completed.", MB_OK);
    return Standard_False;
  }

  // calculate correct width/height ratio
  Standard_Real viewRatio = (Standard_Real)viewWidth/viewHeight;
  fitDimensionsRatio(tempWidth, tempHeight, viewRatio);

  // width and height for printing area
  int width  = (int) (tempWidth  * theScaleFactor);
  int height = (int) (tempHeight * theScaleFactor);

  // device independent bitmap for the whole view
#ifdef HAVE_FREEIMAGE
  FipHandle  aViewImage  = NULL;
  BYTE*      aViewBuffer = NULL;
#else
  HDC     hMemDC          = CreateCompatibleDC (hPrnDC);
  HBITMAP hViewBitmap     = NULL;
  HGDIOBJ hViewBitmapOld  = NULL;
  VOID*   aViewBuffer    = NULL;
#endif

  // Frame buffer initialization
  OpenGl_FrameBuffer* aFrameBuffer = NULL;
  OpenGl_FrameBuffer* aPrevBuffer = (OpenGl_FrameBuffer*) ACView.ptrFBO;
  Standard_Integer aFrameWidth (0),  aFrameHeight (0),
                   aPrevBufferX (0), aPrevBufferY (0);

  bool IsTiling = (printAlgorithm == 1);

  // try to use existing frame buffer
  if (aPrevBuffer)
  {
    GLsizei aPrevWidth  = aPrevBuffer->GetSizeX();
    GLsizei aPrevHeight = aPrevBuffer->GetSizeY();
    bool isUsable = false;

    // check if its possible to use previous frame buffer
    if (!IsTiling && aPrevWidth >= width && aPrevHeight >= height)
    {
      aFrameWidth  = (Standard_Integer) width;
      aFrameHeight = (Standard_Integer) height;
      isUsable = true;
    }
    else if (IsTiling)
    {
      getDimensionsTiling (aFrameWidth, aFrameHeight, width, height);
      if (aPrevWidth >= aFrameWidth && aPrevHeight >= aFrameHeight)
        isUsable = true;
    }

    // if it is enough memory for image paste dc operation
    if (isUsable)
    {
#ifdef HAVE_FREEIMAGE
      // try to allocate fipImage and necessary resources
      fipImage* anImagePtr = new fipImage (FIT_BITMAP, aFrameWidth,
                                           aFrameHeight, 24);

      // if allocated succesfully
      if (anImagePtr->isValid())
      {
        aViewImage  = anImagePtr;
        aViewBuffer = aViewImage->accessPixels ();
      }
      else
        delete anImagePtr;

      if (!aViewBuffer)
      {
        isUsable = false;
        aViewBuffer = NULL;
        aViewImage  = NULL;
      }
#else
      // try to allocate compatible bitmap and necessary resources
      initBitmapBuffer (hMemDC, hViewBitmap, 
                        aFrameWidth, aFrameHeight, aViewBuffer);
      if (!aViewBuffer)
      {
        isUsable = false;
        if (hViewBitmap)
          DeleteObject (hViewBitmap);
        hViewBitmap = NULL;
      }
      else
        hViewBitmapOld = SelectObject (hMemDC, hViewBitmap);
#endif
    }

    // use previous frame buffer
    if (isUsable)
    {
      aPrevBufferX = aPrevWidth;
      aPrevBufferY = aPrevHeight;
      aFrameBuffer = aPrevBuffer;
      aFrameBuffer->ChangeViewport (aFrameWidth, aFrameHeight);
    }
  }

  // if previous buffer cannot be used, try to init a new one
  if (!aFrameBuffer)
  {
    aFrameBuffer = new OpenGl_FrameBuffer();

    // try to create the framebuffer with the best possible size
    Standard_Integer aMaxWidth(0), aMaxHeight(0);
    getMaxFrameSize (aMaxWidth, aMaxHeight);
    while (aMaxWidth > 1 && aMaxHeight > 1)
    {
      aFrameWidth  = aMaxWidth;
      aFrameHeight = aMaxHeight;

      // calculate dimensions for different printing algorithms
      if (!IsTiling)
        initBufferStretch (aFrameWidth, aFrameHeight, width, height);
      else
        initBufferTiling (aFrameWidth, aFrameHeight, width, height);

      // try to initialize framebuffer
      if (aFrameBuffer->Init (GetGlContext(), aFrameWidth, aFrameHeight))
      {
#ifdef HAVE_FREEIMAGE
        // try to allocate fipImage and necessary resources
        fipImage* anImagePtr = new fipImage (FIT_BITMAP, aFrameWidth,
                                            aFrameHeight, 24);

        // if allocated succesfully
        if (anImagePtr->isValid())
        {
          aViewImage  = anImagePtr;
          aViewBuffer = aViewImage->accessPixels ();
        }
        else
          delete anImagePtr;

        if (!aViewBuffer)
        {
          aFrameBuffer->Release (GetGlContext());
          aViewBuffer = NULL;
          aViewImage  = NULL;
        }
        else
          break;
#else
        // try to allocate compatible bitmap and necessary resources
        initBitmapBuffer (hMemDC, hViewBitmap, 
                          aFrameWidth, aFrameHeight, aViewBuffer);
        if (!aViewBuffer)
        {
          if (hViewBitmap)
            DeleteObject (hViewBitmap);
          aFrameBuffer->Release (GetGlContext());
          hViewBitmap = NULL;
        }
        else
        {
          hViewBitmapOld = SelectObject (hMemDC, hViewBitmap);
          break;
        }
#endif
      }

      // not initialized, decrease dimensions
      aMaxWidth  = aMaxWidth  >> 1;
      aMaxHeight = aMaxHeight >> 1;
    }

    // check if there are proper dimensions 
    if (aMaxWidth <= 1 || aMaxHeight <= 1)
    {
      MessageBox (NULL, "Print failed: can't allocate buffer for printing.",
                  "The operation couldn't be completed.", MB_OK);

      if (aFrameBuffer)
        delete aFrameBuffer;
#ifndef HAVE_FREEIMAGE
      if (hMemDC)
        DeleteDC (hMemDC);
#endif

      return Standard_False;
    }
  }

  // setup printing context and viewport
  GLint aViewPortBack[4]; 
  GLint anAlignBack     = 1;

  OpenGl_PrinterContext aPrinterContext (myGContext);
  aPrinterContext.SetLayerViewport ((GLsizei)aFrameWidth,
                                    (GLsizei)aFrameHeight);
  glGetIntegerv (GL_VIEWPORT, aViewPortBack);
  glGetIntegerv (GL_PACK_ALIGNMENT, &anAlignBack);
  glPixelStorei (GL_PACK_ALIGNMENT, 4);

  // start document if the printer context is not actually a memory dc
  // (memory dc could be used for testing purposes)
  DOCINFO di;
  if (GetObjectType (hPrnDC) == OBJ_DC)
  {
    // Initalize printing procedure
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = "Open Cascade Document - print v3d view";
    di.lpszOutput = filename;

    // if can't print the document
    if (StartDoc (hPrnDC, &di) <= 0 || StartPage (hPrnDC) <= 0)
    {
      MessageBox (NULL, "Print failed: printer can't start operation.",
                  "The operation couldn't be completed.", MB_OK);
#ifndef HAVE_FREEIMAGE
      if (hViewBitmap)
      {
        SelectObject (hMemDC, hViewBitmapOld);
        DeleteObject (hViewBitmap);
      }
      DeleteDC (hMemDC);
#endif

      return Standard_False;
    }
  }

  // activate the offscreen buffer
  aFrameBuffer->BindBuffer (GetGlContext());

  // calculate offset for centered printing
  int aDevOffx = (int)(devWidth  - width) /2;
  int aDevOffy = (int)(devHeight - height)/2;

  // operation complete flag
  bool isDone = true;

  // Set up status for printing
  if (!showBackground)
    NamedStatus |= OPENGL_NS_WHITEBACK;

  if (!IsTiling)
  {
    aPrinterContext.SetScale ((GLfloat)aFrameWidth /viewWidth,
                              (GLfloat)aFrameHeight/viewHeight);
    aFrameBuffer->SetupViewport ();
    Redraw1(ACView, ACUnderLayer, ACOverLayer, 0);
    RedrawImmediatMode();
    glReadPixels (0, 0, aFrameWidth, aFrameHeight,
                  GL_BGR_EXT, GL_UNSIGNED_BYTE, (GLvoid* )aViewBuffer);

    // copy result to the printer device and check for errors
#ifdef HAVE_FREEIMAGE
    if (!aViewImage->rescale(width, height, FILTER_BICUBIC) ||
        !imagePasteDC (hPrnDC, aViewImage, aDevOffx, aDevOffy, width, height))
      isDone = imageStretchDC (hPrnDC, aViewImage, aDevOffx, aDevOffy,
                               width, height);
#else
    if (width > aFrameWidth && height > aFrameHeight)
    {
      SetStretchBltMode (hPrnDC, STRETCH_HALFTONE);
      isDone = (StretchBlt (hPrnDC, aDevOffx, aDevOffy, width, height,
                            hMemDC, 0, 0, aFrameWidth, aFrameHeight, SRCCOPY) != 0); // to avoid warning C4800
    }
    else
    {
      isDone = (BitBlt (hPrnDC, aDevOffx, aDevOffy, width, height,
                        hMemDC, 0, 0, SRCCOPY) != 0); // to avoid warning C4800
    }
#endif
  }
  else
  {
    // calculate total count of frames and cropping size
    Standard_Integer aPxCropx = 0;
    Standard_Integer aPxCropy = 0;
    Standard_Integer aTotalx = 
                     (Standard_Integer)floor ((float)width /aFrameWidth);
    Standard_Integer aTotaly = 
                     (Standard_Integer)floor ((float)height/aFrameHeight);
    if (width %aFrameWidth != 0)
    {
      aPxCropx = (aFrameWidth - width%aFrameWidth)/2;
      aTotalx++;
    }
    if (height%aFrameHeight != 0)
    {
      aPxCropy = (aFrameHeight - height%aFrameHeight)/2;
      aTotaly++;
    }

    int anOddPixelx = (width %aFrameWidth) %2;
    int anOddPixely = (height%aFrameHeight)%2;

    // calculate scale factor for full frames
    Standard_Real aScalex = (Standard_Real)width /aFrameWidth;
    Standard_Real aScaley = (Standard_Real)height/aFrameHeight;

    // calculate and set the text scaling factor for printing context
    GLfloat aScaleRatex = (GLfloat)aFrameWidth /viewWidth;
    GLfloat aScaleRatey = (GLfloat)aFrameHeight/viewHeight;
    aPrinterContext.SetScale (aScaleRatex*(GLfloat)aScalex,
                              aScaleRatey*(GLfloat)aScaley);

    // initialize projection matrix for printer context
    TColStd_Array2OfReal aProj (0, 3, 0, 3);
    Standard_Real aDef = 0;
    aProj.Init (aDef);
    aProj(2,2) = 1.0;
    aProj(3,3) = 1.0;

    // projection matrix offsets for printer context
    // offsets are even numbers
    Standard_Real aOffsetx(0), aOffsety(0);
    aOffsetx = -(aTotalx-1);
    aOffsety = -(aTotaly-1);

    // rect of frame image that will be copied
    // and coordinates in view image where to put it
    Standard_Integer aLeft = 0, aRight = 0, aBottom = 0, aTop = 0;
    Standard_Integer aSubLeft = (Standard_Integer)aDevOffx;
    Standard_Integer aSubTop  = (Standard_Integer)aDevOffy;

    // draw sequence of full frames
    for (int i = 0; i < aTotalx; i++)
    {
      // offsets are even numbers
      aOffsety = -(aTotaly-1);
      aSubTop  =  (Standard_Integer)aDevOffy;

      // calculate cropped frame rect
      aLeft  = (i == 0) ? aPxCropx : 0;
      aRight = (i == aTotalx-1) ? aFrameWidth-(aPxCropx+anOddPixelx) :
                                  aFrameWidth;

      for (int j = 0; j < aTotaly; j++)
      {
        // no offset for single frames
        aProj(3,0) = (aTotalx == 1) ? 0 : -aOffsetx;
        aProj(3,1) = (aTotaly == 1) ? 0 :  aOffsety;

        // set projection matrix
        aProj(0,0) = aScalex;
        aProj(1,1) = aScaley;
        aPrinterContext.SetProjTransformation (aProj);

        // calculate cropped frame rect
        aTop    = (j == 0)         ? aPxCropy : 0;
        aBottom = (j == aTotaly-1) ? aFrameHeight-(aPxCropy+anOddPixely) :
                                     aFrameHeight;

        // draw to the offscreen buffer and capture the result
        aFrameBuffer->SetupViewport ();
        Redraw1(ACView, ACUnderLayer, ACOverLayer, 0);
        RedrawImmediatMode();
        glReadPixels (0, 0, aFrameWidth, aFrameHeight,
                      GL_BGR_EXT, GL_UNSIGNED_BYTE, (GLvoid* )aViewBuffer);
#ifdef HAVE_FREEIMAGE
        // cut out pixels that are out of printing area
        isDone = imagePasteDC (hPrnDC, aViewImage, aSubLeft, aSubTop,
                               aRight-aLeft, aBottom-aTop, aLeft, aTop);
#else
        isDone = (BitBlt (hPrnDC, aSubLeft, aSubTop, aRight-aLeft, aBottom-aTop,
                          hMemDC, aLeft, aTop, SRCCOPY) != 0); // to avoid warning C4800
#endif

        // stop operation if errors
        if (!isDone)
          break;

        // calculate new view offset for y-coordinate
        aOffsety += 2.0;
        aSubTop  += aBottom-aTop;
      }

      // stop operation if errors
      if (!isDone)
        break;
 
      // calculate new view offset for x-coordinate
      aOffsetx += 2.0;
      aSubLeft += aRight-aLeft;
    }
  }

  // complete printing or indicate an error
  if (GetObjectType (hPrnDC) == OBJ_DC && isDone == true)
  {
    EndPage (hPrnDC);
    EndDoc (hPrnDC);
  }
  else if (isDone == false)
  {
    MessageBox (NULL, "Print failed: insufficient memory or spool error.\nPlease use smaller printer resolution.",
                "The opeartion couldn't be completed.", MB_OK);
    if (GetObjectType (hPrnDC) == OBJ_DC)
      AbortDoc (hPrnDC);
  }
  
  // return OpenGl to the previous state
  aPrinterContext.Deactivate ();
  glPixelStorei (GL_PACK_ALIGNMENT, anAlignBack);
  aFrameBuffer->UnbindBuffer (GetGlContext());
  glViewport (aViewPortBack[0], aViewPortBack[1], 
              aViewPortBack[2], aViewPortBack[3]);
  if (aPrevBuffer)
  {
    aPrevBuffer->ChangeViewport (aPrevBufferX, aPrevBufferY);
  }
  else
  {
    aFrameBuffer->Release (GetGlContext ());
    delete aFrameBuffer;
  }

  // delete resources
#ifndef HAVE_FREEIMAGE
  if (hViewBitmap)
  {
    SelectObject (hMemDC, hViewBitmapOld);
    DeleteObject (hViewBitmap);
  }
  DeleteDC (hMemDC);
#endif

  // Reset status after printing
  NamedStatus &= ~OPENGL_NS_WHITEBACK;

  return (Standard_Boolean) isDone;

#else // not WNT
  return Standard_False;
#endif 
}

/*----------------------------------------------------------------------*/

//redrawView
void OpenGl_Workspace::Redraw1 (const Graphic3d_CView& ACView, 
                               const Aspect_CLayer2d& ACUnderLayer, 
                               const Aspect_CLayer2d& ACOverLayer,
                               const int aswap)
{
  if (myDisplay.IsNull() || myView.IsNull())
    return;

  myDisplay->UpdateUserMarkers();

  // Request reset of material
  NamedStatus |= OPENGL_NS_RESMAT;

  /* GL_DITHER on/off pour le background */
  if (myBackDither)
    glEnable (GL_DITHER);
  else
    glDisable (GL_DITHER);

  GLbitfield toClear = GL_COLOR_BUFFER_BIT;
  if ( myUseZBuffer )
  {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);

    // SAV checking if depth test was deprecated somewhere outside
    if ( myUseDepthTest )
      glEnable(GL_DEPTH_TEST);
    else
      glDisable(GL_DEPTH_TEST);

    glClearDepth(1.0);
	toClear |= GL_DEPTH_BUFFER_BIT;
  }
  else
  {
    glDisable(GL_DEPTH_TEST);
  }

  if ( NamedStatus & OPENGL_NS_WHITEBACK )
  {
    // Set background to white
    glClearColor (1.F, 1.F, 1.F, 1.F);
	toClear |= GL_DEPTH_BUFFER_BIT;
  }
  else
  {
    glClearColor (myBgColor.rgb[0], myBgColor.rgb[1], myBgColor.rgb[2], 0.F);
  }
  glClear (toClear);

  Handle(OpenGl_Workspace) aWS(this);
  myView->Render(aWS,ACView,ACUnderLayer,ACOverLayer);

  // Swap the buffers
  if ( aswap )
  {
#ifndef WNT
    glXSwapBuffers ((Display*)myDisplay->GetDisplay (), myWindow );
#else
    SwapBuffers ( wglGetCurrentDC () );
    glFlush();
#endif  /* WNT */
    myBackBufferRestored = Standard_False;
  }
  else
    glFlush();
}

/*----------------------------------------------------------------------*/

//TelCopyBuffers
void OpenGl_Workspace::CopyBuffers (Tint vid, int FrontToBack, Tfloat xm, Tfloat ym, Tfloat zm, Tfloat XM, Tfloat YM, Tfloat ZM, Tint flag)
{
  if (FrontToBack) myBackBufferRestored = Standard_False;

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();
  gluOrtho2D ((GLdouble) 0., (GLdouble) myWidth, 0., (GLdouble) myHeight);
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glLoadIdentity ();

  DisableFeatures();

  GLsizei width = myWidth+1, height = myHeight+1;
  Tfloat xmr = 0, ymr = 0;

  if (flag) 
  {
    if (!myView.IsNull()) //szvgl: use vid here!
	{
    // Calculate bounding box and store the projected rectangle
    Tfloat xr[8], yr[8];
    // Project bounding box
    if (myView->ProjectObjectToRaster (myWidth, myHeight, xm, ym, zm, xr[0], yr[0]) &&
        myView->ProjectObjectToRaster (myWidth, myHeight, xm, YM, zm, xr[1], yr[1]) &&
        myView->ProjectObjectToRaster (myWidth, myHeight, XM, YM, zm, xr[2], yr[2]) &&
        myView->ProjectObjectToRaster (myWidth, myHeight, XM, ym, zm, xr[3], yr[3]) &&
        myView->ProjectObjectToRaster (myWidth, myHeight, xm, ym, ZM, xr[4], yr[4]) &&
        myView->ProjectObjectToRaster (myWidth, myHeight, xm, YM, ZM, xr[5], yr[5]) &&
        myView->ProjectObjectToRaster (myWidth, myHeight, XM, YM, ZM, xr[6], yr[6]) &&
        myView->ProjectObjectToRaster (myWidth, myHeight, XM, ym, ZM, xr[7], yr[7]))
    {
      Tfloat XMR, YMR;
      xmr = ymr = (float ) shortreallast ();
      XMR = YMR = (float ) shortrealfirst ();
      /*
      * Recherche du rectangle projete
      */
      Tint i;
      for (i=0; i<8; i++) {
        if (xmr > xr[i]) xmr = xr[i];
        if (ymr > yr[i]) ymr = yr[i];
        if (XMR < xr[i]) XMR = xr[i];
        if (YMR < yr[i]) YMR = yr[i];
      }
      /* pour eviter les bavures de pixels ! */
      xmr--;ymr--;
      XMR++;YMR++;

      /*
      * Ajout CAL : 10/05/96
      * Si les MinMax viennent d'un ensemble de markers
      * on ne tient pas compte du scale factor de ceux-ci
      * dans les valeurs de MinMax. En effet, ce facteur
      * est dans l'espace pixel et les MinMax dans l'espace
      * du modele. Donc ajout d'un delta de pixels
      * en esperant que les applis n'utilisent pas des
      * markers tres gros !
      */
      xmr -= CALL_DEF_DELTA; ymr -= CALL_DEF_DELTA;
      XMR += CALL_DEF_DELTA; YMR += CALL_DEF_DELTA;

      /*
      * Le rectangle projete peut-etre clippe
      */
      width = (GLsizei) (XMR-xmr+1);
      height = (GLsizei) (YMR-ymr+1);
      /*
      * (xmr,ymr) coin inferieur gauche
      * (XMR,YMR) coin superieur droit
      */
      /* cas ou 1 coin est en dehors de la fenetre */
      if (xmr < 0) { width  = (GLsizei) (XMR+1); xmr = 0; }
      if (ymr < 0) { height = (GLsizei) (YMR+1); ymr = 0; }
      if (XMR > myWidth)  { width  = (GLsizei) (myWidth-xmr+1); }
      if (YMR > myHeight) { height = (GLsizei) (myHeight-ymr+1); }

      /* cas ou les 2 coins sont en dehors de la fenetre */
      if (XMR < 0) { xmr = 0; width = height = 1; }
      if (YMR < 0) { ymr = 0; width = height = 1; }
      if (xmr > myWidth)  { xmr = 0; width = height = 1; }
      if (ymr > myHeight) { ymr = 0; width = height = 1; }
    }
	}
  }

  glDrawBuffer (FrontToBack? GL_BACK : GL_FRONT);
  glReadBuffer (FrontToBack? GL_FRONT : GL_BACK);
  /* copie complete */
  glRasterPos2i ((GLint) xmr, (GLint) ymr);
  glCopyPixels ((GLint) xmr, (GLint) ymr, width, height, GL_COLOR);

  EnableFeatures();

  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  glMatrixMode (GL_MODELVIEW);
  glPopMatrix ();

  glDrawBuffer (GL_BACK);
}

/*----------------------------------------------------------------------*/

//call_subr_displayCB
void OpenGl_Workspace::DisplayCallback (const Graphic3d_CView& ACView, int reason)
{
  if( ACView.GDisplayCB )
  {
    Aspect_GraphicCallbackStruct callData;
    callData.reason = reason;
    callData.display = (DISPLAY*)myDisplay->GetDisplay();
    callData.window = (WINDOW)myWindow;
    callData.wsID = ACView.WsId;
    callData.viewID = ACView.ViewId;
    callData.gcontext = myGContext;

    int status = (*ACView.GDisplayCB)( ACView.DefWindow.XWindow, ACView.GClientData, &callData );
  }
}

/*----------------------------------------------------------------------*/
