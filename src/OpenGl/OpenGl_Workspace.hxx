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

#include <Graphic3d_BufferType.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_CappingAlgo.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_Material.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_RenderFilter.hxx>
#include <OpenGl_ShaderObject.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_TextParam.hxx>
#include <OpenGl_TextureBufferArb.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_Window.hxx>

class OpenGl_View;
class Image_PixMap;

class OpenGl_RaytraceFilter;
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
  virtual Standard_Boolean ShouldRender (const Handle(OpenGl_Workspace)& theWorkspace,
                                         const OpenGl_Element*           theElement) Standard_OVERRIDE;

private:

  Handle(OpenGl_RenderFilter) myPrevRenderFilter;

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_RaytraceFilter,OpenGl_RenderFilter)
};

class OpenGl_Workspace;
DEFINE_STANDARD_HANDLE(OpenGl_Workspace,Standard_Transient)

//! Rendering workspace.
//! Provides methods to render primitives and maintain GL state.
class OpenGl_Workspace : public Standard_Transient
{
public:

  //! Constructor of rendering workspace.
  Standard_EXPORT OpenGl_Workspace (OpenGl_View* theView, const Handle(OpenGl_Window)& theWindow);

  //! Destructor
  virtual ~OpenGl_Workspace() {}

  //! Activate rendering context.
  Standard_EXPORT Standard_Boolean Activate();

  OpenGl_View* View() const { return myView; }

  const Handle(OpenGl_Context)& GetGlContext() { return myGlContext; }

  Standard_EXPORT Handle(OpenGl_FrameBuffer) FBOCreate (const Standard_Integer theWidth, const Standard_Integer theHeight);

  Standard_EXPORT void FBORelease (Handle(OpenGl_FrameBuffer)& theFbo);

  Standard_Boolean BufferDump (const Handle(OpenGl_FrameBuffer)& theFbo,
                               Image_PixMap&                     theImage,
                               const Graphic3d_BufferType&       theBufferType);

  Standard_EXPORT Standard_Integer Width()  const;

  Standard_EXPORT Standard_Integer Height() const;

  //! Setup Z-buffer usage flag (without affecting GL state!).
  //! Returns previously set flag.
  Standard_Boolean SetUseZBuffer (const Standard_Boolean theToUse)
  {
    const Standard_Boolean wasUsed = myUseZBuffer;
    myUseZBuffer = theToUse;
    return wasUsed;
  }

  //! @return true if usage of Z buffer is enabled.
  Standard_Boolean& UseZBuffer() { return myUseZBuffer; }

  //! @return true if depth writing is enabled.
  Standard_Boolean& UseDepthWrite() { return myUseDepthWrite; }

  //! @return true if clipping algorithm enabled
  Standard_EXPORT Standard_Boolean IsCullingEnabled() const;

  //// RELATED TO STATUS ////

  //! Return true if active group might activate face culling (e.g. primitives are closed).
  bool ToAllowFaceCulling() const { return myToAllowFaceCulling; }

  //! Allow or disallow face culling.
  //! This call does NOT affect current state of back face culling;
  //! ApplyAspectFace() should be called to update state.
  void SetAllowFaceCulling (bool theToAllow) { myToAllowFaceCulling = theToAllow; }

  //! Return true if following structures should apply highlight color.
  bool ToHighlight() const { return !myHighlightStyle.IsNull(); }

  //! Return highlight style.
  const Handle(Graphic3d_PresentationAttributes)& HighlightStyle() const { return myHighlightStyle; }

  //! Set highlight style.
  void SetHighlightStyle (const Handle(Graphic3d_PresentationAttributes)& theStyle) {  myHighlightStyle = theStyle; }

  //! Return line color taking into account highlight flag.
  const OpenGl_Vec4& LineColor() const
  {
    return !myHighlightStyle.IsNull()
         ?  myHighlightStyle->ColorRGBA()
         :  myAspectLineSet->Aspect()->ColorRGBA();
  }

  //! Return edge color taking into account highlight flag.
  const OpenGl_Vec4& EdgeColor() const
  {
    return !myHighlightStyle.IsNull()
         ?  myHighlightStyle->ColorRGBA()
         :  myAspectFaceSet->AspectEdge()->Aspect()->ColorRGBA();
  }

  //! Return marker color taking into account highlight flag.
  const OpenGl_Vec4& MarkerColor() const
  {
    return !myHighlightStyle.IsNull()
         ?  myHighlightStyle->ColorRGBA()
         :  myAspectMarkerSet->Aspect()->ColorRGBA();
  }

  //! Return Interior color taking into account highlight flag.
  const OpenGl_Vec4& InteriorColor() const
  {
    return !myHighlightStyle.IsNull()
         ?  myHighlightStyle->ColorRGBA()
         :  myAspectFaceSet->Aspect()->InteriorColorRGBA();
  }

  //! Return text color taking into account highlight flag.
  const OpenGl_Vec4& TextColor() const
  {
    return !myHighlightStyle.IsNull()
         ?  myHighlightStyle->ColorRGBA()
         :  myAspectTextSet->Aspect()->ColorRGBA();
  }

