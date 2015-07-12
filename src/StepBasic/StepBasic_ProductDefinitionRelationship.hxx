// Created on: 2000-07-03
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepBasic_ProductDefinitionRelationship_HeaderFile
#define _StepBasic_ProductDefinitionRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <MMgt_TShared.hxx>
class TCollection_HAsciiString;
class StepBasic_ProductDefinition;


class StepBasic_ProductDefinitionRelationship;
DEFINE_STANDARD_HANDLE(StepBasic_ProductDefinitionRelationship, MMgt_TShared)

//! Representation of STEP entity ProductDefinitionRelationship
class StepBasic_ProductDefinitionRelationship : public MMgt_TShared
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepBasic_ProductDefinitionRelationship();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aId, const Handle(TCollection_HAsciiString)& aName, const Standard_Boolean hasDescription, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepBasic_ProductDefinition)& aRelatingProductDefinition, const Handle(StepBasic_ProductDefinition)& aRelatedProductDefinition);
  
  //! Returns field Id
  Standard_EXPORT Handle(TCollection_HAsciiString) Id() const;
  
  //! Set field Id
  Standard_EXPORT void SetId (const Handle(TCollection_HAsciiString)& Id);
  
  //! Returns field Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  //! Set field Name
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& Name);
  
  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;
  
  //! Set field Description
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& Description);
  
  //! Returns True if optional field Description is defined
  Standard_EXPORT Standard_Boolean HasDescription() const;
  
  //! Returns field RelatingProductDefinition
  Standard_EXPORT Handle(StepBasic_ProductDefinition) RelatingProductDefinition() const;
  
  //! Set field RelatingProductDefinition
  Standard_EXPORT void SetRelatingProductDefinition (const Handle(StepBasic_ProductDefinition)& RelatingProductDefinition);
  
  //! Returns field RelatedProductDefinition
  Standard_EXPORT Handle(StepBasic_ProductDefinition) RelatedProductDefinition() const;
  
  //! Set field RelatedProductDefinition
  Standard_EXPORT void SetRelatedProductDefinition (const Handle(StepBasic_ProductDefinition)& RelatedProductDefinition);




  DEFINE_STANDARD_RTTI(StepBasic_ProductDefinitionRelationship,MMgt_TShared)

protected:




private:


  Handle(TCollection_HAsciiString) theId;
  Handle(TCollection_HAsciiString) theName;
  Handle(TCollection_HAsciiString) theDescription;
  Handle(StepBasic_ProductDefinition) theRelatingProductDefinition;
  Handle(StepBasic_ProductDefinition) theRelatedProductDefinition;
  Standard_Boolean defDescription;


};







#endif // _StepBasic_ProductDefinitionRelationship_HeaderFile
