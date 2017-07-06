// Created on: 1997-05-05
// Created by: Jerome LEMONIER
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Extrema_ExtPS.hxx>
#include <Extrema_POnSurf.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomPlate_PointConstraint,Standard_Transient)

//---------------------------------------------------------
//         Constructeurs avec un point 
//---------------------------------------------------------
GeomPlate_PointConstraint::GeomPlate_PointConstraint(const gp_Pnt& Pt, 
                                             const Standard_Integer Order, 
                                             const Standard_Real TolDist)
:
myOrder(Order),
myLProp(2,TolDist),
myPoint(Pt),
myTolDist(TolDist),
hasPnt2dOnSurf(Standard_False)
{ if ((myOrder>1)||(myOrder<-1))
    throw Standard_Failure("GeomPlate_PointConstraint : the constraint must 0 or -1 with a point");
}

//---------------------------------------------------------
//         Constructeurs avec un point sur surface
//---------------------------------------------------------
GeomPlate_PointConstraint::GeomPlate_PointConstraint
(const Standard_Real U,
 const Standard_Real V,
 const Handle(Geom_Surface)& Surf,
 const Standard_Integer Order,
 const Standard_Real TolDist,
 const Standard_Real TolAng,
 const Standard_Real TolCurv)
:myOrder(Order),
 myLProp(2,TolDist),
 mySurf(Surf),
 myU(U),
 myV(V),
 myTolDist(TolDist),
 myTolAng(TolAng),
 myTolCurv(TolCurv),
 hasPnt2dOnSurf(Standard_False)

{ 
  Surf->D2(myU,myV,myPoint,myD11,myD12,myD21,myD22,myD23);
  myLProp.SetSurface(Surf);
}

//---------------------------------------------------------
// Fonction : D0
//---------------------------------------------------------
void GeomPlate_PointConstraint::D0(gp_Pnt& P) const
{ P=myPoint;
}

//---------------------------------------------------------
// Fonction : D1
//---------------------------------------------------------
void GeomPlate_PointConstraint::D1(gp_Pnt& P,gp_Vec& V1,gp_Vec& V2) const
{ P=myPoint;
  V1=myD11;
  V2=myD12;
}

//---------------------------------------------------------
// Fonction : D2
//---------------------------------------------------------
void GeomPlate_PointConstraint::D2(gp_Pnt& P,gp_Vec& V1,gp_Vec& V2,gp_Vec& V3,gp_Vec& V4,gp_Vec& V5) const
{ P=myPoint;
  V1=myD11;
  V2=myD12;
  V3=myD21;
  V4=myD22;
  V5=myD23;
}

//---------------------------------------------------------
// Fonction : SetG0Criterion
//---------------------------------------------------------
void GeomPlate_PointConstraint :: SetG0Criterion( const Standard_Real TolDist )
{
  myTolDist = TolDist;
}
//---------------------------------------------------------
// Fonction : SetG1Criterion
//---------------------------------------------------------
void GeomPlate_PointConstraint :: SetG1Criterion( const Standard_Real TolAng )
{
  myTolAng = TolAng;
}
//---------------------------------------------------------
// Fonction : SetG2Criterion
//---------------------------------------------------------
void GeomPlate_PointConstraint :: SetG2Criterion( const Standard_Real TolCurv )
{
  myTolCurv = TolCurv;
}

//---------------------------------------------------------
// Fonction : G0Criterion
//---------------------------------------------------------
Standard_Real GeomPlate_PointConstraint::G0Criterion() const
{ return myTolDist;
}

//---------------------------------------------------------
// Fonction : G1Criterion 
//---------------------------------------------------------
Standard_Real GeomPlate_PointConstraint::G1Criterion() const
{ return myTolAng;
}

//---------------------------------------------------------
// Fonction : G2Criterion 
//---------------------------------------------------------
Standard_Real GeomPlate_PointConstraint::G2Criterion() const
{ return myTolCurv;
}

//---------------------------------------------------------
// Fonction : Surface 
//---------------------------------------------------------
//Handle(Geom_Surface) GeomPlate_PointConstraint::Surface() const
//{  throw Standard_Failure("GeomPlate_PointConstraint.cxx : The surface does not exist");
//}
//------------------------------------------------------------
//Fonction : LPropSurf
//------------------------------------------------------------
GeomLProp_SLProps &GeomPlate_PointConstraint::LPropSurf()
{ // if (myFrontiere.IsNull())
  //  throw Standard_Failure("GeomPlate_CurveConstraint.cxx : Curve must be on a Surface");
 // gp_Pnt2d P2d= myFrontiere->ChangeCurve().GetCurve()->Value(U);
myLProp.SetParameters(myU,myV);
return myLProp;
}


//------------------------------------------------------------
//Fonction : Order
//------------------------------------------------------------
Standard_Integer GeomPlate_PointConstraint::Order() const
{
return myOrder;
}
//------------------------------------------------------------
//Fonction : SetOrder
//------------------------------------------------------------
void GeomPlate_PointConstraint::SetOrder(const Standard_Integer Order) 
{ myOrder=Order;
}

//------------------------------------------------------------
//Fonction : HasPnt2dOnSurf
//------------------------------------------------------------
Standard_Boolean GeomPlate_PointConstraint::HasPnt2dOnSurf() const
{ 
  return hasPnt2dOnSurf;
}
//------------------------------------------------------------
//Fonction : SetPnt2dOnSurf
//------------------------------------------------------------
void GeomPlate_PointConstraint::SetPnt2dOnSurf(const gp_Pnt2d& Pnt2d) 
{ 
  myPt2d=Pnt2d;
  hasPnt2dOnSurf = Standard_True;
}
//------------------------------------------------------------
//Fonction : Pnt2dOnSurf
//------------------------------------------------------------
gp_Pnt2d GeomPlate_PointConstraint::Pnt2dOnSurf() const
{ return myPt2d;
}

