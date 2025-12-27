// Created on: 1999-11-26
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepBasic_ActionMethod.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ActionMethod, Standard_Transient)

//=================================================================================================

StepBasic_ActionMethod::StepBasic_ActionMethod()
{
  defDescription = false;
}

//=================================================================================================

void StepBasic_ActionMethod::Init(const occ::handle<TCollection_HAsciiString>& aName,
                                  const bool                  hasDescription,
                                  const occ::handle<TCollection_HAsciiString>& aDescription,
                                  const occ::handle<TCollection_HAsciiString>& aConsequence,
                                  const occ::handle<TCollection_HAsciiString>& aPurpose)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription)
  {
    theDescription = aDescription;
  }
  else
    theDescription.Nullify();

  theConsequence = aConsequence;

  thePurpose = aPurpose;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_ActionMethod::Name() const
{
  return theName;
}

//=================================================================================================

void StepBasic_ActionMethod::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  theName = aName;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_ActionMethod::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepBasic_ActionMethod::SetDescription(const occ::handle<TCollection_HAsciiString>& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

bool StepBasic_ActionMethod::HasDescription() const
{
  return defDescription;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_ActionMethod::Consequence() const
{
  return theConsequence;
}

//=================================================================================================

void StepBasic_ActionMethod::SetConsequence(const occ::handle<TCollection_HAsciiString>& aConsequence)
{
  theConsequence = aConsequence;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_ActionMethod::Purpose() const
{
  return thePurpose;
}

//=================================================================================================

void StepBasic_ActionMethod::SetPurpose(const occ::handle<TCollection_HAsciiString>& aPurpose)
{
  thePurpose = aPurpose;
}
