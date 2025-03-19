// Created on: 1993-06-21
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

#ifndef _GeomToStep_MakeBoundedCurve_HeaderFile
#define _GeomToStep_MakeBoundedCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>

class StepGeom_BoundedCurve;
class Geom_BoundedCurve;
class Geom2d_BoundedCurve;

//! This class implements the mapping between classes
//! BoundedCurve from Geom, Geom2d and the class BoundedCurve from
//! StepGeom which describes a BoundedCurve from prostep.
//! As BoundedCurve is an abstract BoundedCurve this class
//! is an access to the sub-class required.
class GeomToStep_MakeBoundedCurve : public GeomToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomToStep_MakeBoundedCurve(
    const Handle(Geom_BoundedCurve)& C,
    const StepData_Factors&          theLocalFactors = StepData_Factors());

  Standard_EXPORT GeomToStep_MakeBoundedCurve(
    const Handle(Geom2d_BoundedCurve)& C,
    const StepData_Factors&            theLocalFactors = StepData_Factors());

  Standard_EXPORT const Handle(StepGeom_BoundedCurve)& Value() const;

protected:
private:
  Handle(StepGeom_BoundedCurve) theBoundedCurve;
};

#endif // _GeomToStep_MakeBoundedCurve_HeaderFile
