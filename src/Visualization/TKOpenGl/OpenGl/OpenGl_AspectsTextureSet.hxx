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
  OpenGl_AspectsTextureSet()
      : myIsTextureReady(false)
  {
  }

  //! Return TRUE if resource is up-to-date.
  bool IsReady() const { return myIsTextureReady; }

  //! Invalidate resource state.
  void Invalidate() { myIsTextureReady = false; }

  //! Return textures array.
  const occ::handle<OpenGl_TextureSet>& TextureSet(
    const occ::handle<OpenGl_Context>&     theCtx,
    const occ::handle<Graphic3d_Aspects>&  theAspect,
    const occ::handle<OpenGl_PointSprite>& theSprite,
    const occ::handle<OpenGl_PointSprite>& theSpriteA,
    bool                                   theToHighlight)
  {
    if (!myIsTextureReady)
    {
      build(theCtx, theAspect, theSprite, theSpriteA);
      myIsTextureReady = true;
    }
    return theToHighlight && !myTextures[1].IsNull() ? myTextures[1] : myTextures[0];
  }

  //! Update texture resource up-to-date state.
  Standard_EXPORT void UpdateRediness(const occ::handle<Graphic3d_Aspects>& theAspect);

  //! Release texture resource.
  Standard_EXPORT void Release(OpenGl_Context* theCtx);

private:
  //! Build texture resource.
  Standard_EXPORT void build(const occ::handle<OpenGl_Context>&     theCtx,
                             const occ::handle<Graphic3d_Aspects>&  theAspect,
                             const occ::handle<OpenGl_PointSprite>& theSprite,
                             const occ::handle<OpenGl_PointSprite>& theSpriteA);

private:
  occ::handle<OpenGl_TextureSet> myTextures[2];
  bool                           myIsTextureReady;
};

#endif // _OpenGl_AspectsTextureSet_Header
