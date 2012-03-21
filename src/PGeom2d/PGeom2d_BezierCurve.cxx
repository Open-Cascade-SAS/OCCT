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
