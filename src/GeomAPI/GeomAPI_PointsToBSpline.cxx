// Created on: 1994-03-21
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <GeomAPI_PointsToBSpline.ixx>

#include <AppDef_BSplineCompute.hxx>
#include <AppDef_MultiLine.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <math_Vector.hxx>
#include <BSplCLib.hxx>
#include <AppDef_MultiLine.hxx>
#include <AppDef_MultiPointConstraint.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
#include <AppParCurves_Constraint.hxx>
#include <AppDef_TheVariational.hxx>


//=======================================================================
//function : GeomAPI_PointsToBSpline
//purpose  : 
//=======================================================================

GeomAPI_PointsToBSpline::GeomAPI_PointsToBSpline()
{
  myIsDone = Standard_False;
}


//=======================================================================
//function : GeomAPI_PointsToBSpline
//purpose  : 
//=======================================================================

GeomAPI_PointsToBSpline::GeomAPI_PointsToBSpline
  (const TColgp_Array1OfPnt& Points,
   const Standard_Integer    DegMin, 
   const Standard_Integer    DegMax,
   const GeomAbs_Shape       Continuity,
   const Standard_Real       Tol3D)
{
  myIsDone = Standard_False;
  Init(Points,DegMin,DegMax,Continuity,Tol3D);
}

//=======================================================================
//function : GeomAPI_PointsToBSpline
//purpose  : 
//=======================================================================

GeomAPI_PointsToBSpline::GeomAPI_PointsToBSpline
  (const TColgp_Array1OfPnt& Points,
   const Approx_ParametrizationType ParType,
   const Standard_Integer    DegMin, 
   const Standard_Integer    DegMax,
   const GeomAbs_Shape       Continuity,
   const Standard_Real       Tol3D)
{
  myIsDone = Standard_False;
  Init(Points,ParType,DegMin,DegMax,Continuity,Tol3D);
}


//=======================================================================
//function : GeomAPI_PointsToBSpline
//purpose  : 
//=======================================================================

GeomAPI_PointsToBSpline::GeomAPI_PointsToBSpline
  (const TColgp_Array1OfPnt&   Points,
   const TColStd_Array1OfReal& Params,
   const Standard_Integer      DegMin, 
   const Standard_Integer      DegMax,
   const GeomAbs_Shape         Continuity,
   const Standard_Real         Tol3D)
{
  myIsDone = Standard_False;
  Init(Points,Params,DegMin,DegMax,Continuity,Tol3D);
}

//=======================================================================
//function : GeomAPI_PointsToBSpline
//purpose  : 
//=======================================================================

