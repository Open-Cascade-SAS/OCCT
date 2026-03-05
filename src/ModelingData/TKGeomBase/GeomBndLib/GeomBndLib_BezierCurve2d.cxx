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

#include <Geom2dAdaptor_Curve.hxx>
#include "GeomBndLib_SplineHelpers.pxx"

//=================================================================================================

Bnd_Box2d GeomBndLib_BezierCurve2d::Box(double theTol) const
{
  return Box(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
}

//=================================================================================================

Bnd_Box2d GeomBndLib_BezierCurve2d::Box(double theU1, double theU2, double theTol) const
{
  return GeomBndLib_SplineHelpers::
    BezierCurveBox<Geom2d_BezierCurve, Geom2dAdaptor_Curve, Bnd_Box2d, gp_Pnt2d>(myGeom,
                                                                                 theU1,
                                                                                 theU2,
                                                                                 theTol);
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
