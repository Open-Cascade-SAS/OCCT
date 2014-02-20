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
