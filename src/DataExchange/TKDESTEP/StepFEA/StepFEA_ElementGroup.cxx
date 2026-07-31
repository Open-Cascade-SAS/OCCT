// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepFEA_ElementGroup.hxx>
#include <StepFEA_FeaModel.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_ElementGroup, StepFEA_FeaGroup)

//=================================================================================================

StepFEA_ElementGroup::StepFEA_ElementGroup() = default;

//=================================================================================================

void StepFEA_ElementGroup::Init(
  const occ::handle<TCollection_HAsciiString>& aGroup_Name,
  const occ::handle<TCollection_HAsciiString>& aGroup_Description,
  const occ::handle<StepFEA_FeaModel>&         aFeaGroup_ModelRef,
  const occ::handle<NCollection_HArray1<occ::handle<StepFEA_ElementRepresentation>>>& aElements)
{
  StepFEA_FeaGroup::Init(aGroup_Name, aGroup_Description, aFeaGroup_ModelRef);

  theElements = aElements;
}

//=================================================================================================

occ::handle<NCollection_HArray1<occ::handle<StepFEA_ElementRepresentation>>> StepFEA_ElementGroup::
  Elements() const
{
  return theElements;
}

//=================================================================================================

void StepFEA_ElementGroup::SetElements(
  const occ::handle<NCollection_HArray1<occ::handle<StepFEA_ElementRepresentation>>>& aElements)
{
  theElements = aElements;
}
