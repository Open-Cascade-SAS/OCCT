// Created on: 1993-03-10
// Created by: JCV
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


#include <Geom_Curve.hxx>
#include <Geom_SweptSurface.hxx>
#include <gp_Dir.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_SweptSurface,Geom_Surface)

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================
GeomAbs_Shape Geom_SweptSurface::Continuity () const { return smooth; }

//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

const gp_Dir& Geom_SweptSurface::Direction () const  { return direction; }

//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_SweptSurface::BasisCurve () const 
{ 
  return basisCurve;
}
