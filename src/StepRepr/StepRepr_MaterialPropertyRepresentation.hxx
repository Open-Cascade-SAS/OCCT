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

#ifndef _StepRepr_MaterialPropertyRepresentation_HeaderFile
#define _StepRepr_MaterialPropertyRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_PropertyDefinitionRepresentation.hxx>
class StepRepr_DataEnvironment;
class StepRepr_RepresentedDefinition;
class StepRepr_Representation;


class StepRepr_MaterialPropertyRepresentation;
DEFINE_STANDARD_HANDLE(StepRepr_MaterialPropertyRepresentation, StepRepr_PropertyDefinitionRepresentation)

//! Representation of STEP entity MaterialPropertyRepresentation
class StepRepr_MaterialPropertyRepresentation : public StepRepr_PropertyDefinitionRepresentation
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepRepr_MaterialPropertyRepresentation();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const StepRepr_RepresentedDefinition& aPropertyDefinitionRepresentation_Definition, const Handle(StepRepr_Representation)& aPropertyDefinitionRepresentation_UsedRepresentation, const Handle(StepRepr_DataEnvironment)& aDependentEnvironment);
  
  //! Returns field DependentEnvironment
  Standard_EXPORT Handle(StepRepr_DataEnvironment) DependentEnvironment() const;
  
  //! Set field DependentEnvironment
  Standard_EXPORT void SetDependentEnvironment (const Handle(StepRepr_DataEnvironment)& DependentEnvironment);




  DEFINE_STANDARD_RTTIEXT(StepRepr_MaterialPropertyRepresentation,StepRepr_PropertyDefinitionRepresentation)

protected:




private:


  Handle(StepRepr_DataEnvironment) theDependentEnvironment;


};







#endif // _StepRepr_MaterialPropertyRepresentation_HeaderFile
