// Created on: 1995-01-17
// Created by: Remi LEQUETTE
// Copyright (c) 1995-1999 Matra Datavision
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
#include <AppParCurves_ConstraintCouple.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
#include <BSplCLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <gp_Pnt.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>

//=======================================================================
//function : GeomAPI_PointsToBSplineSurface
//purpose  : 
//=======================================================================
GeomAPI_PointsToBSplineSurface::GeomAPI_PointsToBSplineSurface()
: myIsDone ( Standard_False)
{
}


//=======================================================================
//function : GeomAPI_PointsToBSplineSurface
//purpose  : 
//=======================================================================

GeomAPI_PointsToBSplineSurface::GeomAPI_PointsToBSplineSurface
(const TColgp_Array2OfPnt& Points, 
 const Standard_Integer DegMin,
 const Standard_Integer DegMax,
 const GeomAbs_Shape Continuity, 
 const Standard_Real Tol3D)
: myIsDone ( Standard_False)
{
  Init(Points,DegMin,DegMax,Continuity,Tol3D);
}

//=======================================================================
//function : GeomAPI_PointsToBSplineSurface
//purpose  : 
//=======================================================================

GeomAPI_PointsToBSplineSurface::GeomAPI_PointsToBSplineSurface
(const TColgp_Array2OfPnt& Points, 
 const Approx_ParametrizationType ParType,
 const Standard_Integer DegMin,
 const Standard_Integer DegMax,
 const GeomAbs_Shape Continuity, 
 const Standard_Real Tol3D)
: myIsDone ( Standard_False)
{
  Init(Points,ParType,DegMin,DegMax,Continuity,Tol3D);
}

//=======================================================================
//function : GeomAPI_PointsToBSplineSurface
//purpose  : 
//=======================================================================

GeomAPI_PointsToBSplineSurface::GeomAPI_PointsToBSplineSurface
(const TColgp_Array2OfPnt& Points, 
 const Standard_Real Weight1,
 const Standard_Real Weight2,
 const Standard_Real Weight3,
 const Standard_Integer DegMax,
 const GeomAbs_Shape Continuity, 
 const Standard_Real Tol3D)
: myIsDone ( Standard_False)
{
  Init(Points,Weight1,Weight2,Weight3,DegMax,Continuity,Tol3D);
}



//=======================================================================
//function : GeomAPI_PointsToBSplineSurface
//purpose  : 
//=======================================================================

GeomAPI_PointsToBSplineSurface::GeomAPI_PointsToBSplineSurface
(const TColStd_Array2OfReal& ZPoints, 
 const Standard_Real X0, 
 const Standard_Real dX, 
 const Standard_Real Y0,
 const Standard_Real dY, 
 const Standard_Integer DegMin, 
 const Standard_Integer DegMax, 
 const GeomAbs_Shape Continuity, 
 const Standard_Real Tol3D)
: myIsDone ( Standard_False)
{
  Init(ZPoints,X0,dX,Y0,dY,DegMin,DegMax,Continuity,Tol3D);
}



//=======================================================================
//function : Interpolate
//purpose  : 
//=======================================================================

void GeomAPI_PointsToBSplineSurface::Interpolate(const TColgp_Array2OfPnt& Points)
{
  Interpolate(Points, Approx_ChordLength);
}

//=======================================================================
//function : Interpolate
//purpose  : 
//=======================================================================

