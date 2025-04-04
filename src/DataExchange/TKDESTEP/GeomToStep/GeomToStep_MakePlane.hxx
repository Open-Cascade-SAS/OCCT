// Created on: 1993-06-14
// Created by: Martine LANGLOIS
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

#ifndef _GeomToStep_MakePlane_HeaderFile
#define _GeomToStep_MakePlane_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>
class StepGeom_Plane;
class gp_Pln;
class Geom_Plane;

//! This class implements the mapping between classes
//! Plane from Geom and Pln from gp, and the class
//! Plane from StepGeom which describes a plane from
//! Prostep.
class GeomToStep_MakePlane : public GeomToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomToStep_MakePlane(
    const gp_Pln&           P,
    const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT GeomToStep_MakePlane(
    const Handle(Geom_Plane)& P,
    const StepData_Factors&   theLocalFactors = StepData_Factors());

  Standard_EXPORT const Handle(StepGeom_Plane)& Value() const;

protected:
private:
  Handle(StepGeom_Plane) thePlane;
};

#endif // _GeomToStep_MakePlane_HeaderFile
