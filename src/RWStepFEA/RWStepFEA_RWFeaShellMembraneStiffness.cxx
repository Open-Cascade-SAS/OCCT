// File:	RWStepFEA_RWFeaShellMembraneStiffness.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFeaShellMembraneStiffness.ixx>

//=======================================================================
//function : RWStepFEA_RWFeaShellMembraneStiffness
//purpose  : 
//=======================================================================

RWStepFEA_RWFeaShellMembraneStiffness::RWStepFEA_RWFeaShellMembraneStiffness ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaShellMembraneStiffness::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                      const Standard_Integer num,
                                                      Handle(Interface_Check)& ach,
                                                      const Handle(StepFEA_FeaShellMembraneStiffness) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"fea_shell_membrane_stiffness") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of FeaShellMembraneStiffness

  StepFEA_SymmetricTensor42d aFeaConstants;
  data->ReadEntity (num, 2, "fea_constants", ach, aFeaConstants);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aFeaConstants);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaShellMembraneStiffness::WriteStep (StepData_StepWriter& SW,
                                                       const Handle(StepFEA_FeaShellMembraneStiffness) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of FeaShellMembraneStiffness

  SW.Send (ent->FeaConstants().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaShellMembraneStiffness::Share (const Handle(StepFEA_FeaShellMembraneStiffness) &ent,
                                                   Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of FeaShellMembraneStiffness

  iter.AddItem (ent->FeaConstants().Value());
}
