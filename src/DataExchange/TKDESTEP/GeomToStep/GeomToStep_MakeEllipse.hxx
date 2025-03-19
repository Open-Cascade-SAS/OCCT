// Created on: 1994-09-02
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _GeomToStep_MakeEllipse_HeaderFile
#define _GeomToStep_MakeEllipse_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>

class StepGeom_Ellipse;
class gp_Elips;
class Geom_Ellipse;
class Geom2d_Ellipse;

//! This class implements the mapping between classes
//! Ellipse from Geom, and Circ from gp, and the class
//! Ellipse from StepGeom which describes a Ellipse from
//! Prostep.
class GeomToStep_MakeEllipse : public GeomToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomToStep_MakeEllipse(
    const gp_Elips&         C,
    const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT GeomToStep_MakeEllipse(
    const Handle(Geom_Ellipse)& C,
    const StepData_Factors&     theLocalFactors = StepData_Factors());

  Standard_EXPORT GeomToStep_MakeEllipse(
    const Handle(Geom2d_Ellipse)& C,
    const StepData_Factors&       theLocalFactors = StepData_Factors());

  Standard_EXPORT const Handle(StepGeom_Ellipse)& Value() const;

protected:
private:
  Handle(StepGeom_Ellipse) theEllipse;
};

#endif // _GeomToStep_MakeEllipse_HeaderFile
