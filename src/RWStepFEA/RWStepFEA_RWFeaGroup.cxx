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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepFEA_RWFeaGroup.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_FeaGroup.hxx>
#include <StepFEA_FeaModel.hxx>

//=======================================================================
//function : RWStepFEA_RWFeaGroup
//purpose  : 
//=======================================================================
RWStepFEA_RWFeaGroup::RWStepFEA_RWFeaGroup ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaGroup::ReadStep (const Handle(StepData_StepReaderData)& data,
                                     const Standard_Integer num,
                                     Handle(Interface_Check)& ach,
                                     const Handle(StepFEA_FeaGroup) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"fea_group") ) return;

  // Inherited fields of Group

  Handle(TCollection_HAsciiString) aGroup_Name;
  data->ReadString (num, 1, "group.name", ach, aGroup_Name);

  Handle(TCollection_HAsciiString) aGroup_Description;
  data->ReadString (num, 2, "group.description", ach, aGroup_Description);

  // Own fields of FeaGroup

  Handle(StepFEA_FeaModel) aModelRef;
  data->ReadEntity (num, 3, "model_ref", ach, STANDARD_TYPE(StepFEA_FeaModel), aModelRef);

  // Initialize entity
  ent->Init(aGroup_Name,
            aGroup_Description,
            aModelRef);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaGroup::WriteStep (StepData_StepWriter& SW,
                                      const Handle(StepFEA_FeaGroup) &ent) const
{

  // Inherited fields of Group

  SW.Send (ent->StepBasic_Group::Name());

  SW.Send (ent->StepBasic_Group::Description());

  // Own fields of FeaGroup

  SW.Send (ent->ModelRef());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaGroup::Share (const Handle(StepFEA_FeaGroup) &ent,
                                  Interface_EntityIterator& iter) const
{

  // Inherited fields of Group

  // Own fields of FeaGroup

  iter.AddItem (ent->ModelRef());
}
