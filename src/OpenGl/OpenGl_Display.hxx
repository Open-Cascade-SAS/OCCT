// File:      OpenGl_Display.hxx
// Created:   20 September 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef _OpenGl_Display_Header
#define _OpenGl_Display_Header

#include <Handle_OpenGl_Display.hxx>
#include <MMgt_TShared.hxx>

#include <Standard_CString.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <NCollection_DataMap.hxx>

#include <Aspect_Display.hxx>
#include <Aspect_Drawable.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>

#include <OSD_FontAspect.hxx>

#include <InterfaceGraphic_telem.hxx>

#include <Handle_OpenGl_Window.hxx>

struct OpenGl_Facilities
{
  int AntiAliasing;
  int DepthCueing;
  int DoubleBuffer;
  int ZBuffer;
  int MaxLights;
  int MaxViews;
};

struct OPENGL_MARKER_DATA
{
  unsigned int ListId;
  unsigned int Width;
  unsigned int Height;
  unsigned char* Array;
  IMPLEMENT_MEMORY_OPERATORS
};

typedef NCollection_DataMap<int,OPENGL_MARKER_DATA> OpenGl_MapOfUserMarker;

class OpenGl_AspectText;
struct OpenGl_TextParam;

class OpenGl_Display : public MMgt_TShared
{
 public:
  OpenGl_Display (const Standard_CString ADisplay);
  OpenGl_Display (const Aspect_Display ADisplay);
  virtual ~OpenGl_Display ();

  Aspect_Display GetDisplay () const { return myDisplay; }

  Handle(OpenGl_Window) GetWindow (const Aspect_Drawable AParent) const;
  void SetWindow (const Aspect_Drawable AParent, const Handle(OpenGl_Window) &AWindow);

  const OpenGl_Facilities & Facilities () const { return myFacilities; }

  Standard_Boolean DBuffer () const { return myDBuffer; }
  Standard_Boolean Dither () const { return myDither; }
  Standard_Boolean BackDither () const { return myBackDither; }
  Standard_Boolean Walkthrough () const { return myWalkthrough; }
  Standard_Boolean SymPerspective() const { return mySymPerspective; }
  Standard_Boolean PolygonOffset (Standard_ShortReal &AFactor, Standard_ShortReal &AUnits) const
  {
    AFactor = myOffsetFactor;
    AUnits = myOffsetUnits;
    return Standard_True;
  }
  Standard_Integer AntiAliasingMode () const { return myAntiAliasingMode; }

  // System attributes

  void InitAttributes ();

  const char * GetStringForMarker (const Aspect_TypeOfMarker AType, const Tfloat AVal) const;

  void SetBaseForMarker () const;

  void SetTypeOfLine (const Aspect_TypeOfLine AType) const;

  void SetTypeOfHatch (const int AType) const;

  // User markers

  void AddUserMarker (const Standard_Integer AIndex,
                      const Standard_Integer AMarkWidth,
                      const Standard_Integer AMarkHeight,
                      const Handle(TColStd_HArray1OfByte)& ATexture);

  void UpdateUserMarkers ();

  Standard_Integer GetUserMarkerListIndex (const Standard_Integer AIndex) const;

  // Fonts

  int FindFont (const char* AFontName, const OSD_FontAspect AFontAspect, const int ABestSize = -1, const float AXScale = 1.F, const float AYScale = 1.F);

  void StringSize (const wchar_t *text, int &width, int &ascent, int &descent);

  void RenderText (const wchar_t *text, const int is2d, const float x, const float y, const float z, const OpenGl_AspectText *aspect, const OpenGl_TextParam *param);

  friend class OpenGl_Window;

  // Type definition
  //
  DEFINE_STANDARD_RTTI(OpenGl_Display)

 protected:

  void Init ();

  void ExportText (const wchar_t *text, const int is2d, const float x, const float y, const float z, const OpenGl_AspectText *aspect, const OpenGl_TextParam *param, const float height);

#ifdef HAVE_GL2PS
  static void getGL2PSFontName(const char *src_font, char *ps_font);
#endif

  Aspect_Display   myDisplay;
  OpenGl_Facilities myFacilities;

  Standard_Boolean myDBuffer;
  Standard_Boolean myDither;
  Standard_Boolean myBackDither;
  Standard_Boolean myWalkthrough;
  Standard_Boolean mySymPerspective;
  Standard_ShortReal myOffsetFactor;
  Standard_ShortReal myOffsetUnits;
  Standard_Integer myAntiAliasingMode;

#if (defined(_WIN32) || defined(__WIN32__))
  NCollection_DataMap<Aspect_Drawable,  Handle(OpenGl_Window)> myMapOfWindows;
#else
  NCollection_DataMap<Standard_Integer, Handle(OpenGl_Window)> myMapOfWindows;
#endif

  unsigned int myLinestyleBase;
  unsigned int myPatternBase;
  unsigned int myMarkerBase;

  OpenGl_MapOfUserMarker myMapOfUM;

  Standard_Integer myFont;
  Standard_Integer myFontSize;

 public:
  IMPLEMENT_MEMORY_OPERATORS
};

extern Handle(OpenGl_Display) openglDisplay;

#endif //OpenGl_Workspace_Header
