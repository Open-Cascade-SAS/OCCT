// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepRepr_ConfigurationEffectivity_HeaderFile
#define _StepRepr_ConfigurationEffectivity_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_ProductDefinitionEffectivity.hxx>
class StepRepr_ConfigurationDesign;
class TCollection_HAsciiString;
class StepBasic_ProductDefinitionRelationship;


class StepRepr_ConfigurationEffectivity;
DEFINE_STANDARD_HANDLE(StepRepr_ConfigurationEffectivity, StepBasic_ProductDefinitionEffectivity)

//! Representation of STEP entity ConfigurationEffectivity
class StepRepr_ConfigurationEffectivity : public StepBasic_ProductDefinitionEffectivity
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepRepr_ConfigurationEffectivity();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aEffectivity_Id, const Handle(StepBasic_ProductDefinitionRelationship)& aProductDefinitionEffectivity_Usage, const Handle(StepRepr_ConfigurationDesign)& aConfiguration);
  
  //! Returns field Configuration
  Standard_EXPORT Handle(StepRepr_ConfigurationDesign) Configuration() const;
  
  //! Set field Configuration
  Standard_EXPORT void SetConfiguration (const Handle(StepRepr_ConfigurationDesign)& Configuration);




  DEFINE_STANDARD_RTTIEXT(StepRepr_ConfigurationEffectivity,StepBasic_ProductDefinitionEffectivity)

protected:




private:


  Handle(StepRepr_ConfigurationDesign) theConfiguration;


};







#endif // _StepRepr_ConfigurationEffectivity_HeaderFile
