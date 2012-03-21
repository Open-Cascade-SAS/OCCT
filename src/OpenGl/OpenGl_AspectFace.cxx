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

#include <InterfaceGraphic_Graphic3d.hxx>
#include <Aspect_PolygonOffsetMode.hxx>

/*----------------------------------------------------------------------*/

static const TEL_CONTEXT_FACE myDefaultAspectFace =
{
  Aspect_IS_SOLID,
  TOn, TEL_HS_SOLID, TOn, TelCullNone,
  { 0.2F, 0.8F, 0.1F, 0.0F, /* amb, diff, spec, emsv */
    1.0F, 10.0F, 0.0F, /* trans, shine, env_reflexion */
    0, /* isphysic */
    (OPENGL_AMBIENT_MASK | OPENGL_DIFFUSE_MASK | OPENGL_SPECULAR_MASK), /* color_mask */
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, /* ambient color */
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, /* diffuse color */
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, /* specular color */
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, /* emissive color */
    {{ 1.0F, 1.0F, 1.0F, 1.0F }} /* material color */
  },
  { 0.2F, 0.8F, 0.1F, 0.0F, /* amb, diff, spec, emsv */
    1.0F, 10.0F, 0.0F, /* trans, shine, env_reflexion */
    0, /* isphysic */
    (OPENGL_AMBIENT_MASK | OPENGL_DIFFUSE_MASK | OPENGL_SPECULAR_MASK), /* color_mask */
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, /* ambient color */
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, /* diffuse color */
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, /* specular color */
    {{ 1.0F, 1.0F, 1.0F, 1.0F }}, /* emissive color */
    {{ 1.0F, 1.0F, 1.0F, 1.0F }} /* material color */
  },
  0, -1, { Aspect_POM_Fill, 1.0F, 0.0F }
};

/*----------------------------------------------------------------------*/

static void ConvertMaterial (const CALL_DEF_MATERIAL &material, OPENGL_SURF_PROP &surface)
{
  /* Cas par cas pour l evaluation */
  surface.amb  = material.IsAmbient? material.Ambient : 0.F;
  surface.diff = material.IsDiffuse? material.Diffuse : 0.F;
  surface.spec = material.IsSpecular? material.Specular : 0.F;
  surface.emsv = material.IsEmission? material.Emission : 0.F;

  /* type de materiel */
  surface.isphysic = material.IsPhysic? 1 : 0;

  /* Couleur du materiel */
  surface.color_mask = 0;
  if ( material.IsAmbient )
    surface.color_mask |= OPENGL_AMBIENT_MASK;
  if ( material.IsDiffuse )
    surface.color_mask |= OPENGL_DIFFUSE_MASK;
  if ( material.IsSpecular )
    surface.color_mask |= OPENGL_SPECULAR_MASK;
  if ( material.IsEmission )
    surface.color_mask |= OPENGL_EMISSIVE_MASK;

  /* Couleur eclairage ambient */
  surface.ambcol.rgb[0] = material.ColorAmb.r;
  surface.ambcol.rgb[1] = material.ColorAmb.g;
  surface.ambcol.rgb[2] = material.ColorAmb.b;
  surface.ambcol.rgb[3] = 1.F;

  /* Couleur eclairage diffus */
  surface.difcol.rgb[0] = material.ColorDif.r;
  surface.difcol.rgb[1] = material.ColorDif.g;
  surface.difcol.rgb[2] = material.ColorDif.b;
  surface.difcol.rgb[3] = 1.F;

  /* Couleur eclairage speculaire */
  surface.speccol.rgb[0] = material.ColorSpec.r;
  surface.speccol.rgb[1] = material.ColorSpec.g;
  surface.speccol.rgb[2] = material.ColorSpec.b;
  surface.speccol.rgb[3] = 1.F;

  /* Couleur d emission */
  surface.emscol.rgb[0] = material.ColorEms.r;
  surface.emscol.rgb[1] = material.ColorEms.g;
  surface.emscol.rgb[2] = material.ColorEms.b;
  surface.emscol.rgb[3] = 1.F;

  surface.shine = ( float )128 * material.Shininess;
  surface.env_reflexion = material.EnvReflexion;

  /* Dans la couche C++ :
  * prop->trans = 0. => opaque
  * prop->trans = 1. => transparent
  * in OpenGl it is opposite.
  */
  surface.trans = 1.0F - material.Transparency;
}

/*----------------------------------------------------------------------*/

OpenGl_AspectFace::OpenGl_AspectFace ()
 : myContext(myDefaultAspectFace)
{}

/*----------------------------------------------------------------------*/

