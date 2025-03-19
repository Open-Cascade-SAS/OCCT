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

#ifndef _GeomToStep_MakeAxis1Placement_HeaderFile
#define _GeomToStep_MakeAxis1Placement_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <StepData_Factors.hxx>

#include <GeomToStep_Root.hxx>

class StepGeom_Axis1Placement;
class gp_Ax1;
class gp_Ax2d;
class Geom_Axis1Placement;
class Geom2d_AxisPlacement;

//! This class implements the mapping between classes
//! Axis1Placement from Geom and Ax1 from gp, and the class
//! Axis1Placement from StepGeom which describes an
//! Axis1Placement from Prostep.
class GeomToStep_MakeAxis1Placement : public GeomToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomToStep_MakeAxis1Placement(
    const gp_Ax1&           A,
    const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT GeomToStep_MakeAxis1Placement(
    const gp_Ax2d&          A,
    const StepData_Factors& theLocalFactors = StepData_Factors());

  Standard_EXPORT GeomToStep_MakeAxis1Placement(
    const Handle(Geom_Axis1Placement)& A,
    const StepData_Factors&            theLocalFactors = StepData_Factors());

  Standard_EXPORT GeomToStep_MakeAxis1Placement(
    const Handle(Geom2d_AxisPlacement)& A,
    const StepData_Factors&             theLocalFactors = StepData_Factors());

  Standard_EXPORT const Handle(StepGeom_Axis1Placement)& Value() const;

protected:
private:
  Handle(StepGeom_Axis1Placement) theAxis1Placement;
};

#endif // _GeomToStep_MakeAxis1Placement_HeaderFile
