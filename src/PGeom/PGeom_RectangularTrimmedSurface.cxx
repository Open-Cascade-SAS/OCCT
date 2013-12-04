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

#include <PGeom_RectangularTrimmedSurface.ixx>

//=======================================================================
//function : PGeom_RectangularTrimmedSurface
//purpose  : 
//=======================================================================

PGeom_RectangularTrimmedSurface::PGeom_RectangularTrimmedSurface()
{}


//=======================================================================
//function : PGeom_RectangularTrimmedSurface
//purpose  : 
//=======================================================================

PGeom_RectangularTrimmedSurface::PGeom_RectangularTrimmedSurface
  (const Handle(PGeom_Surface)& aBasisSurface,
   const Standard_Real aFirstU,
   const Standard_Real aLastU,
   const Standard_Real aFirstV,
   const Standard_Real aLastV) :
  basisSurface(aBasisSurface),
  firstU(aFirstU),
  lastU(aLastU),
  firstV(aFirstV),
  lastV(aLastV)
{}


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::BasisSurface
  (const Handle(PGeom_Surface)& aBasisSurface)
{ basisSurface = aBasisSurface;}


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface)  PGeom_RectangularTrimmedSurface::BasisSurface() const 
{ return basisSurface; }


//=======================================================================
//function : FirstU
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::FirstU(const Standard_Real aFirstU)
{ firstU = aFirstU; }


//=======================================================================
//function : FirstU
//purpose  : 
//=======================================================================

Standard_Real  PGeom_RectangularTrimmedSurface::FirstU() const 
{ return firstU; }


//=======================================================================
//function : LastU
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::LastU(const Standard_Real aLastU)
{ lastU = aLastU; }


//=======================================================================
//function : LastU
//purpose  : 
//=======================================================================

Standard_Real  PGeom_RectangularTrimmedSurface::LastU() const 
{ return lastU; }


//=======================================================================
//function : FirstV
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::FirstV(const Standard_Real aFirstV)
{ firstV = aFirstV; }


//=======================================================================
//function : FirstV
//purpose  : 
//=======================================================================

Standard_Real  PGeom_RectangularTrimmedSurface::FirstV() const 
{ return firstV; }


//=======================================================================
//function : LastV
//purpose  : 
//=======================================================================

void  PGeom_RectangularTrimmedSurface::LastV(const Standard_Real aLastV)
{ lastV = aLastV; }


//=======================================================================
//function : LastV
//purpose  : 
//=======================================================================

Standard_Real  PGeom_RectangularTrimmedSurface::LastV() const 
{ return lastV; }
