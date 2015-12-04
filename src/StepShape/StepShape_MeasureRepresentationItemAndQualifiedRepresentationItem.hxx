// Created on: 2001-04-24
// Created by: Christian CAILLET
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem_HeaderFile
#define _StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_HArray1OfValueQualifier.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <Standard_Integer.hxx>
class StepBasic_MeasureWithUnit;
class TCollection_HAsciiString;
class StepBasic_MeasureValueMember;
class StepBasic_Unit;
class StepShape_ValueQualifier;


class StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem;
DEFINE_STANDARD_HANDLE(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem, StepRepr_RepresentationItem)

//! Added for Dimensional Tolerances
//! Complex Type between MeasureRepresentationItem and
//! QualifiedRepresentationItem
class StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem : public StepRepr_RepresentationItem
{

public:

  
  Standard_EXPORT StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepBasic_MeasureValueMember)& aValueComponent, const StepBasic_Unit& aUnitComponent, const Handle(StepShape_HArray1OfValueQualifier)& qualifiers);
  
  Standard_EXPORT void SetMeasure (const Handle(StepBasic_MeasureWithUnit)& Measure);
  
  Standard_EXPORT Handle(StepBasic_MeasureWithUnit) Measure() const;
  
  Standard_EXPORT Handle(StepShape_HArray1OfValueQualifier) Qualifiers() const;
  
  Standard_EXPORT Standard_Integer NbQualifiers() const;
  
  Standard_EXPORT void SetQualifiers (const Handle(StepShape_HArray1OfValueQualifier)& qualifiers);
  
  Standard_EXPORT StepShape_ValueQualifier QualifiersValue (const Standard_Integer num) const;
  
  Standard_EXPORT void SetQualifiersValue (const Standard_Integer num, const StepShape_ValueQualifier& aqualifier);




  DEFINE_STANDARD_RTTIEXT(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem,StepRepr_RepresentationItem)

protected:




private:


  Handle(StepBasic_MeasureWithUnit) myMeasure;
  Handle(StepShape_HArray1OfValueQualifier) theQualifiers;


};







#endif // _StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem_HeaderFile
