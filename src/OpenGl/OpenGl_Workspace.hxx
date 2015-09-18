// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#ifndef _OpenGl_Workspace_Header
#define _OpenGl_Workspace_Header

#include <map>
#include <set>

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
#include <Aspect_PolygonOffsetMode.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <NCollection_Sequence.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_NamedStatus.hxx>
#include <OpenGl_PrinterContext.hxx>
#include <OpenGl_SceneGeometry.hxx>
#include <OpenGl_TextParam.hxx>
#include <OpenGl_RenderFilter.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_LineAttributes.hxx>
#include <OpenGl_CappingAlgo.hxx>

#include <Handle_OpenGl_View.hxx>
#include <Handle_OpenGl_Texture.hxx>

#include <OpenGl_ShaderObject.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_TextureBufferArb.hxx>

class OpenGl_AspectLine;
class OpenGl_AspectMarker;
class OpenGl_AspectText;
class OpenGl_FrameBuffer;
class OpenGl_Structure;
class OpenGl_TriangleSet;
class OpenGl_Element;
class Image_PixMap;

//! OpenGL material definition
struct OpenGl_Material
{

  OpenGl_Vec4 Ambient;  //!< ambient reflection coefficient
  OpenGl_Vec4 Diffuse;  //!< diffuse reflection coefficient
  OpenGl_Vec4 Specular; //!< glossy  reflection coefficient
  OpenGl_Vec4 Emission; //!< material emission
  OpenGl_Vec4 Params;   //!< extra packed parameters

  Standard_ShortReal  Shine()              const { return Params.x(); }
  Standard_ShortReal& ChangeShine()              { return Params.x(); }

  Standard_ShortReal  Transparency()       const { return Params.y(); }
  Standard_ShortReal& ChangeTransparency()       { return Params.y(); }

  //! Initialize material
  void Init (const OPENGL_SURF_PROP& theProps);

  //! Returns packed (serialized) representation of material properties
  const OpenGl_Vec4* Packed() const { return reinterpret_cast<const OpenGl_Vec4*> (this); }
  static Standard_Integer NbOfVec4() { return 5; }

};

DEFINE_STANDARD_HANDLE (OpenGl_RaytraceFilter, OpenGl_RenderFilter)

//! Graphical ray-tracing filter.
//! Filters out all raytracable structures.
class OpenGl_RaytraceFilter : public OpenGl_RenderFilter
{
public:

  //! Default constructor.
  OpenGl_RaytraceFilter() {}

  //! Returns the previously set filter.
  const Handle(OpenGl_RenderFilter)& PrevRenderFilter()
  {
    return myPrevRenderFilter;
  }

  //! Remembers the previously set filter.
  void SetPrevRenderFilter (const Handle(OpenGl_RenderFilter)& theFilter)
  {
    myPrevRenderFilter = theFilter;
  }

  //! Checks whether the element can be rendered or not.
  //! @param theElement [in] the element to check.
  //! @return True if element can be rendered.
  virtual Standard_Boolean CanRender (const OpenGl_Element* theElement);

private:

  Handle(OpenGl_RenderFilter) myPrevRenderFilter;

public:

  DEFINE_STANDARD_RTTI(OpenGl_RaytraceFilter)
};

//! Represents window with GL context.
//! Provides methods to render primitives and maintain GL state.
class OpenGl_Workspace : public OpenGl_Window
{
public:

  //! Main constructor - prepare GL context for specified window.
  OpenGl_Workspace (const Handle(OpenGl_GraphicDriver)& theDriver,
                    const CALL_DEF_WINDOW&        theCWindow,
                    Aspect_RenderingContext       theGContext,
                    const Handle(OpenGl_Caps)&    theCaps,
                    const Handle(OpenGl_Context)& theShareCtx);

  //! Destructor
  virtual ~OpenGl_Workspace();

  void SetActiveView (const Handle(OpenGl_View)& theView,
                      const Standard_Integer     theViewId)
  {
    myView   = theView;
    myViewId = theViewId;
  }

  const Handle(OpenGl_View)& ActiveView() const { return myView; }

  Standard_Integer ActiveViewId() const { return myViewId; }

  //! Redraw the window.
  void Redraw (const Graphic3d_CView& theCView,
               const Aspect_CLayer2d& theCUnderLayer,
               const Aspect_CLayer2d& theCOverLayer);

