// File:	RWStepFEA_RWFeaGroup.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
