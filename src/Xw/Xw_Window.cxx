// Created on: 2013-04-06
// Created by: Kirill Gavrilov
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Xw_Window.hxx>

#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && !defined(__ANDROID__) && !defined(__QNX__)

#include <Aspect_Convert.hxx>
#include <Aspect_WindowDefinitionError.hxx>

#include <GL/glx.h>

IMPLEMENT_STANDARD_RTTIEXT(Xw_Window,Aspect_Window)

namespace
{

  //! Search for RGBA double-buffered visual with stencil buffer.
  static int TheDoubleBuffVisual[] =
  {
    GLX_RGBA,
    GLX_DEPTH_SIZE, 16,
    GLX_STENCIL_SIZE, 1,
    GLX_DOUBLEBUFFER,
    None
  };

  //! Search for RGBA double-buffered visual with stencil buffer.
  static int TheDoubleBuffFBConfig[] =
  {
    GLX_X_RENDERABLE,  True,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    GLX_DEPTH_SIZE,    16,
    GLX_STENCIL_SIZE,  1,
    GLX_DOUBLEBUFFER,  True,
    None
  };

}

// =======================================================================
// function : Xw_Window
// purpose  :
// =======================================================================
Xw_Window::Xw_Window (const Handle(Aspect_DisplayConnection)& theXDisplay,
                      const Standard_CString theTitle,
                      const Standard_Integer thePxLeft,
                      const Standard_Integer thePxTop,
                      const Standard_Integer thePxWidth,
                      const Standard_Integer thePxHeight,
                      const Aspect_FBConfig  theFBConfig)
: Aspect_Window(),
  myDisplay  (theXDisplay),
  myXWindow  (0),
  myFBConfig (theFBConfig),
  myXLeft    (thePxLeft),
  myYTop     (thePxTop),
  myXRight   (thePxLeft + thePxWidth),
  myYBottom  (thePxTop + thePxHeight),
  myIsOwnWin (Standard_True)
{
  int aDummy = 0;
  if (thePxWidth <= 0 || thePxHeight <= 0)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, Coordinate(s) out of range");
  }
  else if (theXDisplay.IsNull())
  {
    throw Aspect_WindowDefinitionError("Xw_Window, X Display connection is undefined");
    return;
  }
  else if (!glXQueryExtension (myDisplay->GetDisplay(), &aDummy, &aDummy))
  {
    throw Aspect_WindowDefinitionError("Xw_Window, GLX extension is unavailable");
    return;
  }

  Display* aDisp   = myDisplay->GetDisplay();
  int      aScreen = DefaultScreen(aDisp);
  Window   aParent = RootWindow   (aDisp, aScreen);
  XVisualInfo* aVisInfo = NULL;

  if (myFBConfig == NULL)
  {
    // FBConfigs were added in GLX version 1.3
    int aGlxMajor = 0;
    int aGlxMinor = 0;
    const bool hasFBCfg = glXQueryVersion (aDisp, &aGlxMajor, &aGlxMinor)
                       && ((aGlxMajor == 1 && aGlxMinor >= 3) || (aGlxMajor > 1));
    if (hasFBCfg)
    {
      int aFBCount = 0;
      GLXFBConfig* aFBCfgList = NULL;
      if (hasFBCfg)
      {
        aFBCfgList = glXChooseFBConfig (aDisp, aScreen, TheDoubleBuffFBConfig, &aFBCount);
      }
      if(aFBCfgList != NULL
      && aFBCount >= 1)
      {
        myFBConfig = aFBCfgList[0];
        aVisInfo   = glXGetVisualFromFBConfig (aDisp, myFBConfig);
      }
      XFree (aFBCfgList);
    }
  }

  if (aVisInfo == NULL)
  {
    aVisInfo = glXChooseVisual (aDisp, aScreen, TheDoubleBuffVisual);
  }
  if (aVisInfo == NULL)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, couldn't find compatible Visual (RGBA, double-buffered)");
    return;
  }

  unsigned long aMask = 0;
  XSetWindowAttributes aWinAttr;
  memset(&aWinAttr, 0, sizeof(XSetWindowAttributes));
  aWinAttr.event_mask = ExposureMask | StructureNotifyMask;
  aMask |= CWEventMask;
  aWinAttr.colormap = XCreateColormap(aDisp, aParent, aVisInfo->visual, AllocNone);
  aWinAttr.border_pixel = 0;
  aWinAttr.override_redirect = False;

  myXWindow = XCreateWindow(aDisp, aParent,
                            myXLeft, myYTop, thePxWidth, thePxHeight,
                            0, aVisInfo->depth,
                            InputOutput,
                            aVisInfo->visual,
                            CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect, &aWinAttr);
  XFree (aVisInfo); aVisInfo = NULL;
  if (myXWindow == 0)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, Unable to create window");
    return;
  }

  // if parent - desktop
  XSizeHints aSizeHints;
  aSizeHints.x      = myXLeft;
  aSizeHints.y      = myYTop;
  aSizeHints.flags  = PPosition;
  aSizeHints.width  = thePxWidth;
  aSizeHints.height = thePxHeight;
  aSizeHints.flags |= PSize;
  XSetStandardProperties (aDisp, myXWindow, theTitle, theTitle, None,
                          NULL, 0, &aSizeHints);

  /*XTextProperty aTitleProperty;
  aTitleProperty.encoding = None;
  char* aTitle = (char* )theTitle;
  Xutf8TextListToTextProperty(aDisp, &aTitle, 1, XUTF8StringStyle, &aTitleProperty);
  XSetWMName      (aDisp, myXWindow, &aTitleProperty);
  XSetWMProperties(aDisp, myXWindow, &aTitleProperty, &aTitleProperty, NULL, 0, NULL, NULL, NULL);*/

  XFlush (aDisp);
}

