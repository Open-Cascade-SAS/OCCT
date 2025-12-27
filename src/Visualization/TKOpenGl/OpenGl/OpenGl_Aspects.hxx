// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _OpenGl_Aspects_Header
#define _OpenGl_Aspects_Header

#include <OpenGl_Element.hxx>
#include <OpenGl_AspectsProgram.hxx>
#include <OpenGl_AspectsTextureSet.hxx>
#include <OpenGl_AspectsSprite.hxx>
#include <Graphic3d_Aspects.hxx>

//! The element holding Graphic3d_Aspects.
class OpenGl_Aspects : public OpenGl_Element
{
public:
  //! Empty constructor.
  Standard_EXPORT OpenGl_Aspects();

  //! Create and assign parameters.
  Standard_EXPORT OpenGl_Aspects(const occ::handle<Graphic3d_Aspects>& theAspect);

  //! Return aspect.
  const occ::handle<Graphic3d_Aspects>& Aspect() const { return myAspect; }

  //! Assign parameters.
  Standard_EXPORT void SetAspect(const occ::handle<Graphic3d_Aspects>& theAspect);

  //! Returns Shading Model.
  Graphic3d_TypeOfShadingModel ShadingModel() const { return myShadingModel; }

  //! Set if lighting should be disabled or not.
  void SetNoLighting() { myShadingModel = Graphic3d_TypeOfShadingModel_Unlit; }

  //! Returns textures map.
  const occ::handle<OpenGl_TextureSet>& TextureSet(const occ::handle<OpenGl_Context>& theCtx,
                                              bool theToHighlight = false) const
  {
    const occ::handle<OpenGl_PointSprite>& aSprite  = myResSprite.Sprite(theCtx, myAspect, false);
    const occ::handle<OpenGl_PointSprite>& aSpriteA = myResSprite.Sprite(theCtx, myAspect, true);
    return myResTextureSet.TextureSet(theCtx, myAspect, aSprite, aSpriteA, theToHighlight);
  }

  //! Init and return OpenGl shader program resource.
  //! @return shader program resource.
  const occ::handle<OpenGl_ShaderProgram>& ShaderProgramRes(const occ::handle<OpenGl_Context>& theCtx) const
  {
    return myResProgram.ShaderProgram(theCtx, myAspect->ShaderProgram());
  }

  //! @return marker size
  float MarkerSize() const { return myResSprite.MarkerSize(); }

  //! Return TRUE if OpenGl point sprite resource defines texture.
  bool HasPointSprite(const occ::handle<OpenGl_Context>& theCtx) const
  {
    return myResSprite.HasPointSprite(theCtx, myAspect);
  }

  //! Return TRUE if OpenGl point sprite resource defined by obsolete Display List (bitmap).
  bool IsDisplayListSprite(const occ::handle<OpenGl_Context>& theCtx) const
  {
    return myResSprite.IsDisplayListSprite(theCtx, myAspect);
  }

  //! Init and return OpenGl point sprite resource.
  //! @return point sprite texture.
  const occ::handle<OpenGl_PointSprite>& SpriteRes(const occ::handle<OpenGl_Context>& theCtx,
                                              bool                          theIsAlphaSprite) const
  {
    return myResSprite.Sprite(theCtx, myAspect, theIsAlphaSprite);
  }

  Standard_EXPORT virtual void Render(const occ::handle<OpenGl_Workspace>& theWorkspace) const
    override;
  Standard_EXPORT virtual void Release(OpenGl_Context* theContext) override;

  //! Update presentation aspects parameters after their modification.
  virtual void SynchronizeAspects() override { SetAspect(myAspect); }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                int  theDepth = -1) const override;

protected:
  //! OpenGl resources
  mutable OpenGl_AspectsProgram    myResProgram;
  mutable OpenGl_AspectsTextureSet myResTextureSet;
  mutable OpenGl_AspectsSprite     myResSprite;

  occ::handle<Graphic3d_Aspects>    myAspect;
  Graphic3d_TypeOfShadingModel myShadingModel;

public:
  DEFINE_STANDARD_ALLOC
};

#endif // _OpenGl_Aspects_Header
