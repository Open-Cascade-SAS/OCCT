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


#include <InterfaceGraphic.hxx>

#include <OpenGl_Window.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_Display.hxx>
#include <OpenGl_ResourceCleaner.hxx>
#include <OpenGl_ResourceTexture.hxx>

#include <Aspect_GraphicDeviceDefinitionError.hxx>
#include <TCollection_AsciiString.hxx>

#include <GL/glu.h> // gluOrtho2D()

IMPLEMENT_STANDARD_HANDLE(OpenGl_Window,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Window,MMgt_TShared)

namespace
{
  static const TEL_COLOUR THE_DEFAULT_BG_COLOR = { { 0.F, 0.F, 0.F, 1.F } };

  static GLCONTEXT ThePreviousCtx = 0; // to share GL resources
#if (!defined(_WIN32) && !defined(__WIN32__))
  static GLXContext TheDeadGlxCtx; // Context to be destroyed
  static Display*   TheDeadGlxDpy; // Display associated with TheDeadGlxCtx
#endif
  
#if (defined(_WIN32) || defined(__WIN32__))
  static int find_pixel_format (HDC hDC, PIXELFORMATDESCRIPTOR* pfd, const Standard_Boolean dbuff)
  {
    PIXELFORMATDESCRIPTOR pfd0;
    memset (&pfd0, 0, sizeof (PIXELFORMATDESCRIPTOR));
    pfd0.nSize           = sizeof (PIXELFORMATDESCRIPTOR);
    pfd0.nVersion        = 1;
    pfd0.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | (dbuff ? PFD_DOUBLEBUFFER : PFD_SUPPORT_GDI);
    pfd0.iPixelType      = PFD_TYPE_RGBA;
    pfd0.iLayerType      = PFD_MAIN_PLANE;

    int       iPixelFormat = 0;
    int       iGood = 0;
    const int cBits[] = { 32, 24 };
    const int dBits[] = { 32, 24, 16 };

    int i, j;
    for (i = 0; i < sizeof(dBits) / sizeof(int); i++)
    {
      pfd0.cDepthBits = dBits[i];
      iGood = 0;
      for (j = 0; j < sizeof(cBits) / sizeof(int); j++)
      {
        pfd0.cColorBits = cBits[j];
        iPixelFormat = ChoosePixelFormat (hDC, &pfd0);
        if (iPixelFormat)
        {
          pfd->cDepthBits = 0;
          pfd->cColorBits = 0;
          DescribePixelFormat (hDC, iPixelFormat, sizeof (PIXELFORMATDESCRIPTOR), pfd);
          if (pfd->cColorBits >= cBits[j] && pfd->cDepthBits >= dBits[i])
            break;
          if (iGood == 0)
            iGood = iPixelFormat;
        }
      }
      if (j < sizeof(cBits) / sizeof(int))
        break;
    }

    if (iPixelFormat == 0)
      iPixelFormat = iGood;

    return iPixelFormat;
  }
#else
  static Bool WaitForNotify (Display* theDisp, XEvent* theEv, char* theArg)
  {
    return (theEv->type == MapNotify) && (theEv->xmap.window == (Window )theArg);
  }
#endif

};

// =======================================================================
// function : OpenGl_Window
// purpose  :
// =======================================================================
OpenGl_Window::OpenGl_Window (const Handle(OpenGl_Display)& theDisplay,
                              const CALL_DEF_WINDOW&        theCWindow,
                              Aspect_RenderingContext       theGContext)
: myDisplay (theDisplay),
  myWindow (0),
  myGContext ((GLCONTEXT )theGContext),
  myGlContext (new OpenGl_Context()),
  myOwnGContext (theGContext == 0),
#if (defined(_WIN32) || defined(__WIN32__))
  myWindowDC (0),
  mySysPalInUse (FALSE),
