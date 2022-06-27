// Created on: 1998-07-24
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _StepRepr_MaterialDesignation_HeaderFile
#define _StepRepr_MaterialDesignation_HeaderFile

#include <Standard.hxx>

#include <StepRepr_CharacterizedDefinition.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;


class StepRepr_MaterialDesignation;
DEFINE_STANDARD_HANDLE(StepRepr_MaterialDesignation, Standard_Transient)


class StepRepr_MaterialDesignation : public Standard_Transient
{

public:

  
  Standard_EXPORT StepRepr_MaterialDesignation();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const StepRepr_CharacterizedDefinition& aOfDefinition);
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  Standard_EXPORT void SetOfDefinition (const StepRepr_CharacterizedDefinition& aOfDefinition);
  
  Standard_EXPORT StepRepr_CharacterizedDefinition OfDefinition() const;




  DEFINE_STANDARD_RTTIEXT(StepRepr_MaterialDesignation,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) name;
  StepRepr_CharacterizedDefinition ofDefinition;


};







#endif // _StepRepr_MaterialDesignation_HeaderFile
