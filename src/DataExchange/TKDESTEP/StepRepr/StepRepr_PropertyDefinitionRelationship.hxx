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

#ifndef _StepRepr_PropertyDefinitionRelationship_HeaderFile
#define _StepRepr_PropertyDefinitionRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepRepr_PropertyDefinition;

class StepRepr_PropertyDefinitionRelationship;
DEFINE_STANDARD_HANDLE(StepRepr_PropertyDefinitionRelationship, Standard_Transient)

//! Representation of STEP entity PropertyDefinitionRelationship
class StepRepr_PropertyDefinitionRelationship : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepRepr_PropertyDefinitionRelationship();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&    aName,
                            const Handle(TCollection_HAsciiString)&    aDescription,
                            const Handle(StepRepr_PropertyDefinition)& aRelatingPropertyDefinition,
                            const Handle(StepRepr_PropertyDefinition)& aRelatedPropertyDefinition);

  //! Returns field Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& Name);

  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const Handle(TCollection_HAsciiString)& Description);

  //! Returns field RelatingPropertyDefinition
  Standard_EXPORT Handle(StepRepr_PropertyDefinition) RelatingPropertyDefinition() const;

  //! Set field RelatingPropertyDefinition
  Standard_EXPORT void SetRelatingPropertyDefinition(
    const Handle(StepRepr_PropertyDefinition)& RelatingPropertyDefinition);

  //! Returns field RelatedPropertyDefinition
  Standard_EXPORT Handle(StepRepr_PropertyDefinition) RelatedPropertyDefinition() const;

  //! Set field RelatedPropertyDefinition
  Standard_EXPORT void SetRelatedPropertyDefinition(
    const Handle(StepRepr_PropertyDefinition)& RelatedPropertyDefinition);

  DEFINE_STANDARD_RTTIEXT(StepRepr_PropertyDefinitionRelationship, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString)    theName;
  Handle(TCollection_HAsciiString)    theDescription;
  Handle(StepRepr_PropertyDefinition) theRelatingPropertyDefinition;
  Handle(StepRepr_PropertyDefinition) theRelatedPropertyDefinition;
};

#endif // _StepRepr_PropertyDefinitionRelationship_HeaderFile
