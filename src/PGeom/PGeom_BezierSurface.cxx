// Created on: 1993-03-03
// Created by: Philippe DAUTRY
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
