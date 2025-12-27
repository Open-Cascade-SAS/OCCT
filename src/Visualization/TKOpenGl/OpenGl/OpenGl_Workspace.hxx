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

#ifndef OpenGl_Workspace_HeaderFile
#define OpenGl_Workspace_HeaderFile

#include <Graphic3d_BufferType.hxx>
#include <Graphic3d_PresentationAttributes.hxx>

#include <OpenGl_Aspects.hxx>
#include <OpenGl_Vec.hxx>

class OpenGl_FrameBuffer;
class OpenGl_Group;
class OpenGl_View;
class OpenGl_Window;
class Image_PixMap;

//! Rendering workspace.
//! Provides methods to render primitives and maintain GL state.
class OpenGl_Workspace : public Standard_Transient
{
public:
  //! Constructor of rendering workspace.
  Standard_EXPORT OpenGl_Workspace(OpenGl_View* theView, const occ::handle<OpenGl_Window>& theWindow);

  //! Destructor
  virtual ~OpenGl_Workspace() {}

  //! Activate rendering context.
  Standard_EXPORT bool Activate();

  OpenGl_View* View() const { return myView; }

  const occ::handle<OpenGl_Context>& GetGlContext() { return myGlContext; }

  Standard_EXPORT occ::handle<OpenGl_FrameBuffer> FBOCreate(const int theWidth,
                                                       const int theHeight);

  Standard_EXPORT void FBORelease(occ::handle<OpenGl_FrameBuffer>& theFbo);

  bool BufferDump(const occ::handle<OpenGl_FrameBuffer>& theFbo,
                              Image_PixMap&                     theImage,
                              const Graphic3d_BufferType&       theBufferType);

  Standard_EXPORT int Width() const;

  Standard_EXPORT int Height() const;

  //! Setup Z-buffer usage flag (without affecting GL state!).
  //! Returns previously set flag.
  bool SetUseZBuffer(const bool theToUse)
  {
    const bool wasUsed = myUseZBuffer;
    myUseZBuffer                   = theToUse;
    return wasUsed;
  }

  //! @return true if usage of Z buffer is enabled.
  bool& UseZBuffer() { return myUseZBuffer; }

  //! @return true if depth writing is enabled.
  bool& UseDepthWrite() { return myUseDepthWrite; }

  //! Configure default polygon offset parameters.
  //! Return previous settings.
  Standard_EXPORT Graphic3d_PolygonOffset
    SetDefaultPolygonOffset(const Graphic3d_PolygonOffset& theOffset);

  //// RELATED TO STATUS ////

  //! Return true if active group might activate face culling (e.g. primitives are closed).
  bool ToAllowFaceCulling() const { return myToAllowFaceCulling; }

  //! Allow or disallow face culling.
  //! This call does NOT affect current state of back face culling;
  //! ApplyAspectFace() should be called to update state.
  bool SetAllowFaceCulling(bool theToAllow)
  {
    const bool wasAllowed = myToAllowFaceCulling;
    myToAllowFaceCulling  = theToAllow;
    return wasAllowed;
  }

  //! Return true if following structures should apply highlight color.
  bool ToHighlight() const { return !myHighlightStyle.IsNull(); }

  //! Return highlight style.
  const occ::handle<Graphic3d_PresentationAttributes>& HighlightStyle() const
  {
    return myHighlightStyle;
  }

  //! Set highlight style.
  void SetHighlightStyle(const occ::handle<Graphic3d_PresentationAttributes>& theStyle)
  {
    myHighlightStyle = theStyle;
  }

  //! Return edge color taking into account highlight flag.
  const NCollection_Vec4<float>& EdgeColor() const
  {
    return !myHighlightStyle.IsNull() ? myHighlightStyle->ColorRGBA()
                                      : myAspectsSet->Aspect()->EdgeColorRGBA();
  }

  //! Return Interior color taking into account highlight flag.
  const NCollection_Vec4<float>& InteriorColor() const
  {
    return !myHighlightStyle.IsNull() ? myHighlightStyle->ColorRGBA()
                                      : myAspectsSet->Aspect()->InteriorColorRGBA();
  }

  //! Return text color taking into account highlight flag.
  const NCollection_Vec4<float>& TextColor() const
  {
    return !myHighlightStyle.IsNull() ? myHighlightStyle->ColorRGBA()
                                      : myAspectsSet->Aspect()->ColorRGBA();
  }

