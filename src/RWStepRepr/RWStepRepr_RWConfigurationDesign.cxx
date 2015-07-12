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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWConfigurationDesign.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ConfigurationDesign.hxx>
#include <StepRepr_ConfigurationItem.hxx>

//=======================================================================
//function : RWStepRepr_RWConfigurationDesign
//purpose  : 
//=======================================================================
RWStepRepr_RWConfigurationDesign::RWStepRepr_RWConfigurationDesign ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWConfigurationDesign::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                 const Standard_Integer num,
                                                 Handle(Interface_Check)& ach,
                                                 const Handle(StepRepr_ConfigurationDesign) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"configuration_design") ) return;

  // Own fields of ConfigurationDesign

  Handle(StepRepr_ConfigurationItem) aConfiguration;
  data->ReadEntity (num, 1, "configuration", ach, STANDARD_TYPE(StepRepr_ConfigurationItem), aConfiguration);

  StepRepr_ConfigurationDesignItem aDesign;
  data->ReadEntity (num, 2, "design", ach, aDesign);

  // Initialize entity
  ent->Init(aConfiguration,
            aDesign);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWConfigurationDesign::WriteStep (StepData_StepWriter& SW,
                                                  const Handle(StepRepr_ConfigurationDesign) &ent) const
{

  // Own fields of ConfigurationDesign

  SW.Send (ent->Configuration());

  SW.Send (ent->Design().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWConfigurationDesign::Share (const Handle(StepRepr_ConfigurationDesign) &ent,
                                              Interface_EntityIterator& iter) const
{

  // Own fields of ConfigurationDesign

  iter.AddItem (ent->Configuration());

  iter.AddItem (ent->Design().Value());
}
