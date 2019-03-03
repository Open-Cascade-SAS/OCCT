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

#include <OpenGl_AspectsTextureSet.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_Sampler.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_TextureSet.hxx>

#include <Graphic3d_TextureParams.hxx>

#include <Image_PixMap.hxx>

namespace
{
  static const TCollection_AsciiString THE_EMPTY_KEY;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_AspectsTextureSet::Release (OpenGl_Context* theCtx)
{
  if (myTextures.IsNull())
  {
    return;
  }

  for (OpenGl_TextureSet::Iterator aTextureIter (myTextures); aTextureIter.More(); aTextureIter.Next())
  {
    Handle(OpenGl_Texture)& aTextureRes = aTextureIter.ChangeValue();
    if (aTextureRes.IsNull())
    {
      continue;
    }

    if (theCtx != NULL)
    {
      if (aTextureRes->ResourceId().IsEmpty())
      {
        theCtx->DelayedRelease (aTextureRes);
      }
      else
      {
        const TCollection_AsciiString aName = aTextureRes->ResourceId();
        aTextureRes.Nullify(); // we need nullify all handles before ReleaseResource() call
        theCtx->ReleaseResource (aName, Standard_True);
      }
    }
    aTextureRes.Nullify();
  }
  myIsTextureReady = Standard_False;
}

// =======================================================================
// function : UpdateRediness
// purpose  :
// =======================================================================
void OpenGl_AspectsTextureSet::UpdateRediness (const Handle(Graphic3d_TextureSet)& theTextures)
{
  const Standard_Integer aNbTexturesOld = !myTextures.IsNull()  ? myTextures->Size()  : 0;
  const Standard_Integer aNbTexturesNew = !theTextures.IsNull() ? theTextures->Size() : 0;
  if (aNbTexturesOld != aNbTexturesNew)
  {
    myIsTextureReady = Standard_False;
    return;
  }
  if (aNbTexturesOld == 0)
  {
    return;
  }

  Graphic3d_TextureSet::Iterator aTextureIter (theTextures);
  OpenGl_TextureSet::Iterator aResIter (myTextures);
  for (; aResIter.More(); aResIter.Next(), aTextureIter.Next())
  {
    const Handle(OpenGl_Texture)&       aResource = aResIter.Value();
    const Handle(Graphic3d_TextureMap)& aTexture  = aTextureIter.Value();
    if (aTexture.IsNull() != aResource.IsNull())
    {
      myIsTextureReady = Standard_False;
      return;
    }
    else if (aTexture.IsNull())
    {
      continue;
    }

    const TCollection_AsciiString& aTextureKey = aTexture->GetId();
    if (aTextureKey.IsEmpty() || aResource->ResourceId() != aTextureKey)
    {
      myIsTextureReady = Standard_False;
      return;
    }
    else if (aResource->Revision() != aTexture->Revision())
    {
      myIsTextureReady = Standard_False;
      return;
    }
    else
    {
      // just invalidate texture parameters
      aResource->Sampler()->SetParameters (aTexture->GetParams());
    }
  }
}

// =======================================================================
// function : build
// purpose  :
// =======================================================================
void OpenGl_AspectsTextureSet::build (const Handle(OpenGl_Context)& theCtx,
                                      const Handle(Graphic3d_TextureSet)& theTextures)
{
  // release old texture resources
  const Standard_Integer aNbTexturesOld = !myTextures.IsNull()  ? myTextures->Size()  : 0;
  const Standard_Integer aNbTexturesNew = !theTextures.IsNull() ? theTextures->Size() : 0;
  if (aNbTexturesOld != aNbTexturesNew)
  {
    Release (theCtx.get());
    if (aNbTexturesNew > 0)
    {
      myTextures = new OpenGl_TextureSet (theTextures->Size());
    }
    else
    {
      myTextures.Nullify();
    }
  }
  if (myTextures.IsNull())
  {
    return;
  }

  Graphic3d_TextureSet::Iterator aTextureIter (theTextures);
  OpenGl_TextureSet::Iterator aResIter (myTextures);
  for (; aResIter.More(); aResIter.Next(), aTextureIter.Next())
  {
    Handle(OpenGl_Texture)& aResource = aResIter.ChangeValue();
    const Handle(Graphic3d_TextureMap)& aTexture = aTextureIter.Value();
    if (!aResource.IsNull())
    {
      if (!aTexture.IsNull()
       &&  aTexture->GetId()    == aResource->ResourceId()
       &&  aTexture->Revision() != aResource->Revision())
      {
        if (Handle(Image_PixMap) anImage = aTexture->GetImage())
        {
          aResource->Sampler()->SetParameters (aTexture->GetParams());
          aResource->Init (theCtx, *anImage.operator->(), aTexture->Type());
          aResource->SetRevision (aTexture->Revision());
          continue;
        }
      }

      if (aResource->ResourceId().IsEmpty())
      {
        theCtx->DelayedRelease (aResource);
        aResource.Nullify();
      }
      else
      {
        const TCollection_AsciiString aTextureKey = aResource->ResourceId();
        aResource.Nullify(); // we need nullify all handles before ReleaseResource() call
        theCtx->ReleaseResource (aTextureKey, Standard_True);
      }
    }

    if (!aTexture.IsNull())
    {
      const TCollection_AsciiString& aTextureKeyNew = aTexture->GetId();
      if (aTextureKeyNew.IsEmpty()
      || !theCtx->GetResource<Handle(OpenGl_Texture)> (aTextureKeyNew, aResource))
      {
        aResource = new OpenGl_Texture (aTextureKeyNew, aTexture->GetParams());
        if (Handle(Image_PixMap) anImage = aTexture->GetImage())
        {
          aResource->Init (theCtx, *anImage.operator->(), aTexture->Type());
          aResource->SetRevision (aTexture->Revision());
        }
        if (!aTextureKeyNew.IsEmpty())
        {
          theCtx->ShareResource (aTextureKeyNew, aResource);
        }
      }
      else
      {
        aResource->Sampler()->SetParameters (aTexture->GetParams());
      }
    }
  }
}
