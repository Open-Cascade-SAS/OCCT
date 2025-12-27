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

#include <StepBasic_Certification.hxx>
#include <StepBasic_CertificationType.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_Certification, Standard_Transient)

//=================================================================================================

StepBasic_Certification::StepBasic_Certification() {}

//=================================================================================================

void StepBasic_Certification::Init(const occ::handle<TCollection_HAsciiString>&    aName,
                                   const occ::handle<TCollection_HAsciiString>&    aPurpose,
                                   const occ::handle<StepBasic_CertificationType>& aKind)
{

  theName = aName;

  thePurpose = aPurpose;

  theKind = aKind;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_Certification::Name() const
{
  return theName;
}

//=================================================================================================

void StepBasic_Certification::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  theName = aName;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_Certification::Purpose() const
{
  return thePurpose;
}

//=================================================================================================

void StepBasic_Certification::SetPurpose(const occ::handle<TCollection_HAsciiString>& aPurpose)
{
  thePurpose = aPurpose;
}

//=================================================================================================

occ::handle<StepBasic_CertificationType> StepBasic_Certification::Kind() const
{
  return theKind;
}

//=================================================================================================

void StepBasic_Certification::SetKind(const occ::handle<StepBasic_CertificationType>& aKind)
{
  theKind = aKind;
}
