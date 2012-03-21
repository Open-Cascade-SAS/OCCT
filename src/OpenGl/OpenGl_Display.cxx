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

#include <OpenGl_Display.hxx>

#include <OSD_Environment.hxx>
#include <TCollection_AsciiString.hxx>
#include <Aspect_GraphicDeviceDefinitionError.hxx>

#include <OpenGl_Light.hxx>

#if (!defined(_WIN32) && !defined(__WIN32__))
  #include <X11/Xlib.h> // XOpenDisplay()
#endif

IMPLEMENT_STANDARD_HANDLE(OpenGl_Display,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Display,MMgt_TShared)

Handle(OpenGl_Display) openglDisplay;

namespace
{
  #if (defined(_WIN32) || defined(__WIN32__))
    static char* TheDummyDisplay = "DISPLAY";
  #endif

  static const OpenGl_Facilities myDefaultFacilities = { 1, 1, 1, 1, OpenGLMaxLights, 10000 };
};

/*----------------------------------------------------------------------*/

OpenGl_Display::OpenGl_Display (const Standard_CString theDisplay)
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
  myPatternBase(0),
  myMarkerBase(0),
  myFont(-1),
  myFontSize(-1)
{
#if (defined(_WIN32) || defined(__WIN32__))
  myDisplay = TheDummyDisplay;
#else
  if (theDisplay != NULL && *theDisplay != '\0')
  {
    OSD_Environment aDispEnv ("DISPLAY", theDisplay);
    aDispEnv.Build();
  }

  // Specifies the hardware display name, which determines the
  // display and communications domain to be used.
  // On a POSIX system, if the display_name is NULL, it defaults
  // to the value of the DISPLAY environment variable.
  myDisplay = XOpenDisplay (NULL);
#endif

  Init();
}

/*----------------------------------------------------------------------*/

OpenGl_Display::OpenGl_Display (const Aspect_Display theDisplay)
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
  myPatternBase(0),
  myMarkerBase(0),
  myFont(-1),
  myFontSize(-1)
{
#if (defined(_WIN32) || defined(__WIN32__))
  myDisplay = TheDummyDisplay;
#else
  myDisplay = theDisplay;
#endif

  Init();
}

/*----------------------------------------------------------------------*/

OpenGl_Display::~OpenGl_Display ()
{
  // Delete line styles
  if (myLinestyleBase)
  {
    glDeleteLists((GLuint)myLinestyleBase,5);
    myLinestyleBase = 0;
  }
  // Delete surface patterns
  if (myPatternBase)
  {
    glDeleteLists((GLuint)myPatternBase,TEL_HS_USER_DEF_START);
    myPatternBase = 0;
  }
  // Delete markers
  if (myMarkerBase)
  {
    glDeleteLists((GLuint)myMarkerBase,60);
    myMarkerBase = 0;
  }
  // Delete user markers
  OpenGl_MapOfUserMarker::Iterator itm(myMapOfUM);
  for (; itm.More(); itm.Next())
  {
    const OPENGL_MARKER_DATA &aData = itm.Value();
    if (aData.Array)
    {
      delete[] aData.Array;
    }
    else if (aData.ListId != 0)
    {
      glDeleteLists ( aData.ListId, 1 );
    }
  }
  myDisplay = NULL;
}

/*----------------------------------------------------------------------*/

Handle(OpenGl_Window) OpenGl_Display::GetWindow (const Aspect_Drawable AParent) const
{
  Handle(OpenGl_Window) aWindow;
  if ( myMapOfWindows.IsBound( AParent ) )
  {
    aWindow = myMapOfWindows.Find( AParent );
  }
  return aWindow;
}

/*----------------------------------------------------------------------*/

void OpenGl_Display::SetWindow (const Aspect_Drawable AParent, const Handle(OpenGl_Window) &AWindow)
{
  if ( !myMapOfWindows.IsBound( AParent ) )
  {
    myMapOfWindows.Bind( AParent, AWindow );
  }
}

/*----------------------------------------------------------------------*/

//GenerateMarkerBitmap
void OpenGl_Display::AddUserMarker (const Standard_Integer AIndex,
                                   const Standard_Integer AMarkWidth,
                                   const Standard_Integer AMarkHeight,
                                   const Handle(TColStd_HArray1OfByte)& ATexture)
{
  if (!myMapOfUM.IsBound(AIndex))
  {
    const OPENGL_MARKER_DATA anEmptyData = { 0, 0, 0, NULL };
    myMapOfUM.Bind(AIndex,anEmptyData);
  }

  OPENGL_MARKER_DATA &aData = myMapOfUM.ChangeFind(AIndex);

  if (aData.Array)
  {
    delete[] aData.Array;
    aData.Array = NULL;
  }

  unsigned char *anArray = new unsigned char[ATexture->Length()];

  const int aByteWidth = AMarkWidth / 8;
  int i, anIndex = ATexture->Upper() - ATexture->Lower() - aByteWidth + 1;
  for ( ; anIndex >= 0; anIndex -= aByteWidth )
    for ( i = 0; i < aByteWidth; i++ )
      anArray[ATexture->Upper() - ATexture->Lower() - aByteWidth + 1 - anIndex + i ] = ATexture->Value( anIndex + i + 1 );

  aData.Width = AMarkWidth;
  aData.Height = AMarkHeight;
  aData.Array = anArray;
}

/*----------------------------------------------------------------------*/

void OpenGl_Display::UpdateUserMarkers ()
{
  OpenGl_MapOfUserMarker::Iterator itm(myMapOfUM);
  for (; itm.More(); itm.Next())
  {
    OPENGL_MARKER_DATA &aData = itm.ChangeValue();
    if (aData.Array)
    {
      if (aData.ListId == 0)
        aData.ListId = glGenLists(1);

      glNewList( (GLuint)aData.ListId, GL_COMPILE );

      GLint w = ( GLsizei ) aData.Width;
      GLint h = ( GLsizei ) aData.Height;
      glBitmap( w, h,
                0.5F * ( float )aData.Width, 0.5F * ( float )aData.Height,
                ( float )30.0, ( float )30.0,
                ( GLubyte* )aData.Array );

      glEndList();

      delete[] aData.Array;
      aData.Array = NULL;
    }
  }
}

/*----------------------------------------------------------------------*/

Standard_Integer OpenGl_Display::GetUserMarkerListIndex (const Standard_Integer AIndex) const
{
  if (myMapOfUM.IsBound(AIndex))
  {
    const OPENGL_MARKER_DATA &aData = myMapOfUM.Find(AIndex);
    if (!aData.Array)
      return aData.ListId;
  }
  return -1;
}

/*----------------------------------------------------------------------*/

void OpenGl_Display::Init()
{
  if (myDisplay != NULL)
  {
  #if (!defined(_WIN32) && !defined(__WIN32__))
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
  #if (!defined(_WIN32) && !defined(__WIN32__))
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