// =======================================================================
// function : Xw_Window
// purpose  :
// =======================================================================
Xw_Window::Xw_Window (const Handle(Aspect_DisplayConnection)& theXDisplay,
                      const Window theXWin,
                      const Aspect_FBConfig theFBConfig)
: Aspect_Window(),
  myDisplay  (theXDisplay),
  myXWindow  (theXWin),
  myFBConfig (theFBConfig),
  myXLeft    (0),
  myYTop     (0),
  myXRight   (512),
  myYBottom  (512),
  myIsOwnWin (Standard_False)
{
  int aDummy = 0;
  if (theXWin == 0)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, given invalid X window");
    return;
  }
  else if (theXDisplay.IsNull())
  {
    throw Aspect_WindowDefinitionError("Xw_Window, X Display connection is undefined");
    return;
  }
  else if (!glXQueryExtension (myDisplay->GetDisplay(), &aDummy, &aDummy))
  {
    myXWindow = 0;
    throw Aspect_WindowDefinitionError("Xw_Window, GLX extension is unavailable");
    return;
  }

  Display* aDisp = myDisplay->GetDisplay();

  XWindowAttributes aWinAttr;
  XGetWindowAttributes (aDisp, myXWindow, &aWinAttr);
  const int  aScreen      = DefaultScreen (aDisp);
  const long aVisInfoMask = VisualIDMask | VisualScreenMask;
  XVisualInfo aVisInfoTmp;
  aVisInfoTmp.visualid = aWinAttr.visual->visualid;
  aVisInfoTmp.screen   = aScreen;
  int aNbItems = 0;
  XVisualInfo* aVisInfo = XGetVisualInfo (aDisp, aVisInfoMask, &aVisInfoTmp, &aNbItems);
  if (aVisInfo == NULL)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, Visual is unavailable");
    return;
  }
  XFree (aVisInfo);

  DoResize();
}

// =======================================================================
// function : ~Xw_Window
// purpose  :
// =======================================================================
Xw_Window::~Xw_Window()
{
  if (myIsOwnWin && myXWindow != 0 && !myDisplay.IsNull())
  {
    XDestroyWindow (myDisplay->GetDisplay(), myXWindow);
  }
}

// =======================================================================
// function : XWindow
// purpose  :
// =======================================================================
Window Xw_Window::XWindow() const
{
  return myXWindow;
}

