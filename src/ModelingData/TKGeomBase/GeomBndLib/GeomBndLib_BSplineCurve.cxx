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

#include <GeomBndLib_BSplineCurve.hxx>

#include <Geom_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include "GeomBndLib_SplineHelpers.pxx"

//=================================================================================================

Bnd_Box GeomBndLib_BSplineCurve::Box(double theTol) const
{
  return Box(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
}

//=================================================================================================

Bnd_Box GeomBndLib_BSplineCurve::Box(double theU1, double theU2, double theTol) const
{
  return GeomBndLib_SplineHelpers::BSplineCurveBox<Geom_BSplineCurve,
                                                   GeomAdaptor_Curve,
                                                   Bnd_Box,
                                                   gp_Pnt>(myGeom, theU1, theU2, theTol);
}

//=================================================================================================

Bnd_Box GeomBndLib_BSplineCurve::BoxOptimal(double theU1, double theU2, double theTol) const
{
  GeomAdaptor_Curve aGACurve(myGeom);
  return GeomBndLib_SplineHelpers::CurveBoxOptimal<GeomAdaptor_Curve, Bnd_Box, gp_Pnt>(
    aGACurve, theU1, theU2, theTol);
}
