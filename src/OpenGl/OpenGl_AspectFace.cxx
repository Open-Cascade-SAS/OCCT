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
  static OPENGL_SURF_PROP THE_DEFAULT_MATERIAL =
  {
    0.2F,  0.8F, 0.1F, 0.0F, // amb, diff, spec, emsv
    1.0F, 10.0F, 1.0F, 0.0F, // trans, shine, index, env_reflexion
    0, // isphysic
    (OPENGL_AMBIENT_MASK | OPENGL_DIFFUSE_MASK | OPENGL_SPECULAR_MASK), // color_mask
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, // ambient color
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, // diffuse color
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, // specular color
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, // emissive color
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}  // material color
  };

  static const TCollection_AsciiString THE_EMPTY_KEY;
};

// =======================================================================
// function : convertMaterial
// purpose  :
// =======================================================================
void OpenGl_AspectFace::convertMaterial (const CALL_DEF_MATERIAL& theMat,
                                         OPENGL_SURF_PROP&        theSurf)
{
  theSurf.amb  = theMat.IsAmbient  ? theMat.Ambient  : 0.0f;
  theSurf.diff = theMat.IsDiffuse  ? theMat.Diffuse  : 0.0f;
  theSurf.spec = theMat.IsSpecular ? theMat.Specular : 0.0f;
  theSurf.emsv = theMat.IsEmission ? theMat.Emission : 0.0f;

  theSurf.isphysic = theMat.IsPhysic ? 1 : 0; // type of material

  // color of material
  theSurf.color_mask = 0;
  if (theMat.IsAmbient)
  {
    theSurf.color_mask |= OPENGL_AMBIENT_MASK;
  }
  if (theMat.IsDiffuse)
  {
    theSurf.color_mask |= OPENGL_DIFFUSE_MASK;
  }
  if (theMat.IsSpecular)
  {
    theSurf.color_mask |= OPENGL_SPECULAR_MASK;
  }
  if (theMat.IsEmission)
  {
    theSurf.color_mask |= OPENGL_EMISSIVE_MASK;
  }

  // ambient color
  theSurf.ambcol.rgb[0] = theMat.ColorAmb.r;
  theSurf.ambcol.rgb[1] = theMat.ColorAmb.g;
  theSurf.ambcol.rgb[2] = theMat.ColorAmb.b;
  theSurf.ambcol.rgb[3] = 1.0f;

  // diffuse color
  theSurf.difcol.rgb[0] = theMat.ColorDif.r;
  theSurf.difcol.rgb[1] = theMat.ColorDif.g;
  theSurf.difcol.rgb[2] = theMat.ColorDif.b;
  theSurf.difcol.rgb[3] = 1.0f;

  // specular color
  theSurf.speccol.rgb[0] = theMat.ColorSpec.r;
  theSurf.speccol.rgb[1] = theMat.ColorSpec.g;
  theSurf.speccol.rgb[2] = theMat.ColorSpec.b;
  theSurf.speccol.rgb[3] = 1.0f;

  // emission color
  theSurf.emscol.rgb[0] = theMat.ColorEms.r;
  theSurf.emscol.rgb[1] = theMat.ColorEms.g;
  theSurf.emscol.rgb[2] = theMat.ColorEms.b;
  theSurf.emscol.rgb[3] = 1.0f;

  theSurf.shine = 128.0f * float(theMat.Shininess);
  theSurf.env_reflexion = theMat.EnvReflexion;

  // trans = 0. => opaque
  // trans = 1. => transparent
  // in OpenGl it is opposite.
  theSurf.trans = 1.0f - theMat.Transparency;
  theSurf.index = theMat.RefractionIndex;

  // material BSDF (for physically-based rendering)
  theSurf.BSDF = theMat.BSDF;
}

// =======================================================================
// function : OpenGl_AspectFace
// purpose  :
// =======================================================================
OpenGl_AspectFace::OpenGl_AspectFace()
: myInteriorStyle (Aspect_IS_SOLID),
  myEdge (TOff),
  myHatch (TEL_HS_SOLID),
  myDistinguishingMode (TOff),
  myCullingMode (TelCullNone),
  myIntFront (THE_DEFAULT_MATERIAL),
  myIntBack (THE_DEFAULT_MATERIAL),
  myPolygonOffset (THE_DEFAULT_POFFSET),
  myDoTextureMap (false)
{}

