// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Standard_Transient.hxx>
#include <StepBasic_SourceItem.hxx>
#include <StepData_SelectNamed.hxx>
#include <TCollection_HAsciiString.hxx>

//=================================================================================================

StepBasic_SourceItem::StepBasic_SourceItem() {}

//=================================================================================================

int StepBasic_SourceItem::CaseNum(const occ::handle<Standard_Transient>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepData_SelectNamed)))
    return 1;
  return 0;
}

//=================================================================================================

occ::handle<StepData_SelectMember> StepBasic_SourceItem::NewMember() const
{
  occ::handle<StepData_SelectNamed> member = new StepData_SelectNamed;
  return member;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_SourceItem::Identifier() const
{
  return occ::down_cast<TCollection_HAsciiString>(Value());
}
