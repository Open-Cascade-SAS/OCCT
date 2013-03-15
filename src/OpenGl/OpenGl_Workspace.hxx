// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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
#include <Graphic3d_CView.hxx>
#include <Graphic3d_TypeOfComposition.hxx>
#include <Graphic3d_TypeOfTexture.hxx>
#include <Graphic3d_PtrFrameBuffer.hxx>
#include <Graphic3d_BufferType.hxx>
#include <Handle_Graphic3d_TextureParams.hxx>

#include <Aspect_CLayer2d.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_PrintAlgo.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <NCollection_Sequence.hxx>

#include <OpenGl_tsm.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_Display.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_NamedStatus.hxx>
#include <OpenGl_PrinterContext.hxx>
#include <OpenGl_TextParam.hxx>

#include <Handle_OpenGl_View.hxx>
#include <Handle_OpenGl_Texture.hxx>

class OpenGl_AspectLine;
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
  Standard_Boolean Print (const Handle(OpenGl_PrinterContext)& thePrintContext,
                          const Graphic3d_CView& theCView,
                          const Aspect_CLayer2d& theCUnderLayer,
                          const Aspect_CLayer2d& theCOverLayer,
                          const Aspect_Handle    theHPrintDC,
                          const Standard_Boolean theToShowBackground,
                          const Standard_CString theFileName,
                          const Aspect_PrintAlgo thePrintAlgorithm,
                          const Standard_Real    theScaleFactor);

  const Handle(OpenGl_PrinterContext)& PrinterContext() const
  {
    return myPrintContext;
  }

  void DisplayCallback (const Graphic3d_CView& theCView, int theReason);

  Standard_Boolean SetImmediateModeDrawToFront (const Standard_Boolean theDrawToFrontBuffer);
  Standard_Boolean BeginAddMode();
  void EndAddMode();
  void ClearImmediatMode (const Graphic3d_CView& theCView,
                          const Standard_Boolean theToFlush);
  void RedrawImmediatMode();
  Standard_Boolean BeginImmediatMode (const Graphic3d_CView& theCView,
                                      const Standard_Boolean theUseDepthTest,
                                      const Standard_Boolean theRetainMode);
  void EndImmediatMode();
  void DrawStructure (const OpenGl_Structure* theStructure);

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
  inline const OpenGl_TextParam* AspectTextParams() const
  {
    return TextParam_applied;
  }

  //! Clear the applied aspect state.
  void ResetAppliedAspect();

  Standard_EXPORT Handle(OpenGl_Texture) DisableTexture();
  Standard_EXPORT Handle(OpenGl_Texture) EnableTexture (const Handle(OpenGl_Texture)&          theTexture,
                                                        const Handle(Graphic3d_TextureParams)& theParams = NULL);

protected:

  void CopyBuffers (const Standard_Boolean theFrontToBack);

  virtual Standard_Boolean Activate();

  // TEMPORARY!!!
  void Redraw1 (const Graphic3d_CView& theCView,
                const Aspect_CLayer2d& theCUnderLayer,
                const Aspect_CLayer2d& theCOverLayer,
                const int theToSwap);

  void UpdateMaterial (const int flag);

  void setTextureParams (Handle(OpenGl_Texture)&                theTexture,
                         const Handle(Graphic3d_TextureParams)& theParams);

protected: //! @name protected fields

  Handle(OpenGl_PrinterContext) myPrintContext;
  Handle(OpenGl_View)    myView;            // WSViews - now just one view is supported
  Standard_Boolean       myIsTransientOpen; // transientOpen
  Standard_Boolean       myRetainMode;
  Standard_Boolean       myTransientDrawToFront; //!< optimization flag for immediate mode (to render directly to the front buffer)

  NCollection_Sequence<const OpenGl_Structure*> myTransientList;

  Standard_Boolean       myUseTransparency;
  Standard_Boolean       myUseZBuffer;
  Standard_Boolean       myUseDepthTest;
  Standard_Boolean       myUseGLLight;
  Standard_Boolean       myBackBufferRestored;

protected: //! @name fields related to status

  Handle(OpenGl_Texture) myTextureBound;    //!< currently bound texture (managed by OpenGl_AspectFace and OpenGl_View environment texture)
  const OpenGl_AspectLine *AspectLine_set, *AspectLine_applied;
  const OpenGl_AspectFace *AspectFace_set, *AspectFace_applied;
  const OpenGl_AspectMarker *AspectMarker_set, *AspectMarker_applied;
  const OpenGl_AspectText *AspectText_set, *AspectText_applied;

  const OpenGl_TextParam *TextParam_set, *TextParam_applied;

  const OpenGl_Matrix* ViewMatrix_applied;
  const OpenGl_Matrix* StructureMatrix_applied;

  const TEL_POFFSET_PARAM* PolygonOffset_applied;

  OpenGl_AspectFace myAspectFaceHl; // Hiddenline aspect

public: //! @name type definition

  DEFINE_STANDARD_RTTI(OpenGl_Workspace)
  DEFINE_STANDARD_ALLOC

};

#endif // _OpenGl_Workspace_Header
