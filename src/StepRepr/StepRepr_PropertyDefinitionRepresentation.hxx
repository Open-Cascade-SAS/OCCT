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

#ifndef _StepRepr_PropertyDefinitionRepresentation_HeaderFile
#define _StepRepr_PropertyDefinitionRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_RepresentedDefinition.hxx>
#include <Standard_Transient.hxx>
class StepRepr_Representation;
class StepRepr_RepresentedDefinition;


class StepRepr_PropertyDefinitionRepresentation;
DEFINE_STANDARD_HANDLE(StepRepr_PropertyDefinitionRepresentation, Standard_Transient)

//! Representation of STEP entity PropertyDefinitionRepresentation
class StepRepr_PropertyDefinitionRepresentation : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepRepr_PropertyDefinitionRepresentation();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const StepRepr_RepresentedDefinition& aDefinition, const Handle(StepRepr_Representation)& aUsedRepresentation);
  
  //! Returns field Definition
  Standard_EXPORT StepRepr_RepresentedDefinition Definition() const;
  
  //! Set field Definition
  Standard_EXPORT void SetDefinition (const StepRepr_RepresentedDefinition& Definition);
  
  //! Returns field UsedRepresentation
  Standard_EXPORT Handle(StepRepr_Representation) UsedRepresentation() const;
  
  //! Set field UsedRepresentation
  Standard_EXPORT void SetUsedRepresentation (const Handle(StepRepr_Representation)& UsedRepresentation);




  DEFINE_STANDARD_RTTIEXT(StepRepr_PropertyDefinitionRepresentation,Standard_Transient)

protected:




private:


  StepRepr_RepresentedDefinition theDefinition;
  Handle(StepRepr_Representation) theUsedRepresentation;


};







#endif // _StepRepr_PropertyDefinitionRepresentation_HeaderFile
