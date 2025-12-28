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

#include <StepBasic_VersionedActionRequest.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_VersionedActionRequest, Standard_Transient)

//=================================================================================================

StepBasic_VersionedActionRequest::StepBasic_VersionedActionRequest()
{
  defDescription = false;
}

//=================================================================================================

void StepBasic_VersionedActionRequest::Init(const occ::handle<TCollection_HAsciiString>& aId,
                                            const occ::handle<TCollection_HAsciiString>& aVersion,
                                            const occ::handle<TCollection_HAsciiString>& aPurpose,
                                            const bool                  hasDescription,
                                            const occ::handle<TCollection_HAsciiString>& aDescription)
{

  theId = aId;

  theVersion = aVersion;

  thePurpose = aPurpose;

  defDescription = hasDescription;
  if (defDescription)
  {
    theDescription = aDescription;
  }
  else
    theDescription.Nullify();
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_VersionedActionRequest::Id() const
{
  return theId;
}

//=================================================================================================

void StepBasic_VersionedActionRequest::SetId(const occ::handle<TCollection_HAsciiString>& aId)
{
  theId = aId;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_VersionedActionRequest::Version() const
{
  return theVersion;
}

//=================================================================================================

void StepBasic_VersionedActionRequest::SetVersion(const occ::handle<TCollection_HAsciiString>& aVersion)
{
  theVersion = aVersion;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_VersionedActionRequest::Purpose() const
{
  return thePurpose;
}

//=================================================================================================

void StepBasic_VersionedActionRequest::SetPurpose(const occ::handle<TCollection_HAsciiString>& aPurpose)
{
  thePurpose = aPurpose;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_VersionedActionRequest::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepBasic_VersionedActionRequest::SetDescription(
  const occ::handle<TCollection_HAsciiString>& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

bool StepBasic_VersionedActionRequest::HasDescription() const
{
  return defDescription;
}
