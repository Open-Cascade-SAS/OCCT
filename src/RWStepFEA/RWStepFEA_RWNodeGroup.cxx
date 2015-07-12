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
#include <RWStepFEA_RWNodeGroup.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_FeaModel.hxx>
#include <StepFEA_HArray1OfNodeRepresentation.hxx>
#include <StepFEA_NodeGroup.hxx>
#include <StepFEA_NodeRepresentation.hxx>

//=======================================================================
//function : RWStepFEA_RWNodeGroup
//purpose  : 
//=======================================================================
RWStepFEA_RWNodeGroup::RWStepFEA_RWNodeGroup ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWNodeGroup::ReadStep (const Handle(StepData_StepReaderData)& data,
                                      const Standard_Integer num,
                                      Handle(Interface_Check)& ach,
                                      const Handle(StepFEA_NodeGroup) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"node_group") ) return;

  // Inherited fields of Group

  Handle(TCollection_HAsciiString) aGroup_Name;
  data->ReadString (num, 1, "group.name", ach, aGroup_Name);

  Handle(TCollection_HAsciiString) aGroup_Description;
  data->ReadString (num, 2, "group.description", ach, aGroup_Description);

  // Inherited fields of FeaGroup

  Handle(StepFEA_FeaModel) aFeaGroup_ModelRef;
  data->ReadEntity (num, 3, "fea_group.model_ref", ach, STANDARD_TYPE(StepFEA_FeaModel), aFeaGroup_ModelRef);

  // Own fields of NodeGroup

  Handle(StepFEA_HArray1OfNodeRepresentation) aNodes;
  Standard_Integer sub4 = 0;
  if ( data->ReadSubList (num, 4, "nodes", ach, sub4) ) {
    Standard_Integer nb0 = data->NbParams(sub4);
    aNodes = new StepFEA_HArray1OfNodeRepresentation (1, nb0);
    Standard_Integer num2 = sub4;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepFEA_NodeRepresentation) anIt0;
      data->ReadEntity (num2, i0, "node_representation", ach, STANDARD_TYPE(StepFEA_NodeRepresentation), anIt0);
      aNodes->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aGroup_Name,
            aGroup_Description,
            aFeaGroup_ModelRef,
            aNodes);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWNodeGroup::WriteStep (StepData_StepWriter& SW,
                                       const Handle(StepFEA_NodeGroup) &ent) const
{

  // Inherited fields of Group

  SW.Send (ent->StepBasic_Group::Name());

  SW.Send (ent->StepBasic_Group::Description());

  // Inherited fields of FeaGroup

  SW.Send (ent->StepFEA_FeaGroup::ModelRef());

  // Own fields of NodeGroup

  SW.OpenSub();
  for (Standard_Integer i3=1; i3 <= ent->Nodes()->Length(); i3++ ) {
    Handle(StepFEA_NodeRepresentation) Var0 = ent->Nodes()->Value(i3);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWNodeGroup::Share (const Handle(StepFEA_NodeGroup) &ent,
                                   Interface_EntityIterator& iter) const
{

  // Inherited fields of Group

  // Inherited fields of FeaGroup

  iter.AddItem (ent->StepFEA_FeaGroup::ModelRef());

  // Own fields of NodeGroup

  for (Standard_Integer i2=1; i2 <= ent->Nodes()->Length(); i2++ ) {
    Handle(StepFEA_NodeRepresentation) Var0 = ent->Nodes()->Value(i2);
    iter.AddItem (Var0);
  }
}
