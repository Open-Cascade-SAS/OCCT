// File:	RWStepFEA_RWFeaAreaDensity.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFeaAreaDensity.ixx>

//=======================================================================
//function : RWStepFEA_RWFeaAreaDensity
//purpose  : 
//=======================================================================

RWStepFEA_RWFeaAreaDensity::RWStepFEA_RWFeaAreaDensity ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaAreaDensity::ReadStep (const Handle(StepData_StepReaderData)& data,
                                           const Standard_Integer num,
                                           Handle(Interface_Check)& ach,
                                           const Handle(StepFEA_FeaAreaDensity) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"fea_area_density") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of FeaAreaDensity

  Standard_Real aFeaConstant;
  data->ReadReal (num, 2, "fea_constant", ach, aFeaConstant);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aFeaConstant);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaAreaDensity::WriteStep (StepData_StepWriter& SW,
                                            const Handle(StepFEA_FeaAreaDensity) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of FeaAreaDensity

  SW.Send (ent->FeaConstant());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaAreaDensity::Share (const Handle(StepFEA_FeaAreaDensity) &ent,
                                        Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of FeaAreaDensity
}