  //! Return text Subtitle color taking into account highlight flag.
  const OpenGl_Vec4& TextSubtitleColor() const
  {
    return !myHighlightStyle.IsNull()
         ?  myHighlightStyle->ColorRGBA()
         :  myAspectTextSet->Aspect()->ColorSubTitleRGBA();
  }

  //! Currently set line aspect (can differ from applied).
  const OpenGl_AspectLine*   AspectLine()   const { return myAspectLineSet; }

  //! Currently set face aspect (can differ from applied).
  const OpenGl_AspectFace*   AspectFace()   const { return myAspectFaceSet; }

  //! Currently set marker aspect (can differ from applied).
  const OpenGl_AspectMarker* AspectMarker() const { return myAspectMarkerSet; }

  //! Currently set text aspect (can differ from applied).
  const OpenGl_AspectText*   AspectText()   const { return myAspectTextSet; }

  //! Assign new line aspect (will be applied within ApplyAspectLine()).
  Standard_EXPORT const OpenGl_AspectLine*   SetAspectLine   (const OpenGl_AspectLine*   theAspect);

  //! Assign new face aspect (will be applied within ApplyAspectFace()).
  Standard_EXPORT const OpenGl_AspectFace*   SetAspectFace   (const OpenGl_AspectFace*   theAspect);

  //! Assign new marker aspect (will be applied within ApplyAspectMarker()).
  Standard_EXPORT const OpenGl_AspectMarker* SetAspectMarker (const OpenGl_AspectMarker* theAspect);

  //! Assign new text aspect (will be applied within ApplyAspectText()).
  Standard_EXPORT const OpenGl_AspectText*   SetAspectText   (const OpenGl_AspectText*   theAspect);

  //! Apply line aspect.
  //! @return aspect set by SetAspectLine()
  const OpenGl_AspectLine* ApplyAspectLine() { return myAspectLineSet; }

  //! Apply face aspect.
  //! @return aspect set by SetAspectFace()
  Standard_EXPORT const OpenGl_AspectFace*   ApplyAspectFace();

  //! Apply marker aspect.
  //! @return aspect set by SetAspectMarker()
  Standard_EXPORT const OpenGl_AspectMarker* ApplyAspectMarker();

  //! Apply text aspect.
  //! @return aspect set by SetAspectText()
  const OpenGl_AspectText* ApplyAspectText() { return myAspectTextSet; }

  //! Clear the applied aspect state to default values.
  void ResetAppliedAspect();

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
  void SetPolygonOffset (const Graphic3d_PolygonOffset& theParams);

  //! Returns currently applied polygon offset parameters.
  const Graphic3d_PolygonOffset& AppliedPolygonOffset() { return myPolygonOffsetApplied; }

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

  //! Sets a new environment texture.
  void SetEnvironmentTexture (const Handle(OpenGl_TextureSet)& theTexture) { myEnvironmentTexture = theTexture; }

  //! Returns environment texture.
  const Handle(OpenGl_TextureSet)& EnvironmentTexture() const { return myEnvironmentTexture; }

protected: //! @name protected fields

  OpenGl_View*                     myView;
  Handle(OpenGl_Window)            myWindow;
  Handle(OpenGl_Context)           myGlContext;
  Standard_Boolean                 myUseZBuffer;
  Standard_Boolean                 myUseDepthWrite;
  Handle(OpenGl_CappingAlgoFilter) myDefaultCappingAlgoFilter;
  OpenGl_AspectFace                myNoneCulling;
  OpenGl_AspectFace                myFrontCulling;

protected: //! @name fields related to status

  Handle(OpenGl_RenderFilter) myRenderFilter;
  const OpenGl_AspectLine*   myAspectLineSet;
  const OpenGl_AspectFace*   myAspectFaceSet;
  Handle(Graphic3d_AspectFillArea3d) myAspectFaceApplied;
  const OpenGl_AspectMarker* myAspectMarkerSet;
  Handle(Graphic3d_AspectMarker3d) myAspectMarkerApplied;
  const OpenGl_AspectText*   myAspectTextSet;
  Handle(Graphic3d_PresentationAttributes) myAspectFaceAppliedWithHL;

  const OpenGl_Matrix* ViewMatrix_applied;
  const OpenGl_Matrix* StructureMatrix_applied;

  bool            myToAllowFaceCulling; //!< allow back face culling
  Handle(Graphic3d_PresentationAttributes) myHighlightStyle; //!< active highlight style

  OpenGl_Matrix myModelViewMatrix; //!< Model matrix with applied structure transformations

  Graphic3d_PolygonOffset myPolygonOffsetApplied; //!< currently applied polygon offset

  OpenGl_AspectFace myAspectFaceHl; //!< Hiddenline aspect

  Handle(OpenGl_TextureSet) myEnvironmentTexture;

public: //! @name type definition

  DEFINE_STANDARD_RTTIEXT(OpenGl_Workspace,Standard_Transient)
  DEFINE_STANDARD_ALLOC

};

#endif // _OpenGl_Workspace_Header
