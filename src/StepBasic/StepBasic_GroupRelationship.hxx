// Created on: 2000-05-10
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

#ifndef _StepBasic_GroupRelationship_HeaderFile
#define _StepBasic_GroupRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepBasic_Group;


class StepBasic_GroupRelationship;
DEFINE_STANDARD_HANDLE(StepBasic_GroupRelationship, Standard_Transient)

//! Representation of STEP entity GroupRelationship
class StepBasic_GroupRelationship : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepBasic_GroupRelationship();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Standard_Boolean hasDescription, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepBasic_Group)& aRelatingGroup, const Handle(StepBasic_Group)& aRelatedGroup);
  
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
  
  //! Returns field RelatingGroup
  Standard_EXPORT Handle(StepBasic_Group) RelatingGroup() const;
  
  //! Set field RelatingGroup
  Standard_EXPORT void SetRelatingGroup (const Handle(StepBasic_Group)& RelatingGroup);
  
  //! Returns field RelatedGroup
  Standard_EXPORT Handle(StepBasic_Group) RelatedGroup() const;
  
  //! Set field RelatedGroup
  Standard_EXPORT void SetRelatedGroup (const Handle(StepBasic_Group)& RelatedGroup);




  DEFINE_STANDARD_RTTIEXT(StepBasic_GroupRelationship,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) theName;
  Handle(TCollection_HAsciiString) theDescription;
  Handle(StepBasic_Group) theRelatingGroup;
  Handle(StepBasic_Group) theRelatedGroup;
  Standard_Boolean defDescription;


};







#endif // _StepBasic_GroupRelationship_HeaderFile
