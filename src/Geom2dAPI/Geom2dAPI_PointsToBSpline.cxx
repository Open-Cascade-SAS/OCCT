// Created on: 1994-03-23
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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


#include <AppDef_BSplineCompute.hxx>
#include <AppDef_MultiLine.hxx>
#include <AppDef_MultiPointConstraint.hxx>
#include <AppDef_Variational.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <BSplCLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dAPI_PointsToBSpline.hxx>
#include <math_Vector.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
//function : Geom2dAPI_PointsToBSpline
//purpose  : 
//=======================================================================
Geom2dAPI_PointsToBSpline::Geom2dAPI_PointsToBSpline()
{
  myIsDone = Standard_False;
}


//=======================================================================
//function : Geom2dAPI_PointsToBSpline
//purpose  : 
//=======================================================================

Geom2dAPI_PointsToBSpline::Geom2dAPI_PointsToBSpline
  (const TColgp_Array1OfPnt2d& Points,
   const Standard_Integer      DegMin, 
   const Standard_Integer      DegMax,
   const GeomAbs_Shape         Continuity,
   const Standard_Real         Tol2D)
{
  Init(Points,DegMin,DegMax,Continuity,Tol2D);
}


//=======================================================================
//function : Geom2dAPI_PointsToBSpline
//purpose  : 
//=======================================================================

Geom2dAPI_PointsToBSpline::Geom2dAPI_PointsToBSpline
  (const TColStd_Array1OfReal& YValues,
   const Standard_Real         X0,
   const Standard_Real         DX,
   const Standard_Integer      DegMin, 
   const Standard_Integer      DegMax,
   const GeomAbs_Shape         Continuity,
   const Standard_Real         Tol2D)
{
  Init(YValues,X0,DX,DegMin,DegMax,Continuity,Tol2D);
}

//=======================================================================
//function : Geom2dAPI_PointsToBSpline
//purpose  : 
//=======================================================================

Geom2dAPI_PointsToBSpline::Geom2dAPI_PointsToBSpline
  (const TColgp_Array1OfPnt2d& Points,
   const Approx_ParametrizationType ParType,
   const Standard_Integer    DegMin, 
   const Standard_Integer    DegMax,
   const GeomAbs_Shape       Continuity,
   const Standard_Real       Tol2D)
{
  myIsDone = Standard_False;
  Init(Points,ParType,DegMin,DegMax,Continuity,Tol2D);
}

//=======================================================================
//function : Geom2dAPI_PointsToBSpline
//purpose  : 
//=======================================================================

Geom2dAPI_PointsToBSpline::Geom2dAPI_PointsToBSpline
  (const TColgp_Array1OfPnt2d&   Points,
   const TColStd_Array1OfReal& Params,
   const Standard_Integer      DegMin, 
   const Standard_Integer      DegMax,
   const GeomAbs_Shape         Continuity,
   const Standard_Real         Tol2D)
{
  myIsDone = Standard_False;
  Init(Points,Params,DegMin,DegMax,Continuity,Tol2D);
}


//=======================================================================
//function : Geom2dAPI_PointsToBSpline
//purpose  : 
//=======================================================================