#endif
  myWidth ((Standard_Integer )theCWindow.dx),
  myHeight ((Standard_Integer )theCWindow.dy),
  myBgColor (THE_DEFAULT_BG_COLOR),
  myDither (theDisplay->Dither()),
  myBackDither (theDisplay->BackDither())
{
  myBgColor.rgb[0] = theCWindow.Background.r;
  myBgColor.rgb[1] = theCWindow.Background.g;
  myBgColor.rgb[2] = theCWindow.Background.b;

  WINDOW aParent = (WINDOW )theCWindow.XWindow;
  DISPLAY* aDisp = (DISPLAY* )myDisplay->GetDisplay();

#if (!defined(_WIN32) && !defined(__WIN32__))
  XWindowAttributes wattr;
  XGetWindowAttributes (aDisp, aParent, &wattr);
  const int scr = DefaultScreen (aDisp);

  XVisualInfo* aVis = NULL;
  {
    unsigned long aVisInfoMask = VisualIDMask | VisualScreenMask;
    XVisualInfo aVisInfo;
    aVisInfo.visualid = wattr.visual->visualid;
    aVisInfo.screen   = scr;
    int aNbItems;
    aVis = XGetVisualInfo (aDisp, aVisInfoMask, &aVisInfo, &aNbItems);
  }

  WINDOW win;

  if (!myOwnGContext)
  {
    if (aVis != NULL)
    {
      Aspect_GraphicDeviceDefinitionError::Raise ("OpenGl_Window::CreateWindow: XGetVisualInfo failed.");
      return;
    }

    win = aParent;
  }
  else
  {
    GLCONTEXT ctx;

  #if defined(__linux) || defined(Linux)
    if (aVis != NULL)
    {
      // check Visual for OpenGl context's parameters compability
      int isGl = 0, isDoubleBuffer = 0, isRGBA = 0, aDepthSize = 0;

      if (glXGetConfig (aDisp, aVis, GLX_USE_GL, &isGl) != 0)
        isGl = 0;

      if (glXGetConfig (aDisp, aVis, GLX_RGBA, &isRGBA) != 0)
        isRGBA = 0;

      if (glXGetConfig (aDisp, aVis, GLX_DOUBLEBUFFER, &isDoubleBuffer) != 0)
        isDoubleBuffer = 0;

      if (glXGetConfig (aDisp, aVis, GLX_DEPTH_SIZE, &aDepthSize) != 0)
        aDepthSize = 0;

      if (!isGl || !aDepthSize || !isRGBA  || (isDoubleBuffer ? 1 : 0) != (myDisplay->DBuffer()? 1 : 0))
      {
        XFree (aVis);
        aVis = NULL;
      }
    }
  #endif

    if (aVis == NULL)
    {
      int anIter = 0;
      int anAttribs[11];
      anAttribs[anIter++] = GLX_RGBA;

      anAttribs[anIter++] = GLX_DEPTH_SIZE;
      anAttribs[anIter++] = 1;

      anAttribs[anIter++] = GLX_RED_SIZE;
      anAttribs[anIter++] = (wattr.depth <= 8) ? 0 : 1;

      anAttribs[anIter++] = GLX_GREEN_SIZE;
      anAttribs[anIter++] = (wattr.depth <= 8) ? 0 : 1;

      anAttribs[anIter++] = GLX_BLUE_SIZE;
      anAttribs[anIter++] = (wattr.depth <= 8) ? 0 : 1;

      if (myDisplay->DBuffer())
        anAttribs[anIter++] = GLX_DOUBLEBUFFER;

      anAttribs[anIter++] = None;

      aVis = glXChooseVisual (aDisp, scr, anAttribs);
      if (aVis == NULL)
      {
        Aspect_GraphicDeviceDefinitionError::Raise ("OpenGl_Window::CreateWindow: glXChooseVisual failed.");
        return;
      }
    }

    if (TheDeadGlxCtx)
    {
      // recover display lists from TheDeadGlxCtx, then destroy it
      ctx = glXCreateContext (aDisp, aVis, TheDeadGlxCtx, GL_TRUE);

      OpenGl_ResourceCleaner::GetInstance()->RemoveContext (TheDeadGlxCtx);
      glXDestroyContext (TheDeadGlxDpy, TheDeadGlxCtx);

      TheDeadGlxCtx = 0;
    }
    else if (ThePreviousCtx == 0)
    {
      ctx = glXCreateContext (aDisp, aVis, NULL, GL_TRUE);
    }
    else
    {
      // ctx est une copie du previous
      ctx = glXCreateContext (aDisp, aVis, ThePreviousCtx, GL_TRUE);
    }

    if (!ctx)
    {
      Aspect_GraphicDeviceDefinitionError::Raise ("OpenGl_Window::CreateWindow: glXCreateContext failed.");
      return;
    }

    OpenGl_ResourceCleaner::GetInstance()->AppendContext (ctx, true);

    ThePreviousCtx = ctx;

    Colormap cmap = XCreateColormap (aDisp, aParent, aVis->visual, AllocNone);

    XColor color;
    color.red   = (unsigned short) (myBgColor.rgb[0] * 0xFFFF);
    color.green = (unsigned short) (myBgColor.rgb[1] * 0xFFFF);
    color.blue  = (unsigned short) (myBgColor.rgb[2] * 0xFFFF);
    color.flags = DoRed | DoGreen | DoBlue;
    XAllocColor (aDisp, cmap, &color);

    XSetWindowAttributes cwa;
    cwa.colormap         = cmap;
    cwa.event_mask       = StructureNotifyMask;
    cwa.border_pixel     = color.pixel;
    cwa.background_pixel = color.pixel;

    if (aVis->visualid == wattr.visual->visualid)
    {
      win = aParent;
    }
    else
    {
      unsigned long mask = CWBackPixel | CWColormap | CWBorderPixel | CWEventMask;
      win = XCreateWindow (aDisp, aParent, 0, 0, myWidth, myHeight, 0/*bw*/, aVis->depth, InputOutput, aVis->visual, mask, &cwa);
    }

    XSetWindowBackground (aDisp, win, cwa.background_pixel);
    XClearWindow (aDisp, win);

    if (win != aParent)
    {
      XEvent anEvent;
      XMapWindow (aDisp, win);
      XIfEvent (aDisp, &anEvent, WaitForNotify, (char* )win);
    }

    myGContext = ctx;
  }

  /*
  * Le BackDitherProp est utilise pour le clear du background
  * Pour eviter une difference de couleurs avec la couleur choisie
  * par l'application (XWindow) il faut desactiver le dithering
  * au dessus de 8 plans.
  *
  * Pour le DitherProp:
  * On cherchera a activer le Dithering que si le Visual a au moins
  * 8 plans pour le GLX_RED_SIZE. Le test est plus sur car on peut
  * avoir une profondeur superieure a 12 mais avoir besoin du dithering.
  * (Carte Impact avec GLX_RED_SIZE a 5 par exemple)
  */

  int value;
  glXGetConfig (aDisp, aVis, GLX_RED_SIZE, &value);

  if (myDither)
    myDither = (value < 8);

  if (myBackDither)
    myBackDither = (aVis->depth <= 8);

  XFree ((char* )aVis);

  myWindow = win;

#else

  myWindowDC = GetDC (aParent);

  PIXELFORMATDESCRIPTOR pfd;
  int iPixelFormat = find_pixel_format (myWindowDC, &pfd, myDisplay->DBuffer());
  if (iPixelFormat == 0)
  {
    ReleaseDC (aParent, myWindowDC);
    myWindowDC = 0;

    TCollection_AsciiString msg ("OpenGl_Window::CreateWindow: ChoosePixelFormat failed. Error code: ");
    msg += (int )GetLastError();
    Aspect_GraphicDeviceDefinitionError::Raise (msg.ToCString());
    return;
  }

  if (pfd.dwFlags & PFD_NEED_PALETTE)
  {
    WINDOW_DATA* wd = (WINDOW_DATA* )GetWindowLongPtr (aParent, GWLP_USERDATA);

    mySysPalInUse = (pfd.dwFlags & PFD_NEED_SYSTEM_PALETTE) ? TRUE : FALSE;
    InterfaceGraphic_RealizePalette (myWindowDC, wd->hPal, FALSE, mySysPalInUse);
  }

  if (myDither)
    myDither = (pfd.cColorBits <= 8);

  if (myBackDither)
    myBackDither = (pfd.cColorBits <= 8);

  if (!SetPixelFormat (myWindowDC, iPixelFormat, &pfd))
  {
    ReleaseDC (aParent, myWindowDC);
    myWindowDC = NULL;

    TCollection_AsciiString msg("OpenGl_Window::CreateWindow: SetPixelFormat failed. Error code: ");
    msg += (int)GetLastError();
    Aspect_GraphicDeviceDefinitionError::Raise (msg.ToCString());
    return;
  }

  if (!myOwnGContext)
  {
    ThePreviousCtx = myGContext;
  }
  else
  {
    myGContext = wglCreateContext (myWindowDC);
    if (myGContext == NULL)
    {
      ReleaseDC (aParent, myWindowDC);
      myWindowDC = NULL;

      TCollection_AsciiString msg ("OpenGl_Window::CreateWindow: wglCreateContext failed. Error code: ");
      msg += (int )GetLastError();
      Aspect_GraphicDeviceDefinitionError::Raise (msg.ToCString());
      return;
    }

    Standard_Boolean isShared = Standard_True;
    if (ThePreviousCtx == NULL)
    {
      ThePreviousCtx = myGContext;
    }
    else
    {
      // if we already have some shared context
      GLCONTEXT shareCtx = OpenGl_ResourceCleaner::GetInstance()->GetSharedContext();
      if (shareCtx != NULL)
      {
        // try to share context with one from resource cleaner list
        isShared = (Standard_Boolean )wglShareLists (shareCtx, myGContext);
      }
      else
      {
        isShared = (Standard_Boolean )wglShareLists (ThePreviousCtx, myGContext);
        // add shared ThePreviousCtx to a control list if it's not there
        if (isShared)
          OpenGl_ResourceCleaner::GetInstance()->AppendContext (ThePreviousCtx, isShared);
      }
    }

    // add the context to OpenGl_ResourceCleaner control list
    OpenGl_ResourceCleaner::GetInstance()->AppendContext (myGContext, isShared);
  }

  myWindow = aParent;
#endif

  Init();
  myGlContext->Init();
}

