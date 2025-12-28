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

#include <StepShape_ValueQualifier.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <Standard_Integer.hxx>
class StepBasic_MeasureWithUnit;
class TCollection_HAsciiString;
class StepBasic_MeasureValueMember;
class StepBasic_Unit;
class StepShape_ValueQualifier;

//! Added for Dimensional Tolerances
//! Complex Type between MeasureRepresentationItem and
//! QualifiedRepresentationItem
class StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem
    : public StepRepr_RepresentationItem
{

public:
  Standard_EXPORT StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&          aName,
                            const occ::handle<StepBasic_MeasureValueMember>&      aValueComponent,
                            const StepBasic_Unit&                            aUnitComponent,
                            const occ::handle<NCollection_HArray1<StepShape_ValueQualifier>>& qualifiers);

  Standard_EXPORT void SetMeasure(const occ::handle<StepBasic_MeasureWithUnit>& Measure);

  Standard_EXPORT occ::handle<StepBasic_MeasureWithUnit> Measure() const;

  Standard_EXPORT occ::handle<NCollection_HArray1<StepShape_ValueQualifier>> Qualifiers() const;

  Standard_EXPORT int NbQualifiers() const;

  Standard_EXPORT void SetQualifiers(const occ::handle<NCollection_HArray1<StepShape_ValueQualifier>>& qualifiers);

  Standard_EXPORT StepShape_ValueQualifier QualifiersValue(const int num) const;

  Standard_EXPORT void SetQualifiersValue(const int          num,
                                          const StepShape_ValueQualifier& aqualifier);

  DEFINE_STANDARD_RTTIEXT(StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem,
                          StepRepr_RepresentationItem)

private:
  occ::handle<StepBasic_MeasureWithUnit>         myMeasure;
  occ::handle<NCollection_HArray1<StepShape_ValueQualifier>> theQualifiers;
};

#endif // _StepShape_MeasureRepresentationItemAndQualifiedRepresentationItem_HeaderFile
