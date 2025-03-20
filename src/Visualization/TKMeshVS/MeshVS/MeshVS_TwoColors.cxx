// Created on: 2003-10-08
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

#include <MeshVS_TwoColors.hxx>

//=================================================================================================

MeshVS_TwoColors BindTwoColors(const Quantity_Color& theCol1, const Quantity_Color& theCol2)
{
  MeshVS_TwoColors                aRes;
  NCollection_Vec3<Standard_Real> aColor_sRGB;
  theCol1.Values(aColor_sRGB.r(), aColor_sRGB.g(), aColor_sRGB.b(), Quantity_TOC_sRGB);
  aRes.r1 = unsigned(aColor_sRGB.r() * 255.0);
  aRes.g1 = unsigned(aColor_sRGB.g() * 255.0);
  aRes.b1 = unsigned(aColor_sRGB.b() * 255.0);
  theCol2.Values(aColor_sRGB.r(), aColor_sRGB.g(), aColor_sRGB.b(), Quantity_TOC_sRGB);
  aRes.r2 = unsigned(aColor_sRGB.r() * 255.0);
  aRes.g2 = unsigned(aColor_sRGB.g() * 255.0);
  aRes.b2 = unsigned(aColor_sRGB.b() * 255.0);

  return aRes;
}

//=================================================================================================

Quantity_Color ExtractColor(MeshVS_TwoColors& theTwoColors, const Standard_Integer Index)
{
  Quantity_Color aRes;
  Standard_Real  max = 255.0;

  if (Index == 1)
    aRes.SetValues(Standard_Real(theTwoColors.r1) / max,
                   Standard_Real(theTwoColors.g1) / max,
                   Standard_Real(theTwoColors.b1) / max,
                   Quantity_TOC_sRGB);
  else if (Index == 2)
    aRes.SetValues(Standard_Real(theTwoColors.r2) / max,
                   Standard_Real(theTwoColors.g2) / max,
                   Standard_Real(theTwoColors.b2) / max,
                   Quantity_TOC_sRGB);

  return aRes;
}

//=================================================================================================

void ExtractColors(MeshVS_TwoColors& theTwoColors, Quantity_Color& theCol1, Quantity_Color& theCol2)
{
  Standard_Real max = 255.0;
  theCol1.SetValues(Standard_Real(theTwoColors.r1) / max,
                    Standard_Real(theTwoColors.g1) / max,
                    Standard_Real(theTwoColors.b1) / max,
                    Quantity_TOC_sRGB);
  theCol2.SetValues(Standard_Real(theTwoColors.r2) / max,
                    Standard_Real(theTwoColors.g2) / max,
                    Standard_Real(theTwoColors.b2) / max,
                    Quantity_TOC_sRGB);
}
