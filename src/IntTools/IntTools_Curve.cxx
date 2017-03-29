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

//=======================================================================
//function : IntTools_Curve::IntTools_Curve
//purpose  : 
//=======================================================================
IntTools_Curve::IntTools_Curve()
:
  myTolerance(0.0),
  myTangentialTolerance(0.0)
{
}
//=======================================================================
//function : IntTools_Curve::IntTools_Curve
//purpose  : 
//=======================================================================
IntTools_Curve::IntTools_Curve(const Handle(Geom_Curve)& the3dCurve,
                               const Handle(Geom2d_Curve)& the2dCurve1,
                               const Handle(Geom2d_Curve)& the2dCurve2,
                               const Standard_Real theTolerance,
                               const Standard_Real theTangentialTolerance)
:
  myTolerance(theTolerance),
  myTangentialTolerance(theTangentialTolerance)
{
  SetCurves(the3dCurve, the2dCurve1, the2dCurve2);
}
//=======================================================================
//function : HasBounds
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Curve::HasBounds() const 
{
  Handle(Geom_BoundedCurve) aC3DBounded =
    Handle(Geom_BoundedCurve)::DownCast(my3dCurve);
  Standard_Boolean bIsBounded = !aC3DBounded.IsNull();
  return bIsBounded;
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_Curve::Bounds(Standard_Real& theFirst,
                                        Standard_Real& theLast,
                                        gp_Pnt& theFirstPnt,
                                        gp_Pnt& theLastPnt) const
{
  Standard_Boolean bIsBounded = HasBounds();
  if (bIsBounded) {
    theFirst = my3dCurve->FirstParameter();
    theLast  = my3dCurve->LastParameter();
    my3dCurve->D0(theFirst, theFirstPnt);
    my3dCurve->D0(theLast,  theLastPnt);
  }
  return bIsBounded;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_Curve::D0(const Standard_Real& thePar,
                                    gp_Pnt& thePnt) const
{
  Standard_Boolean bInside = !(thePar < my3dCurve->FirstParameter() &&
                               thePar > my3dCurve->LastParameter());
  if (bInside) {
    my3dCurve->D0(thePar, thePnt);
  }
  return bInside;
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================
GeomAbs_CurveType IntTools_Curve::Type() const
{
  GeomAdaptor_Curve aGAC(my3dCurve);
  GeomAbs_CurveType aType = aGAC.GetType();
  return aType;
}
