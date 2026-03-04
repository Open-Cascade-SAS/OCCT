// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomBndLib_OffsetCurve.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <GeomBndLib_Circle.hxx>
#include <GeomBndLib_Curve.hxx>
#include <GeomBndLib_Line.hxx>
#include <GeomBndLib_OtherCurve.hxx>
#include <ElCLib.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Precision.hxx>

#include <algorithm>
#include <cmath>

namespace
{

//! Try the exact line fast path. Returns a non-void box on success.
Bnd_Box tryOffsetLine(const occ::handle<Geom_Curve>&       theBasis,
                      const occ::handle<Geom_OffsetCurve>& theOff,
                      double                               theU1,
                      double                               theU2,
                      double                               theTol)
{
  const occ::handle<Geom_Line> aLine = occ::down_cast<Geom_Line>(theBasis);
  if (aLine.IsNull())
  {
    return Bnd_Box{};
  }
  const gp_Dir& aDir      = theOff->Direction();
  const gp_Lin  aBasisLin = aLine->Lin();
  gp_Vec        aShift    = gp_Vec(aBasisLin.Direction()).Crossed(gp_Vec(aDir));
  if (aShift.SquareMagnitude() <= Precision::SquareConfusion())
  {
    return Bnd_Box{};
  }
  aShift.Normalize();
  aShift *= theOff->Offset();
  const gp_Pnt aLoc = aBasisLin.Location().Translated(aShift);
  Bnd_Box      aBox = GeomBndLib_Line::Box(gp_Lin(aLoc, aBasisLin.Direction()), theU1, theU2, 0.);
  aBox.Enlarge(theTol);
  return aBox;
}

//! Try the exact axis-aligned circle fast path. Returns a non-void box on success.
Bnd_Box tryOffsetCircle(const occ::handle<Geom_Curve>&       theBasis,
                        const occ::handle<Geom_OffsetCurve>& theOff,
                        double                               theU1,
                        double                               theU2,
                        double                               theTol)
{
  const occ::handle<Geom_Circle> aCircle = occ::down_cast<Geom_Circle>(theBasis);
  if (aCircle.IsNull())
  {
    return Bnd_Box{};
  }
  const gp_Dir& aDir       = theOff->Direction();
  const gp_Circ aBasisCirc = aCircle->Circ();
  const double  aDotAxis   = std::abs(aBasisCirc.Axis().Direction().Dot(aDir));
  if (std::abs(1.0 - aDotAxis) > Precision::Angular())
  {
    return Bnd_Box{};
  }
  const double aUMid = 0.5 * (theU1 + theU2);
  gp_Pnt       aP;
  gp_Vec       aV1;
  ElCLib::D1(aUMid, aBasisCirc, aP, aV1);
  if (aV1.SquareMagnitude() <= Precision::SquareConfusion())
  {
    return Bnd_Box{};
  }
  gp_Vec aNormal = aV1.Crossed(gp_Vec(aDir));
  if (aNormal.SquareMagnitude() <= Precision::SquareConfusion())
  {
    return Bnd_Box{};
  }
  aNormal.Normalize();
  gp_Vec aRadial(aBasisCirc.Location(), aP);
  if (aRadial.SquareMagnitude() <= Precision::SquareConfusion())
  {
    return Bnd_Box{};
  }
  aRadial.Normalize();
  const double aSign = aNormal.Dot(aRadial);
  if (std::abs(std::abs(aSign) - 1.0) > 1e-9)
  {
    return Bnd_Box{};
  }
  const double aNewRadius = aBasisCirc.Radius() + theOff->Offset() * aSign;
  Bnd_Box      aBox;
  if (aNewRadius > Precision::Confusion())
  {
    gp_Circ aOffsetCirc = aBasisCirc;
    aOffsetCirc.SetRadius(aNewRadius);
    aBox = GeomBndLib_Circle::Box(aOffsetCirc, theU1, theU2, 0.);
  }
  else if (std::abs(aNewRadius) <= Precision::Confusion())
  {
    aBox.Add(aBasisCirc.Location());
  }
  else
  {
    return Bnd_Box{};
  }
  aBox.Enlarge(theTol);
  return aBox;
}

} // namespace

//=================================================================================================

Bnd_Box GeomBndLib_OffsetCurve::Box(double theU1, double theU2, double theTol) const
{
  // Specialized fast paths for line and axis-aligned circle.
  const occ::handle<Geom_Curve>& aBasis = myGeom->BasisCurve();

  Bnd_Box aBox = tryOffsetLine(aBasis, myGeom, theU1, theU2, theTol);
  if (!aBox.IsVoid())
  {
    return aBox;
  }
  aBox = tryOffsetCircle(aBasis, myGeom, theU1, theU2, theTol);
  if (!aBox.IsVoid())
  {
    return aBox;
  }

  // Generic fallback: enlarge basis curve box per coordinate.
  // The offset displacement magnitude per coordinate k is bounded by |d| * sqrt(1 - Vk^2).
  const double  anOffset = std::abs(myGeom->Offset());
  const gp_Dir& aDir     = myGeom->Direction();

  GeomBndLib_Curve aCurveEval(aBasis);
  aBox = aCurveEval.Box(theU1, theU2, 0.);

  if (aBox.IsVoid())
  {
    return aBox;
  }

  const double aDx       = aDir.X();
  const double aDy       = aDir.Y();
  const double aDz       = aDir.Z();
  const double aEnlargeX = anOffset * std::sqrt(std::max(0.0, 1.0 - aDx * aDx));
  const double aEnlargeY = anOffset * std::sqrt(std::max(0.0, 1.0 - aDy * aDy));
  const double aEnlargeZ = anOffset * std::sqrt(std::max(0.0, 1.0 - aDz * aDz));

  const bool isOpenXmin = aBox.IsOpenXmin();
  const bool isOpenXmax = aBox.IsOpenXmax();
  const bool isOpenYmin = aBox.IsOpenYmin();
  const bool isOpenYmax = aBox.IsOpenYmax();
  const bool isOpenZmin = aBox.IsOpenZmin();
  const bool isOpenZmax = aBox.IsOpenZmax();

  if (aBox.HasFinitePart())
  {
    const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aBox.FinitePart().Get();
    aBox.Update(aXmin - aEnlargeX, aYmin - aEnlargeY, aZmin - aEnlargeZ,
                aXmax + aEnlargeX, aYmax + aEnlargeY, aZmax + aEnlargeZ);
  }

  if (isOpenXmin) aBox.OpenXmin();
  if (isOpenXmax) aBox.OpenXmax();
  if (isOpenYmin) aBox.OpenYmin();
  if (isOpenYmax) aBox.OpenYmax();
  if (isOpenZmin) aBox.OpenZmin();
  if (isOpenZmax) aBox.OpenZmax();

  aBox.Enlarge(theTol);
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_OffsetCurve::BoxOptimal(double theU1,
                                           double theU2,
                                           double theTol) const
{
  // Reuse exact fast paths.
  const occ::handle<Geom_Curve>& aBasis = myGeom->BasisCurve();

  Bnd_Box aBox = tryOffsetLine(aBasis, myGeom, theU1, theU2, theTol);
  if (!aBox.IsVoid())
  {
    return aBox;
  }
  aBox = tryOffsetCircle(aBasis, myGeom, theU1, theU2, theTol);
  if (!aBox.IsVoid())
  {
    return aBox;
  }

  // Sampling fallback for precise result on general offset curves.
  GeomAdaptor_Curve     anAdaptor(myGeom);
  GeomBndLib_OtherCurve anOther(anAdaptor);
  return anOther.BoxOptimal(theU1, theU2, theTol);
}
