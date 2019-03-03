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
#include <Graphic3d_BSDF.hxx>

//! The element holding Graphic3d_Aspects.
class OpenGl_Aspects : public OpenGl_Element
{
public:

  //! Empty constructor.
  Standard_EXPORT OpenGl_Aspects();

  //! Create and assign parameters.
  Standard_EXPORT OpenGl_Aspects (const Handle(Graphic3d_Aspects)& theAspect);

  //! Return aspect.
  const Handle(Graphic3d_Aspects)& Aspect() const { return myAspect; }

  //! Assign parameters.
  Standard_EXPORT void SetAspect (const Handle(Graphic3d_Aspects)& theAspect);

  //! Returns Shading Model.
  Graphic3d_TypeOfShadingModel ShadingModel() const { return myShadingModel; }

  //! Set if lighting should be disabled or not.
  void SetNoLighting() { myShadingModel = Graphic3d_TOSM_UNLIT; }

  //! Returns textures map.
  const Handle(OpenGl_TextureSet)& TextureSet (const Handle(OpenGl_Context)& theCtx) const
  {
    return myResTextureSet.TextureSet (theCtx, myAspect->TextureSet());
  }

  //! Init and return OpenGl shader program resource.
  //! @return shader program resource.
  const Handle(OpenGl_ShaderProgram)& ShaderProgramRes (const Handle(OpenGl_Context)& theCtx) const
  {
    return myResProgram.ShaderProgram (theCtx, myAspect->ShaderProgram());
  }

  //! @return marker size
  Standard_ShortReal MarkerSize() const { return myResSprite.MarkerSize(); }

  //! Init and return OpenGl point sprite resource.
  //! @return point sprite texture.
  const Handle(OpenGl_TextureSet)& SpriteRes (const Handle(OpenGl_Context)& theCtx) const
  {
    return myResSprite.Sprite (theCtx, myAspect);
  }

  //! Init and return OpenGl highlight point sprite resource.
  //! @return point sprite texture for highlight.
  const Handle(OpenGl_TextureSet)& SpriteHighlightRes (const Handle(OpenGl_Context)& theCtx) const
  {
    return myResSprite.SpriteA (theCtx, myAspect);
  }

  Standard_EXPORT virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const Standard_OVERRIDE;
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext) Standard_OVERRIDE;

  //! Update presentation aspects parameters after their modification.
  virtual void SynchronizeAspects() Standard_OVERRIDE { SetAspect (myAspect); }

protected:

  //! OpenGl resources
  mutable OpenGl_AspectsProgram    myResProgram;
  mutable OpenGl_AspectsTextureSet myResTextureSet;
  mutable OpenGl_AspectsSprite     myResSprite;

  Handle(Graphic3d_Aspects)    myAspect;
  Graphic3d_TypeOfShadingModel myShadingModel;

public:

  DEFINE_STANDARD_ALLOC

};

#endif // _OpenGl_Aspects_Header
