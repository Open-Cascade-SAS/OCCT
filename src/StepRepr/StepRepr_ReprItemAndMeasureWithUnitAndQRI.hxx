// Created on: 2015-07-22
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepRepr_ReprItemAndMeasureWithUnitAndQRI_HeaderFile
#define _StepRepr_ReprItemAndMeasureWithUnitAndQRI_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_ReprItemAndMeasureWithUnit.hxx>
class StepShape_QualifiedRepresentationItem;

class StepRepr_ReprItemAndMeasureWithUnitAndQRI;
DEFINE_STANDARD_HANDLE(StepRepr_ReprItemAndMeasureWithUnitAndQRI, StepRepr_ReprItemAndMeasureWithUnit)

//! Base class for complex types (MEASURE_REPRESENTATION_ITEM, MEASURE_WITH_UNIT, QUALIFIED_REPRESENTATION_ITEM
//! REPRESENTATION_ITEM, LENGTH_MEASURE_WITH_UNIT/PLANE_ANGLE_MEASURE_WITH_UNIT).
class StepRepr_ReprItemAndMeasureWithUnitAndQRI : public StepRepr_ReprItemAndMeasureWithUnit
{

public:

  
  Standard_EXPORT StepRepr_ReprItemAndMeasureWithUnitAndQRI();

  Standard_EXPORT void Init (const Handle(StepBasic_MeasureWithUnit)& aMWU, const Handle(StepRepr_RepresentationItem)& aRI, const Handle(StepShape_QualifiedRepresentationItem)& aQRI);
  
  Standard_EXPORT void SetQualifiedRepresentationItem (const Handle(StepShape_QualifiedRepresentationItem)& aQRI);
  
  Standard_EXPORT Handle(StepShape_QualifiedRepresentationItem) GetQualifiedRepresentationItem() const;

  DEFINE_STANDARD_RTTIEXT(StepRepr_ReprItemAndMeasureWithUnitAndQRI,StepRepr_ReprItemAndMeasureWithUnit)

private:
  Handle(StepShape_QualifiedRepresentationItem) myQualifiedRepresentationItem;
};
#endif // _StepRepr_ReprItemAndMeasureWithUnitAndQRI_HeaderFile