void GeomAPI_PointsToBSplineSurface::Interpolate(const TColgp_Array2OfPnt& Points,
						 const Approx_ParametrizationType ParType)
{
  Standard_Integer DegMin, DegMax;
  DegMin = DegMax = 3;
  GeomAbs_Shape CC  = GeomAbs_C2;
  Standard_Real Tol3d = -1.0;
  Init(Points, ParType, DegMin, DegMax, CC, Tol3d);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_PointsToBSplineSurface::Init(const TColgp_Array2OfPnt& Points,
					  const Standard_Integer DegMin, 
					  const Standard_Integer DegMax, 
					  const GeomAbs_Shape Continuity,
					  const Standard_Real Tol3D)
{
  Init(Points, Approx_ChordLength, DegMin, DegMax, Continuity, Tol3D);
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_PointsToBSplineSurface::Init(const TColgp_Array2OfPnt& Points,
					  const Approx_ParametrizationType ParType,
					  const Standard_Integer DegMin, 
					  const Standard_Integer DegMax, 
					  const GeomAbs_Shape Continuity,
					  const Standard_Real Tol3D)
{
  Standard_Integer Imin = Points.LowerRow();
  Standard_Integer Imax = Points.UpperRow();
  Standard_Integer Jmin = Points.LowerCol();
  Standard_Integer Jmax = Points.UpperCol();

  Standard_Real Tol2D = Tol3D;

  // first approximate the V isos:
  AppDef_MultiLine Line(Jmax-Jmin+1);
  Standard_Integer i, j;
//  Standard_Real X, Y;

  for (j = Jmin; j <= Jmax; j++) {
    AppDef_MultiPointConstraint MP(Imax-Imin+1, 0);
    for (i = Imin; i <= Imax; i++) {
      MP.SetPoint(i, Points(i,j));
    }
    Line.SetValue(j, MP);
  }

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
  
  if (Tol3D <= 0.0) {
    TheComputer.Interpol(Line);
  }
  else {
    TheComputer.Perform(Line);
  }

  const AppParCurves_MultiBSpCurve& TheCurve = TheComputer.Value();
  
  Standard_Integer VDegree = TheCurve.Degree();
  TColgp_Array1OfPnt Poles(1,TheCurve.NbPoles());
  const TColStd_Array1OfReal& VKnots = TheCurve.Knots();
  const TColStd_Array1OfInteger& VMults = TheCurve.Multiplicities();
  

  Standard_Integer nbisosu = Imax-Imin+1;
  AppDef_MultiLine Line2(nbisosu);

  for (i = 1; i <= nbisosu; i++) {
    TheCurve.Curve(i, Poles);
    AppDef_MultiPointConstraint MP(Poles.Upper(),0);
    for (j = 1; j <= Poles.Upper(); j++) {
      MP.SetPoint(j, Poles(j));
    }
    Line2.SetValue(i, MP);
  }


  // approximate the resulting poles:
  
  AppDef_BSplineCompute TheComputer2
    (DegMin,DegMax,Tol3D,Tol2D,nbit,Standard_True,ParType,UseSquares);
  if (Tol3D <= 0.0) {
    TheComputer2.Interpol(Line2);
  }
  else {
    TheComputer2.Perform(Line2);
  }
  
  const AppParCurves_MultiBSpCurve& TheCurve2 = TheComputer2.Value();
  
  Standard_Integer UDegree = TheCurve2.Degree();
  TColgp_Array1OfPnt Poles2(1,TheCurve2.NbPoles());
  const TColStd_Array1OfReal& UKnots = TheCurve2.Knots();
  const TColStd_Array1OfInteger& UMults = TheCurve2.Multiplicities();
  

  // computing the surface
  TColgp_Array2OfPnt ThePoles(1, Poles2.Upper(), 1, Poles.Upper());
  for ( j = 1; j <= Poles.Upper(); j++) {
    TheCurve2.Curve(j, Poles2);
    for (i = 1; i<= Poles2.Upper(); i++) {
      ThePoles(i, j) = Poles2(i);
    }
  }


  mySurface = new Geom_BSplineSurface(ThePoles, UKnots, VKnots, UMults, VMults,
				      UDegree, VDegree);

  myIsDone = Standard_True;
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_PointsToBSplineSurface::Init(const TColgp_Array2OfPnt& Points,
					  const Standard_Real Weight1,
					  const Standard_Real Weight2,
					  const Standard_Real Weight3,
					  const Standard_Integer DegMax, 
					  const GeomAbs_Shape Continuity,
					  const Standard_Real Tol3D)
{
  Standard_Integer Imin = Points.LowerRow();
  Standard_Integer Imax = Points.UpperRow();
  Standard_Integer Jmin = Points.LowerCol();
  Standard_Integer Jmax = Points.UpperCol();

  Standard_Integer nbit = 2;
  if(Tol3D <= 1.e-3) nbit = 0;

  // first approximate the V isos:
  Standard_Integer NbPointJ = Jmax-Jmin+1;
  Standard_Integer NbPointI = Imax-Imin+1;
  Standard_Integer i, j;

  AppDef_MultiLine Line(NbPointJ);

  for (j = Jmin; j <= Jmax; j++) {
    AppDef_MultiPointConstraint MP(NbPointI, 0);
    for (i = Imin; i <= Imax; i++) {
      MP.SetPoint(i, Points(i,j));
    }
    Line.SetValue(j, MP);
  }


  Handle(AppParCurves_HArray1OfConstraintCouple) TABofCC = 
    new AppParCurves_HArray1OfConstraintCouple(1, NbPointJ);
  AppParCurves_Constraint  Constraint=AppParCurves_NoConstraint;
  
  for(i = 1; i <= NbPointJ; ++i) {
    AppParCurves_ConstraintCouple ACC(i,Constraint);
    TABofCC->SetValue(i,ACC);
  }
  

  AppDef_Variational Variation(Line, 1, NbPointJ, TABofCC);

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

  Variation.SetCriteriumWeight(Weight1, Weight2, Weight3);

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

  const AppParCurves_MultiBSpCurve& TheCurve = Variation.Value();
  
  Standard_Integer VDegree = TheCurve.Degree();
  TColgp_Array1OfPnt Poles(1,TheCurve.NbPoles());
  const TColStd_Array1OfReal& VKnots = TheCurve.Knots();
  const TColStd_Array1OfInteger& VMults = TheCurve.Multiplicities();
  

  AppDef_MultiLine Line2(NbPointI);

  for (i = 1; i <= NbPointI; i++) {
    TheCurve.Curve(i, Poles);
    AppDef_MultiPointConstraint MP(Poles.Upper(),0);
    for (j = 1; j <= Poles.Upper(); j++) {
      MP.SetPoint(j, Poles(j));
    }
    Line2.SetValue(i, MP);
  }


  Handle(AppParCurves_HArray1OfConstraintCouple) TABofCC2 = 
    new AppParCurves_HArray1OfConstraintCouple(1, NbPointI);
  
  for(i = 1; i <= NbPointI; ++i) {
    AppParCurves_ConstraintCouple ACC(i,Constraint);
    TABofCC2->SetValue(i,ACC);
  }
  

  AppDef_Variational Variation2(Line2, 1, NbPointI, TABofCC2);

  Variation2.SetMaxDegree(DegMax);
  Variation2.SetContinuity(Continuity);
  Variation2.SetMaxSegment(theMaxSegments);

  Variation2.SetTolerance(Tol3D);
  Variation2.SetWithMinMax(theWithMinMax);
  Variation2.SetNbIterations(nbit);

  Variation2.SetCriteriumWeight(Weight1, Weight2, Weight3);

  if(!Variation2.IsCreated()) {
    return;
  }
  
  if(Variation2.IsOverConstrained()) {
    return;
  }

  try {
    Variation2.Approximate();
  }
  catch (Standard_Failure) {
    return;
  }

  if(!Variation2.IsDone()) {
    return;
  }

  const AppParCurves_MultiBSpCurve& TheCurve2 = Variation2.Value();  
  
  Standard_Integer UDegree = TheCurve2.Degree();
  TColgp_Array1OfPnt Poles2(1,TheCurve2.NbPoles());
  const TColStd_Array1OfReal& UKnots = TheCurve2.Knots();
  const TColStd_Array1OfInteger& UMults = TheCurve2.Multiplicities();
  

  // computing the surface
  TColgp_Array2OfPnt ThePoles(1, Poles2.Upper(), 1, Poles.Upper());
  for ( j = 1; j <= Poles.Upper(); j++) {
    TheCurve2.Curve(j, Poles2);
    for (i = 1; i<= Poles2.Upper(); i++) {
      ThePoles(i, j) = Poles2(i);
    }
  }


  mySurface = new Geom_BSplineSurface(ThePoles, UKnots, VKnots, UMults, VMults,
				      UDegree, VDegree);

  myIsDone = Standard_True;
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_PointsToBSplineSurface::Interpolate(const TColStd_Array2OfReal& ZPoints,
						 const Standard_Real X0, 
						 const Standard_Real dX, 
						 const Standard_Real Y0, 
						 const Standard_Real dY)
{
  Standard_Integer DegMin, DegMax;
  DegMin = DegMax = 3;
  Standard_Real Tol3D  = -1.0;
  GeomAbs_Shape CC = GeomAbs_C2;
  Init(ZPoints, X0, dX, Y0, dY, DegMin, DegMax, CC, Tol3D);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomAPI_PointsToBSplineSurface::Init(const TColStd_Array2OfReal& ZPoints,
					  const Standard_Real X0, 
					  const Standard_Real dX, 
					  const Standard_Real Y0, 
					  const Standard_Real dY, 
					  const Standard_Integer DegMin,
					  const Standard_Integer DegMax,
					  const GeomAbs_Shape Continuity,
					  const Standard_Real Tol3D)
{
  Standard_Integer Imin = ZPoints.LowerRow();
  Standard_Integer Imax = ZPoints.UpperRow();
  Standard_Integer Jmin = ZPoints.LowerCol();
  Standard_Integer Jmax = ZPoints.UpperCol();
  Standard_Real length;

  Standard_Real Tol2D = Tol3D;

  // first approximate the V isos:
  AppDef_MultiLine Line(Jmax-Jmin+1);
  math_Vector Param(Jmin, Jmax);
  Standard_Integer i, j;
//  Standard_Real X, Y;
  length = dY * (Jmax-Jmin);

  for (j = Jmin; j <= Jmax; j++) {
    AppDef_MultiPointConstraint MP(0, Imax-Imin+1);
    for (i = Imin; i <= Imax; i++) {
      MP.SetPoint2d(i, gp_Pnt2d(0.0, ZPoints(i, j)));
    }
    Param(j) = (Standard_Real)(j-Jmin)/(Standard_Real)(Jmax-Jmin);
    Line.SetValue(j, MP);
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
  
  if (Tol3D <= 0.0) {
    TheComputer.Interpol(Line);
  }
  else {
    TheComputer.Perform(Line);
  }


  const AppParCurves_MultiBSpCurve& TheCurve = TheComputer.Value();
  
  Standard_Integer VDegree = TheCurve.Degree();
  TColgp_Array1OfPnt2d Poles(1,TheCurve.NbPoles());
  Standard_Integer nk = TheCurve.Knots().Length();
  TColStd_Array1OfReal VKnots(1,nk);
  TColStd_Array1OfInteger VMults(1,nk);
  

  // compute Y values for the poles
  TColStd_Array1OfReal YPoles(1,Poles.Upper());
  
  // start with a line
  TColStd_Array1OfReal    TempPoles(1,2);
  TColStd_Array1OfReal    TempKnots(1,2);
  TColStd_Array1OfInteger TempMults(1,2);
  TempMults.Init(2);
  TempPoles(1) = Y0;
  TempPoles(2) = Y0 + length;
  TempKnots(1) = 0.;
  TempKnots(2) = 1.;
  
  // increase the Degree
  TColStd_Array1OfReal    NewTempPoles(1,VDegree+1);
  TColStd_Array1OfReal    NewTempKnots(1,2);
  TColStd_Array1OfInteger NewTempMults(1,2);
  BSplCLib::IncreaseDegree(1,VDegree,Standard_False,1,
			   TempPoles,TempKnots,TempMults,
			   NewTempPoles,NewTempKnots,NewTempMults);
  
  
  // insert the Knots
  BSplCLib::InsertKnots(VDegree,Standard_False,1,
			NewTempPoles,NewTempKnots,NewTempMults,
			TheCurve.Knots(),&TheCurve.Multiplicities(),
			YPoles,VKnots,VMults,
			Epsilon(1));
  
  // scale the knots
  for (i = 1; i <= nk; i++) {
    VKnots(i) = Y0 + length * VKnots(i);
  }
  

  Standard_Integer nbisosu = Imax-Imin+1;
  AppDef_MultiLine Line2(nbisosu);
  math_Vector Param2(1, nbisosu);
  length = dX*(Imax-Imin);

  for (i = 1; i <= nbisosu; i++) {
    TheCurve.Curve(i, Poles);
    AppDef_MultiPointConstraint MP(0, Poles.Upper());
    for (j = 1; j <= Poles.Upper(); j++) {
      MP.SetPoint2d(j, gp_Pnt2d(0.0, Poles(j).Y()));
    }
    Param2(i) = (Standard_Real)(i-1)/(Standard_Real)(nbisosu-1);
    Line2.SetValue(i, MP);
  }


  // approximate the resulting poles:
  
  AppDef_BSplineCompute TheComputer2
    (Param2, DegMin,DegMax,Tol3D,Tol2D,0, Standard_True, Standard_True);
  if (Tol3D <= 0.0) {
    TheComputer2.Interpol(Line2);
  }
  else {
    TheComputer2.Perform(Line2);
  }
  
  const AppParCurves_MultiBSpCurve& TheCurve2 = TheComputer2.Value();
  
  Standard_Integer UDegree = TheCurve2.Degree();
  TColgp_Array1OfPnt2d Poles2(1,TheCurve2.NbPoles());
  Standard_Integer nk2 = TheCurve2.Knots().Length();
  TColStd_Array1OfReal UKnots(1,nk2);
  TColStd_Array1OfInteger UMults(1,nk2);
  

  // compute X values for the poles
  TColStd_Array1OfReal XPoles(1,Poles2.Upper());
  
  // start with a line
  TempPoles(1) = X0;
  TempPoles(2) = X0 + length;
  TempKnots(1) = 0.;
  TempKnots(2) = 1.;
  TempMults(1) = TempMults(2) = 2;
  
  // increase the Degree
  TColStd_Array1OfReal    NewTempPoles2(1,UDegree+1);
  BSplCLib::IncreaseDegree(1,UDegree,Standard_False,1,
			   TempPoles,TempKnots,TempMults,
			   NewTempPoles2,NewTempKnots,NewTempMults);
  
  
  // insert the Knots
  BSplCLib::InsertKnots(UDegree,Standard_False,1,
			NewTempPoles2,NewTempKnots,NewTempMults,
			TheCurve2.Knots(),&TheCurve2.Multiplicities(),
			XPoles,UKnots,UMults,
			Epsilon(1));
  
  // scale the knots
  for (i = 1; i <= nk2; i++) {
    UKnots(i) = X0 + length * UKnots(i);
  }
  
  // creating the surface
  TColgp_Array2OfPnt ThePoles(1, Poles2.Upper(), 1, Poles.Upper());

  for ( j = 1; j <= Poles.Upper(); j++) {
    TheCurve2.Curve(j, Poles2);
    for (i = 1; i<= Poles2.Upper(); i++) {
      ThePoles(i, j).SetCoord(XPoles(i), YPoles(j), Poles2(i).Y());
    }
  }


  mySurface = new Geom_BSplineSurface(ThePoles, UKnots, VKnots, UMults, VMults,
				      UDegree, VDegree);

  myIsDone = Standard_True;
  
}


//=======================================================================
//function : Handle(Geom_BSplineSurface)&
//purpose  : 
//=======================================================================

const Handle(Geom_BSplineSurface)& GeomAPI_PointsToBSplineSurface::Surface()
const 
{
  StdFail_NotDone_Raise_if
    ( !myIsDone,
     "GeomAPI_PointsToBSplineSurface: Surface not done");

  return mySurface;
}



//=======================================================================
//function : Handle
//purpose  : 
//=======================================================================

GeomAPI_PointsToBSplineSurface::operator Handle(Geom_BSplineSurface)() const
{
  return Surface();
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean GeomAPI_PointsToBSplineSurface::IsDone () const
{
  return myIsDone;
}


