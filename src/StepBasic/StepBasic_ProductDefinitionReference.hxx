// Created on: 2016-03-30
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _StepBasic_ProductDefinitionReference_HeaderFile
#define _StepBasic_ProductDefinitionReference_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;

class StepBasic_ExternalSource;

class StepBasic_ProductDefinitionReference;
DEFINE_STANDARD_HANDLE(StepBasic_ProductDefinitionReference, Standard_Transient)

//! Representation of STEP entity Product_Definition_Reference
class StepBasic_ProductDefinitionReference : public Standard_Transient
{
public:
  //! Empty constructor
  Standard_EXPORT StepBasic_ProductDefinitionReference();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(StepBasic_ExternalSource)& theSource,
                            const Handle(TCollection_HAsciiString)& theProductId,
                            const Handle(TCollection_HAsciiString)& theProductDefinitionFormationId,
                            const Handle(TCollection_HAsciiString)& theProductDefinitionId,
                            const Handle(TCollection_HAsciiString)& theIdOwningOrganizationName);

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(StepBasic_ExternalSource)& theSource,
                            const Handle(TCollection_HAsciiString)& theProductId,
                            const Handle(TCollection_HAsciiString)& theProductDefinitionFormationId,
                            const Handle(TCollection_HAsciiString)& theProductDefinitionId);

  //! Returns field Source
  inline Handle(StepBasic_ExternalSource) Source() const { return mySource; }

  //! Set field Source
  inline void SetSource(const Handle(StepBasic_ExternalSource)& theSource) { mySource = theSource; }

  //! Returns field ProductId
  inline Handle(TCollection_HAsciiString) ProductId() const { return myProductId; }

  //! Set field ProductId
  inline void SetProductId(const Handle(TCollection_HAsciiString)& theProductId)
  {
    myProductId = theProductId;
  }

  //! Returns field ProductDefinitionFormationId
  inline Handle(TCollection_HAsciiString) ProductDefinitionFormationId() const
  {
    return myProductDefinitionFormationId;
  }

  //! Set field ProductDefinitionFormationId
  inline void SetProductDefinitionFormationId(
    const Handle(TCollection_HAsciiString)& theProductDefinitionFormationId)
  {
    myProductDefinitionFormationId = theProductDefinitionFormationId;
  }

  //! Returns field ProductDefinitionId
  inline Handle(TCollection_HAsciiString) ProductDefinitionId() const
  {
    return myProductDefinitionId;
  }

  //! Set field ProductDefinitionId
  inline void SetProductDefinitionId(const Handle(TCollection_HAsciiString)& theProductDefinitionId)
  {
    myProductDefinitionId = theProductDefinitionId;
  }

  //! Returns field IdOwningOrganizationName
  inline Handle(TCollection_HAsciiString) IdOwningOrganizationName() const
  {
    return myIdOwningOrganizationName;
  }

  //! Set field IdOwningOrganizationName
  inline void SetIdOwningOrganizationName(
    const Handle(TCollection_HAsciiString)& theIdOwningOrganizationName)
  {
    myIdOwningOrganizationName  = theIdOwningOrganizationName;
    hasIdOwningOrganizationName = (!theIdOwningOrganizationName.IsNull());
  }

  //! Returns true if IdOwningOrganizationName exists
  inline Standard_Boolean HasIdOwningOrganizationName() const
  {
    return hasIdOwningOrganizationName;
  }

  DEFINE_STANDARD_RTTIEXT(StepBasic_ProductDefinitionReference, Standard_Transient)

private:
  Handle(StepBasic_ExternalSource) mySource;
  Handle(TCollection_HAsciiString) myProductId;
  Handle(TCollection_HAsciiString) myProductDefinitionFormationId;
  Handle(TCollection_HAsciiString) myProductDefinitionId;
  Handle(TCollection_HAsciiString) myIdOwningOrganizationName;
  Standard_Boolean                 hasIdOwningOrganizationName;
};

#endif // _StepBasic_ProductDefinitionReference_HeaderFile
