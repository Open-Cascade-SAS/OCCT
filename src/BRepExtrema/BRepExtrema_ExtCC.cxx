// Created on: 1993-12-15
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepExtrema_ExtCC.hxx>

#include <BRep_Tool.hxx>
#include <Extrema_POnCurv.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>


//=======================================================================
//function : BRepExtrema_ExtCC
//purpose  : 
//=======================================================================

BRepExtrema_ExtCC::BRepExtrema_ExtCC(const TopoDS_Edge& E1, const TopoDS_Edge& E2)
{
  Initialize(E2);
  Perform(E1);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepExtrema_ExtCC::Initialize(const TopoDS_Edge& E2)
{
  if (!BRep_Tool::IsGeometric(E2))
    return;  // protect against non-geometric type (e.g. polygon)
  Standard_Real V1, V2;
  BRepAdaptor_Curve Curv(E2);
  myHC = new BRepAdaptor_HCurve(Curv);
  Standard_Real Tol = Min(BRep_Tool::Tolerance(E2), Precision::Confusion());
  Tol = Max(Curv.Resolution(Tol), Precision::PConfusion());
  BRep_Tool::Range(E2,V1,V2);
  myExtCC.SetCurve(2,myHC->Curve(),V1,V2);
  myExtCC.SetTolerance(2, Tol);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepExtrema_ExtCC::Perform(const TopoDS_Edge& E1)
{
  if (!BRep_Tool::IsGeometric(E1))
    return;  // protect against non-geometric type (e.g. polygon)
  Standard_Real U1, U2;
  BRepAdaptor_Curve Curv(E1);
  Handle(BRepAdaptor_HCurve) HC = new BRepAdaptor_HCurve(Curv);
  Standard_Real Tol = Min(BRep_Tool::Tolerance(E1), Precision::Confusion());
  Tol = Max(Curv.Resolution(Tol), Precision::PConfusion());
  BRep_Tool::Range(E1,U1,U2);
  myExtCC.SetCurve (1, HC->Curve(), U1, U2);
  myExtCC.SetTolerance(1, Tol);
  // If we enable SetSingleSolutionFlag Extrema will run much quicker on almost parallel curves
  // (e.g. bug 27665), however some solutions will be lost, e.g. see bug 28183.
  //myExtCC.SetSingleSolutionFlag(Standard_True);
  myExtCC.Perform();
}

//=======================================================================
//function : ParameterOnE1
//purpose  : 
//=======================================================================

Standard_Real BRepExtrema_ExtCC::ParameterOnE1(const Standard_Integer N) const
{
  Extrema_POnCurv POnE1, POnE2;
  myExtCC.Points(N, POnE1, POnE2);
  return POnE1.Parameter();
}

//=======================================================================
//function : PointOnE1
//purpose  : 
//=======================================================================

gp_Pnt BRepExtrema_ExtCC::PointOnE1(const Standard_Integer N) const
{
  Extrema_POnCurv POnE1, POnE2;
  myExtCC.Points(N, POnE1, POnE2);
  return POnE1.Value();
}

//=======================================================================
//function : ParameterOnE2
//purpose  : 
//=======================================================================

Standard_Real BRepExtrema_ExtCC::ParameterOnE2(const Standard_Integer N) const
{
  Extrema_POnCurv POnE1, POnE2;
  myExtCC.Points(N, POnE1, POnE2);
  return POnE2.Parameter();
}

//=======================================================================
//function : PointOnE2
//purpose  : 
//=======================================================================

gp_Pnt BRepExtrema_ExtCC::PointOnE2(const Standard_Integer N) const
{
  Extrema_POnCurv POnE1, POnE2;
  myExtCC.Points(N, POnE1, POnE2);
  return POnE2.Value();
}


//=======================================================================
//function : TrimmedSquareDistances
//purpose  : 
//=======================================================================

void BRepExtrema_ExtCC::TrimmedSquareDistances
  (Standard_Real& dist11,
   Standard_Real& dist12,
   Standard_Real& dist21,
   Standard_Real& dist22,
   gp_Pnt& pnt11,
   gp_Pnt& pnt12,
   gp_Pnt& pnt21,
   gp_Pnt& pnt22) const
{
  myExtCC.TrimmedSquareDistances(dist11,dist12,dist21,dist22,
                                 pnt11,pnt12,pnt21,pnt22);
}