// =======================================================================
// function : SetAspect
// purpose  :
// =======================================================================
void OpenGl_AspectFace::SetAspect (const CALL_DEF_CONTEXTFILLAREA& theAspect)
{
  myInteriorStyle = (Aspect_InteriorStyle )theAspect.Style;
  myEdge = theAspect.Edge ? TOn : TOff;

  //TelInteriorStyleIndex
  switch (theAspect.Hatch)
  {
    case 0: /* Aspect_HS_HORIZONTAL */
      myHatch = TEL_HS_HORIZONTAL;
      break;
    case 1: /* Aspect_HS_HORIZONTAL_WIDE */
      myHatch = TEL_HS_HORIZONTAL_SPARSE;
      break;
    case 2: /* Aspect_HS_VERTICAL */
      myHatch = TEL_HS_VERTICAL;
      break;
    case 3: /* Aspect_HS_VERTICAL_WIDE */
      myHatch = TEL_HS_VERTICAL_SPARSE;
      break;
    case 4: /* Aspect_HS_DIAGONAL_45 */
      myHatch = TEL_HS_DIAG_45;
      break;
    case 5: /* Aspect_HS_DIAGONAL_45_WIDE */
      myHatch = TEL_HS_DIAG_45_SPARSE;
      break;
    case 6: /* Aspect_HS_DIAGONAL_135 */
      myHatch = TEL_HS_DIAG_135;
      break;
    case 7: /* Aspect_HS_DIAGONAL_135_WIDE */
      myHatch = TEL_HS_DIAG_135_SPARSE;
      break;
    case 8: /* Aspect_HS_GRID */
      myHatch = TEL_HS_GRID;
      break;
    case 9: /* Aspect_HS_GRID_WIDE */
      myHatch = TEL_HS_GRID_SPARSE;
      break;
    case 10: /* Aspect_HS_GRID_DIAGONAL */
      myHatch = TEL_HS_CROSS;
      break;
    case 11: /* Aspect_HS_GRID_DIAGONAL_WIDE */
      myHatch = TEL_HS_CROSS_SPARSE;
      break;
    default:
      myHatch = 0;
      break;
  }

  myDistinguishingMode = theAspect.Distinguish ? TOn : TOff;
  myCullingMode = theAspect.BackFace ? TelCullBack : TelCullNone;

  convertMaterial (theAspect.Front, myIntFront);
  convertMaterial (theAspect.Back,  myIntBack);

  //TelInteriorColour
  myIntFront.matcol.rgb[0] = (float )theAspect.IntColor.r;
  myIntFront.matcol.rgb[1] = (float )theAspect.IntColor.g;
  myIntFront.matcol.rgb[2] = (float )theAspect.IntColor.b;
  myIntFront.matcol.rgb[3] = 1.0f;

  //TelBackInteriorColour
  myIntBack.matcol.rgb[0] = (float )theAspect.BackIntColor.r;
  myIntBack.matcol.rgb[1] = (float )theAspect.BackIntColor.g;
  myIntBack.matcol.rgb[2] = (float )theAspect.BackIntColor.b;
  myIntBack.matcol.rgb[3] = 1.0f;

  //TelPolygonOffset
  myPolygonOffset.mode   = (Aspect_PolygonOffsetMode )theAspect.PolygonOffsetMode;
  myPolygonOffset.factor = theAspect.PolygonOffsetFactor;
  myPolygonOffset.units  = theAspect.PolygonOffsetUnits;

  CALL_DEF_CONTEXTLINE anEdgeAspect;
  anEdgeAspect.Color.r  = (float )theAspect.EdgeColor.r;
  anEdgeAspect.Color.g  = (float )theAspect.EdgeColor.g;
  anEdgeAspect.Color.b  = (float )theAspect.EdgeColor.b;
  anEdgeAspect.LineType = (Aspect_TypeOfLine )theAspect.LineType;
  anEdgeAspect.Width    = (float )theAspect.Width;
  myAspectEdge.SetAspect (anEdgeAspect);

  myDoTextureMap = (theAspect.Texture.doTextureMap != 0);

  // update texture binding
  myTexture = theAspect.Texture.TextureMap;

  const TCollection_AsciiString& aTextureKey = myTexture.IsNull() ? THE_EMPTY_KEY : myTexture->GetId();
  if (aTextureKey.IsEmpty() || myResources.TextureId != aTextureKey)
  {
    myResources.ResetTextureReadiness();
  }

  // update shader program binding
  myShaderProgram = theAspect.ShaderProgram;

  const TCollection_AsciiString& aShaderKey = myShaderProgram.IsNull() ? THE_EMPTY_KEY : myShaderProgram->GetId();
  if (aShaderKey.IsEmpty() || myResources.ShaderProgramId != aShaderKey)
  {
    myResources.ResetShaderReadiness();
  }
}

