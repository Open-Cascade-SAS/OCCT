// Created on: 2003-12-17
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#include <Font_NameOfFont.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_DrawerAttribute.hxx>
#include <MeshVS_Tool.hxx>
#include <Precision.hxx>

//=================================================================================================

occ::handle<Graphic3d_AspectFillArea3d> MeshVS_Tool::CreateAspectFillArea3d(
  const occ::handle<MeshVS_Drawer>& theDr,
  const Graphic3d_MaterialAspect&   Mat,
  const bool                        UseDefaults)
{
  occ::handle<Graphic3d_AspectFillArea3d> anAsp;
  if (theDr.IsNull())
    return anAsp;

  Aspect_InteriorStyle     anIntStyle = Aspect_IS_EMPTY;
  Quantity_Color           anIntColor = Quantity_NOC_CYAN1, anEdgeColor = Quantity_NOC_WHITE;
  Aspect_TypeOfLine        anEdgeType  = Aspect_TOL_SOLID;
  double                   anEdgeWidth = 1.0;
  Aspect_HatchStyle        aHStyle     = Aspect_HS_HORIZONTAL;
  Graphic3d_MaterialAspect aFrMat = Mat, aBackMat = Mat;

  int anIntStyleI = (int)Aspect_IS_EMPTY;
  int anEdgeTypeI = (int)Aspect_TOL_SOLID;
  int aHStyleI    = (int)Aspect_HS_HORIZONTAL;

  if (!theDr->GetColor(MeshVS_DA_InteriorColor, anIntColor) && !UseDefaults)
    return anAsp;

  Quantity_Color aBackIntColor = anIntColor;
  if (!theDr->GetColor(MeshVS_DA_BackInteriorColor, aBackIntColor) && !UseDefaults)
    return anAsp;

  if (!theDr->GetColor(MeshVS_DA_EdgeColor, anEdgeColor) && !UseDefaults)
    return anAsp;

  if (!theDr->GetDouble(MeshVS_DA_EdgeWidth, anEdgeWidth) && !UseDefaults)
    return anAsp;

  if (!theDr->GetInteger(MeshVS_DA_InteriorStyle, anIntStyleI) && !UseDefaults)
    return anAsp;
  else
    anIntStyle = (Aspect_InteriorStyle)anIntStyleI;

  if (!theDr->GetInteger(MeshVS_DA_EdgeType, anEdgeTypeI) && !UseDefaults)
    return anAsp;
  else
    anEdgeType = (Aspect_TypeOfLine)anEdgeTypeI;

  if (!theDr->GetInteger(MeshVS_DA_HatchStyle, aHStyleI) && !UseDefaults)
    return anAsp;
  else
    aHStyle = (Aspect_HatchStyle)aHStyleI;

  anAsp = new Graphic3d_AspectFillArea3d(anIntStyle,
                                         anIntColor,
                                         anEdgeColor,
                                         anEdgeType,
                                         anEdgeWidth,
                                         aFrMat,
                                         aBackMat);
  anAsp->SetBackInteriorColor(aBackIntColor);
  anAsp->SetHatchStyle(aHStyle);

  return anAsp;
}

//=================================================================================================

occ::handle<Graphic3d_AspectFillArea3d> MeshVS_Tool::CreateAspectFillArea3d(
  const occ::handle<MeshVS_Drawer>& theDr,
  const bool                        UseDefaults)
{
  Graphic3d_MaterialAspect aFrMat    = Graphic3d_NameOfMaterial_Brass;
  Graphic3d_MaterialAspect aBackMat  = Graphic3d_NameOfMaterial_Brass;
  int                      aFrMatI   = (int)Graphic3d_NameOfMaterial_Brass;
  int                      aBackMatI = (int)Graphic3d_NameOfMaterial_Brass;

  if (!theDr->GetInteger(MeshVS_DA_FrontMaterial, aFrMatI) && !UseDefaults)
    return 0;
  else
    aFrMat = (Graphic3d_MaterialAspect)(Graphic3d_NameOfMaterial)aFrMatI;

  if (!theDr->GetInteger(MeshVS_DA_BackMaterial, aBackMatI) && !UseDefaults)
    return 0;
  else
    aBackMat = (Graphic3d_MaterialAspect)(Graphic3d_NameOfMaterial)aBackMatI;

  occ::handle<Graphic3d_AspectFillArea3d> aFill =
    CreateAspectFillArea3d(theDr, aFrMat, UseDefaults);
  aFill->SetBackMaterial(aBackMat);

  return aFill;
}

//=================================================================================================

