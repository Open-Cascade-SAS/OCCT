// File:	RWStepFEA_RWFeaLinearElasticity.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFeaLinearElasticity.ixx>

#include <StepFEA_SymmetricTensor43dMember.hxx>
#include <TColStd_HArray1OfReal.hxx>


//=======================================================================
//function : RWStepFEA_RWFeaLinearElasticity
//purpose  : 
//=======================================================================

RWStepFEA_RWFeaLinearElasticity::RWStepFEA_RWFeaLinearElasticity ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaLinearElasticity::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                const Standard_Integer num,
                                                Handle(Interface_Check)& ach,
                                                const Handle(StepFEA_FeaLinearElasticity) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"fea_linear_elasticity") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of FeaLinearElasticity

  StepFEA_SymmetricTensor43d aFeaConstants;
  data->ReadEntity (num, 2, "fea_constants", ach, aFeaConstants);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aFeaConstants);

}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaLinearElasticity::WriteStep (StepData_StepWriter& SW,
                                                 const Handle(StepFEA_FeaLinearElasticity) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of FeaLinearElasticity

  SW.Send (ent->FeaConstants().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaLinearElasticity::Share (const Handle(StepFEA_FeaLinearElasticity) &ent,
                                             Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of FeaLinearElasticity

  iter.AddItem (ent->FeaConstants().Value());
}
