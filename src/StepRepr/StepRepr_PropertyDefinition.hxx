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

#ifndef _StepRepr_PropertyDefinition_HeaderFile
#define _StepRepr_PropertyDefinition_HeaderFile

#include <Standard.hxx>

#include <StepRepr_CharacterizedDefinition.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;


class StepRepr_PropertyDefinition;
DEFINE_STANDARD_HANDLE(StepRepr_PropertyDefinition, Standard_Transient)

//! Representation of STEP entity PropertyDefinition
class StepRepr_PropertyDefinition : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepRepr_PropertyDefinition();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Standard_Boolean hasDescription, const Handle(TCollection_HAsciiString)& aDescription, const StepRepr_CharacterizedDefinition& aDefinition);
  
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
  
  //! Returns field Definition
  Standard_EXPORT StepRepr_CharacterizedDefinition Definition() const;
  
  //! Set field Definition
  Standard_EXPORT void SetDefinition (const StepRepr_CharacterizedDefinition& Definition);




  DEFINE_STANDARD_RTTIEXT(StepRepr_PropertyDefinition,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) theName;
  Handle(TCollection_HAsciiString) theDescription;
  StepRepr_CharacterizedDefinition theDefinition;
  Standard_Boolean defDescription;


};







#endif // _StepRepr_PropertyDefinition_HeaderFile
