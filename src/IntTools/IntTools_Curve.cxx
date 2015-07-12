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


#include <Geom2d_Curve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Curve.hxx>

//=======================================================================
//function : IntTools_Curve::IntTools_Curve
//purpose  : 
//=======================================================================
IntTools_Curve::IntTools_Curve()
{
}
//=======================================================================
//function : IntTools_Curve::IntTools_Curve
//purpose  : 
//=======================================================================
  IntTools_Curve::IntTools_Curve(const Handle(Geom_Curve)& Curve3d,
				 const Handle(Geom2d_Curve)& FirstCurve2d,
				 const Handle(Geom2d_Curve)& SecondCurve2d)
{
  SetCurves(Curve3d, FirstCurve2d, SecondCurve2d);
}
//=======================================================================
//function : SetCurves
//purpose  : 
//=======================================================================
  void IntTools_Curve::SetCurves(const Handle(Geom_Curve)& Curve3d,
				 const Handle(Geom2d_Curve)& FirstCurve2d,
				 const Handle(Geom2d_Curve)& SecondCurve2d) 
{
  SetCurve(Curve3d);
  SetFirstCurve2d(FirstCurve2d);
  SetSecondCurve2d(SecondCurve2d);
}

//=======================================================================
//function : HasBounds
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Curve::HasBounds() const 
{
  Standard_Boolean bBounded;

  Handle(Geom_BoundedCurve) aC3DBounded =
    Handle(Geom_BoundedCurve)::DownCast(my3dCurve);
  
  bBounded=!aC3DBounded.IsNull();
  
  return bBounded ;
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================
  void IntTools_Curve::Bounds(Standard_Real& aT1,
			      Standard_Real& aT2,
			      gp_Pnt& aP1,
			      gp_Pnt& aP2) const 
{
  aT1=0.;
  aT2=0.;
  aP1.SetCoord(0.,0.,0.);
  aP2.SetCoord(0.,0.,0.);
  if (HasBounds()) {
    aT1=my3dCurve->FirstParameter();
    aT2=my3dCurve->LastParameter();
    my3dCurve->D0(aT1, aP1);
    my3dCurve->D0(aT2, aP2);
  }
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Curve::D0(Standard_Real& aT,
				      gp_Pnt& aP) const 
{
  Standard_Real aF, aL;

  aF=my3dCurve->FirstParameter();
  aL=my3dCurve->LastParameter();
  if (aT<aF || aT>aL) {
    return Standard_False;
  }
  my3dCurve->D0(aT, aP);
  return Standard_True;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================
   GeomAbs_CurveType IntTools_Curve::Type() const 
{
  GeomAdaptor_Curve aGAC(my3dCurve);
  GeomAbs_CurveType aType=aGAC.GetType();
  return aType;
}
