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

#include <PGeom_OffsetSurface.ixx>

//=======================================================================
//function : PGeom_OffsetSurface
//purpose  : 
//=======================================================================

PGeom_OffsetSurface::PGeom_OffsetSurface()
{}


//=======================================================================
//function : PGeom_OffsetSurface
//purpose  : 
//=======================================================================

PGeom_OffsetSurface::PGeom_OffsetSurface
  (const Handle(PGeom_Surface)& aBasisSurface,
   const Standard_Real aOffsetValue) :
  basisSurface(aBasisSurface),
  offsetValue(aOffsetValue)
{}


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

void  PGeom_OffsetSurface::BasisSurface
  (const Handle(PGeom_Surface)& aBasisSurface)
{ basisSurface = aBasisSurface; }


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PGeom_OffsetSurface::BasisSurface() const 
{ return basisSurface; }


//=======================================================================
//function : OffsetValue
//purpose  : 
//=======================================================================

void  PGeom_OffsetSurface::OffsetValue(const Standard_Real aOffsetValue)
{ offsetValue = aOffsetValue; }


//=======================================================================
//function : OffsetValue
//purpose  : 
//=======================================================================

Standard_Real  PGeom_OffsetSurface::OffsetValue() const 
{ return offsetValue; }
