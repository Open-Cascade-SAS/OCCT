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

#ifndef _StepBasic_PersonAndOrganization_HeaderFile
#define _StepBasic_PersonAndOrganization_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepBasic_Person;
class StepBasic_Organization;

class StepBasic_PersonAndOrganization;
DEFINE_STANDARD_HANDLE(StepBasic_PersonAndOrganization, Standard_Transient)

class StepBasic_PersonAndOrganization : public Standard_Transient
{

public:
  //! Returns a PersonAndOrganization
  Standard_EXPORT StepBasic_PersonAndOrganization();

  Standard_EXPORT void Init(const Handle(StepBasic_Person)&       aThePerson,
                            const Handle(StepBasic_Organization)& aTheOrganization);

  Standard_EXPORT void SetThePerson(const Handle(StepBasic_Person)& aThePerson);

  Standard_EXPORT Handle(StepBasic_Person) ThePerson() const;

  Standard_EXPORT void SetTheOrganization(const Handle(StepBasic_Organization)& aTheOrganization);

  Standard_EXPORT Handle(StepBasic_Organization) TheOrganization() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_PersonAndOrganization, Standard_Transient)

protected:
private:
  Handle(StepBasic_Person)       thePerson;
  Handle(StepBasic_Organization) theOrganization;
};

#endif // _StepBasic_PersonAndOrganization_HeaderFile
