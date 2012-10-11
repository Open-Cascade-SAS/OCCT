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


#ifndef _OpenGl_Workspace_Header
#define _OpenGl_Workspace_Header

#include <Handle_OpenGl_Workspace.hxx>
#include <OpenGl_Window.hxx>

#include <TColStd_Array2OfReal.hxx>
#include <Quantity_Color.hxx>
#include <Graphic3d_TypeOfComposition.hxx>

#include <Graphic3d_CView.hxx>
#include <Aspect_CLayer2d.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_PrintAlgo.hxx>
#include <Graphic3d_PtrFrameBuffer.hxx>
#include <Graphic3d_BufferType.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_tsm.hxx>

#include <OpenGl_Display.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_NamedStatus.hxx>
#include <OpenGl_TextParam.hxx>

#include <Handle_OpenGl_View.hxx>

class OpenGl_AspectLine;
class OpenGl_AspectFace;
class OpenGl_AspectMarker;
class OpenGl_AspectText;
class OpenGl_FrameBuffer;
class OpenGl_Structure;
class Image_PixMap;

//! Reprepsents window with GL context.
//! Provides methods to render primitives and maintan GL state.
class OpenGl_Workspace : public OpenGl_Window
{
public:

  //! Main constructor - prepare GL context for specified window.
  OpenGl_Workspace (const Handle(OpenGl_Display)& theDisplay,
                    const CALL_DEF_WINDOW&        theCWindow,
                    Aspect_RenderingContext       theGContext,
                    const Handle(OpenGl_Context)& theShareCtx);

  //! Destructor
  virtual ~OpenGl_Workspace();

  void SetActiveView (const Handle(OpenGl_View)& theView) { myView = theView; }
  const Handle(OpenGl_View)& ActiveView () const { return myView; }

  //! Redraw the window.
  void Redraw (const Graphic3d_CView& theCView,
               const Aspect_CLayer2d& theCUnderLayer,
               const Aspect_CLayer2d& theCOverLayer);

  //! Deprecated. Simply calls Redraw().
  void Update (const Graphic3d_CView& theCView,
               const Aspect_CLayer2d& theCUnderLayer,
               const Aspect_CLayer2d& theCOverLayer)
  {
    Redraw (theCView, theCUnderLayer, theCOverLayer);
  }

  //! Special method to perform printing.
  //! System-specific and currently only Win platform implemented.
  Standard_Boolean Print (const Graphic3d_CView& theCView, 
                          const Aspect_CLayer2d& theCUnderLayer, 
                          const Aspect_CLayer2d& theCOverLayer,
                          const Aspect_Handle    theHPrintDC,
                          const Standard_Boolean theToShowBackground,
                          const Standard_CString theFileName,
                          const Aspect_PrintAlgo thePrintAlgorithm,
                          const Standard_Real    theScaleFactor);

  void DisplayCallback (const Graphic3d_CView& theCView, int theReason);

  // szvgl: defined in OpenGl_Workspace_1.cxx
  void BeginAnimation (const Standard_Boolean theUseDegeneration,
                       const Standard_Boolean theUpdateAM); 
  void EndAnimation();
  void EraseAnimation();

  Standard_Boolean BeginAddMode();
  void EndAddMode();
  void ClearImmediatMode (const Graphic3d_CView& theCView,
                          const Standard_Boolean theToFlush);
  void RedrawImmediatMode();
  Standard_Boolean BeginImmediatMode (const Graphic3d_CView& theCView,
                                      const Standard_Boolean theUseDepthTest,
                                      const Standard_Boolean theRetainMode);
  void EndImmediatMode();
  void Transform (const TColStd_Array2OfReal& theMatrix, const Graphic3d_TypeOfComposition theType);
  void DrawStructure (const OpenGl_Structure* theStructure);
  void BeginPolyline();
  void EndPolyline();
  void Draw (const Standard_ShortReal theX, const Standard_ShortReal theY, const Standard_ShortReal theZ);
  void Move (const Standard_ShortReal theX, const Standard_ShortReal theY, const Standard_ShortReal theZ);
  void SetLineColor (const Standard_ShortReal theR, const Standard_ShortReal theG, const Standard_ShortReal theB);
  void SetLineType (const Standard_Integer theType);
  void SetLineWidth (const Standard_ShortReal theWidth);
  void SetMinMax (const Standard_ShortReal X1, const Standard_ShortReal Y1, const Standard_ShortReal Z1,
                  const Standard_ShortReal X2, const Standard_ShortReal Y2, const Standard_ShortReal Z2);

  Graphic3d_PtrFrameBuffer FBOCreate (const Standard_Integer theWidth, const Standard_Integer theHeight);
  void FBORelease (Graphic3d_PtrFrameBuffer theFBOPtr);
  Standard_Boolean BufferDump (OpenGl_FrameBuffer*         theFBOPtr,
                               Image_PixMap&               theImage,
                               const Graphic3d_BufferType& theBufferType);

