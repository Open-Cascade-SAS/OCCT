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
