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

#include <GeomBndLib_OffsetCurve2d.hxx>

#include <Geom2dAdaptor_Curve.hxx>
#include <GeomBndLib_Circle2d.hxx>
#include <GeomBndLib_Curve2d.hxx>
#include <GeomBndLib_Line2d.hxx>
#include <GeomBndLib_OtherCurve2d.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Precision.hxx>

#include <cmath>

//=================================================================================================

Bnd_Box2d GeomBndLib_OffsetCurve2d::Box(double theU1, double theU2, double theTol) const
{
  // Specialized fast paths for line and circle.
  // For the general fallback, enlarge the basis curve box by |offset|.
  const occ::handle<Geom2d_Curve>& aBasis   = myGeom->BasisCurve();
  const double                     anOffset = std::abs(myGeom->Offset());

  if (const occ::handle<Geom2d_Line> aLine = occ::down_cast<Geom2d_Line>(aBasis))
  {
    const gp_Lin2d aBasisLin = aLine->Lin2d();
    gp_Vec2d       aNormal(aBasisLin.Direction().Y(), -aBasisLin.Direction().X());
    if (aNormal.SquareMagnitude() > Precision::SquareConfusion())
    {
      aNormal.Normalize();
      aNormal *= myGeom->Offset();
      const gp_Pnt2d aLoc = aBasisLin.Location().Translated(aNormal);
      Bnd_Box2d      aLocalBox =
        GeomBndLib_Line2d::Box(gp_Lin2d(aLoc, aBasisLin.Direction()), theU1, theU2, 0.);
      aLocalBox.Enlarge(theTol);
      return aLocalBox;
    }
  }

  if (const occ::handle<Geom2d_Circle> aCircle = occ::down_cast<Geom2d_Circle>(aBasis))
  {
    const gp_Circ2d aBasisCirc = aCircle->Circ2d();
    const double    aUMid      = 0.5 * (theU1 + theU2);
    gp_Pnt2d        aP;
    gp_Vec2d        aV1;
    ElCLib::D1(aUMid, aBasisCirc, aP, aV1);
    if (aV1.SquareMagnitude() > Precision::SquareConfusion())
    {
      gp_Vec2d aNormal(aV1.Y(), -aV1.X());
      if (aNormal.SquareMagnitude() > Precision::SquareConfusion())
      {
        aNormal.Normalize();
        gp_Vec2d aRadial(aBasisCirc.Location(), aP);
        if (aRadial.SquareMagnitude() > Precision::SquareConfusion())
        {
          aRadial.Normalize();
          const double aSign = aNormal.Dot(aRadial);
          if (std::abs(std::abs(aSign) - 1.0) <= 1e-9)
          {
            const double aNewRadius = aBasisCirc.Radius() + myGeom->Offset() * aSign;
            if (aNewRadius > Precision::Confusion())
            {
              gp_Circ2d aOffsetCirc = aBasisCirc;
              aOffsetCirc.SetRadius(aNewRadius);
              Bnd_Box2d aLocalBox = GeomBndLib_Circle2d::Box(aOffsetCirc, theU1, theU2, 0.);
              aLocalBox.Enlarge(theTol);
              return aLocalBox;
            }
            if (std::abs(aNewRadius) <= Precision::Confusion())
            {
              Bnd_Box2d aLocalBox;
              aLocalBox.Add(aBasisCirc.Location());
              aLocalBox.Enlarge(theTol);
              return aLocalBox;
            }
          }
        }
      }
    }
  }

  // Generic fallback: enlarge basis curve box by the offset distance.
  GeomBndLib_Curve2d aCurveEval(aBasis);
  Bnd_Box2d          aLocalBox = aCurveEval.Box(theU1, theU2, 0.);
  aLocalBox.Enlarge(anOffset);
  aLocalBox.Enlarge(theTol);
  return aLocalBox;
}

//=================================================================================================

Bnd_Box2d GeomBndLib_OffsetCurve2d::BoxOptimal(double theU1, double theU2, double theTol) const
{
  // Sample the actual offset curve directly for a tight bounding box.
  Geom2dAdaptor_Curve     anAdaptor(myGeom);
  GeomBndLib_OtherCurve2d anOther(anAdaptor);
  return anOther.BoxOptimal(theU1, theU2, theTol);
}
