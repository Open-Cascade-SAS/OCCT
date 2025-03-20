// Created on: 2002-12-15
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

#ifndef _StepBasic_ProductDefinitionFormationRelationship_HeaderFile
#define _StepBasic_ProductDefinitionFormationRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepBasic_ProductDefinitionFormation;

class StepBasic_ProductDefinitionFormationRelationship;
DEFINE_STANDARD_HANDLE(StepBasic_ProductDefinitionFormationRelationship, Standard_Transient)

//! Representation of STEP entity ProductDefinitionFormationRelationship
class StepBasic_ProductDefinitionFormationRelationship : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepBasic_ProductDefinitionFormationRelationship();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&             aId,
    const Handle(TCollection_HAsciiString)&             aName,
    const Handle(TCollection_HAsciiString)&             aDescription,
    const Handle(StepBasic_ProductDefinitionFormation)& aRelatingProductDefinitionFormation,
    const Handle(StepBasic_ProductDefinitionFormation)& aRelatedProductDefinitionFormation);

  //! Returns field Id
  Standard_EXPORT Handle(TCollection_HAsciiString) Id() const;

  //! Set field Id
  Standard_EXPORT void SetId(const Handle(TCollection_HAsciiString)& Id);

  //! Returns field Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& Name);

  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const Handle(TCollection_HAsciiString)& Description);

  //! Returns field RelatingProductDefinitionFormation
  Standard_EXPORT Handle(StepBasic_ProductDefinitionFormation) RelatingProductDefinitionFormation()
    const;

  //! Set field RelatingProductDefinitionFormation
  Standard_EXPORT void SetRelatingProductDefinitionFormation(
    const Handle(StepBasic_ProductDefinitionFormation)& RelatingProductDefinitionFormation);

  //! Returns field RelatedProductDefinitionFormation
  Standard_EXPORT Handle(StepBasic_ProductDefinitionFormation) RelatedProductDefinitionFormation()
    const;

  //! Set field RelatedProductDefinitionFormation
  Standard_EXPORT void SetRelatedProductDefinitionFormation(
    const Handle(StepBasic_ProductDefinitionFormation)& RelatedProductDefinitionFormation);

  DEFINE_STANDARD_RTTIEXT(StepBasic_ProductDefinitionFormationRelationship, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString)             theId;
  Handle(TCollection_HAsciiString)             theName;
  Handle(TCollection_HAsciiString)             theDescription;
  Handle(StepBasic_ProductDefinitionFormation) theRelatingProductDefinitionFormation;
  Handle(StepBasic_ProductDefinitionFormation) theRelatedProductDefinitionFormation;
};

#endif // _StepBasic_ProductDefinitionFormationRelationship_HeaderFile
