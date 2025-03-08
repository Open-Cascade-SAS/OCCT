// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OpenGl_TextureSet.hxx>

#include <OpenGl_Texture.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_TextureSet, Standard_Transient)

//=================================================================================================

OpenGl_TextureSet::OpenGl_TextureSet(const Handle(OpenGl_Texture)& theTexture)
    : myTextures(0, 0),
      myTextureSetBits(Graphic3d_TextureSetBits_NONE)
{
  if (!theTexture.IsNull())
  {
    myTextures.ChangeFirst().Texture = theTexture;
    myTextures.ChangeFirst().Unit    = theTexture->Sampler()->Parameters()->TextureUnit();
  }
}

//=================================================================================================

bool OpenGl_TextureSet::IsModulate() const
{
  return myTextures.IsEmpty() || myTextures.First().Texture.IsNull()
         || myTextures.First().Texture->Sampler()->Parameters()->IsModulate();
}

//=================================================================================================

bool OpenGl_TextureSet::HasNonPointSprite() const
{
  if (myTextures.IsEmpty())
  {
    return false;
  }
  else if (myTextures.Size() == 1)
  {
    return !myTextures.First().Texture.IsNull() && !myTextures.First().Texture->IsPointSprite();
  }
  return !myTextures.First().Texture.IsNull();
}

//=================================================================================================

bool OpenGl_TextureSet::HasPointSprite() const
{
  return !myTextures.IsEmpty() && !myTextures.Last().Texture.IsNull()
         && myTextures.Last().Texture->IsPointSprite();
}
