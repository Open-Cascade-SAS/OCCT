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

#include <StepBasic_CertificationType.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_CertificationType, Standard_Transient)

//=================================================================================================

StepBasic_CertificationType::StepBasic_CertificationType() {}

//=================================================================================================

void StepBasic_CertificationType::Init(const Handle(TCollection_HAsciiString)& aDescription)
{

  theDescription = aDescription;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepBasic_CertificationType::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepBasic_CertificationType::SetDescription(
  const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}
