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

#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ApplicationProtocolDefinition.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ApplicationProtocolDefinition, Standard_Transient)

StepBasic_ApplicationProtocolDefinition::StepBasic_ApplicationProtocolDefinition() {}

void StepBasic_ApplicationProtocolDefinition::Init(
  const occ::handle<TCollection_HAsciiString>&     aStatus,
  const occ::handle<TCollection_HAsciiString>&     aApplicationInterpretedModelSchemaName,
  const int                                        aApplicationProtocolYear,
  const occ::handle<StepBasic_ApplicationContext>& aApplication)
{
  // --- classe own fields ---
  status                                = aStatus;
  applicationInterpretedModelSchemaName = aApplicationInterpretedModelSchemaName;
  applicationProtocolYear               = aApplicationProtocolYear;
  application                           = aApplication;
}

void StepBasic_ApplicationProtocolDefinition::SetStatus(
  const occ::handle<TCollection_HAsciiString>& aStatus)
{
  status = aStatus;
}

occ::handle<TCollection_HAsciiString> StepBasic_ApplicationProtocolDefinition::Status() const
{
  return status;
}

void StepBasic_ApplicationProtocolDefinition::SetApplicationInterpretedModelSchemaName(
  const occ::handle<TCollection_HAsciiString>& aApplicationInterpretedModelSchemaName)
{
  applicationInterpretedModelSchemaName = aApplicationInterpretedModelSchemaName;
}

occ::handle<TCollection_HAsciiString> StepBasic_ApplicationProtocolDefinition::
  ApplicationInterpretedModelSchemaName() const
{
  return applicationInterpretedModelSchemaName;
}

void StepBasic_ApplicationProtocolDefinition::SetApplicationProtocolYear(
  const int aApplicationProtocolYear)
{
  applicationProtocolYear = aApplicationProtocolYear;
}

int StepBasic_ApplicationProtocolDefinition::ApplicationProtocolYear() const
{
  return applicationProtocolYear;
}

void StepBasic_ApplicationProtocolDefinition::SetApplication(
  const occ::handle<StepBasic_ApplicationContext>& aApplication)
{
  application = aApplication;
}

occ::handle<StepBasic_ApplicationContext> StepBasic_ApplicationProtocolDefinition::Application()
  const
{
  return application;
}