// =======================================================================
// function : IsMapped
// purpose  :
// =======================================================================
Standard_Boolean Xw_Window::IsMapped() const
{
  if (myXWindow == 0)
  {
    return false;
  }
  else if (IsVirtual())
  {
    return Standard_True;
  }

  XFlush (myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  XGetWindowAttributes (myDisplay->GetDisplay(), myXWindow, &aWinAttr);
  return aWinAttr.map_state == IsUnviewable
      || aWinAttr.map_state == IsViewable;
}

// =======================================================================
// function : Map
// purpose  :
// =======================================================================
void Xw_Window::Map() const
{
  if (IsVirtual() || myXWindow == 0)
  {
    return;
  }

  XMapWindow (myDisplay->GetDisplay(), myXWindow);
  XFlush (myDisplay->GetDisplay());
}

// =======================================================================
// function : Unmap
// purpose  :
// =======================================================================
void Xw_Window::Unmap() const
{
  if (IsVirtual() || myXWindow == 0)
  {
    return;
  }

  XIconifyWindow (myDisplay->GetDisplay(), myXWindow, DefaultScreen(myDisplay->GetDisplay()));
}

// =======================================================================
// function : DoResize
// purpose  :
// =======================================================================
Aspect_TypeOfResize Xw_Window::DoResize() const
{
  if (myXWindow == 0)
  {
    return Aspect_TOR_UNKNOWN;
  }

  XFlush (myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  XGetWindowAttributes (myDisplay->GetDisplay(), myXWindow, &aWinAttr);
  if (aWinAttr.map_state == IsUnmapped)
  {
    return Aspect_TOR_UNKNOWN;
  }

  Standard_Integer aMask = 0;
  Aspect_TypeOfResize aMode = Aspect_TOR_UNKNOWN;

  if (Abs (aWinAttr.x                     - myXLeft  ) > 2) aMask |= 1;
  if (Abs ((aWinAttr.x + aWinAttr.width)  - myXRight ) > 2) aMask |= 2;
  if (Abs (aWinAttr.y                     - myYTop   ) > 2) aMask |= 4;
  if (Abs ((aWinAttr.y + aWinAttr.height) - myYBottom) > 2) aMask |= 8;
  switch (aMask)
  {
    case 0:  aMode = Aspect_TOR_NO_BORDER;               break;
    case 1:  aMode = Aspect_TOR_LEFT_BORDER;             break;
    case 2:  aMode = Aspect_TOR_RIGHT_BORDER;            break;
    case 4:  aMode = Aspect_TOR_TOP_BORDER;              break;
    case 5:  aMode = Aspect_TOR_LEFT_AND_TOP_BORDER;     break;
    case 6:  aMode = Aspect_TOR_TOP_AND_RIGHT_BORDER;    break;
    case 8:  aMode = Aspect_TOR_BOTTOM_BORDER;           break;
    case 9:  aMode = Aspect_TOR_BOTTOM_AND_LEFT_BORDER;  break;
    case 10: aMode = Aspect_TOR_RIGHT_AND_BOTTOM_BORDER; break;
    default: break;
  }

  *((Standard_Integer* )&myXLeft   ) = aWinAttr.x;
  *((Standard_Integer* )&myXRight  ) = aWinAttr.x + aWinAttr.width;
  *((Standard_Integer* )&myYTop    ) = aWinAttr.y;
  *((Standard_Integer* )&myYBottom ) = aWinAttr.y + aWinAttr.height;
  return aMode;
}

// =======================================================================
// function : DoMapping
// purpose  :
// =======================================================================
Standard_Boolean Xw_Window::DoMapping() const
{
  return Standard_True; // IsMapped()
}

// =======================================================================
// function : Ratio
// purpose  :
// =======================================================================
Standard_Real Xw_Window::Ratio() const
{
  if (myXWindow == 0)
  {
    return 1.0;
  }

  XFlush (myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  XGetWindowAttributes (myDisplay->GetDisplay(), myXWindow, &aWinAttr);
  return Standard_Real(aWinAttr.width) / Standard_Real(aWinAttr.height);
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void Xw_Window::Position (Standard_Integer& X1, Standard_Integer& Y1,
                          Standard_Integer& X2, Standard_Integer& Y2) const
{
  if (myXWindow == 0)
  {
    return;
  }

  XFlush (myDisplay->GetDisplay());
  XWindowAttributes anAttributes;
  XGetWindowAttributes (myDisplay->GetDisplay(), myXWindow, &anAttributes);
  Window aChild;
  XTranslateCoordinates (myDisplay->GetDisplay(), anAttributes.root, myXWindow,
                         0, 0, &anAttributes.x, &anAttributes.y, &aChild);

  X1 = -anAttributes.x;
  X2 = X1 + anAttributes.width;
  Y1 = -anAttributes.y;
  Y2 = Y1 + anAttributes.height;
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void Xw_Window::Size (Standard_Integer& theWidth,
                      Standard_Integer& theHeight) const
{
  if (myXWindow == 0)
  {
    return;
  }

  XFlush (myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  XGetWindowAttributes (myDisplay->GetDisplay(), myXWindow, &aWinAttr);
  theWidth  = aWinAttr.width;
  theHeight = aWinAttr.height;
}

#endif //  Win32 or Mac OS X