// =======================================================================
// function : ~OpenGl_Window
// purpose  :
// =======================================================================
OpenGl_Window::~OpenGl_Window()
{
  DISPLAY* aDisp = (DISPLAY* )myDisplay->GetDisplay();
  if (aDisp == NULL || !myOwnGContext)
    return;

#if (defined(_WIN32) || defined(__WIN32__))
  OpenGl_ResourceCleaner::GetInstance()->RemoveContext (myGContext);

  if (wglGetCurrentContext() != NULL)
    wglDeleteContext (myGContext);
  ReleaseDC (myWindow, myWindowDC);

  if (myDisplay->myMapOfWindows.Size() == 0)
    ThePreviousCtx = 0;
#else
  // FSXXX sync necessary if non-direct rendering
  glXWaitGL();

  if (ThePreviousCtx == myGContext)
  {
    ThePreviousCtx = NULL;
    if (myDisplay->myMapOfWindows.Size() > 0)
    {
      NCollection_DataMap<Standard_Integer, Handle(OpenGl_Window)>::Iterator it (myDisplay->myMapOfWindows);
      ThePreviousCtx = it.Value()->myGContext;
    }

    // if this is the last remaining context, do not destroy it yet, to avoid
    // losing any shared display lists (fonts...)
    if (ThePreviousCtx)
    {
      OpenGl_ResourceCleaner::GetInstance()->RemoveContext(myGContext);
      glXDestroyContext(aDisp, myGContext);
    }
    else
    {
      TheDeadGlxCtx = myGContext;
      TheDeadGlxDpy = aDisp;
    }
  }
  else
  {
    OpenGl_ResourceCleaner::GetInstance()->RemoveContext (myGContext);
    glXDestroyContext (aDisp, myGContext);
  }
#endif
}

