// Created on: 1993-03-04
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

#include <PGeom_SweptSurface.ixx>

//=======================================================================
//function : PGeom_SweptSurface
//purpose  : 
//=======================================================================

PGeom_SweptSurface::PGeom_SweptSurface()
{}


//=======================================================================
//function : PGeom_SweptSurface
//purpose  : 
//=======================================================================

PGeom_SweptSurface::PGeom_SweptSurface
  (const Handle(PGeom_Curve)& aBasisCurve,
   const gp_Dir& aDirection) :
  basisCurve(aBasisCurve),
  direction(aDirection)
{}


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

void  PGeom_SweptSurface::BasisCurve
  (const Handle(PGeom_Curve)& aBasisCurve)
{ basisCurve = aBasisCurve; }


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(PGeom_Curve)  PGeom_SweptSurface::BasisCurve() const 
{ return basisCurve; }


//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

void  PGeom_SweptSurface::Direction(const gp_Dir& aDirection)
{ direction = aDirection; }


//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

gp_Dir  PGeom_SweptSurface::Direction() const 
{ return direction; }
