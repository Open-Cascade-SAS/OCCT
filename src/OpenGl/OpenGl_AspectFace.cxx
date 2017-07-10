// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <Aspect_PolygonOffsetMode.hxx>
#include <NCollection_Vec3.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Sampler.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_Workspace.hxx>

#include <Graphic3d_ShaderProgram.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_TypeOfReflection.hxx>
#include <Graphic3d_MaterialAspect.hxx>

#include <Image_PixMap.hxx>

namespace
{
  //! Initialize default material in this way for backward compatibility.
  inline Graphic3d_MaterialAspect initDefaultMaterial()
  {
    Graphic3d_MaterialAspect aMat;
    aMat.SetMaterialType (Graphic3d_MATERIAL_ASPECT);
    aMat.SetAmbient  (0.2f);
    aMat.SetDiffuse  (0.8f);
    aMat.SetSpecular (0.1f);
    aMat.SetEmissive (0.0f);
    aMat.SetAmbientColor (Quantity_NOC_WHITE);
    aMat.SetDiffuseColor (Quantity_NOC_WHITE);
    aMat.SetEmissiveColor(Quantity_NOC_WHITE);
    aMat.SetSpecularColor(Quantity_NOC_WHITE);
    aMat.SetShininess (10.0f / 128.0f);
    aMat.SetRefractionIndex (1.0f);
    return aMat;
  }

  static const TCollection_AsciiString  THE_EMPTY_KEY;
  static const Graphic3d_MaterialAspect THE_DEFAULT_MATERIAL = initDefaultMaterial();
}

// =======================================================================
// function : OpenGl_AspectFace
// purpose  :
// =======================================================================
OpenGl_AspectFace::OpenGl_AspectFace()
: myAspect (new Graphic3d_AspectFillArea3d (Aspect_IS_SOLID, Quantity_NOC_WHITE,
                                            Quantity_NOC_WHITE, Aspect_TOL_SOLID, 1.0,
                                            THE_DEFAULT_MATERIAL, THE_DEFAULT_MATERIAL)),
  myIsNoLighting (false)
{
  myAspect->SetHatchStyle (Handle(Graphic3d_HatchStyle)());
}

// =======================================================================
// function : OpenGl_AspectFace
// purpose  :
// =======================================================================
OpenGl_AspectFace::OpenGl_AspectFace (const Handle(Graphic3d_AspectFillArea3d)& theAspect)
: myIsNoLighting (false)
{
  SetAspect (theAspect);
}

// =======================================================================
// function : SetAspect
// purpose  :
// =======================================================================
void OpenGl_AspectFace::SetAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspect)
{
  myAspect = theAspect;

  const Graphic3d_MaterialAspect& aMat = theAspect->FrontMaterial();
  myIsNoLighting = !aMat.ReflectionMode (Graphic3d_TOR_AMBIENT)
                && !aMat.ReflectionMode (Graphic3d_TOR_DIFFUSE)
                && !aMat.ReflectionMode (Graphic3d_TOR_SPECULAR)
                && !aMat.ReflectionMode (Graphic3d_TOR_EMISSION);

  myAspectEdge.Aspect()->SetColor (theAspect->EdgeColor());
  myAspectEdge.Aspect()->SetType  (theAspect->EdgeLineType());
  myAspectEdge.Aspect()->SetWidth (theAspect->EdgeWidth());

  // update texture binding
  myResources.UpdateTexturesRediness (myAspect->TextureSet());

  // update shader program binding
  const TCollection_AsciiString& aShaderKey = myAspect->ShaderProgram().IsNull() ? THE_EMPTY_KEY : myAspect->ShaderProgram()->GetId();
  if (aShaderKey.IsEmpty() || myResources.ShaderProgramId != aShaderKey)
  {
    myResources.ResetShaderReadiness();
  }
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_AspectFace::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  theWorkspace->SetAspectFace (this);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_AspectFace::Release (OpenGl_Context* theContext)
{
  myResources.ReleaseTextures (theContext);
  if (!myResources.ShaderProgram.IsNull()
   && theContext)
  {
    theContext->ShaderManager()->Unregister (myResources.ShaderProgramId,
                                             myResources.ShaderProgram);
  }
  myResources.ShaderProgramId.Clear();
  myResources.ResetShaderReadiness();
}

// =======================================================================
// function : ReleaseTextures
// purpose  :
// =======================================================================
void OpenGl_AspectFace::Resources::ReleaseTextures (OpenGl_Context* theCtx)
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
// function : UpdateTexturesRediness
// purpose  :
// =======================================================================
void OpenGl_AspectFace::Resources::UpdateTexturesRediness (const Handle(Graphic3d_TextureSet)& theTextures)
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
// function : BuildTextures
// purpose  :
// =======================================================================
void OpenGl_AspectFace::Resources::BuildTextures (const Handle(OpenGl_Context)& theCtx,
                                                  const Handle(Graphic3d_TextureSet)& theTextures)
{
  // release old texture resources
  const Standard_Integer aNbTexturesOld = !myTextures.IsNull()  ? myTextures->Size()  : 0;
  const Standard_Integer aNbTexturesNew = !theTextures.IsNull() ? theTextures->Size() : 0;
  if (aNbTexturesOld != aNbTexturesNew)
  {
    ReleaseTextures (theCtx.get());
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

// =======================================================================
// function : BuildShader
// purpose  :
// =======================================================================
void OpenGl_AspectFace::Resources::BuildShader (const Handle(OpenGl_Context)&          theCtx,
                                                const Handle(Graphic3d_ShaderProgram)& theShader)
{
  if (theCtx->core20fwd == NULL)
  {
    return;
  }

  // release old shader program resources
  if (!ShaderProgram.IsNull())
  {
    theCtx->ShaderManager()->Unregister (ShaderProgramId, ShaderProgram);
    ShaderProgramId.Clear();
    ShaderProgram.Nullify();
  }
  if (theShader.IsNull())
  {
    return;
  }

  theCtx->ShaderManager()->Create (theShader, ShaderProgramId, ShaderProgram);
}