// =======================================================================
// function : Activate
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Window::Activate()
{
  DISPLAY* aDisp = (DISPLAY* )myDisplay->GetDisplay();
  if (aDisp == NULL)
    return Standard_False;

#if (defined(_WIN32) || defined(__WIN32__))
  if (!wglMakeCurrent (myWindowDC, myGContext))
  {
    //GLenum errorcode = glGetError();
    //const GLubyte *errorstring = gluErrorString(errorcode);
    //printf("wglMakeCurrent failed: %d %s\n", errorcode, errorstring);
    return Standard_False;
  }
#else
  if (!glXMakeCurrent (aDisp, myWindow, myGContext))
  {
    // if there is no current context it might be impossible to use glGetError correctly
    //printf("glXMakeCurrent failed!\n");
    return Standard_False;
  }
#endif

  return Standard_True;
}

// =======================================================================
// function : Resize
// purpose  : call_subr_resize
// =======================================================================
void OpenGl_Window::Resize (const CALL_DEF_WINDOW& theCWindow)
{
  DISPLAY* aDisp = (DISPLAY* )myDisplay->GetDisplay();
  if (aDisp == NULL)
    return;

  // If the size is not changed - do nothing
  if ((myWidth == theCWindow.dx) && (myHeight == theCWindow.dy))
    return;

  myWidth  = (Standard_Integer )theCWindow.dx;
  myHeight = (Standard_Integer )theCWindow.dy;

#if (!defined(_WIN32) && !defined(__WIN32__))
  XResizeWindow (aDisp, myWindow, (unsigned int )myWidth, (unsigned int )myHeight);
  XSync (aDisp, False);
#endif

  Init();
}