GeomAPI_PointsToBSpline::GeomAPI_PointsToBSpline
  (const TColgp_Array1OfPnt&   Points,
   const Standard_Real         W1,
   const Standard_Real         W2,
   const Standard_Real         W3,
   const Standard_Integer      DegMax,
   const GeomAbs_Shape         Continuity,
   const Standard_Real         Tol3D)
{
  myIsDone = Standard_False;
  Init(Points,W1,W2,W3,DegMax,Continuity,Tol3D);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_PointsToBSpline::Init
  (const TColgp_Array1OfPnt& Points,
   const Standard_Integer    DegMin,
   const Standard_Integer    DegMax,
   const GeomAbs_Shape       Continuity,
   const Standard_Real       Tol3D)
{
  myIsDone = Standard_False;
  Init(Points,Approx_ChordLength,DegMin,DegMax,Continuity,Tol3D);
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_PointsToBSpline::Init
  (const TColgp_Array1OfPnt& Points,
   const Approx_ParametrizationType ParType,
   const Standard_Integer    DegMin,
   const Standard_Integer    DegMax,
   const GeomAbs_Shape       Continuity,
   const Standard_Real       Tol3D)
{
  Standard_Real Tol2D = 0.; // dummy argument for BSplineCompute.

  Standard_Integer nbit = 2;
  Standard_Boolean UseSquares = Standard_False;
  if(Tol3D <= 1.e-3) UseSquares = Standard_True;


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

  TColgp_Array1OfPnt Poles(1,TheCurve.NbPoles());

  TheCurve.Curve(1, Poles);
  
  myCurve = new Geom_BSplineCurve(Poles, 
				  TheCurve.Knots(),
				  TheCurve.Multiplicities(),
				  TheCurve.Degree());
  myIsDone = Standard_True;
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_PointsToBSpline::Init
  (const TColgp_Array1OfPnt&   Points,
   const TColStd_Array1OfReal& Params,
   const Standard_Integer      DegMin,
   const Standard_Integer      DegMax,
   const GeomAbs_Shape         Continuity,
   const Standard_Real         Tol3D)
{
  if (Params.Length() != Points.Length()) Standard_OutOfRange::Raise("");

  Standard_Real Tol2D = 0.; // dummy argument for BSplineCompute.
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
  
  TColgp_Array1OfPnt Poles(1,TheCurve.NbPoles());
  TColStd_Array1OfReal Knots(TheCurve.Knots().Lower(), 
			     TheCurve.Knots().Upper());

  TheCurve.Curve(1, Poles);
  Knots = TheCurve.Knots();
  BSplCLib::Reparametrize(Params(Params.Lower()),
			  Params(Params.Upper()),
			  Knots);

  myCurve = new Geom_BSplineCurve(Poles, 
				  Knots,
				  TheCurve.Multiplicities(),
				  TheCurve.Degree());
  myIsDone = Standard_True;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_PointsToBSpline::Init
  (const TColgp_Array1OfPnt&   Points,
   const Standard_Real         W1,
   const Standard_Real         W2,
   const Standard_Real         W3,
   const Standard_Integer      DegMax,
   const GeomAbs_Shape         Continuity,
   const Standard_Real         Tol3D)
{
  Standard_Integer NbPoint = Points.Length(), i;

 
  Standard_Integer nbit = 2;
  if(Tol3D <= 1.e-3) nbit = 0;

 //Variational algo

  AppDef_MultiLine multL(NbPoint);
  for(i = 1; i <= NbPoint; ++i) {
    AppDef_MultiPointConstraint mpc(1, 0);
    mpc.SetPoint(1, Points.Value(Points.Lower() + i - 1));
    multL.SetValue(i, mpc);
  }

  Handle(AppParCurves_HArray1OfConstraintCouple) TABofCC = 
    new AppParCurves_HArray1OfConstraintCouple(1, NbPoint);
  AppParCurves_Constraint  Constraint=AppParCurves_NoConstraint;
  
  for(i = 1; i <= NbPoint; ++i) {
    AppParCurves_ConstraintCouple ACC(i,Constraint);
    TABofCC->SetValue(i,ACC);
  }
  

  AppDef_TheVariational Variation(multL, 1, NbPoint, TABofCC);

//===================================
  Standard_Integer theMaxSegments = 1000;
  Standard_Boolean theWithMinMax = Standard_False;
//===================================      

  Variation.SetMaxDegree(DegMax);
  Variation.SetContinuity(Continuity);
  Variation.SetMaxSegment(theMaxSegments);

  Variation.SetTolerance(Tol3D);
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

  TColgp_Array1OfPnt Poles(1,TheCurve.NbPoles());

  TheCurve.Curve(1, Poles);
  
  myCurve = new Geom_BSplineCurve(Poles, 
				  TheCurve.Knots(),
				  TheCurve.Multiplicities(),
				  TheCurve.Degree());
  myIsDone = Standard_True;

}


//=======================================================================
//function : Handle_Geom_BSplineCurve&
//purpose  : 
//=======================================================================

const Handle(Geom_BSplineCurve)& GeomAPI_PointsToBSpline::Curve() const 
{
  if ( !myIsDone) 
    StdFail_NotDone::Raise("GeomAPI_PointsToBSpline::Curve ");
  return myCurve;
}



//=======================================================================
//function : Geom_BSplineCurve
//purpose  : 
//=======================================================================

GeomAPI_PointsToBSpline::operator Handle(Geom_BSplineCurve)() const
{
  return myCurve;
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean GeomAPI_PointsToBSpline::IsDone() const 
{
  return myIsDone; 
}
