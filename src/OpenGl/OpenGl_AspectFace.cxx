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
