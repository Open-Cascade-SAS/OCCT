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

#ifndef _StepShape_MeasureQualification_HeaderFile
#define _StepShape_MeasureQualification_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_HArray1OfValueQualifier.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;
class StepShape_ValueQualifier;

class StepShape_MeasureQualification;
DEFINE_STANDARD_HANDLE(StepShape_MeasureQualification, Standard_Transient)

//! Added for Dimensional Tolerances
class StepShape_MeasureQualification : public Standard_Transient
{

public:
  Standard_EXPORT StepShape_MeasureQualification();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&          name,
                            const Handle(TCollection_HAsciiString)&          description,
                            const Handle(StepBasic_MeasureWithUnit)&         qualified_measure,
                            const Handle(StepShape_HArray1OfValueQualifier)& qualifiers);

  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& name);

  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;

  Standard_EXPORT void SetDescription(const Handle(TCollection_HAsciiString)& description);

  Standard_EXPORT Handle(StepBasic_MeasureWithUnit) QualifiedMeasure() const;

  Standard_EXPORT void SetQualifiedMeasure(
    const Handle(StepBasic_MeasureWithUnit)& qualified_measure);

  Standard_EXPORT Handle(StepShape_HArray1OfValueQualifier) Qualifiers() const;

  Standard_EXPORT Standard_Integer NbQualifiers() const;

  Standard_EXPORT void SetQualifiers(const Handle(StepShape_HArray1OfValueQualifier)& qualifiers);

  Standard_EXPORT StepShape_ValueQualifier QualifiersValue(const Standard_Integer num) const;

  Standard_EXPORT void SetQualifiersValue(const Standard_Integer          num,
                                          const StepShape_ValueQualifier& aqualifier);

  DEFINE_STANDARD_RTTIEXT(StepShape_MeasureQualification, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString)          theName;
  Handle(TCollection_HAsciiString)          theDescription;
  Handle(StepBasic_MeasureWithUnit)         theQualifiedMeasure;
  Handle(StepShape_HArray1OfValueQualifier) theQualifiers;
};

#endif // _StepShape_MeasureQualification_HeaderFile
