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

#ifndef _GeomToStep_MakeLine_HeaderFile
#define _GeomToStep_MakeLine_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>
class StepGeom_Line;
class gp_Lin;
class gp_Lin2d;
class Geom_Line;
class Geom2d_Line;

//! This class implements the mapping between classes
//! Line from Geom and Lin from gp, and the class
//! Line from StepGeom which describes a line from
//! Prostep.
class GeomToStep_MakeLine : public GeomToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomToStep_MakeLine(const gp_Lin&           L,
                                      const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT GeomToStep_MakeLine(const gp_Lin2d&         L,
                                      const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT GeomToStep_MakeLine(const Handle(Geom_Line)& C,
                                      const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT GeomToStep_MakeLine(const Handle(Geom2d_Line)& C,
                                      const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT const Handle(StepGeom_Line)& Value() const;

protected:
private:
  Handle(StepGeom_Line) theLine;
};

#endif // _GeomToStep_MakeLine_HeaderFile
