// Created on: 1995-05-04
// Created by: Dieter THIEMANN
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomToStep_MakeParabola_HeaderFile
#define _GeomToStep_MakeParabola_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>
class StepGeom_Parabola;
class Geom2d_Parabola;
class Geom_Parabola;

//! This class implements the mapping between the class
//! Parabola from Geom and the class Parabola from
//! StepGeom which describes a Parabola from ProSTEP
class GeomToStep_MakeParabola : public GeomToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomToStep_MakeParabola(
    const Handle(Geom2d_Parabola)& C,
    const StepData_Factors&        theLocalFactors = StepData_Factors());

  Standard_EXPORT GeomToStep_MakeParabola(
    const Handle(Geom_Parabola)& C,
    const StepData_Factors&      theLocalFactors = StepData_Factors());

  Standard_EXPORT const Handle(StepGeom_Parabola)& Value() const;

protected:
private:
  Handle(StepGeom_Parabola) theParabola;
};

#endif // _GeomToStep_MakeParabola_HeaderFile
