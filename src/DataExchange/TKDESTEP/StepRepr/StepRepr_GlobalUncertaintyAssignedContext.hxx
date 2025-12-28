// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepRepr_GlobalUncertaintyAssignedContext_HeaderFile
#define _StepRepr_GlobalUncertaintyAssignedContext_HeaderFile

#include <Standard.hxx>

#include <StepBasic_UncertaintyMeasureWithUnit.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepBasic_UncertaintyMeasureWithUnit;

class StepRepr_GlobalUncertaintyAssignedContext : public StepRepr_RepresentationContext
{

public:
  //! Returns a GlobalUncertaintyAssignedContext
  Standard_EXPORT StepRepr_GlobalUncertaintyAssignedContext();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                      aContextIdentifier,
    const occ::handle<TCollection_HAsciiString>&                      aContextType,
    const occ::handle<NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>>& aUncertainty);

  Standard_EXPORT void SetUncertainty(
    const occ::handle<NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>>& aUncertainty);

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>> Uncertainty() const;

  Standard_EXPORT occ::handle<StepBasic_UncertaintyMeasureWithUnit> UncertaintyValue(
    const int num) const;

  Standard_EXPORT int NbUncertainty() const;

  DEFINE_STANDARD_RTTIEXT(StepRepr_GlobalUncertaintyAssignedContext, StepRepr_RepresentationContext)

private:
  occ::handle<NCollection_HArray1<occ::handle<StepBasic_UncertaintyMeasureWithUnit>>> uncertainty;
};

#endif // _StepRepr_GlobalUncertaintyAssignedContext_HeaderFile
