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

#include <GeomBndLib_BezierCurve2d.hxx>

#include <Geom2d_Geometry.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Precision.hxx>
#include "GeomBndLib_SplineHelpers.pxx"

//=================================================================================================

Bnd_Box2d GeomBndLib_BezierCurve2d::Box(double theTol) const
{
  return Box(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
}

//=================================================================================================

Bnd_Box2d GeomBndLib_BezierCurve2d::Box(double theU1, double theU2, double theTol) const
{
  occ::handle<Geom2d_BezierCurve> aCurve = myGeom;
  const double                    aU1    = aCurve->FirstParameter();
  const double                    aU2    = aCurve->LastParameter();
  const double                    aTrim1 = std::max(theU1, aU1);
  const double                    aTrim2 = std::min(theU2, aU2);
  constexpr double                anEps  = Precision::PConfusion();
  if (std::abs(aU1 - aTrim1) > anEps || std::abs(aU2 - aTrim2) > anEps)
  {
    const occ::handle<Geom2d_Geometry> aCopy = aCurve->Copy();
    aCurve                                   = occ::down_cast<Geom2d_BezierCurve>(aCopy);
    aCurve->Segment(aTrim1, aTrim2);
  }

  Bnd_Box2d aBox;
  for (int anIdx = 1; anIdx <= aCurve->NbPoles(); ++anIdx)
  {
    aBox.Add(aCurve->Pole(anIdx));
  }
  aBox.Enlarge(theTol);
  return aBox;
}

//=================================================================================================

Bnd_Box2d GeomBndLib_BezierCurve2d::BoxOptimal(double theU1, double theU2, double theTol) const
{
  Geom2dAdaptor_Curve aGACurve(myGeom);
  return GeomBndLib_SplineHelpers::CurveBoxOptimal<Geom2dAdaptor_Curve, Bnd_Box2d, gp_Pnt2d>(
    aGACurve,
    theU1,
    theU2,
    theTol);
}
