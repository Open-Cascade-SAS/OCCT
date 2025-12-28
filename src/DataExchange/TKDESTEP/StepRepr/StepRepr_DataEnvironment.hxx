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

#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;

//! Representation of STEP entity DataEnvironment
class StepRepr_DataEnvironment : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepRepr_DataEnvironment();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                           aName,
    const occ::handle<TCollection_HAsciiString>&                           aDescription,
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_PropertyDefinitionRepresentation>>>& aElements);

  //! Returns field Name
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& Name);

  //! Returns field Description
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const occ::handle<TCollection_HAsciiString>& Description);

  //! Returns field Elements
  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepRepr_PropertyDefinitionRepresentation>>> Elements() const;

  //! Set field Elements
  Standard_EXPORT void SetElements(
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_PropertyDefinitionRepresentation>>>& Elements);

  DEFINE_STANDARD_RTTIEXT(StepRepr_DataEnvironment, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString>                           theName;
  occ::handle<TCollection_HAsciiString>                           theDescription;
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_PropertyDefinitionRepresentation>>> theElements;
};

#endif // _StepRepr_DataEnvironment_HeaderFile
