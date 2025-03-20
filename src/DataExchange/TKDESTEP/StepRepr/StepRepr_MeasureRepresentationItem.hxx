// Created on: 1999-09-08
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepRepr_MeasureRepresentationItem_HeaderFile
#define _StepRepr_MeasureRepresentationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentationItem.hxx>
class StepBasic_MeasureWithUnit;
class TCollection_HAsciiString;
class StepBasic_MeasureValueMember;
class StepBasic_Unit;

class StepRepr_MeasureRepresentationItem;
DEFINE_STANDARD_HANDLE(StepRepr_MeasureRepresentationItem, StepRepr_RepresentationItem)

//! Implements a measure_representation_item entity
//! which is used for storing validation properties
//! (e.g. area) for shapes
class StepRepr_MeasureRepresentationItem : public StepRepr_RepresentationItem
{

public:
  //! Creates empty object
  Standard_EXPORT StepRepr_MeasureRepresentationItem();

  //! Init all fields
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&     aName,
                            const Handle(StepBasic_MeasureValueMember)& aValueComponent,
                            const StepBasic_Unit&                       aUnitComponent);

  Standard_EXPORT void SetMeasure(const Handle(StepBasic_MeasureWithUnit)& Measure);

  Standard_EXPORT Handle(StepBasic_MeasureWithUnit) Measure() const;

  DEFINE_STANDARD_RTTIEXT(StepRepr_MeasureRepresentationItem, StepRepr_RepresentationItem)

protected:
private:
  Handle(StepBasic_MeasureWithUnit) myMeasure;
};

#endif // _StepRepr_MeasureRepresentationItem_HeaderFile
