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

#ifndef _OpenGl_AspectsTextureSet_Header
#define _OpenGl_AspectsTextureSet_Header

#include <Graphic3d_Aspects.hxx>
#include <Graphic3d_TextureMap.hxx>

class OpenGl_Context;
class OpenGl_TextureSet;
class OpenGl_PointSprite;

//! OpenGl resources for custom textures.
class OpenGl_AspectsTextureSet
{
public:
  DEFINE_STANDARD_ALLOC
public:
  //! Empty constructor.
  OpenGl_AspectsTextureSet() : myIsTextureReady (Standard_False) {}

  //! Return TRUE if resource is up-to-date.
  bool IsReady() const { return myIsTextureReady; }

  //! Invalidate resource state.
  void Invalidate() { myIsTextureReady = false; }

  //! Return textures array.
  const Handle(OpenGl_TextureSet)& TextureSet (const Handle(OpenGl_Context)& theCtx,
                                               const Handle(Graphic3d_Aspects)& theAspect,
                                               const Handle(OpenGl_PointSprite)& theSprite,
                                               const Handle(OpenGl_PointSprite)& theSpriteA,
                                               bool theToHighlight)
  {
    if (!myIsTextureReady)
    {
      build (theCtx, theAspect, theSprite, theSpriteA);
      myIsTextureReady = true;
    }
    return theToHighlight && !myTextures[1].IsNull()
         ? myTextures[1]
         : myTextures[0];
  }

  //! Update texture resource up-to-date state.
  Standard_EXPORT void UpdateRediness (const Handle(Graphic3d_Aspects)& theAspect);

  //! Release texture resource.
  Standard_EXPORT void Release (OpenGl_Context* theCtx);

private:

  //! Build texture resource.
  Standard_EXPORT void build (const Handle(OpenGl_Context)& theCtx,
                              const Handle(Graphic3d_Aspects)& theAspect,
                              const Handle(OpenGl_PointSprite)& theSprite,
                              const Handle(OpenGl_PointSprite)& theSpriteA);

private:

  Handle(OpenGl_TextureSet) myTextures[2];
  Standard_Boolean          myIsTextureReady;

};

#endif // _OpenGl_AspectsTextureSet_Header