occ::handle<Graphic3d_AspectLine3d> MeshVS_Tool::CreateAspectLine3d(
  const occ::handle<MeshVS_Drawer>& theDr,
  const bool                        UseDefaults)
{
  occ::handle<Graphic3d_AspectLine3d> anAsp;
  if (theDr.IsNull())
    return anAsp;

  Quantity_Color    aBeamColor = Quantity_NOC_YELLOW;
  Aspect_TypeOfLine aBeamType  = Aspect_TOL_SOLID;
  double            aBeamWidth = 1.0;
  int               aBeamTypeI = (int)Aspect_TOL_SOLID;

  if (!theDr->GetColor(MeshVS_DA_BeamColor, aBeamColor) && !UseDefaults)
    return anAsp;

  if (!theDr->GetDouble(MeshVS_DA_BeamWidth, aBeamWidth) && !UseDefaults)
    return anAsp;

  if (!theDr->GetInteger(MeshVS_DA_BeamType, aBeamTypeI) && !UseDefaults)
    return anAsp;
  else
    aBeamType = (Aspect_TypeOfLine)aBeamTypeI;

  anAsp = new Graphic3d_AspectLine3d(aBeamColor, aBeamType, aBeamWidth);

  return anAsp;
}

//=================================================================================================

occ::handle<Graphic3d_AspectMarker3d> MeshVS_Tool::CreateAspectMarker3d(
  const occ::handle<MeshVS_Drawer>& theDr,
  const bool                        UseDefaults)
{
  occ::handle<Graphic3d_AspectMarker3d> anAsp;
  if (theDr.IsNull())
    return anAsp;

  Quantity_Color      aMColor = Quantity_NOC_YELLOW;
  Aspect_TypeOfMarker aMType  = Aspect_TOM_X;
  double              aMScale = 1.0;
  int                 aMTypeI = (int)Aspect_TOM_X;

  if (!theDr->GetColor(MeshVS_DA_MarkerColor, aMColor) && !UseDefaults)
    return anAsp;

  if (!theDr->GetDouble(MeshVS_DA_MarkerScale, aMScale) && !UseDefaults)
    return anAsp;

  if (!theDr->GetInteger(MeshVS_DA_MarkerType, aMTypeI) && !UseDefaults)
    return anAsp;
  else
    aMType = (Aspect_TypeOfMarker)aMTypeI;

  anAsp = new Graphic3d_AspectMarker3d(aMType, aMColor, aMScale);

  return anAsp;
}

//=================================================================================================

occ::handle<Graphic3d_AspectText3d> MeshVS_Tool::CreateAspectText3d(
  const occ::handle<MeshVS_Drawer>& theDr,
  const bool                        UseDefaults)
{
  occ::handle<Graphic3d_AspectText3d> anAsp;
  if (theDr.IsNull())
    return anAsp;

  Quantity_Color           aTColor     = Quantity_NOC_YELLOW;
  double                   anExpFactor = 1.0, aSpace = 0.0;
  const char*              aFont       = Font_NOF_ASCII_MONO;
  Aspect_TypeOfStyleText   aStyle      = Aspect_TOST_NORMAL;
  Aspect_TypeOfDisplayText aDispText   = Aspect_TODT_NORMAL;
  TCollection_AsciiString  aFontString = Font_NOF_ASCII_MONO;
  Font_FontAspect          aFontAspect = Font_FA_Regular;
  int                      aStyleI     = (int)Aspect_TOST_NORMAL;
  int                      aDispTextI  = (int)Aspect_TODT_NORMAL;
  // Bold font is used by default for better text readability
  int aFontAspectI = (int)Font_FA_Bold;

  if (!theDr->GetColor(MeshVS_DA_TextColor, aTColor) && !UseDefaults)
    return anAsp;

  if (!theDr->GetDouble(MeshVS_DA_TextExpansionFactor, anExpFactor) && !UseDefaults)
    return anAsp;

  if (!theDr->GetDouble(MeshVS_DA_TextSpace, aSpace) && !UseDefaults)
    return anAsp;

  if (!theDr->GetAsciiString(MeshVS_DA_TextFont, aFontString) && !UseDefaults)
    return anAsp;
  else
    aFont = aFontString.ToCString();

  if (!theDr->GetInteger(MeshVS_DA_TextStyle, aStyleI) && !UseDefaults)
    return anAsp;
  else
    aStyle = (Aspect_TypeOfStyleText)aStyleI;

  if (!theDr->GetInteger(MeshVS_DA_TextDisplayType, aDispTextI) && !UseDefaults)
    return anAsp;
  else
    aDispText = (Aspect_TypeOfDisplayText)aDispTextI;

  if (!theDr->GetInteger(MeshVS_DA_TextFontAspect, aFontAspectI) && !UseDefaults)
    return anAsp;
  else
    aFontAspect = (Font_FontAspect)aFontAspectI;

  anAsp = new Graphic3d_AspectText3d(aTColor, aFont, anExpFactor, aSpace, aStyle, aDispText);
  anAsp->SetTextFontAspect(aFontAspect);
  return anAsp;
}

//=================================================================================================