// =======================================================================
// function : ReadDepths
// purpose  : TelReadDepths
// =======================================================================
void OpenGl_Window::ReadDepths (const Standard_Integer theX,     const Standard_Integer theY,
                                const Standard_Integer theWidth, const Standard_Integer theHeight,
                                float* theDepths)
{
  if (theDepths == NULL || !Activate())
    return;

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D (0.0, (GLdouble )myWidth, 0.0, (GLdouble )myHeight);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();

  glRasterPos2i (theX, theY);
  DisableFeatures();
  glReadPixels (theX, theY, theWidth, theHeight, GL_DEPTH_COMPONENT, GL_FLOAT, theDepths);
  EnableFeatures();
}

// =======================================================================
// function : SetBackgroundColor
// purpose  : call_subr_set_background
// =======================================================================
void OpenGl_Window::SetBackgroundColor (const Standard_ShortReal theR,
                                        const Standard_ShortReal theG,
                                        const Standard_ShortReal theB)
{
  myBgColor.rgb[0] = theR;
  myBgColor.rgb[1] = theG;
  myBgColor.rgb[2] = theB;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Window::Init()
{
  if (!Activate())
    return;

#if (defined(_WIN32) || defined(__WIN32__))
  RECT cr;
  GetClientRect (myWindow, &cr);
  myWidth  = cr.right - cr.left;
  myHeight = cr.bottom - cr.top;
#else
  Window aRootWin;
  int aDummy;
  unsigned int aDummyU;
  unsigned int aNewWidth  = 0;
  unsigned int aNewHeight = 0;
  DISPLAY* aDisp = (DISPLAY* )myDisplay->GetDisplay();
  XGetGeometry (aDisp, myWindow, &aRootWin, &aDummy, &aDummy, &aNewWidth, &aNewHeight, &aDummyU, &aDummyU);
  myWidth  = aNewWidth;
  myHeight = aNewHeight;
#endif

  glMatrixMode (GL_MODELVIEW);
  glViewport (0, 0, myWidth, myHeight);

  glDisable (GL_SCISSOR_TEST);
  glDrawBuffer (GL_BACK);
}

// =======================================================================
// function : EnablePolygonOffset
// purpose  : call_subr_enable_polygon_offset
// =======================================================================
void OpenGl_Window::EnablePolygonOffset() const
{
  Standard_ShortReal aFactor, aUnits;
  myDisplay->PolygonOffset (aFactor, aUnits);
  glPolygonOffset (aFactor, aUnits);
  glEnable (GL_POLYGON_OFFSET_FILL);
}

// =======================================================================
// function : DisablePolygonOffset
// purpose  : call_subr_disable_polygon_offset
// =======================================================================
void OpenGl_Window::DisablePolygonOffset() const
{
  glDisable (GL_POLYGON_OFFSET_FILL);
}

// =======================================================================
// function : EnableFeatures
// purpose  :
// =======================================================================
void OpenGl_Window::EnableFeatures() const
{
  /*glPixelTransferi (GL_MAP_COLOR, GL_TRUE);*/

  if (myDither)
    glEnable (GL_DITHER);
  else
    glDisable (GL_DITHER);
}

// =======================================================================
// function : DisableFeatures
// purpose  :
// =======================================================================
void OpenGl_Window::DisableFeatures() const
{
  glDisable (GL_DITHER);
  glPixelTransferi (GL_MAP_COLOR, GL_FALSE);

  /*
  * Disable stuff that's likely to slow down glDrawPixels.
  * (Omit as much of this as possible, when you know in advance
  * that the OpenGL state will already be set correctly.)
  */
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_FOG);
  glDisable(GL_LIGHTING);

  glDisable(GL_LOGIC_OP);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);
  glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
  glPixelTransferi(GL_RED_SCALE, 1);
  glPixelTransferi(GL_RED_BIAS, 0);
  glPixelTransferi(GL_GREEN_SCALE, 1);
  glPixelTransferi(GL_GREEN_BIAS, 0);
  glPixelTransferi(GL_BLUE_SCALE, 1);
  glPixelTransferi(GL_BLUE_BIAS, 0);
  glPixelTransferi(GL_ALPHA_SCALE, 1);
  glPixelTransferi(GL_ALPHA_BIAS, 0);

  /*
  * Disable extensions that could slow down glDrawPixels.
  * (Actually, you should check for the presence of the proper
  * extension before making these calls.  I've omitted that
  * code for simplicity.)
  */

