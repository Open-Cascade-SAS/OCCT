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

#include <GeomBndLib_SurfaceOfRevolution.hxx>

#include <GeomBndLib_Circle.hxx>
#include <GeomBndLib_Curve.hxx>
#include <Geom_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

namespace
{

//! Add the bounding box contribution of a single revolution circle
//! for a point revolved around the axis.
void addRevolutionCircle(const gp_Pnt& theOrigin,
                         const gp_Dir& theAxisDir,
                         const gp_Pnt& theBasisPt,
                         const double  theUMin,
                         const double  theUMax,
                         Bnd_Box&      theBox)
{
  // Project the basis curve point onto the revolution axis.
  const gp_XYZ aDelta  = theBasisPt.XYZ() - theOrigin.XYZ();
  const double  aH     = aDelta.Dot(theAxisDir.XYZ());
  const gp_XYZ aCenter = theOrigin.XYZ() + aH * theAxisDir.XYZ();

  // Radial vector from axis to basis point.
  const gp_XYZ aRadial = theBasisPt.XYZ() - aCenter;
  const double  aRadius = aRadial.Modulus();

  if (aRadius < Precision::Confusion())
  {
    // Point is on the axis — revolution is a single point.
    theBox.Add(theBasisPt);
    return;
  }

  // Construct the revolution circle.
  const gp_Dir aXDir(aRadial);
  const gp_Ax2 anAx2(gp_Pnt(aCenter), theAxisDir, aXDir);
  const gp_Circ aCirc(anAx2, aRadius);

  GeomBndLib_Circle::Add(aCirc, theUMin, theUMax, 0., theBox);
}

} // namespace

//=================================================================================================

void GeomBndLib_SurfaceOfRevolution::Add(double theTol, Bnd_Box& theBox) const
{
  double aU1 = 0., aU2 = 0., aV1 = 0., aV2 = 0.;
  myGeom->Bounds(aU1, aU2, aV1, aV2);
  Add(aU1, aU2, aV1, aV2, theTol, theBox);
}

//=================================================================================================

void GeomBndLib_SurfaceOfRevolution::Add(double   theUMin,
                                         double   theUMax,
                                         double   theVMin,
                                         double   theVMax,
                                         double   theTol,
                                         Bnd_Box& theBox) const
{
  const occ::handle<Geom_Curve>& aBasisCurve = myGeom->BasisCurve();
  const gp_Ax1                   anAxis      = myGeom->Axis();
  const gp_Pnt&                  anOrigin    = anAxis.Location();
  const gp_Dir&                  anAxisDir   = anAxis.Direction();

  // Clamp V range to basis curve bounds.
  const double aVFirst = aBasisCurve->FirstParameter();
  const double aVLast  = aBasisCurve->LastParameter();
  const double aVMin   = Precision::IsNegativeInfinite(theVMin) ? aVFirst : theVMin;
  const double aVMax   = Precision::IsPositiveInfinite(theVMax) ? aVLast : theVMax;

  // Compute the basis curve bounding box for [VMin, VMax].
  // Then revolve all 8 corners of the box around the axis.
  // This is conservative: the curve is inside its box, so the revolved box
  // contains the revolved curve. It avoids sampling and captures all curve features.
  GeomBndLib_Curve aCurveEval(aBasisCurve);
  Bnd_Box          aCurveBox;
  aCurveEval.Add(aVMin, aVMax, 0., aCurveBox);

  if (aCurveBox.IsVoid())
  {
    theBox.Enlarge(theTol);
    return;
  }
  if (aCurveBox.IsOpen())
  {
    theBox.SetWhole();
    theBox.Enlarge(theTol);
    return;
  }

  // Get() returns {Xmin, Xmax, Ymin, Ymax, Zmin, Zmax}.
  const auto [aXmin, aXmax, aYmin, aYmax, aZmin, aZmax] = aCurveBox.Get();

  // Revolve all 8 corners of the basis curve bounding box.
  const double aXVals[2] = {aXmin, aXmax};
  const double aYVals[2] = {aYmin, aYmax};
  const double aZVals[2] = {aZmin, aZmax};

  for (int ix = 0; ix < 2; ++ix)
  {
    for (int iy = 0; iy < 2; ++iy)
    {
      for (int iz = 0; iz < 2; ++iz)
      {
        const gp_Pnt aCorner(aXVals[ix], aYVals[iy], aZVals[iz]);
        addRevolutionCircle(anOrigin, anAxisDir, aCorner, theUMin, theUMax, theBox);
      }
    }
  }

  theBox.Enlarge(theTol);
}