Geom2dAPI_PointsToBSpline::Geom2dAPI_PointsToBSpline
  (const TColgp_Array1OfPnt2d&   Points,
   const Standard_Real         W1,
   const Standard_Real         W2,
   const Standard_Real         W3,
   const Standard_Integer      DegMax,
   const GeomAbs_Shape         Continuity,
   const Standard_Real         Tol2D)
{
  myIsDone = Standard_False;
  Init(Points,W1,W2,W3,DegMax,Continuity,Tol2D);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Geom2dAPI_PointsToBSpline::Init
  (const TColgp_Array1OfPnt2d& Points,
   const Approx_ParametrizationType ParType,
   const Standard_Integer      DegMin,
   const Standard_Integer      DegMax,
   const GeomAbs_Shape         Continuity,
   const Standard_Real         Tol2D)
{
  Standard_Real Tol3D = 0.; // dummy argument for BSplineCompute.


  Standard_Integer nbit = 2;
  Standard_Boolean UseSquares = Standard_False;
  if(Tol2D <= 1.e-3) UseSquares = Standard_True;

  AppDef_BSplineCompute TheComputer
    (DegMin,DegMax,Tol3D,Tol2D,nbit,Standard_True,ParType,UseSquares);

  switch( Continuity) {
  case GeomAbs_C0:
    TheComputer.SetContinuity(0); break;

  case GeomAbs_G1: 
  case GeomAbs_C1: 
    TheComputer.SetContinuity(1); break;

  case GeomAbs_G2:
  case GeomAbs_C2:
    TheComputer.SetContinuity(2); break;

  default: 
    TheComputer.SetContinuity(3);
  }
  
  TheComputer.Perform(Points);

  AppParCurves_MultiBSpCurve TheCurve = TheComputer.Value();
  
  TColgp_Array1OfPnt2d Poles(1,TheCurve.NbPoles());

  TheCurve.Curve(1, Poles);
  
  myCurve = new Geom2d_BSplineCurve(Poles, 
				    TheCurve.Knots(),
				    TheCurve.Multiplicities(),
				    TheCurve.Degree());
  myIsDone = Standard_True;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Geom2dAPI_PointsToBSpline::Init
  (const TColStd_Array1OfReal& YValues,
   const Standard_Real         X0,
   const Standard_Real         DX,
   const Standard_Integer      DegMin, 
   const Standard_Integer      DegMax,
   const GeomAbs_Shape         Continuity,
   const Standard_Real         Tol2D)
{
  // first approximate the Y values (with dummy 0 as X values)

  Standard_Real Tol3D = 0.; // dummy argument for BSplineCompute.
  TColgp_Array1OfPnt2d Points(YValues.Lower(),YValues.Upper());
  math_Vector Param(YValues.Lower(),YValues.Upper());
  Standard_Real length = DX * (YValues.Upper() - YValues.Lower());
  Standard_Integer i;
  
  for (i = YValues.Lower(); i <= YValues.Upper(); i++) {
    Param(i) = (X0+(i-1)*DX)/(X0+length);
    Points(i).SetCoord(0.0, YValues(i));
  }

  AppDef_BSplineCompute TheComputer
    (Param, DegMin,DegMax,Tol3D,Tol2D,0, Standard_True, Standard_True);

  switch( Continuity) {
  case GeomAbs_C0:
    TheComputer.SetContinuity(0); break;

  case GeomAbs_G1: 
  case GeomAbs_C1: 
    TheComputer.SetContinuity(1); break;

  case GeomAbs_G2:
  case GeomAbs_C2:
    TheComputer.SetContinuity(2); break;

  default: 
    TheComputer.SetContinuity(3);
  }
  
  TheComputer.Perform(Points);

  const AppParCurves_MultiBSpCurve& TheCurve = TheComputer.Value();
  
  Standard_Integer Degree = TheCurve.Degree();
  TColgp_Array1OfPnt2d Poles(1,TheCurve.NbPoles());
  Standard_Integer nk = TheCurve.Knots().Length();
  TColStd_Array1OfReal Knots(1,nk);
  TColStd_Array1OfInteger Mults(1,nk);

  TheCurve.Curve(1, Poles);



  // compute X values for the poles
  TColStd_Array1OfReal XPoles(1,Poles.Upper());

  // start with a line
  TColStd_Array1OfReal    TempPoles(1,2);
  TColStd_Array1OfReal    TempKnots(1,2);
  TColStd_Array1OfInteger TempMults(1,2);
  TempMults.Init(2);
  TempPoles(1) = X0;
  TempPoles(2) = X0 + length;
  TempKnots(1) = 0.;
  TempKnots(2) = 1.;

  // increase the Degree
  TColStd_Array1OfReal    NewTempPoles(1,Degree+1);
  TColStd_Array1OfReal    NewTempKnots(1,2);
  TColStd_Array1OfInteger NewTempMults(1,2);
  BSplCLib::IncreaseDegree(1,Degree,Standard_False,1,
			   TempPoles,TempKnots,TempMults,
			   NewTempPoles,NewTempKnots,NewTempMults);


  // insert the Knots
  BSplCLib::InsertKnots(Degree,Standard_False,1,
			NewTempPoles,NewTempKnots,NewTempMults,
			TheCurve.Knots(),&TheCurve.Multiplicities(),
			XPoles,Knots,Mults,
			Epsilon(1));
  
  // scale the knots
  for (i = 1; i <= nk; i++) {
    Knots(i) = X0 + length * Knots(i);
  }

  // set the Poles
  for (i = 1; i <= Poles.Upper(); i++) {
    Poles(i).SetX(XPoles(i));
  }



  myCurve = new Geom2d_BSplineCurve(Poles, Knots, Mults, Degree);
  myIsDone = Standard_True;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Geom2dAPI_PointsToBSpline::Init
  (const TColgp_Array1OfPnt2d& Points,
   const Standard_Integer    DegMin,
   const Standard_Integer    DegMax,
   const GeomAbs_Shape       Continuity,
   const Standard_Real       Tol2D)
{
  myIsDone = Standard_False;
  Init(Points,Approx_ChordLength,DegMin,DegMax,Continuity,Tol2D);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Geom2dAPI_PointsToBSpline::Init
  (const TColgp_Array1OfPnt2d&   Points,
   const TColStd_Array1OfReal& Params,
   const Standard_Integer      DegMin,
   const Standard_Integer      DegMax,
   const GeomAbs_Shape         Continuity,
   const Standard_Real         Tol2D)
{
  if (Params.Length() != Points.Length()) throw Standard_OutOfRange ("Geom2dAPI_PointsToBSpline::Init() - invalid input");

  Standard_Real Tol3D = 0.; // dummy argument for BSplineCompute.
  Standard_Integer Nbp = Params.Length();
  math_Vector theParams(1,Nbp);
  theParams(1) = 0.;
  theParams(Nbp) = 1.;

  Standard_Real Uf = Params(Params.Lower());
  Standard_Real Ul = Params(Params.Upper()) - Uf;
  for (Standard_Integer i=2; i<Nbp; i++) {
    theParams(i) = (Params(i)-Uf)/Ul;
  }
  
  AppDef_BSplineCompute TheComputer
    (DegMin,DegMax,Tol3D,Tol2D,0,
     Standard_True,Approx_IsoParametric,Standard_True);

  TheComputer.SetParameters(theParams);

  switch( Continuity) {
  case GeomAbs_C0:
    TheComputer.SetContinuity(0); break;

  case GeomAbs_G1: 
  case GeomAbs_C1: 
    TheComputer.SetContinuity(1); break;

  case GeomAbs_G2:
  case GeomAbs_C2:
    TheComputer.SetContinuity(2); break;

  default: 
    TheComputer.SetContinuity(3);
  }

  TheComputer.Perform(Points);

  AppParCurves_MultiBSpCurve TheCurve = TheComputer.Value();
  
  TColgp_Array1OfPnt2d Poles(1,TheCurve.NbPoles());

  TheCurve.Curve(1, Poles);
  
  myCurve = new Geom2d_BSplineCurve(Poles, 
				    TheCurve.Knots(),
				    TheCurve.Multiplicities(),
				    TheCurve.Degree());
  myIsDone = Standard_True;  

}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Geom2dAPI_PointsToBSpline::Init
  (const TColgp_Array1OfPnt2d&   Points,
   const Standard_Real         W1,
   const Standard_Real         W2,
   const Standard_Real         W3,
   const Standard_Integer      DegMax,
   const GeomAbs_Shape         Continuity,
   const Standard_Real         Tol2D)
{
  Standard_Integer NbPoint = Points.Length(), i;

 
  Standard_Integer nbit = 2;
  if(Tol2D <= 1.e-3) nbit = 0;

 //Variational algo

  AppDef_MultiLine multL(NbPoint);
  for(i = 1; i <= NbPoint; ++i) {
    AppDef_MultiPointConstraint mpc(0, 1);
    mpc.SetPoint2d(1, Points.Value(Points.Lower() + i - 1));
    multL.SetValue(i, mpc);
  }

  Handle(AppParCurves_HArray1OfConstraintCouple) TABofCC = 
    new AppParCurves_HArray1OfConstraintCouple(1, NbPoint);
  AppParCurves_Constraint  Constraint=AppParCurves_NoConstraint;
  
  for(i = 1; i <= NbPoint; ++i) {
    AppParCurves_ConstraintCouple ACC(i,Constraint);
    TABofCC->SetValue(i,ACC);
  }
  

  AppDef_Variational Variation(multL, 1, NbPoint, TABofCC);

//===================================
  Standard_Integer theMaxSegments = 1000;
  Standard_Boolean theWithMinMax = Standard_False;
//===================================      

  Variation.SetMaxDegree(DegMax);
  Variation.SetContinuity(Continuity);
  Variation.SetMaxSegment(theMaxSegments);

  Variation.SetTolerance(Tol2D);
  Variation.SetWithMinMax(theWithMinMax);
  Variation.SetNbIterations(nbit);

  Variation.SetCriteriumWeight(W1, W2, W3);

  if(!Variation.IsCreated()) {
    return;
  }
  
  if(Variation.IsOverConstrained()) {
    return;
  }

  try {
    Variation.Approximate();
  }
  catch (Standard_Failure) {
    return;
  }

  if(!Variation.IsDone()) {
    return;
  }

  AppParCurves_MultiBSpCurve TheCurve = Variation.Value();

  TColgp_Array1OfPnt2d Poles(1,TheCurve.NbPoles());

  TheCurve.Curve(1, Poles);
  
  myCurve = new Geom2d_BSplineCurve(Poles, 
				    TheCurve.Knots(),
				    TheCurve.Multiplicities(),
				    TheCurve.Degree());

  myIsDone = Standard_True;

}

//=======================================================================
//function : Handle(Geom2d_BSplineCurve)&
//purpose  : 
//=======================================================================

const Handle(Geom2d_BSplineCurve)& Geom2dAPI_PointsToBSpline::Curve() const 
{
  if ( !myIsDone) 
    throw StdFail_NotDone(" ");
  return myCurve;
}



//=======================================================================
//function : Geom2d_BSplineCurve
//purpose  : 
//=======================================================================

Geom2dAPI_PointsToBSpline::operator Handle(Geom2d_BSplineCurve)() const
{
  return myCurve;
}

//=======================================================================
//function : Geom2d_BSplineCurve
//purpose  : 
//=======================================================================

Standard_Boolean Geom2dAPI_PointsToBSpline::IsDone() const
{
  return myIsDone;
}


