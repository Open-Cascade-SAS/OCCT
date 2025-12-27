// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <IntTools_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>

//=================================================================================================

IntTools_Curve::IntTools_Curve()
    : myTolerance(0.0),
      myTangentialTolerance(0.0)
{
}

//=================================================================================================

IntTools_Curve::IntTools_Curve(const occ::handle<Geom_Curve>&   the3dCurve,
                               const occ::handle<Geom2d_Curve>& the2dCurve1,
                               const occ::handle<Geom2d_Curve>& the2dCurve2,
                               const double         theTolerance,
                               const double         theTangentialTolerance)
    : myTolerance(theTolerance),
      myTangentialTolerance(theTangentialTolerance)
{
  SetCurves(the3dCurve, the2dCurve1, the2dCurve2);
}

//=================================================================================================

bool IntTools_Curve::HasBounds() const
{
  occ::handle<Geom_BoundedCurve> aC3DBounded = occ::down_cast<Geom_BoundedCurve>(my3dCurve);
  bool          bIsBounded  = !aC3DBounded.IsNull();
  return bIsBounded;
}

//=================================================================================================

bool IntTools_Curve::Bounds(double& theFirst,
                                        double& theLast,
                                        gp_Pnt&        theFirstPnt,
                                        gp_Pnt&        theLastPnt) const
{
  bool bIsBounded = HasBounds();
  if (bIsBounded)
  {
    theFirst = my3dCurve->FirstParameter();
    theLast  = my3dCurve->LastParameter();
    my3dCurve->D0(theFirst, theFirstPnt);
    my3dCurve->D0(theLast, theLastPnt);
  }
  return bIsBounded;
}

//=================================================================================================

bool IntTools_Curve::D0(const double& thePar, gp_Pnt& thePnt) const
{
  bool bInside =
    !(thePar < my3dCurve->FirstParameter() && thePar > my3dCurve->LastParameter());
  if (bInside)
  {
    my3dCurve->D0(thePar, thePnt);
  }
  return bInside;
}

//=================================================================================================

GeomAbs_CurveType IntTools_Curve::Type() const
{
  GeomAdaptor_Curve aGAC(my3dCurve);
  GeomAbs_CurveType aType = aGAC.GetType();
  return aType;
}
