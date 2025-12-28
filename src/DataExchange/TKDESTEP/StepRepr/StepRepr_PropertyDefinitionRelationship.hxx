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

//! Representation of STEP entity PropertyDefinitionRelationship
class StepRepr_PropertyDefinitionRelationship : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepRepr_PropertyDefinitionRelationship();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&    aName,
    const occ::handle<TCollection_HAsciiString>&    aDescription,
    const occ::handle<StepRepr_PropertyDefinition>& aRelatingPropertyDefinition,
    const occ::handle<StepRepr_PropertyDefinition>& aRelatedPropertyDefinition);

  //! Returns field Name
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const occ::handle<TCollection_HAsciiString>& Name);

  //! Returns field Description
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const occ::handle<TCollection_HAsciiString>& Description);

  //! Returns field RelatingPropertyDefinition
  Standard_EXPORT occ::handle<StepRepr_PropertyDefinition> RelatingPropertyDefinition() const;

  //! Set field RelatingPropertyDefinition
  Standard_EXPORT void SetRelatingPropertyDefinition(
    const occ::handle<StepRepr_PropertyDefinition>& RelatingPropertyDefinition);

  //! Returns field RelatedPropertyDefinition
  Standard_EXPORT occ::handle<StepRepr_PropertyDefinition> RelatedPropertyDefinition() const;

  //! Set field RelatedPropertyDefinition
  Standard_EXPORT void SetRelatedPropertyDefinition(
    const occ::handle<StepRepr_PropertyDefinition>& RelatedPropertyDefinition);

  DEFINE_STANDARD_RTTIEXT(StepRepr_PropertyDefinitionRelationship, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString>    theName;
  occ::handle<TCollection_HAsciiString>    theDescription;
  occ::handle<StepRepr_PropertyDefinition> theRelatingPropertyDefinition;
  occ::handle<StepRepr_PropertyDefinition> theRelatedPropertyDefinition;
};

#endif // _StepRepr_PropertyDefinitionRelationship_HeaderFile
