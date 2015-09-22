// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_FrameBuffer.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <OpenGl_telem_util.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#include <OpenGl_PrinterContext.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_View.hxx>

#include <Standard_NotImplemented.hxx>

#if (defined(_WIN32) || defined(__WIN32__)) && defined(HAVE_FREEIMAGE)
  #include <NCollection_Handle.hxx>
  #include <FreeImagePlus.h>
  #ifdef _MSC_VER
    #pragma comment( lib, "FreeImage.lib" )
    #pragma comment( lib, "FreeImagePlus.lib" )
  #endif
  typedef NCollection_Handle<fipImage> FipHandle;
#endif

#ifdef _WIN32

#ifndef HAVE_FREEIMAGE

  // =======================================================================
// function : initBitmapBuffer
// purpose  : init device independent bitmap to hold printing data
// =======================================================================
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

#else /* HAVE_FREEIMAGE */

// =======================================================================
// function : imagePasteDC
// purpose  : copy the data from image buffer to the device context
// =======================================================================
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

// =======================================================================
// function : imageStretchDC
// purpose  : copy pixels from image to dc by stretching them
// =======================================================================
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

  if ((unsigned)aPassed != heightPx)
    return false;

  return true;
}

#endif /* HAVE_FREEIMAGE */

// =======================================================================
// function : getNearestPowOfTwo
// purpose  : get the nearest power of two for theNumber
// =======================================================================
static GLsizei getNearestPowOfTwo (const GLsizei theNumber)
{
  GLsizei aLast = 1;
  for (GLsizei p2 = 1; p2 <= theNumber; aLast = p2, p2 <<= 1);
  return aLast;
}

// =======================================================================
// function : getMaxFrameSize
// purpose  : get the maximum possible frame size
// =======================================================================
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

// =======================================================================
// function : fitDimensionsRatio
// purpose  : calculate correct width/height ratio for theWidth and
//            theHeight parameters
// =======================================================================
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

// =======================================================================
// function : initBufferStretch
// purpose  : calculate initialization sizes for frame buffer
//            when the stretch algorithm is selected
// =======================================================================
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

// =======================================================================
// function : initBufferTiling
// purpose  : calculate initialization sizes for frame buffer
//            when the tile algorithm is selected
// =======================================================================
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

#endif /* _WIN32 */

