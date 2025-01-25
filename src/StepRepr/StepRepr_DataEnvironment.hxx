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

#ifndef _StepRepr_DataEnvironment_HeaderFile
#define _StepRepr_DataEnvironment_HeaderFile

#include <Standard.hxx>

#include <StepRepr_HArray1OfPropertyDefinitionRepresentation.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;

class StepRepr_DataEnvironment;
DEFINE_STANDARD_HANDLE(StepRepr_DataEnvironment, Standard_Transient)

//! Representation of STEP entity DataEnvironment
class StepRepr_DataEnvironment : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepRepr_DataEnvironment();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&                           aName,
    const Handle(TCollection_HAsciiString)&                           aDescription,
    const Handle(StepRepr_HArray1OfPropertyDefinitionRepresentation)& aElements);

  //! Returns field Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& Name);

  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const Handle(TCollection_HAsciiString)& Description);

  //! Returns field Elements
  Standard_EXPORT Handle(StepRepr_HArray1OfPropertyDefinitionRepresentation) Elements() const;

  //! Set field Elements
  Standard_EXPORT void SetElements(
    const Handle(StepRepr_HArray1OfPropertyDefinitionRepresentation)& Elements);

  DEFINE_STANDARD_RTTIEXT(StepRepr_DataEnvironment, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString)                           theName;
  Handle(TCollection_HAsciiString)                           theDescription;
  Handle(StepRepr_HArray1OfPropertyDefinitionRepresentation) theElements;
};

#endif // _StepRepr_DataEnvironment_HeaderFile
