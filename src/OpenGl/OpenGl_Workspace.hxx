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

#include <Graphic3d_PtrFrameBuffer.hxx>
#include <Graphic3d_BufferType.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_CappingAlgo.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_LineAttributes.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_NamedStatus.hxx>
#include <OpenGl_PrinterContext.hxx>
#include <OpenGl_RenderFilter.hxx>
#include <OpenGl_ShaderObject.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_TextParam.hxx>
#include <OpenGl_TextureBufferArb.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_Window.hxx>

class OpenGl_AspectLine;
class OpenGl_AspectMarker;
class OpenGl_AspectText;
class OpenGl_Element;
class OpenGl_View;
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
  virtual Standard_Boolean CanRender (const OpenGl_Element* theElement);

private:

  Handle(OpenGl_RenderFilter) myPrevRenderFilter;

public:

  DEFINE_STANDARD_RTTI(OpenGl_RaytraceFilter, OpenGl_RenderFilter)
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
  Standard_EXPORT virtual ~OpenGl_Workspace();

  //! Activate rendering context.
  Standard_EXPORT Standard_Boolean Activate();

  OpenGl_View* View() const { return myView; }

  const Handle(OpenGl_Context)& GetGlContext() { return myGlContext; }

  Standard_EXPORT Graphic3d_PtrFrameBuffer FBOCreate (const Standard_Integer theWidth, const Standard_Integer theHeight);

  Standard_EXPORT void FBORelease (Graphic3d_PtrFrameBuffer theFBOPtr);

  Standard_Boolean BufferDump (OpenGl_FrameBuffer*         theFBOPtr,
                               Image_PixMap&               theImage,
                               const Graphic3d_BufferType& theBufferType);

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

  Handle(OpenGl_PrinterContext)& PrinterContext() { return myPrintContext; }

  //! @return true if usage of Z buffer is enabled.
  Standard_Boolean& UseZBuffer() { return myUseZBuffer; }

  //! @return true if depth writing is enabled.
  Standard_Boolean& UseDepthWrite() { return myUseDepthWrite; }

  //! @return true if usage of GL light is enabled.
  Standard_EXPORT Standard_Boolean  UseGLLight() const;

  //! @return true if antialiasing is enabled.
  Standard_EXPORT Standard_Integer AntiAliasingMode() const;

  //! @return true if clipping algorithm enabled
  Standard_EXPORT Standard_Boolean IsCullingEnabled() const;

  Standard_Integer NamedStatus;

  //// RELATED TO STATUS ////

  const TEL_COLOUR* HighlightColor;

  Standard_EXPORT const OpenGl_AspectLine*   SetAspectLine   (const OpenGl_AspectLine*   theAspect);
  Standard_EXPORT const OpenGl_AspectFace*   SetAspectFace   (const OpenGl_AspectFace*   theAspect);
  Standard_EXPORT const OpenGl_AspectMarker* SetAspectMarker (const OpenGl_AspectMarker* theAspect);
  Standard_EXPORT const OpenGl_AspectText*   SetAspectText   (const OpenGl_AspectText*   theAspect);

  //// THESE METHODS ARE EXPORTED AS THEY PROVIDE STATE INFO TO USERDRAW
  Standard_EXPORT const OpenGl_AspectLine*   AspectLine   (const Standard_Boolean theWithApply);
  Standard_EXPORT const OpenGl_AspectFace*   AspectFace   (const Standard_Boolean theWithApply);
  Standard_EXPORT const OpenGl_AspectMarker* AspectMarker (const Standard_Boolean theWithApply);
  Standard_EXPORT const OpenGl_AspectText*   AspectText   (const Standard_Boolean theWithApply);

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

  void updateMaterial (const int theFlag);

  void setTextureParams (Handle(OpenGl_Texture)&                theTexture,
                         const Handle(Graphic3d_TextureParams)& theParams);

protected: //! @name protected fields

  OpenGl_View*                     myView;
  Handle(OpenGl_Window)            myWindow;
  Handle(OpenGl_Context)           myGlContext;
  Handle(OpenGl_PrinterContext)    myPrintContext;
  Handle(OpenGl_LineAttributes)    myLineAttribs;
  Standard_Integer                 myAntiAliasingMode;
  Standard_Boolean                 myUseZBuffer;
  Standard_Boolean                 myUseDepthWrite;
  Standard_Boolean                 myUseGLLight;
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

  DEFINE_STANDARD_RTTI(OpenGl_Workspace, Standard_Transient)
  DEFINE_STANDARD_ALLOC

};

#endif // _OpenGl_Workspace_Header