bool MeshVS_Tool::GetNormal(const NCollection_Array1<double>& Nodes, gp_Vec& Norm)
{
  int first = Nodes.Lower(), last = Nodes.Upper(), count = (last - first + 1) / 3, i, j;
  if (first == 0)
  {
    first = 1;
    count = int(Nodes.Value(0));
  }

  if (count < 3)
    return false;

  bool res = true;

  double           normal[3], first_vec[3], cur_vec[3], xx, yy, zz;
  constexpr double conf = Precision::Confusion();

  for (i = 0; i < 3; i++)
  {
    normal[i]    = 0.0;
    first_vec[i] = Nodes.Value(first + 3 + i) - Nodes.Value(first + i);
  }

  for (i = 2; i < count; i++)
  {
    for (j = 0; j < 3; j++)
      cur_vec[j] = Nodes.Value(first + 3 * i + j) - Nodes.Value(first + j);

    xx = first_vec[1] * cur_vec[2] - first_vec[2] * cur_vec[1];
    yy = first_vec[2] * cur_vec[0] - first_vec[0] * cur_vec[2];
    zz = first_vec[0] * cur_vec[1] - first_vec[1] * cur_vec[0];

    cur_vec[0] = xx;
    cur_vec[1] = yy;
    cur_vec[2] = zz;

    if (fabs(cur_vec[0]) > conf || fabs(cur_vec[1]) > conf || fabs(cur_vec[2]) > conf)
    {
      double cur =
        std::sqrt(cur_vec[0] * cur_vec[0] + cur_vec[1] * cur_vec[1] + cur_vec[2] * cur_vec[2]);
      for (int k = 0; k < 3; k++)
        cur_vec[k] /= cur;
    }

    if (fabs(normal[0]) <= conf && fabs(normal[1]) <= conf && fabs(normal[2]) <= conf)
      for (int k = 0; k < 3; k++)
        normal[k] = cur_vec[k];

    if (fabs(normal[0] - cur_vec[0]) > conf || fabs(normal[1] - cur_vec[1]) > conf
        || fabs(normal[2] - cur_vec[2]) > conf)
    {
      res = false;
      break;
    }
  }

  if (res)
    Norm.SetCoord(normal[0], normal[1], normal[2]);

  return res;
}

//=================================================================================================

bool MeshVS_Tool::GetAverageNormal(const NCollection_Array1<double>& Nodes, gp_Vec& Norm)
{
  int first = Nodes.Lower(), last = Nodes.Upper(), count = (last - first + 1) / 3, i, j;
  if (first == 0)
  {
    first = 1;
    count = int(Nodes.Value(0));
  }

  if (count < 3)
    return false;

  bool res = true;

  double           normal[3], first_vec[3], cur_vec[3], xx, yy, zz;
  constexpr double conf = Precision::Confusion();

  for (i = 0; i < 3; i++)
  {
    normal[i]    = 0.0;
    first_vec[i] = Nodes.Value(first + 3 + i) - Nodes.Value(first + i);
  }

  gp_XYZ* norm_vec = new gp_XYZ[count - 2];
  for (i = 0; i < count - 2; i++)
    norm_vec[i].SetCoord(0, 0, 0);

  for (i = 2; i < count; i++)
  {
    for (j = 0; j < 3; j++)
      cur_vec[j] = Nodes.Value(first + 3 * i + j) - Nodes.Value(first + j);

    xx = first_vec[1] * cur_vec[2] - first_vec[2] * cur_vec[1];
    yy = first_vec[2] * cur_vec[0] - first_vec[0] * cur_vec[2];
    zz = first_vec[0] * cur_vec[1] - first_vec[1] * cur_vec[0];

    cur_vec[0] = xx;
    cur_vec[1] = yy;
    cur_vec[2] = zz;

    if (fabs(cur_vec[0]) > conf || fabs(cur_vec[1]) > conf || fabs(cur_vec[2]) > conf)
    {
      double cur =
        std::sqrt(cur_vec[0] * cur_vec[0] + cur_vec[1] * cur_vec[1] + cur_vec[2] * cur_vec[2]);
      for (int k = 0; k < 3; k++)
        cur_vec[k] /= cur;
    }

    norm_vec[i - 2].SetCoord(cur_vec[0], cur_vec[1], cur_vec[2]);

    if (fabs(normal[0]) <= conf && fabs(normal[1]) <= conf && fabs(normal[2]) <= conf)
      for (int k = 0; k < 3; k++)
        normal[k] = cur_vec[k];

    if (fabs(normal[0] - cur_vec[0]) > conf || fabs(normal[1] - cur_vec[1]) > conf
        || fabs(normal[2] - cur_vec[2]) > conf)
    {
      res = false;
    }
  }

  if (!res)
  {
    for (j = 0; j < 3; j++)
    {
      normal[j] = 0.0;
      for (i = 0; i < count - 2; i++)
        normal[j] += norm_vec[i].Coord(j + 1);
      normal[j] /= (count - 2);
    }
  }
  delete[] norm_vec;

  Norm.SetCoord(normal[0], normal[1], normal[2]);

  return res;
}
