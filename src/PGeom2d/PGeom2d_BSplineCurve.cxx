// File:	PGeom2d_BSplineCurve.cxx
// Created:	Wed Mar  3 15:08:56 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_BSplineCurve.ixx>

//=======================================================================
//function : PGeom2d_BSplineCurve
//purpose  : 
//=======================================================================

PGeom2d_BSplineCurve::PGeom2d_BSplineCurve()
{}


//=======================================================================
//function : PGeom2d_BSplineCurve
//purpose  : 
//=======================================================================

PGeom2d_BSplineCurve::PGeom2d_BSplineCurve
  (const Standard_Boolean aRational,
   const Standard_Boolean aPeriodic,
   const Standard_Integer aSpineDegree,
   const Handle(PColgp_HArray1OfPnt2d)& aPoles,
   const Handle(PColStd_HArray1OfReal)& aWeights,
   const Handle(PColStd_HArray1OfReal)& aKnots,
   const Handle(PColStd_HArray1OfInteger)& aMultiplicities) :
  rational(aRational),
  periodic(aPeriodic),
  spineDegree(aSpineDegree),
  poles(aPoles),
  weights(aWeights),
  knots(aKnots),
  multiplicities(aMultiplicities)
{}


//=======================================================================
//function : Periodic
//purpose  : 
//=======================================================================

void  PGeom2d_BSplineCurve::Periodic(const Standard_Boolean aPeriodic)
{ periodic = aPeriodic; }


//=======================================================================
//function : Periodic
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom2d_BSplineCurve::Periodic() const 
{ return periodic; }


//=======================================================================
//function : Rational
//purpose  : 
//=======================================================================

void  PGeom2d_BSplineCurve::Rational(const Standard_Boolean aRational)
{ rational = aRational; }


//=======================================================================
//function : Rational
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom2d_BSplineCurve::Rational() const 
{ return rational; }


//=======================================================================
//function : SpineDegree
//purpose  : 
//=======================================================================

void  PGeom2d_BSplineCurve::SpineDegree(const Standard_Integer aSpineDegree)
{ spineDegree = aSpineDegree; }


//=======================================================================
//function : SpineDegree
//purpose  : 
//=======================================================================

Standard_Integer  PGeom2d_BSplineCurve::SpineDegree() const 
{ return spineDegree; }


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  PGeom2d_BSplineCurve::Poles(const Handle(PColgp_HArray1OfPnt2d)& aPoles)
{ poles = aPoles; }


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

Handle(PColgp_HArray1OfPnt2d)  PGeom2d_BSplineCurve::Poles() const 
{ return poles; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void  PGeom2d_BSplineCurve::Weights
  (const Handle(PColStd_HArray1OfReal)& aWeights)
{ weights = aWeights; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal)  PGeom2d_BSplineCurve::Weights() const 
{ return weights; }


//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void  PGeom2d_BSplineCurve::Knots
  (const Handle(PColStd_HArray1OfReal)& aKnots)
{ knots = aKnots; }


//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal)  PGeom2d_BSplineCurve::Knots() const 
{ return knots; }


//=======================================================================
//function : Multiplicities
//purpose  : 
//=======================================================================

void  PGeom2d_BSplineCurve::Multiplicities
  (const Handle(PColStd_HArray1OfInteger)& aMultiplicities)
{ multiplicities = aMultiplicities; }


//=======================================================================
//function : Multiplicities
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfInteger)
     PGeom2d_BSplineCurve::Multiplicities() const 
{ return multiplicities; }
