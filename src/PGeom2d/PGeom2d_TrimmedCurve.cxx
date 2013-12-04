// Created on: 1993-03-04
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

#include <PGeom2d_TrimmedCurve.ixx>

//=======================================================================
//function : PGeom2d_TrimmedCurve
//purpose  : 
//=======================================================================

PGeom2d_TrimmedCurve::PGeom2d_TrimmedCurve()
{}


//=======================================================================
//function : PGeom2d_TrimmedCurve
//purpose  : 
//=======================================================================

PGeom2d_TrimmedCurve::PGeom2d_TrimmedCurve
  (const Handle(PGeom2d_Curve)& aBasisCurve,
   const Standard_Real aFirstU,
   const Standard_Real aLastU) :
  basisCurve(aBasisCurve),
  firstU(aFirstU),
  lastU(aLastU)
{}


//=======================================================================
//function : FirstU
//purpose  : 
//=======================================================================

void  PGeom2d_TrimmedCurve::FirstU(const Standard_Real aFirstU)
{ firstU = aFirstU; }


//=======================================================================
//function : FirstU
//purpose  : 
//=======================================================================

Standard_Real  PGeom2d_TrimmedCurve::FirstU() const 
{ return firstU; }


//=======================================================================
//function : LastU
//purpose  : 
//=======================================================================

void  PGeom2d_TrimmedCurve::LastU(const Standard_Real aLastU)
{ lastU = aLastU; }


//=======================================================================
//function : LastU
//purpose  : 
//=======================================================================

Standard_Real  PGeom2d_TrimmedCurve::LastU() const 
{ return lastU; }


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

void  PGeom2d_TrimmedCurve::BasisCurve(const Handle(PGeom2d_Curve)& aBasisCurve)
{ basisCurve = aBasisCurve; }


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(PGeom2d_Curve)  PGeom2d_TrimmedCurve::BasisCurve() const 
{ return basisCurve; }