#ifdef GL_EXT_convolution
  glDisable(GL_CONVOLUTION_1D_EXT);
  glDisable(GL_CONVOLUTION_2D_EXT);
  glDisable(GL_SEPARABLE_2D_EXT);
#endif

#ifdef GL_EXT_histogram
  glDisable(GL_HISTOGRAM_EXT);
  glDisable(GL_MINMAX_EXT);
#endif

#ifdef GL_EXT_texture3D
  glDisable(GL_TEXTURE_3D_EXT);
#endif
}

// =======================================================================
// function : MakeFrontBufCurrent
// purpose  : TelMakeFrontBufCurrent
// =======================================================================
void OpenGl_Window::MakeFrontBufCurrent() const
{
  glDrawBuffer (GL_FRONT);
}

// =======================================================================
// function : MakeBackBufCurrent
// purpose  : TelMakeBackBufCurrent
// =======================================================================
void OpenGl_Window::MakeBackBufCurrent() const
{
  glDrawBuffer (GL_BACK);
}

// =======================================================================
// function : MakeFrontAndBackBufCurrent
// purpose  : TelMakeFrontAndBackBufCurrent
// =======================================================================
void OpenGl_Window::MakeFrontAndBackBufCurrent() const
{
  glDrawBuffer (GL_FRONT_AND_BACK);
}