// =======================================================================
// function : Print
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_View::Print (const Aspect_Handle    thePrinterDC,
                                     const Standard_Boolean theToShowBackground,
                                     const Standard_CString theFileName,
                                     const Aspect_PrintAlgo thePrintAlgorithm,
                                     const Standard_Real    theScaleFactor)
{
#ifdef _WIN32
  if (!myWorkspace->Activate())
  {
    //MessageBox (NULL, "Print failed: can't setup the view for printing.",
    //            "The operation couldn't be completed.", MB_OK);
    return Standard_False;
  }

  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();

  // printer page dimensions
  HDC hPrnDC = (HDC) thePrinterDC;
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
  int viewWidth  = myWindow->Width();
  int viewHeight = myWindow->Height();
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
  OpenGl_FrameBuffer* aPrevBuffer = (OpenGl_FrameBuffer*)myFBO;
  Standard_Integer aFrameWidth (0),  aFrameHeight (0),
                   aPrevBufferX (0), aPrevBufferY (0);

  bool IsTiling = (thePrintAlgorithm == Aspect_PA_TILE);

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
      // use previous frame buffer with its dimensions
      aFrameWidth  = aPrevWidth;
      aFrameHeight = aPrevHeight;
      isUsable = true;
    }

    // if it is enough memory for image paste dc operation
    if (isUsable)
    {
#ifdef HAVE_FREEIMAGE
      // try to allocate fipImage and necessary resources
      fipImage* anImagePtr = new fipImage (FIT_BITMAP, aFrameWidth,
                                           aFrameHeight, 24);

      // if allocated successfully
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
      if (aFrameBuffer->Init (aCtx, aFrameWidth, aFrameHeight))
      {
#ifdef HAVE_FREEIMAGE
        // try to allocate fipImage and necessary resources
        fipImage* anImagePtr = new fipImage (FIT_BITMAP, aFrameWidth,
                                            aFrameHeight, 24);

        // if allocated successfully
        if (anImagePtr->isValid())
        {
          aViewImage  = anImagePtr;
          aViewBuffer = aViewImage->accessPixels ();
        }
        else
          delete anImagePtr;

        if (!aViewBuffer)
        {
          aFrameBuffer->Release (aCtx.operator->());
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
          aFrameBuffer->Release (aCtx.operator->());
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
  myWorkspace->PrinterContext() = new OpenGl_PrinterContext();
  myWorkspace->PrinterContext()->SetLayerViewport ((GLsizei )aFrameWidth, (GLsizei )aFrameHeight);
  GLint aViewPortBack[4];
  GLint anAlignBack = 1;
  glGetIntegerv (GL_VIEWPORT, aViewPortBack);
  glGetIntegerv (GL_PACK_ALIGNMENT, &anAlignBack);
  glPixelStorei (GL_PACK_ALIGNMENT, 4);

  // start document if the printer context is not actually a memory dc
  // (memory dc could be used for testing purposes)
  DOCINFO di;
  if (GetObjectType (hPrnDC) == OBJ_DC)
  {
    // Initialize printing procedure
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = "Open Cascade Document - print v3d view";
    di.lpszOutput = theFileName;

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

      myWorkspace->PrinterContext().Nullify();
      return Standard_False;
    }
  }

  // calculate offset for centered printing
  int aDevOffx = (int)(devWidth  - width) /2;
  int aDevOffy = (int)(devHeight - height)/2;

  // operation complete flag
  bool isDone = true;

  // Set up status for printing
  if (!theToShowBackground)
  {
    myWorkspace->NamedStatus |= OPENGL_NS_WHITEBACK;
  }

  // switch to mono camera for image dump
  const Graphic3d_Camera::Projection aProjectType = myCamera->ProjectionType() != Graphic3d_Camera::Projection_Stereo
                                                  ? myCamera->ProjectionType()
                                                  : Graphic3d_Camera::Projection_Perspective;
  if (!IsTiling)
  {
    myWorkspace->PrinterContext()->SetScale ((GLfloat )aFrameWidth /viewWidth, (GLfloat )aFrameHeight/viewHeight);
    redraw (aProjectType, aFrameBuffer);
    if (!myTransientDrawToFront)
    {
      // render to FBO only if allowed to render to back buffer
      myBackBufferRestored = Standard_True;
      myIsImmediateDrawn   = Standard_False;
      redrawImmediate (aProjectType, NULL, aFrameBuffer);
      myBackBufferRestored = Standard_False;
      myIsImmediateDrawn   = Standard_False;
    }
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
    myWorkspace->PrinterContext()->SetScale (aScaleRatex * (GLfloat )aScalex, aScaleRatey * (GLfloat )aScaley);

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
        myWorkspace->PrinterContext()->SetProjTransformation (aProj);

        // calculate cropped frame rect
        aTop    = (j == 0)         ? aPxCropy : 0;
        aBottom = (j == aTotaly-1) ? aFrameHeight-(aPxCropy+anOddPixely) :
                                     aFrameHeight;

        // draw to the offscreen buffer and capture the result
        redraw (aProjectType, aFrameBuffer);
        if (!myTransientDrawToFront)
        {
          // render to FBO only if forces to render to back buffer
          myBackBufferRestored = Standard_True;
          myIsImmediateDrawn   = Standard_False;
          redrawImmediate (aProjectType, NULL, aFrameBuffer);
          myBackBufferRestored = Standard_False;
          myIsImmediateDrawn   = Standard_False;
        }
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
  glPixelStorei (GL_PACK_ALIGNMENT, anAlignBack);
  aFrameBuffer->UnbindBuffer (aCtx);
  glViewport (aViewPortBack[0], aViewPortBack[1],
              aViewPortBack[2], aViewPortBack[3]);
  if (aPrevBuffer)
  {
    aPrevBuffer->ChangeViewport (aPrevBufferX, aPrevBufferY);
  }
  else
  {
    aFrameBuffer->Release (aCtx.operator->());
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
  myWorkspace->NamedStatus &= ~OPENGL_NS_WHITEBACK;
  myWorkspace->PrinterContext().Nullify();
  return (Standard_Boolean) isDone;

#else // not _WIN32
  Standard_NotImplemented::Raise ("OpenGl_View::Print is implemented only on Windows");
  myWorkspace->PrinterContext().Nullify();
  return Standard_False;
#endif
}
