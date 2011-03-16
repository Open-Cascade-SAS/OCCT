// File:	PGeom_BezierCurve.cxx
// Created:	Wed Mar  3 15:44:11 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom_BezierCurve.ixx>

//=======================================================================
//function : PGeom_BezierCurve
//purpose  : 
//=======================================================================

PGeom_BezierCurve::PGeom_BezierCurve()
{}


//=======================================================================
//function : PGeom_BezierCurve
//purpose  : 
//=======================================================================

PGeom_BezierCurve::PGeom_BezierCurve
  (const Handle(PColgp_HArray1OfPnt)& aPoles,
   const Handle(PColStd_HArray1OfReal)& aWeights,
   const Standard_Boolean aRational) :
   rational(aRational),
   poles(aPoles),
   weights(aWeights)
{}


//=======================================================================
//function : Rational
//purpose  : 
//=======================================================================

void  PGeom_BezierCurve::Rational(const Standard_Boolean aRational)
{ rational = aRational; }


//=======================================================================
//function : Rational
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom_BezierCurve::Rational() const 
{ return rational; }


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  PGeom_BezierCurve::Poles
  (const Handle(PColgp_HArray1OfPnt)& aPoles)
{ poles = aPoles; }


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

Handle(PColgp_HArray1OfPnt)  PGeom_BezierCurve::Poles() const 
{ return poles; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void  PGeom_BezierCurve::Weights
  (const Handle(PColStd_HArray1OfReal)& aWeights)
{ weights = aWeights; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal)
     PGeom_BezierCurve::Weights() const 
{ return weights; }
