// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_Display.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Light.hxx>

#include <OSD_Environment.hxx>
#include <TCollection_AsciiString.hxx>
#include <Aspect_GraphicDeviceDefinitionError.hxx>

#if (!defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)))
  #include <X11/Xlib.h> // XOpenDisplay()
#endif

IMPLEMENT_STANDARD_HANDLE(OpenGl_Display,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Display,MMgt_TShared)

namespace
{
  #if (defined(_WIN32) || defined(__WIN32__)) || (defined(__APPLE__) && !defined(MACOSX_USE_GLX))
    static char* TheDummyDisplay = "DISPLAY";
  #endif

  static const OpenGl_Facilities myDefaultFacilities = { 1, 1, 1, 1, OpenGLMaxLights, 10000 };
};

/*----------------------------------------------------------------------*/
#if (defined(_WIN32) || defined(__WIN32__)) || (defined(__APPLE__) && !defined(MACOSX_USE_GLX))
OpenGl_Display::OpenGl_Display (const Handle(Aspect_DisplayConnection)& )
#else
OpenGl_Display::OpenGl_Display (const Handle(Aspect_DisplayConnection)& theDisplayConnection)
#endif
: myDisplay(NULL),
  myFacilities(myDefaultFacilities),
  myDBuffer(Standard_True),
  myDither(Standard_True),
  myBackDither(Standard_False),
  myWalkthrough(Standard_False),
  mySymPerspective(Standard_False),
  myOffsetFactor(1.F),
  myOffsetUnits(0.F),
  myAntiAliasingMode(3),
  myLinestyleBase(0),
  myPatternBase(0)
{
#if (defined(_WIN32) || defined(__WIN32__)) || (defined(__APPLE__) && !defined(MACOSX_USE_GLX))
  myDisplay = TheDummyDisplay;
#else
  myDisplay = theDisplayConnection->GetDisplay();
#endif

  Init();
}

/*----------------------------------------------------------------------*/

OpenGl_Display::~OpenGl_Display ()
{
  ReleaseAttributes (NULL);
  myDisplay = NULL;
}

void OpenGl_Display::ReleaseAttributes (const OpenGl_Context* theGlCtx)
{
  // Delete line styles
  if (myLinestyleBase != 0)
  {
    if (theGlCtx->IsValid())
    {
      glDeleteLists ((GLuint )myLinestyleBase, 5);
    }
    myLinestyleBase = 0;
  }
  // Delete surface patterns
  if (myPatternBase != 0)
  {
    if (theGlCtx->IsValid())
    {
      glDeleteLists ((GLuint )myPatternBase, TEL_HS_USER_DEF_START);
    }
    myPatternBase = 0;
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_Display::Init()
{
  if (myDisplay != NULL)
  {
  #if (!defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)))
    XSynchronize ((Display* )myDisplay, (getenv("CALL_SYNCHRO_X") != NULL) ? 1 : 0);

    if (getenv("CSF_GraphicSync") != NULL)
      XSynchronize ((Display* )myDisplay, 1);

    // does the server know about OpenGL & GLX?
    int aDummy;
    if (!XQueryExtension ((Display* )myDisplay, "GLX", &aDummy, &aDummy, &aDummy))
    {
    #ifdef DEBUG
      std::cerr << "This system doesn't appear to support OpenGL\n";
    #endif
    }
  #endif
  }
  else
  {
    TCollection_AsciiString msg("OpenGl_Display::Init");
  #if (!defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)))
    msg += " : Cannot connect to X server ";
    msg += XDisplayName ((char*) NULL);
  #endif
    Aspect_GraphicDeviceDefinitionError::Raise(msg.ToCString());
  }

  if (getenv("CALL_OPENGL_NO_DBF") != NULL)
    myDBuffer = Standard_False;

  if (getenv("CALL_OPENGL_NO_DITHER") != NULL)
    myDither = Standard_False;

  if (getenv("CALL_OPENGL_NO_BACKDITHER") != NULL)
    myBackDither = Standard_False;

  if (getenv("CSF_WALKTHROUGH") != NULL)
    myWalkthrough = Standard_True;

  /* OCC18942: Test if symmetric perspective projection should be turned on */
  if (getenv("CSF_SYM_PERSPECTIVE") != NULL)
    mySymPerspective = Standard_True;

  const char* pvalue = getenv("CALL_OPENGL_POLYGON_OFFSET");
  if (pvalue)
  {
    float v1, v2;
    const int n = sscanf(pvalue, "%f %f", &v1, &v2);
    if (n > 0) myOffsetFactor = v1;
    if (n > 1) myOffsetUnits  = v2;
  }

  pvalue = getenv("CALL_OPENGL_ANTIALIASING_MODE");
  if (pvalue)
  {
    int v;
    if ( sscanf(pvalue,"%d",&v) > 0 ) myAntiAliasingMode = v;
  }
}
