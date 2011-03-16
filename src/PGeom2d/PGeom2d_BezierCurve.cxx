// File:	PGeom2d_BezierCurve.cxx
// Created:	Wed Mar  3 15:44:11 1993
// Author:	Philippe DAUTRY
//		<fid@phylox>
// Copyright:	Matra Datavision 1993


#include <PGeom2d_BezierCurve.ixx>

//=======================================================================
//function : PGeom2d_BezierCurve
//purpose  : 
//=======================================================================

PGeom2d_BezierCurve::PGeom2d_BezierCurve()
{}


//=======================================================================
//function : PGeom2d_BezierCurve
//purpose  : 
//=======================================================================

PGeom2d_BezierCurve::PGeom2d_BezierCurve
  (const Handle(PColgp_HArray1OfPnt2d)& aPoles,
   const Handle(PColStd_HArray1OfReal)& aWeights,
   const Standard_Boolean aRational) :
   rational(aRational), poles(aPoles),
   weights(aWeights)
{}


//=======================================================================
//function : Rational
//purpose  : 
//=======================================================================

void  PGeom2d_BezierCurve::Rational(const Standard_Boolean aRational)
{ rational = aRational; }


//=======================================================================
//function : Rational
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom2d_BezierCurve::Rational() const 
{ return rational; }


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  PGeom2d_BezierCurve::Poles
  (const Handle(PColgp_HArray1OfPnt2d)& aPoles)
{ poles = aPoles; }


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

Handle(PColgp_HArray1OfPnt2d)  PGeom2d_BezierCurve::Poles() const 
{ return poles; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void  PGeom2d_BezierCurve::Weights
  (const Handle(PColStd_HArray1OfReal)& aWeights)
{ weights = aWeights; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal)
     PGeom2d_BezierCurve::Weights() const 
{ return weights; }