  Standard_Boolean SetImmediateModeDrawToFront (const Standard_Boolean theDrawToFrontBuffer);
  void RedrawImmediate (const Graphic3d_CView& theCView,
                        const Aspect_CLayer2d& theCUnderLayer,
                        const Aspect_CLayer2d& theCOverLayer);

  void Invalidate (const Graphic3d_CView& /*theCView*/)
  {
    myBackBufferRestored = Standard_False;
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

  Graphic3d_PtrFrameBuffer FBOCreate (const Standard_Integer theWidth, const Standard_Integer theHeight);
  void FBORelease (Graphic3d_PtrFrameBuffer theFBOPtr);
  Standard_Boolean BufferDump (OpenGl_FrameBuffer*         theFBOPtr,
                               Image_PixMap&               theImage,
                               const Graphic3d_BufferType& theBufferType);

  Standard_Boolean& UseZBuffer()   { return myUseZBuffer; }
  Standard_Boolean& UseDepthTest() { return myUseDepthTest; }
  Standard_Boolean& UseGLLight()   { return myUseGLLight; }

  Standard_Integer AntiAliasingMode() const { return myAntiAliasingMode; }

  //// RELATED TO STATUS ////

  Standard_Integer   NamedStatus;

  const TEL_COLOUR* HighlightColor;

  Standard_EXPORT const OpenGl_AspectLine*   SetAspectLine   (const OpenGl_AspectLine*   theAspect);
  Standard_EXPORT const OpenGl_AspectFace*   SetAspectFace   (const OpenGl_AspectFace*   theAspect);
  Standard_EXPORT const OpenGl_AspectMarker* SetAspectMarker (const OpenGl_AspectMarker* theAspect);
  Standard_EXPORT const OpenGl_AspectText*   SetAspectText   (const OpenGl_AspectText*   theAspect);

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
  const Handle(OpenGl_Texture)& ActiveTexture() const { return myTextureBound; }

  //! Set filter for restricting rendering of particular elements.
  //! Filter can be applied for rendering passes used by recursive
  //! rendering algorithms for rendering elements of groups.
  //! @param theFilter [in] the filter instance.
  inline void SetRenderFilter (const Handle(OpenGl_RenderFilter)& theFilter)
  {
    myRenderFilter = theFilter;
  }

  //! Get rendering filter.
  //! @return filter instance.
  inline const Handle(OpenGl_RenderFilter)& GetRenderFilter() const
  {
    return myRenderFilter;
  }

  //! @return applied view matrix.
  inline const OpenGl_Matrix* ViewMatrix() const { return ViewMatrix_applied; }

  //! @return applied model structure matrix.
  inline const OpenGl_Matrix* ModelMatrix() const { return StructureMatrix_applied; }

  //! Sets and applies current polygon offset.
  void SetPolygonOffset (int theMode, Standard_ShortReal theFactor, Standard_ShortReal theUnits);

  //! Returns currently applied polygon offset params.
  const TEL_POFFSET_PARAM& AppliedPolygonOffset() { return PolygonOffset_applied; }

  //! @return true if clipping algorithm enabled
  inline Standard_Boolean IsCullingEnabled() const { return myIsCullingEnabled; }

  //! Returns capping algorithm rendering filter.
  const Handle(OpenGl_CappingAlgoFilter)& DefaultCappingAlgoFilter() const
  {
    return myDefaultCappingAlgoFilter;
  }

  //! Returns face aspect for none culling mode.
  const OpenGl_AspectFace& NoneCulling() const
  {
    return myNoneCulling;
  }

  //! Returns face aspect for front face culling mode.
  const OpenGl_AspectFace& FrontCulling() const
  {
    return myFrontCulling;
  }

protected:

  //! Copy content of Back buffer to the Front buffer
  void copyBackToFront();

  //! Blit image from/to specified buffers.
  bool blitBuffers (OpenGl_FrameBuffer* theReadFbo,
                    OpenGl_FrameBuffer* theDrawFbo);

  virtual Standard_Boolean Activate();

  void redraw1 (const Graphic3d_CView&               theCView,
                const Aspect_CLayer2d&               theCUnderLayer,
                const Aspect_CLayer2d&               theCOverLayer,
                OpenGl_FrameBuffer*                  theReadDrawFbo,
                const Graphic3d_Camera::Projection   theProjection);

  //! Setup default FBO.
  void bindDefaultFbo (OpenGl_FrameBuffer* theCustomFbo = NULL);

  //! Blend together views pair into stereo image.
  void drawStereoPair (const Graphic3d_CView& theCView);

  //! Blit snapshot containing main scene (myMainSceneFbos or BackBuffer)
  //! into presentation buffer (myMainSceneFbos -> offscreen FBO or myMainSceneFbos -> BackBuffer or BackBuffer -> FrontBuffer),
  //! and redraw immediate structures on top.
  //!
  //! When scene caching is disabled (myTransientDrawToFront, no double buffer in window, etc.),
  //! the first step (blitting) will be skipped.
  //!
  //! @return false if immediate structures has been rendered directly into FrontBuffer and Buffer Swap should not be called.
  bool redrawImmediate (const Graphic3d_CView& theCView,
                        const Aspect_CLayer2d& theCUnderLayer,
                        const Aspect_CLayer2d& theCOverLayer,
                        OpenGl_FrameBuffer*    theReadFbo,
                        const Graphic3d_Camera::Projection theProjection,
                        OpenGl_FrameBuffer*    theDrawFbo,
                        const Standard_Boolean theIsPartialUpdate = Standard_False);

  void updateMaterial (const int theFlag);

  void setTextureParams (Handle(OpenGl_Texture)&                theTexture,
                         const Handle(Graphic3d_TextureParams)& theParams);

protected: //! @name protected fields

  //! Two framebuffers (left and right views) store cached main presentation
  //! of the view (without presentation of immediate layers).
  Handle(OpenGl_FrameBuffer) myMainSceneFbos[2];
  //! Additional buffers for immediate layer in stereo mode.
  Handle(OpenGl_FrameBuffer) myImmediateSceneFbos[2];
  //! Special flag which is invalidated when myMainSceneFbos can not be blitted for some reason (e.g. driver bugs).
  Standard_Boolean           myHasFboBlit;

  //! Vertices for full-screen quad rendering.
  OpenGl_VertexBuffer        myFullScreenQuad;

  Handle(OpenGl_PrinterContext) myPrintContext;
  Handle(OpenGl_View)           myView;
  Handle(OpenGl_LineAttributes) myLineAttribs;
  Standard_Integer       myViewId;
  Standard_Integer       myAntiAliasingMode;
  Standard_Boolean       myTransientDrawToFront; //!< optimization flag for immediate mode (to render directly to the front buffer)
  Standard_Boolean       myBackBufferRestored;
  Standard_Boolean       myIsImmediateDrawn;     //!< flag indicates that immediate mode buffer contains some data
  Standard_Boolean       myUseZBuffer;
  Standard_Boolean       myUseDepthTest;
  Standard_Boolean       myUseGLLight;
  Standard_Boolean       myIsCullingEnabled;     //!< frustum culling flag

  unsigned int           myFrameCounter;         //!< redraw counter, for debugging

  Handle(OpenGl_CappingAlgoFilter) myDefaultCappingAlgoFilter;
  OpenGl_AspectFace                myNoneCulling;
  OpenGl_AspectFace                myFrontCulling;

protected: //! @name fields related to status

  Handle(OpenGl_RenderFilter) myRenderFilter;
  Handle(OpenGl_Texture) myTextureBound;    //!< currently bound texture (managed by OpenGl_AspectFace and OpenGl_View environment texture)
  const OpenGl_AspectLine *AspectLine_set, *AspectLine_applied;
  const OpenGl_AspectFace *AspectFace_set, *AspectFace_applied;
  const OpenGl_AspectMarker *AspectMarker_set, *AspectMarker_applied;
  const OpenGl_AspectText *AspectText_set, *AspectText_applied;

  const OpenGl_TextParam *TextParam_set, *TextParam_applied;

  const OpenGl_Matrix* ViewMatrix_applied;
  const OpenGl_Matrix* StructureMatrix_applied;

  OpenGl_Material myMatFront;    //!< current front material state (cached to reduce GL context updates)
  OpenGl_Material myMatBack;     //!< current back  material state
  OpenGl_Material myMatTmp;      //!< temporary variable
  TelCullMode     myCullingMode; //!< back face culling mode, applied from face aspect

  OpenGl_Matrix myModelViewMatrix; //!< Model matrix with applied structure transformations

  TEL_POFFSET_PARAM PolygonOffset_applied; //!< Currently applied polygon offset.

  OpenGl_AspectFace myAspectFaceHl; //!< Hiddenline aspect

public: //! @name type definition

  DEFINE_STANDARD_RTTI(OpenGl_Workspace)
  DEFINE_STANDARD_ALLOC

};

#endif // _OpenGl_Workspace_Header
