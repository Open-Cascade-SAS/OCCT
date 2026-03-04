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

#include <GeomBndLib_Curve.hxx>
#include <GeomBndLib_Circle.hxx>
#include <GeomBndLib_Line.hxx>
#include <ElCLib.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Precision.hxx>

#include <cmath>
#include <algorithm>

//=================================================================================================

void GeomBndLib_OffsetCurve::Add(double theU1, double theU2, double theTol, Bnd_Box& theBox) const
{
  // For a 3D offset curve: P_off(t) = P(t) + d * (T(t) x V) / |T(t) x V|
  // The displacement vector (T x V)/|T x V| is always perpendicular to
  // the reference direction V. Its max component along coordinate axis k
  // is sqrt(1 - V_k^2). So per-coordinate enlargement is: |d| * sqrt(1 - V_k^2).
  const occ::handle<Geom_Curve>& aBasis   = myGeom->BasisCurve();
  const double                   anOffset = std::abs(myGeom->Offset());
  const gp_Dir&                  aDir     = myGeom->Direction();
  Bnd_Box                        aLocalBox;

  if (const occ::handle<Geom_Line> aLine = occ::down_cast<Geom_Line>(aBasis))
  {
    const gp_Lin aBasisLin = aLine->Lin();
    gp_Vec       aShift    = gp_Vec(aBasisLin.Direction()).Crossed(gp_Vec(aDir));
    if (aShift.SquareMagnitude() > Precision::SquareConfusion())
    {
      aShift.Normalize();
      aShift *= myGeom->Offset();
      gp_Pnt aLoc = aBasisLin.Location().Translated(aShift);
      GeomBndLib_Line::Add(gp_Lin(aLoc, aBasisLin.Direction()), theU1, theU2, 0., aLocalBox);
      aLocalBox.Enlarge(theTol);
      theBox.Add(aLocalBox);
      return;
    }
  }

  if (const occ::handle<Geom_Circle> aCircle = occ::down_cast<Geom_Circle>(aBasis))
  {
    const gp_Circ aBasisCirc = aCircle->Circ();
    const double  aDotAxis   = std::abs(aBasisCirc.Axis().Direction().Dot(aDir));
    if (std::abs(1.0 - aDotAxis) <= Precision::Angular())
    {
      const double aUMid = 0.5 * (theU1 + theU2);
      gp_Pnt       aP;
      gp_Vec       aV1;
      ElCLib::D1(aUMid, aBasisCirc, aP, aV1);
      if (aV1.SquareMagnitude() > Precision::SquareConfusion())
      {
        gp_Vec aNormal = aV1.Crossed(gp_Vec(aDir));
        if (aNormal.SquareMagnitude() > Precision::SquareConfusion())
        {
          aNormal.Normalize();
          gp_Vec aRadial(aBasisCirc.Location(), aP);
          if (aRadial.SquareMagnitude() > Precision::SquareConfusion())
          {
            aRadial.Normalize();
            const double aSign = aNormal.Dot(aRadial);
            if (std::abs(std::abs(aSign) - 1.0) <= 1e-9)
            {
              const double aNewRadius = aBasisCirc.Radius() + myGeom->Offset() * aSign;
              if (aNewRadius > Precision::Confusion())
              {
                gp_Circ aOffsetCirc = aBasisCirc;
                aOffsetCirc.SetRadius(aNewRadius);
                GeomBndLib_Circle::Add(aOffsetCirc, theU1, theU2, 0., aLocalBox);
                aLocalBox.Enlarge(theTol);
                theBox.Add(aLocalBox);
                return;
              }
              if (std::abs(aNewRadius) <= Precision::Confusion())
              {
                aLocalBox.Add(aBasisCirc.Location());
                aLocalBox.Enlarge(theTol);
                theBox.Add(aLocalBox);
                return;
              }
            }
          }
        }
      }
    }
  }

  GeomBndLib_Curve aCurveEval(aBasis);
  aCurveEval.Add(theU1, theU2, 0., aLocalBox);

  if (aLocalBox.IsVoid())
  {
    return;
  }

  // Per-coordinate enlargement based on reference direction.
  const double aDx = aDir.X();
  const double aDy = aDir.Y();
  const double aDz = aDir.Z();
  const double aEnlargeX = anOffset * std::sqrt(std::max(0.0, 1.0 - aDx * aDx));
  const double aEnlargeY = anOffset * std::sqrt(std::max(0.0, 1.0 - aDy * aDy));
  const double aEnlargeZ = anOffset * std::sqrt(std::max(0.0, 1.0 - aDz * aDz));

  const bool isOpenXmin = aLocalBox.IsOpenXmin();
  const bool isOpenXmax = aLocalBox.IsOpenXmax();
  const bool isOpenYmin = aLocalBox.IsOpenYmin();
  const bool isOpenYmax = aLocalBox.IsOpenYmax();
  const bool isOpenZmin = aLocalBox.IsOpenZmin();
  const bool isOpenZmax = aLocalBox.IsOpenZmax();

  if (aLocalBox.HasFinitePart())
  {
    auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aLocalBox.FinitePart().Get();
    aLocalBox.Update(aXmin - aEnlargeX, aYmin - aEnlargeY, aZmin - aEnlargeZ,
                     aXmax + aEnlargeX, aYmax + aEnlargeY, aZmax + aEnlargeZ);
  }

  if (isOpenXmin)
    aLocalBox.OpenXmin();
  if (isOpenXmax)
    aLocalBox.OpenXmax();
  if (isOpenYmin)
    aLocalBox.OpenYmin();
  if (isOpenYmax)
    aLocalBox.OpenYmax();
  if (isOpenZmin)
    aLocalBox.OpenZmin();
  if (isOpenZmax)
    aLocalBox.OpenZmax();

  aLocalBox.Enlarge(theTol);
  theBox.Add(aLocalBox);
}
