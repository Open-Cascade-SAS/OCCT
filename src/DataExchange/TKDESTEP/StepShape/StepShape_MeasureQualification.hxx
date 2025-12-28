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

#include <StepShape_ValueQualifier.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepShape_ValueQualifier;

//! Added for Dimensional Tolerances
class StepShape_MeasureQualification : public Standard_Transient
{

public:
  Standard_EXPORT StepShape_MeasureQualification();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                      name,
    const occ::handle<TCollection_HAsciiString>&                      description,
    const occ::handle<Standard_Transient>&                            qualified_measure,
    const occ::handle<NCollection_HArray1<StepShape_ValueQualifier>>& qualifiers);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& name);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Description() const;

  Standard_EXPORT void SetDescription(const occ::handle<TCollection_HAsciiString>& description);

  Standard_EXPORT occ::handle<Standard_Transient> QualifiedMeasure() const;

  Standard_EXPORT void SetQualifiedMeasure(
    const occ::handle<Standard_Transient>& qualified_measure);

  Standard_EXPORT occ::handle<NCollection_HArray1<StepShape_ValueQualifier>> Qualifiers() const;

  Standard_EXPORT int NbQualifiers() const;

  Standard_EXPORT void SetQualifiers(
    const occ::handle<NCollection_HArray1<StepShape_ValueQualifier>>& qualifiers);

  Standard_EXPORT StepShape_ValueQualifier QualifiersValue(const int num) const;

  Standard_EXPORT void SetQualifiersValue(const int                       num,
                                          const StepShape_ValueQualifier& aqualifier);

  DEFINE_STANDARD_RTTIEXT(StepShape_MeasureQualification, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString>                      theName;
  occ::handle<TCollection_HAsciiString>                      theDescription;
  occ::handle<Standard_Transient>                            theQualifiedMeasure;
  occ::handle<NCollection_HArray1<StepShape_ValueQualifier>> theQualifiers;
};

#endif // _StepShape_MeasureQualification_HeaderFile
