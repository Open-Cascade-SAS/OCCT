// File:	RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion.ixx>

//=======================================================================
//function : RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion
//purpose  : 
//=======================================================================

RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion::RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                         const Standard_Integer num,
                                                                         Handle(Interface_Check)& ach,
                                                                         const Handle(StepFEA_FeaSecantCoefficientOfLinearThermalExpansion) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"fea_secant_coefficient_of_linear_thermal_expansion") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of FeaSecantCoefficientOfLinearThermalExpansion

  StepFEA_SymmetricTensor23d aFeaConstants;
  data->ReadEntity (num, 2, "fea_constants", ach, aFeaConstants);

  Standard_Real aReferenceTemperature;
  data->ReadReal (num, 3, "reference_temperature", ach, aReferenceTemperature);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aFeaConstants,
            aReferenceTemperature);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion::WriteStep (StepData_StepWriter& SW,
                                                                          const Handle(StepFEA_FeaSecantCoefficientOfLinearThermalExpansion) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of FeaSecantCoefficientOfLinearThermalExpansion

  SW.Send (ent->FeaConstants().Value());

  SW.Send (ent->ReferenceTemperature());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWFeaSecantCoefficientOfLinearThermalExpansion::Share (const Handle(StepFEA_FeaSecantCoefficientOfLinearThermalExpansion) &ent,
                                                                      Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of FeaSecantCoefficientOfLinearThermalExpansion

  iter.AddItem (ent->FeaConstants().Value());
}
