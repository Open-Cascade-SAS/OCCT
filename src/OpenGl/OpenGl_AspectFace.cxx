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
    aMat.SetAmbient  (0.2);
    aMat.SetDiffuse  (0.8);
    aMat.SetSpecular (0.1);
    aMat.SetEmissive (0.0);
    aMat.SetAmbientColor (Quantity_NOC_WHITE);
    aMat.SetDiffuseColor (Quantity_NOC_WHITE);
    aMat.SetEmissiveColor(Quantity_NOC_WHITE);
    aMat.SetSpecularColor(Quantity_NOC_WHITE);
    aMat.SetShininess (10.0 / 128.0);
    aMat.SetRefractionIndex (1.0);
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
  myAspect->SetHatchStyle (Aspect_HS_SOLID);
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
  const TCollection_AsciiString& aTextureKey = myAspect->TextureMap().IsNull() ? THE_EMPTY_KEY : myAspect->TextureMap()->GetId();
  if (aTextureKey.IsEmpty() || myResources.TextureId != aTextureKey)
  {
    myResources.ResetTextureReadiness();
  }
  else if (!myResources.Texture.IsNull()
        && !myAspect->TextureMap().IsNull()
        &&  myResources.Texture->Revision() != myAspect->TextureMap()->Revision())
  {
    myResources.ResetTextureReadiness();
  }

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
  if (!myResources.Texture.IsNull())
  {
    if (theContext)
    {
      if (myResources.TextureId.IsEmpty())
      {
        theContext->DelayedRelease (myResources.Texture);
      }
      else
      {
        myResources.Texture.Nullify(); // we need nullify all handles before ReleaseResource() call
        theContext->ReleaseResource (myResources.TextureId, Standard_True);
      }
    }
    myResources.Texture.Nullify();
  }
  myResources.TextureId.Clear();
  myResources.ResetTextureReadiness();

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
// function : BuildTexture
// purpose  :
// =======================================================================
void OpenGl_AspectFace::Resources::BuildTexture (const Handle(OpenGl_Context)&       theCtx,
                                                 const Handle(Graphic3d_TextureMap)& theTexture)
{
  // release old texture resource
  if (!Texture.IsNull())
  {
    if (!theTexture.IsNull()
     &&  theTexture->GetId()    == TextureId
     &&  theTexture->Revision() != Texture->Revision())
    {
      Handle(Image_PixMap) anImage = theTexture->GetImage();
      if (!anImage.IsNull())
      {
        Texture->Init (theCtx, *anImage.operator->(), theTexture->Type());
        Texture->SetRevision (Texture->Revision());
        return;
      }
    }

    if (TextureId.IsEmpty())
    {
      theCtx->DelayedRelease (Texture);
      Texture.Nullify();
    }
    else
    {
      Texture.Nullify(); // we need nullify all handles before ReleaseResource() call
      theCtx->ReleaseResource (TextureId, Standard_True);
    }
  }

  TextureId = theTexture.IsNull() ? THE_EMPTY_KEY : theTexture->GetId();

  if (!theTexture.IsNull())
  {
    if (TextureId.IsEmpty() || !theCtx->GetResource<Handle(OpenGl_Texture)> (TextureId, Texture))
    {
      Texture = new OpenGl_Texture (theTexture->GetParams());
      Handle(Image_PixMap) anImage = theTexture->GetImage();
      if (!anImage.IsNull())
      {
        Texture->Init (theCtx, *anImage.operator->(), theTexture->Type());
        Texture->SetRevision (Texture->Revision());
      }
      if (!TextureId.IsEmpty())
      {
        theCtx->ShareResource (TextureId, Texture);
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
