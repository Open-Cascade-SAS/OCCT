// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_Context.hxx>

#include <Aspect_PolygonOffsetMode.hxx>
#include <Graphic3d_CGroup.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <Graphic3d_TypeOfReflection.hxx>
#include <Graphic3d_MaterialAspect.hxx>

#include <NCollection_Vec3.hxx>

namespace
{

  static OPENGL_SURF_PROP THE_DEFAULT_MATERIAL =
  {
    0.2F,  0.8F, 0.1F, 0.0F, // amb, diff, spec, emsv
    1.0F, 10.0F, 0.0F,       // trans, shine, env_reflexion
    0, // isphysic
    (OPENGL_AMBIENT_MASK | OPENGL_DIFFUSE_MASK | OPENGL_SPECULAR_MASK), // color_mask
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, // ambient color
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, // diffuse color
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, // specular color
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, // emissive color
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}  // material color
  };

  static TEL_POFFSET_PARAM THE_DEFAULT_POFFSET = { Aspect_POM_Fill, 1.0F, 0.0F };

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
}

// =======================================================================
// function : OpenGl_AspectFace
// purpose  :
// =======================================================================
OpenGl_AspectFace::OpenGl_AspectFace()
: InteriorStyle (Aspect_IS_SOLID),
  Edge (Aspect_IS_SOLID),
  Hatch (TOn),
  DistinguishingMode (TEL_HS_SOLID),
  CullingMode (TelCullNone),
  doTextureMap (0)
{
  IntFront      = THE_DEFAULT_MATERIAL;
  IntBack       = THE_DEFAULT_MATERIAL;
  PolygonOffset = THE_DEFAULT_POFFSET;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_AspectFace::Init (const Handle(OpenGl_Context)&   theContext,
                              const CALL_DEF_CONTEXTFILLAREA& theAspect)
{
  InteriorStyle = (Aspect_InteriorStyle )theAspect.Style;
  Edge = theAspect.Edge ? TOn : TOff;

  //TelInteriorStyleIndex
  switch (theAspect.Hatch)
  {
    case 0: /* Aspect_HS_HORIZONTAL */
      Hatch = TEL_HS_HORIZONTAL;
      break;
    case 1: /* Aspect_HS_HORIZONTAL_WIDE */
      Hatch = TEL_HS_HORIZONTAL_SPARSE;
      break;
    case 2: /* Aspect_HS_VERTICAL */
      Hatch = TEL_HS_VERTICAL;
      break;
    case 3: /* Aspect_HS_VERTICAL_WIDE */
      Hatch = TEL_HS_VERTICAL_SPARSE;
      break;
    case 4: /* Aspect_HS_DIAGONAL_45 */
      Hatch = TEL_HS_DIAG_45;
      break;
    case 5: /* Aspect_HS_DIAGONAL_45_WIDE */
      Hatch = TEL_HS_DIAG_45_SPARSE;
      break;
    case 6: /* Aspect_HS_DIAGONAL_135 */
      Hatch = TEL_HS_DIAG_135;
      break;
    case 7: /* Aspect_HS_DIAGONAL_135_WIDE */
      Hatch = TEL_HS_DIAG_135_SPARSE;
      break;
    case 8: /* Aspect_HS_GRID */
      Hatch = TEL_HS_GRID;
      break;
    case 9: /* Aspect_HS_GRID_WIDE */
      Hatch = TEL_HS_GRID_SPARSE;
      break;
    case 10: /* Aspect_HS_GRID_DIAGONAL */
      Hatch = TEL_HS_CROSS;
      break;
    case 11: /* Aspect_HS_GRID_DIAGONAL_WIDE */
      Hatch = TEL_HS_CROSS_SPARSE;
      break;
    default:
      Hatch = 0;
      break;
  }

  DistinguishingMode = theAspect.Distinguish ? TOn : TOff;
  CullingMode = theAspect.BackFace ? TelCullBack : TelCullNone;

  convertMaterial (theAspect.Front, IntFront);
  convertMaterial (theAspect.Back,  IntBack);

  //TelInteriorColour
  IntFront.matcol.rgb[0] = (float )theAspect.IntColor.r;
  IntFront.matcol.rgb[1] = (float )theAspect.IntColor.g;
  IntFront.matcol.rgb[2] = (float )theAspect.IntColor.b;
  IntFront.matcol.rgb[3] = 1.0f;

  //TelBackInteriorColour
  IntBack.matcol.rgb[0] = (float )theAspect.BackIntColor.r;
  IntBack.matcol.rgb[1] = (float )theAspect.BackIntColor.g;
  IntBack.matcol.rgb[2] = (float )theAspect.BackIntColor.b;
  IntBack.matcol.rgb[3] = 1.0f;

  // setup texture
  doTextureMap = theAspect.Texture.doTextureMap;
  const Handle(Graphic3d_TextureMap)& aNewTexture = theAspect.Texture.TextureMap;
  TCollection_AsciiString aNewKey = aNewTexture.IsNull() ? TCollection_AsciiString() : aNewTexture->GetId();
  TextureParams = aNewTexture.IsNull() ? NULL : aNewTexture->GetParams();
  if (aNewKey.IsEmpty()
   || myTextureId != aNewKey)
  {
    if (!TextureRes.IsNull())
    {
      if (myTextureId.IsEmpty())
      {
        theContext->DelayedRelease (TextureRes);
        TextureRes.Nullify();
      }
      else
      {
        TextureRes.Nullify(); // we need nullify all handles before ReleaseResource() call
        theContext->ReleaseResource (myTextureId);
      }
    }
    myTextureId = aNewKey;

    if (!aNewTexture.IsNull())
    {
      if (aNewKey.IsEmpty() || !theContext->GetResource<Handle(OpenGl_Texture)> (aNewKey, TextureRes))
      {
        TextureRes = new OpenGl_Texture (TextureParams);
        Handle(Image_PixMap) anImage = aNewTexture->GetImage();
        if (!anImage.IsNull())
        {
          TextureRes->Init (theContext, *anImage.operator->(), aNewTexture->Type());
        }
        if (!aNewKey.IsEmpty())
        {
          theContext->ShareResource (aNewKey, TextureRes);
        }
      }
    }
  }

  //TelPolygonOffset
  PolygonOffset.mode   = (Aspect_PolygonOffsetMode )theAspect.PolygonOffsetMode;
  PolygonOffset.factor = theAspect.PolygonOffsetFactor;
  PolygonOffset.units  = theAspect.PolygonOffsetUnits;

  CALL_DEF_CONTEXTLINE anEdgeContext;
  anEdgeContext.Color.r  = (float )theAspect.EdgeColor.r;
  anEdgeContext.Color.g  = (float )theAspect.EdgeColor.g;
  anEdgeContext.Color.b  = (float )theAspect.EdgeColor.b;
  anEdgeContext.LineType = (Aspect_TypeOfLine )theAspect.LineType;
  anEdgeContext.Width    = (float )theAspect.Width;
  myAspectEdge.SetContext (anEdgeContext);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_AspectFace::Init (const Handle(OpenGl_Context)&   theContext,
                              const Handle(Graphic3d_AspectFillArea3d)& theAspect)
{
  CALL_DEF_CONTEXTFILLAREA aCAspect;
  Standard_Real           aWidth;
  Quantity_Color          aBackIntColor;
  Quantity_Color          aEdgeColor;
  Aspect_TypeOfLine       aLType;
  Quantity_Color          aIntColor;
  Aspect_InteriorStyle    aIntStyle;
  NCollection_Vec3<Standard_Real> aColor;

  theAspect->Values (aIntStyle, aIntColor, aBackIntColor, aEdgeColor, aLType, aWidth);
  aIntColor.Values (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_RGB);

  aCAspect.Style      = int (aIntStyle);
  aCAspect.IntColor.r = float (aColor.r());
  aCAspect.IntColor.g = float (aColor.g());
  aCAspect.IntColor.b = float (aColor.b());

  if (theAspect->Distinguish())
  {
    aBackIntColor.Values (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_RGB);
  }

  aCAspect.BackIntColor.r = float (aColor.r());
  aCAspect.BackIntColor.g = float (aColor.g());
  aCAspect.BackIntColor.b = float (aColor.b());

  aCAspect.Edge = theAspect->Edge () ? 1:0;
  aEdgeColor.Values (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_RGB);

  aCAspect.EdgeColor.r = float (aColor.r());
  aCAspect.EdgeColor.g = float (aColor.g());
  aCAspect.EdgeColor.b = float (aColor.b());
  aCAspect.LineType    = int (aLType);
  aCAspect.Width       = float (aWidth);
  aCAspect.Hatch       = int (theAspect->HatchStyle ());

  aCAspect.Distinguish = theAspect->Distinguish () ? 1:0;
  aCAspect.BackFace    = theAspect->BackFace ()    ? 1:0;

  aCAspect.Back.Shininess = float ((theAspect->BackMaterial ()).Shininess ());
  aCAspect.Back.Ambient   = float ((theAspect->BackMaterial ()).Ambient ());
  aCAspect.Back.Diffuse   = float ((theAspect->BackMaterial ()).Diffuse ());
  aCAspect.Back.Specular  = float ((theAspect->BackMaterial ()).Specular ());
  aCAspect.Back.Transparency  = float ((theAspect->BackMaterial ()).Transparency ());
  aCAspect.Back.Emission      = float ((theAspect->BackMaterial ()).Emissive ());

  // Reflection mode
  aCAspect.Back.IsAmbient = ((theAspect->BackMaterial ()).ReflectionMode (Graphic3d_TOR_AMBIENT) ? 1 : 0 );
  aCAspect.Back.IsDiffuse = ((theAspect->BackMaterial ()).ReflectionMode (Graphic3d_TOR_DIFFUSE) ? 1 : 0 );
  aCAspect.Back.IsSpecular = ((theAspect->BackMaterial ()).ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0 );
  aCAspect.Back.IsEmission = ((theAspect->BackMaterial ()).ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0 );

  // Material type
  const Graphic3d_MaterialAspect aBackMat = theAspect->BackMaterial ();
  Standard_Boolean isBackPhys = aBackMat.MaterialType (Graphic3d_MATERIAL_PHYSIC);
  aCAspect.Back.IsPhysic = (isBackPhys ? 1 : 0 );

  // Specular Color
  aCAspect.Back.ColorSpec.r = float (((theAspect->BackMaterial ()).SpecularColor ()).Red ());
  aCAspect.Back.ColorSpec.g = float (((theAspect->BackMaterial ()).SpecularColor ()).Green ());
  aCAspect.Back.ColorSpec.b = float (((theAspect->BackMaterial ()).SpecularColor ()).Blue ());

  // Ambient color
  aCAspect.Back.ColorAmb.r = float (((theAspect->BackMaterial ()).AmbientColor ()).Red ());
  aCAspect.Back.ColorAmb.g = float (((theAspect->BackMaterial ()).AmbientColor ()).Green ());
  aCAspect.Back.ColorAmb.b = float (((theAspect->BackMaterial ()).AmbientColor ()).Blue ());

  // Diffuse color
  aCAspect.Back.ColorDif.r = float (((theAspect->BackMaterial ()).DiffuseColor ()).Red ());
  aCAspect.Back.ColorDif.g = float (((theAspect->BackMaterial ()).DiffuseColor ()).Green ());
  aCAspect.Back.ColorDif.b = float (((theAspect->BackMaterial ()).DiffuseColor ()).Blue ());

  // Emissive color
  aCAspect.Back.ColorEms.r = float (((theAspect->BackMaterial ()).EmissiveColor ()).Red ());
  aCAspect.Back.ColorEms.g = float (((theAspect->BackMaterial ()).EmissiveColor ()).Green ());
  aCAspect.Back.ColorEms.b = float (((theAspect->BackMaterial ()).EmissiveColor ()).Blue ());

  aCAspect.Back.EnvReflexion = float ((theAspect->BackMaterial ()).EnvReflexion());

  aCAspect.Front.Shininess    = float ((theAspect->FrontMaterial ()).Shininess ());
  aCAspect.Front.Ambient      = float ((theAspect->FrontMaterial ()).Ambient ());
  aCAspect.Front.Diffuse      = float ((theAspect->FrontMaterial ()).Diffuse ());
  aCAspect.Front.Specular     = float ((theAspect->FrontMaterial ()).Specular ());
  aCAspect.Front.Transparency = float ((theAspect->FrontMaterial ()).Transparency ());
  aCAspect.Front.Emission     = float ((theAspect->FrontMaterial ()).Emissive ());

  // Reflection mode
  aCAspect.Front.IsAmbient    = ((theAspect->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_AMBIENT) ? 1 : 0);
  aCAspect.Front.IsDiffuse    = ((theAspect->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_DIFFUSE) ? 1 : 0);
  aCAspect.Front.IsSpecular   = ((theAspect->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0);
  aCAspect.Front.IsEmission   = ((theAspect->FrontMaterial ()).ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0);

  // Materail type
  const Graphic3d_MaterialAspect aFrontMat = theAspect->FrontMaterial ();
  Standard_Boolean isFrontPhys = aFrontMat.MaterialType (Graphic3d_MATERIAL_PHYSIC);
  aCAspect.Front.IsPhysic = (isFrontPhys ? 1 : 0 );

  // Specular Color
  aCAspect.Front.ColorSpec.r = float (((theAspect->FrontMaterial ()).SpecularColor ()).Red ());
  aCAspect.Front.ColorSpec.g = float (((theAspect->FrontMaterial ()).SpecularColor ()).Green ());
  aCAspect.Front.ColorSpec.b = float (((theAspect->FrontMaterial ()).SpecularColor ()).Blue ());

  // Ambient color
  aCAspect.Front.ColorAmb.r = float (((theAspect->FrontMaterial ()).AmbientColor ()).Red ());
  aCAspect.Front.ColorAmb.g = float (((theAspect->FrontMaterial ()).AmbientColor ()).Green ());
  aCAspect.Front.ColorAmb.b = float (((theAspect->FrontMaterial ()).AmbientColor ()).Blue ());

  // Diffuse color
  aCAspect.Front.ColorDif.r = float (((theAspect->FrontMaterial ()).DiffuseColor ()).Red ());
  aCAspect.Front.ColorDif.g = float (((theAspect->FrontMaterial ()).DiffuseColor ()).Green ());
  aCAspect.Front.ColorDif.b = float (((theAspect->FrontMaterial ()).DiffuseColor ()).Blue ());

  // Emissive color
  aCAspect.Front.ColorEms.r = float (((theAspect->FrontMaterial ()).EmissiveColor ()).Red ());
  aCAspect.Front.ColorEms.g = float (((theAspect->FrontMaterial ()).EmissiveColor ()).Green ());
  aCAspect.Front.ColorEms.b = float (((theAspect->FrontMaterial ()).EmissiveColor ()).Blue ());

  aCAspect.Front.EnvReflexion = float ((theAspect->FrontMaterial ()).EnvReflexion());
  aCAspect.IsDef = 1;
  aCAspect.Texture.TextureMap   = theAspect->TextureMap();
  aCAspect.Texture.doTextureMap = theAspect->TextureMapState() ? 1 : 0;

  Standard_Integer aPolyMode;
  Standard_ShortReal aPolyFactor, aPolyUnits;
  theAspect->PolygonOffsets (aPolyMode, aPolyFactor, aPolyUnits);
  aCAspect.PolygonOffsetMode   = aPolyMode;
  aCAspect.PolygonOffsetFactor = (Standard_ShortReal)aPolyFactor;
  aCAspect.PolygonOffsetUnits  = (Standard_ShortReal)aPolyUnits;

  Init (theContext, aCAspect);
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
void OpenGl_AspectFace::Release (const Handle(OpenGl_Context)& theContext)
{
  if (!TextureRes.IsNull())
  {
    if (!theContext.IsNull())
    {
      if (myTextureId.IsEmpty())
      {
        theContext->DelayedRelease (TextureRes);
      }
      else
      {
        TextureRes.Nullify(); // we need nullify all handles before ReleaseResource() call
        theContext->ReleaseResource (myTextureId);
      }
    }
    TextureRes.Nullify();
  }
  myTextureId.Clear();
}