  //! Return text Subtitle color taking into account highlight flag.
  const NCollection_Vec4<float>& TextSubtitleColor() const
  {
    return !myHighlightStyle.IsNull() ? myHighlightStyle->ColorRGBA()
                                      : myAspectsSet->Aspect()->ColorSubTitleRGBA();
  }

  //! Currently set aspects (can differ from applied).
  const OpenGl_Aspects* Aspects() const { return myAspectsSet; }

  //! Assign new aspects (will be applied within ApplyAspects()).
  Standard_EXPORT const OpenGl_Aspects* SetAspects(const OpenGl_Aspects* theAspect);

  //! Return TextureSet from set Aspects or Environment texture.
  const occ::handle<OpenGl_TextureSet>& TextureSet() const
  {
    const occ::handle<OpenGl_TextureSet>& aTextureSet =
      myAspectsSet->TextureSet(myGlContext, ToHighlight());
    return !aTextureSet.IsNull() || myAspectsSet->Aspect()->ToMapTexture() ? aTextureSet
                                                                           : myEnvironmentTexture;
  }

  //! Apply aspects.
  //! @param theToBindTextures flag to bind texture set defined by applied aspect
  //! @return aspect set by SetAspects()
  Standard_EXPORT const OpenGl_Aspects* ApplyAspects(bool theToBindTextures = true);

  //! Clear the applied aspect state to default values.
  void ResetAppliedAspect();

  //! Get rendering filter.
  //! @sa ShouldRender()
  int RenderFilter() const { return myRenderFilter; }

  //! Set filter for restricting rendering of particular elements.
  //! @sa ShouldRender()
  void SetRenderFilter(int theFilter) { myRenderFilter = theFilter; }

  //! Checks whether the element can be rendered or not.
  //! @param[in] theElement  the element to check
  //! @param[in] theGroup    the group containing the element
  //! @return True if element can be rendered
  bool ShouldRender(const OpenGl_Element* theElement, const OpenGl_Group* theGroup);

  //! Return the number of skipped transparent elements within active OpenGl_RenderFilter_OpaqueOnly
  //! filter.
  //! @sa OpenGl_LayerList::Render()
  int NbSkippedTransparentElements() { return myNbSkippedTranspElems; }

  //! Reset skipped transparent elements counter.
  //! @sa OpenGl_LayerList::Render()
  void ResetSkippedCounter() { myNbSkippedTranspElems = 0; }

  //! Returns face aspect for none culling mode.
  const OpenGl_Aspects& NoneCulling() const { return myNoneCulling; }

  //! Returns face aspect for front face culling mode.
  const OpenGl_Aspects& FrontCulling() const { return myFrontCulling; }

  //! Sets a new environment texture.
  void SetEnvironmentTexture(const occ::handle<OpenGl_TextureSet>& theTexture)
  {
    myEnvironmentTexture = theTexture;
  }

  //! Returns environment texture.
  const occ::handle<OpenGl_TextureSet>& EnvironmentTexture() const { return myEnvironmentTexture; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

protected: //! @name protected fields
  OpenGl_View*           myView;
  occ::handle<OpenGl_Window>  myWindow;
  occ::handle<OpenGl_Context> myGlContext;
  bool       myUseZBuffer;
  bool       myUseDepthWrite;
  OpenGl_Aspects         myNoneCulling;
  OpenGl_Aspects         myFrontCulling;

protected:                        //! @name fields related to status
                                  // clang-format off
  int myNbSkippedTranspElems; //!< counter of skipped transparent elements for OpenGl_LayerList two rendering passes method
  int myRenderFilter;         //!< active filter for skipping rendering of elements by some criteria (multiple render passes)
                                  // clang-format on

  OpenGl_Aspects            myDefaultAspects;
  const OpenGl_Aspects*     myAspectsSet;
  occ::handle<Graphic3d_Aspects> myAspectsApplied;

  occ::handle<Graphic3d_PresentationAttributes> myAspectFaceAppliedWithHL;

  bool                                     myToAllowFaceCulling; //!< allow back face culling
  occ::handle<Graphic3d_PresentationAttributes> myHighlightStyle;     //!< active highlight style

  OpenGl_Aspects myAspectFaceHl; //!< Hiddenline aspect

  occ::handle<OpenGl_TextureSet> myEnvironmentTexture;

public: //! @name type definition
  DEFINE_STANDARD_RTTIEXT(OpenGl_Workspace, Standard_Transient)
  DEFINE_STANDARD_ALLOC
};

#endif // _OpenGl_Workspace_Header