void OpenGl_AspectFace::SetContext (const CALL_DEF_CONTEXTFILLAREA &AContext)
{
  //TelInteriorStyle
  myContext.InteriorStyle = (Aspect_InteriorStyle) AContext.Style;

  //TelEdgeFlag
  myContext.Edge = AContext.Edge ? TOn : TOff;

  //TelInteriorStyleIndex
  switch( AContext.Hatch )
  {
    case 0 : /* Aspect_HS_HORIZONTAL */
      myContext.Hatch = TEL_HS_HORIZONTAL;
      break;
    case 1 : /* Aspect_HS_HORIZONTAL_WIDE */
      myContext.Hatch = TEL_HS_HORIZONTAL_SPARSE;
      break;
    case 2 : /* Aspect_HS_VERTICAL */
      myContext.Hatch = TEL_HS_VERTICAL;
      break;
    case 3 : /* Aspect_HS_VERTICAL_WIDE */
      myContext.Hatch = TEL_HS_VERTICAL_SPARSE;
      break;
    case 4 : /* Aspect_HS_DIAGONAL_45 */
      myContext.Hatch = TEL_HS_DIAG_45;
      break;
    case 5 : /* Aspect_HS_DIAGONAL_45_WIDE */
      myContext.Hatch = TEL_HS_DIAG_45_SPARSE;
      break;
    case 6 : /* Aspect_HS_DIAGONAL_135 */
      myContext.Hatch = TEL_HS_DIAG_135;
      break;
    case 7 : /* Aspect_HS_DIAGONAL_135_WIDE */
      myContext.Hatch = TEL_HS_DIAG_135_SPARSE;
      break;
    case 8 : /* Aspect_HS_GRID */
      myContext.Hatch = TEL_HS_GRID;
      break;
    case 9 : /* Aspect_HS_GRID_WIDE */
      myContext.Hatch = TEL_HS_GRID_SPARSE;
      break;
    case 10 : /* Aspect_HS_GRID_DIAGONAL */
      myContext.Hatch = TEL_HS_CROSS;
      break;
    case 11 : /* Aspect_HS_GRID_DIAGONAL_WIDE */
      myContext.Hatch = TEL_HS_CROSS_SPARSE;
      break;
    default :
      myContext.Hatch = 0;
      break;
  }

  //TelFaceDistinguishingMode
  myContext.DistinguishingMode = AContext.Distinguish ? TOn : TOff;

  //TelFaceCullingMode
  myContext.CullingMode = AContext.BackFace ? TelCullBack : TelCullNone;

  //TelSurfaceAreaProperties
  ConvertMaterial(AContext.Front,myContext.IntFront);

  //TelBackSurfaceAreaProperties
  ConvertMaterial(AContext.Back,myContext.IntBack);

  //TelInteriorColour
  myContext.IntFront.matcol.rgb[0] = (float) AContext.IntColor.r;
  myContext.IntFront.matcol.rgb[1] = (float) AContext.IntColor.g;
  myContext.IntFront.matcol.rgb[2] = (float) AContext.IntColor.b;
  myContext.IntFront.matcol.rgb[3] = 1.f;

  //TelBackInteriorColour
  myContext.IntBack.matcol.rgb[0] = (float) AContext.BackIntColor.r;
  myContext.IntBack.matcol.rgb[1] = (float) AContext.BackIntColor.g;
  myContext.IntBack.matcol.rgb[2] = (float) AContext.BackIntColor.b;
  myContext.IntBack.matcol.rgb[3] = 1.f;

  //TelDoTextureMap
  myContext.doTextureMap = AContext.Texture.doTextureMap;

  //TelTextureId
  myContext.TexId = AContext.Texture.TexId;

  //TelPolygonOffset
  myContext.PolygonOffset.mode   = (Aspect_PolygonOffsetMode) AContext.PolygonOffsetMode;
  myContext.PolygonOffset.factor = AContext.PolygonOffsetFactor;
  myContext.PolygonOffset.units  = AContext.PolygonOffsetUnits;

  CALL_DEF_CONTEXTLINE anEdgeContext;

  //TelEdgeColour
  anEdgeContext.Color.r = (float) AContext.EdgeColor.r;
  anEdgeContext.Color.g = (float) AContext.EdgeColor.g;
  anEdgeContext.Color.b = (float) AContext.EdgeColor.b;

  //TelEdgeType
  anEdgeContext.LineType = (Aspect_TypeOfLine) AContext.LineType;

  //TelEdgeWidth
  anEdgeContext.Width = (float) AContext.Width;

  myAspectEdge.SetContext(anEdgeContext);
}

/*----------------------------------------------------------------------*/

void OpenGl_AspectFace::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  AWorkspace->SetAspectFace(this);
}

/*----------------------------------------------------------------------*/
