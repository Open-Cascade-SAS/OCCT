// Created on: 2001-04-24
// Created by: Atelier IED
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

#ifndef _StepShape_LimitsAndFits_HeaderFile
#define _StepShape_LimitsAndFits_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;

//! Added for Dimensional Tolerances
class StepShape_LimitsAndFits : public Standard_Transient
{

public:
  Standard_EXPORT StepShape_LimitsAndFits();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& form_variance,
                            const occ::handle<TCollection_HAsciiString>& zone_variance,
                            const occ::handle<TCollection_HAsciiString>& grade,
                            const occ::handle<TCollection_HAsciiString>& source);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> FormVariance() const;

  Standard_EXPORT void SetFormVariance(const occ::handle<TCollection_HAsciiString>& form_variance);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> ZoneVariance() const;

  Standard_EXPORT void SetZoneVariance(const occ::handle<TCollection_HAsciiString>& zone_variance);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Grade() const;

  Standard_EXPORT void SetGrade(const occ::handle<TCollection_HAsciiString>& grade);

  Standard_EXPORT occ::handle<TCollection_HAsciiString> Source() const;

  Standard_EXPORT void SetSource(const occ::handle<TCollection_HAsciiString>& source);

  DEFINE_STANDARD_RTTIEXT(StepShape_LimitsAndFits, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString> theFormVariance;
  occ::handle<TCollection_HAsciiString> theZoneVariance;
  occ::handle<TCollection_HAsciiString> theGrade;
  occ::handle<TCollection_HAsciiString> theSource;
};

#endif // _StepShape_LimitsAndFits_HeaderFile