  void UseTransparency (const Standard_Boolean theFlag);
  Standard_Boolean& UseZBuffer()   { return myUseZBuffer; }
  Standard_Boolean& UseDepthTest() { return myUseDepthTest; }
  Standard_Boolean& UseGLLight()   { return myUseGLLight; }

  //// RELATED TO STATUS ////

  Standard_Integer   NamedStatus;

  Standard_Integer   DegenerateModel;
  Standard_ShortReal SkipRatio;

  const TEL_COLOUR* HighlightColor;

  const OpenGl_Matrix* SetViewMatrix (const OpenGl_Matrix* );
  const OpenGl_Matrix* SetStructureMatrix (const OpenGl_Matrix* );

  const OpenGl_AspectLine*   SetAspectLine   (const OpenGl_AspectLine*   theAspect);
  const OpenGl_AspectFace*   SetAspectFace   (const OpenGl_AspectFace*   theAspect);
  const OpenGl_AspectMarker* SetAspectMarker (const OpenGl_AspectMarker* theAspect);
  const OpenGl_AspectText*   SetAspectText   (const OpenGl_AspectText*   theAspect);

  void SetTextParam (const OpenGl_TextParam* theParam) { TextParam_set = theParam; }

  //// THESE METHODS ARE EXPORTED AS THEY PROVIDE STATE INFO TO USERDRAW
  Standard_EXPORT const OpenGl_AspectLine*   AspectLine   (const Standard_Boolean theWithApply);
  Standard_EXPORT const OpenGl_AspectFace*   AspectFace   (const Standard_Boolean theWithApply);
  Standard_EXPORT const OpenGl_AspectMarker* AspectMarker (const Standard_Boolean theWithApply);
  Standard_EXPORT const OpenGl_AspectText*   AspectText   (const Standard_Boolean theWithApply);

  //! Clear the applied aspect state.
  void ResetAppliedAspect();

  //// RELATED TO FONTS ////

  int FindFont (const char*           theFontName,
                const Font_FontAspect theFontAspect,
                const int             theBestSize = -1,
                const float           theXScale = 1.0f,
                const float           theYScale = 1.0f)
  {
    return myDisplay->FindFont (theFontName, theFontAspect, theBestSize, theXScale, theYScale);
  }

  void StringSize (const wchar_t* theText, int& theWidth, int& theAscent, int& theDescent)
  {
    myDisplay->StringSize (theText, theWidth, theAscent, theDescent);
  }

  void RenderText (const wchar_t* theText, const int theIs2d,
                   const float theX, const float theY, const float theZ)
  {
    const OpenGl_AspectText* anAspect = AspectText (Standard_True);
    myDisplay->RenderText (theText, theIs2d, theX, theY, theZ, anAspect, TextParam_applied);
  }

protected:

  void CopyBuffers (Tint vid, int FrontToBack,
                    Tfloat xm, Tfloat ym, Tfloat zm,
                    Tfloat XM, Tfloat YM, Tfloat ZM,
                    Tint flag);

  virtual Standard_Boolean Activate();

  // TEMPORARY!!!
  void Redraw1 (const Graphic3d_CView& theCView, 
                const Aspect_CLayer2d& theCUnderLayer, 
                const Aspect_CLayer2d& theCOverLayer,
                const int theToSwap);

protected:

  Handle(OpenGl_View) myView;            // WSViews - now just one view is supported
  Tint                myTransientList;   // WSTransient
  Standard_Boolean    myIsTransientOpen; // transientOpen
  Tint                myRetainMode;      // WSRetainMode

  Standard_Boolean    myUseTransparency;
  Standard_Boolean    myUseZBuffer;
  Standard_Boolean    myUseDepthTest;
  Standard_Boolean    myUseGLLight;
  Standard_Boolean    myBackBufferRestored;

  //// RELATED TO STATUS ////

  void UpdateMaterial (const int flag);

  const OpenGl_AspectLine *AspectLine_set, *AspectLine_applied;
  const OpenGl_AspectFace *AspectFace_set, *AspectFace_applied;
  const OpenGl_AspectMarker *AspectMarker_set, *AspectMarker_applied;
  const OpenGl_AspectText *AspectText_set, *AspectText_applied;

  const OpenGl_TextParam *TextParam_set, *TextParam_applied;

  const OpenGl_Matrix* ViewMatrix_applied;
  const OpenGl_Matrix* StructureMatrix_applied;

  const TEL_POFFSET_PARAM* PolygonOffset_applied;

public:

  DEFINE_STANDARD_RTTI(OpenGl_Workspace) // Type definition
  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_Workspace_Header
