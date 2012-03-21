// Created on: 1993-03-03
// Created by: Philippe DAUTRY
// Copyright (c) 1993-1999 Matra Datavision
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