// =======================================================================
// function : SetAspect
// purpose  :
// =======================================================================
void OpenGl_AspectFace::SetAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspect)
{
  CALL_DEF_CONTEXTFILLAREA aFaceContext;
  Standard_Real           aWidth;
  Quantity_Color          aBackIntColor;
  Quantity_Color          aEdgeColor;
  Aspect_TypeOfLine       aLType;
  Quantity_Color          aIntColor;
  Aspect_InteriorStyle    aIntStyle;
  NCollection_Vec3<Standard_Real> aColor;

  theAspect->Values (aIntStyle, aIntColor, aBackIntColor, aEdgeColor, aLType, aWidth);
  aIntColor.Values (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_RGB);

  aFaceContext.Style      = int (aIntStyle);
  aFaceContext.IntColor.r = float (aColor.r());
  aFaceContext.IntColor.g = float (aColor.g());
  aFaceContext.IntColor.b = float (aColor.b());

  if (theAspect->Distinguish())
  {
    aBackIntColor.Values (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_RGB);
  }

  aFaceContext.BackIntColor.r = float (aColor.r());
  aFaceContext.BackIntColor.g = float (aColor.g());
  aFaceContext.BackIntColor.b = float (aColor.b());

  aFaceContext.Edge = theAspect->Edge () ? 1:0;
  aEdgeColor.Values (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_RGB);

  aFaceContext.EdgeColor.r = float (aColor.r());
  aFaceContext.EdgeColor.g = float (aColor.g());
  aFaceContext.EdgeColor.b = float (aColor.b());
  aFaceContext.LineType    = int (aLType);
  aFaceContext.Width       = float (aWidth);
  aFaceContext.Hatch       = int (theAspect->HatchStyle ());

  aFaceContext.Distinguish = theAspect->Distinguish () ? 1:0;
  aFaceContext.BackFace    = theAspect->BackFace ()    ? 1:0;

  aFaceContext.Back.Shininess = float ((theAspect->BackMaterial ()).Shininess ());
  aFaceContext.Back.Ambient   = float ((theAspect->BackMaterial ()).Ambient ());
  aFaceContext.Back.Diffuse   = float ((theAspect->BackMaterial ()).Diffuse ());
  aFaceContext.Back.Specular  = float ((theAspect->BackMaterial ()).Specular ());
  aFaceContext.Back.Transparency  = float ((theAspect->BackMaterial ()).Transparency ());
  aFaceContext.Back.Emission      = float ((theAspect->BackMaterial ()).Emissive ());

  // Reflection mode
  aFaceContext.Back.IsAmbient = ((theAspect->BackMaterial ()).ReflectionMode (Graphic3d_TOR_AMBIENT) ? 1 : 0 );
  aFaceContext.Back.IsDiffuse = ((theAspect->BackMaterial ()).ReflectionMode (Graphic3d_TOR_DIFFUSE) ? 1 : 0 );
  aFaceContext.Back.IsSpecular = ((theAspect->BackMaterial ()).ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0 );
  aFaceContext.Back.IsEmission = ((theAspect->BackMaterial ()).ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0 );

  // Material type
  const Graphic3d_MaterialAspect aBackMat = theAspect->BackMaterial ();
  Standard_Boolean isBackPhys = aBackMat.MaterialType (Graphic3d_MATERIAL_PHYSIC);
  aFaceContext.Back.IsPhysic = (isBackPhys ? 1 : 0 );

  // Specular Color
  aFaceContext.Back.ColorSpec.r = float (((theAspect->BackMaterial ()).SpecularColor ()).Red ());
  aFaceContext.Back.ColorSpec.g = float (((theAspect->BackMaterial ()).SpecularColor ()).Green ());
  aFaceContext.Back.ColorSpec.b = float (((theAspect->BackMaterial ()).SpecularColor ()).Blue ());

  // Ambient color
  aFaceContext.Back.ColorAmb.r = float (((theAspect->BackMaterial ()).AmbientColor ()).Red ());
  aFaceContext.Back.ColorAmb.g = float (((theAspect->BackMaterial ()).AmbientColor ()).Green ());
  aFaceContext.Back.ColorAmb.b = float (((theAspect->BackMaterial ()).AmbientColor ()).Blue ());

  // Diffuse color
  aFaceContext.Back.ColorDif.r = float (((theAspect->BackMaterial ()).DiffuseColor ()).Red ());
  aFaceContext.Back.ColorDif.g = float (((theAspect->BackMaterial ()).DiffuseColor ()).Green ());
  aFaceContext.Back.ColorDif.b = float (((theAspect->BackMaterial ()).DiffuseColor ()).Blue ());

  // Emissive color
  aFaceContext.Back.ColorEms.r = float (((theAspect->BackMaterial ()).EmissiveColor ()).Red ());
  aFaceContext.Back.ColorEms.g = float (((theAspect->BackMaterial ()).EmissiveColor ()).Green ());
  aFaceContext.Back.ColorEms.b = float (((theAspect->BackMaterial ()).EmissiveColor ()).Blue ());

  aFaceContext.Back.EnvReflexion = float ((theAspect->BackMaterial ()).EnvReflexion());

  aFaceContext.Front.Shininess    = float ((theAspect->FrontMaterial ()).Shininess ());
  aFaceContext.Front.Ambient      = float ((theAspect->FrontMaterial ()).Ambient ());
  aFaceContext.Front.Diffuse      = float ((theAspect->FrontMaterial ()).Diffuse ());
  aFaceContext.Front.Specular     = float ((theAspect->FrontMaterial ()).Specular ());
  aFaceContext.Front.Transparency = float ((theAspect->FrontMaterial ()).Transparency ());
  aFaceContext.Front.Emission     = float ((theAspect->FrontMaterial ()).Emissive ());

  // Reflection mode
  aFaceContext.Front.IsAmbient    = ((theAspect->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_AMBIENT) ? 1 : 0);
  aFaceContext.Front.IsDiffuse    = ((theAspect->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_DIFFUSE) ? 1 : 0);
  aFaceContext.Front.IsSpecular   = ((theAspect->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0);
  aFaceContext.Front.IsEmission   = ((theAspect->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0);

  // Material type
  const Graphic3d_MaterialAspect aFrontMat = theAspect->FrontMaterial ();
  Standard_Boolean isFrontPhys = aFrontMat.MaterialType (Graphic3d_MATERIAL_PHYSIC);
  aFaceContext.Front.IsPhysic = (isFrontPhys ? 1 : 0 );

  // Specular Color
  aFaceContext.Front.ColorSpec.r = float (((theAspect->FrontMaterial ()).SpecularColor ()).Red ());
  aFaceContext.Front.ColorSpec.g = float (((theAspect->FrontMaterial ()).SpecularColor ()).Green ());
  aFaceContext.Front.ColorSpec.b = float (((theAspect->FrontMaterial ()).SpecularColor ()).Blue ());

  // Ambient color
  aFaceContext.Front.ColorAmb.r = float (((theAspect->FrontMaterial ()).AmbientColor ()).Red ());
  aFaceContext.Front.ColorAmb.g = float (((theAspect->FrontMaterial ()).AmbientColor ()).Green ());
  aFaceContext.Front.ColorAmb.b = float (((theAspect->FrontMaterial ()).AmbientColor ()).Blue ());

  // Diffuse color
  aFaceContext.Front.ColorDif.r = float (((theAspect->FrontMaterial ()).DiffuseColor ()).Red ());
  aFaceContext.Front.ColorDif.g = float (((theAspect->FrontMaterial ()).DiffuseColor ()).Green ());
  aFaceContext.Front.ColorDif.b = float (((theAspect->FrontMaterial ()).DiffuseColor ()).Blue ());

  // Emissive color
  aFaceContext.Front.ColorEms.r = float (((theAspect->FrontMaterial ()).EmissiveColor ()).Red ());
  aFaceContext.Front.ColorEms.g = float (((theAspect->FrontMaterial ()).EmissiveColor ()).Green ());
  aFaceContext.Front.ColorEms.b = float (((theAspect->FrontMaterial ()).EmissiveColor ()).Blue ());

  aFaceContext.Front.EnvReflexion = float ((theAspect->FrontMaterial ()).EnvReflexion());
  aFaceContext.IsDef = 1;
  aFaceContext.Texture.TextureMap   = theAspect->TextureMap();
  aFaceContext.Texture.doTextureMap = theAspect->TextureMapState() ? 1 : 0;

  Standard_Integer aPolyMode;
  Standard_ShortReal aPolyFactor, aPolyUnits;
  theAspect->PolygonOffsets (aPolyMode, aPolyFactor, aPolyUnits);
  aFaceContext.PolygonOffsetMode   = aPolyMode;
  aFaceContext.PolygonOffsetFactor = (Standard_ShortReal)aPolyFactor;
  aFaceContext.PolygonOffsetUnits  = (Standard_ShortReal)aPolyUnits;

  aFaceContext.ShaderProgram = theAspect->ShaderProgram();

  SetAspect (aFaceContext);
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
