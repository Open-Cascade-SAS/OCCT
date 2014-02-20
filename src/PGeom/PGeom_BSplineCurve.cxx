// Created on: 1993-03-03
// Created by: Philippe DAUTRY
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

#include <PGeom_BSplineCurve.ixx>

//=======================================================================
//function : PGeom_BSplineCurve
//purpose  : 
//=======================================================================

PGeom_BSplineCurve::PGeom_BSplineCurve()
{}


//=======================================================================
//function : PGeom_BSplineCurve
//purpose  : 
//=======================================================================

PGeom_BSplineCurve::PGeom_BSplineCurve
  (const Standard_Boolean aRational,
   const Standard_Boolean aPeriodic,
   const Standard_Integer aSpineDegree,
   const Handle(PColgp_HArray1OfPnt)& aPoles,
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

void  PGeom_BSplineCurve::Periodic(const Standard_Boolean aPeriodic)
{ periodic = aPeriodic; }


//=======================================================================
//function : Periodic
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom_BSplineCurve::Periodic() const 
{ return periodic; }


//=======================================================================
//function : Rational
//purpose  : 
//=======================================================================

void  PGeom_BSplineCurve::Rational(const Standard_Boolean aRational)
{ rational = aRational; }


//=======================================================================
//function : Rational
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom_BSplineCurve::Rational() const 
{ return rational; }


//=======================================================================
//function : SpineDegree
//purpose  : 
//=======================================================================

void  PGeom_BSplineCurve::SpineDegree(const Standard_Integer aSpineDegree)
{ spineDegree = aSpineDegree; }


//=======================================================================
//function : SpineDegree
//purpose  : 
//=======================================================================

Standard_Integer  PGeom_BSplineCurve::SpineDegree() const 
{ return spineDegree; }


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  PGeom_BSplineCurve::Poles(const Handle(PColgp_HArray1OfPnt)& aPoles)
{ poles = aPoles; }


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

Handle(PColgp_HArray1OfPnt)  PGeom_BSplineCurve::Poles() const 
{ return poles; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void  PGeom_BSplineCurve::Weights
  (const Handle(PColStd_HArray1OfReal)& aWeights)
{ weights = aWeights; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal)  PGeom_BSplineCurve::Weights() const 
{ return weights; }


//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void  PGeom_BSplineCurve::Knots
  (const Handle(PColStd_HArray1OfReal)& aKnots)
{ knots = aKnots; }


//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal)  PGeom_BSplineCurve::Knots() const 
{ return knots; }


//=======================================================================
//function : Multiplicities
//purpose  : 
//=======================================================================

void  PGeom_BSplineCurve::Multiplicities
  (const Handle(PColStd_HArray1OfInteger)& aMultiplicities)
{ multiplicities = aMultiplicities; }


//=======================================================================
//function : Multiplicities
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfInteger)
     PGeom_BSplineCurve::Multiplicities() const 
{ return multiplicities; }
