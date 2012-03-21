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



#include <PGeom_BezierSurface.ixx>

//=======================================================================
//function : PGeom_BezierSurface
//purpose  : 
//=======================================================================

PGeom_BezierSurface::PGeom_BezierSurface()
{}


//=======================================================================
//function : PGeom_BezierSurface
//purpose  : 
//=======================================================================

PGeom_BezierSurface::PGeom_BezierSurface
  (const Standard_Boolean aURational,
   const Standard_Boolean aVRational,
   const Handle(PColgp_HArray2OfPnt)& aPoles,
   const Handle(PColStd_HArray2OfReal)& aWeights) :
  uRational(aURational),
  vRational(aVRational),
  poles(aPoles),
  weights(aWeights)
{}


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  PGeom_BezierSurface::Poles(const Handle(PColgp_HArray2OfPnt)& aPoles)
{ poles = aPoles; }


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

Handle(PColgp_HArray2OfPnt)  PGeom_BezierSurface::Poles() const 
{ return poles; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void  PGeom_BezierSurface::Weights(const Handle(PColStd_HArray2OfReal)& aWeights)
{ weights = aWeights; }


//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

Handle(PColStd_HArray2OfReal)  PGeom_BezierSurface::Weights() const 
{ return weights; }


//=======================================================================
//function : URational
//purpose  : 
//=======================================================================

void  PGeom_BezierSurface::URational(const Standard_Boolean aURational)
{ uRational = aURational; }


//=======================================================================
//function : URational
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom_BezierSurface::URational() const 
{ return uRational; }


//=======================================================================
//function : VRational
//purpose  : 
//=======================================================================

void  PGeom_BezierSurface::VRational(const Standard_Boolean aVRational)
{ vRational = aVRational; }


//=======================================================================
//function : VRational
//purpose  : 
//=======================================================================

Standard_Boolean  PGeom_BezierSurface::VRational() const 
{ return vRational; }
