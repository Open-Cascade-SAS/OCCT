// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <RWStepFEA_RWFeaGroup.ixx>

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
