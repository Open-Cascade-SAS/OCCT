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

#include <RWStepFEA_RWElementGroup.ixx>
#include <StepFEA_HArray1OfElementRepresentation.hxx>
#include <StepFEA_ElementRepresentation.hxx>

//=======================================================================
//function : RWStepFEA_RWElementGroup
//purpose  : 
//=======================================================================

RWStepFEA_RWElementGroup::RWStepFEA_RWElementGroup ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWElementGroup::ReadStep (const Handle(StepData_StepReaderData)& data,
                                         const Standard_Integer num,
                                         Handle(Interface_Check)& ach,
                                         const Handle(StepFEA_ElementGroup) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"element_group") ) return;

  // Inherited fields of Group

  Handle(TCollection_HAsciiString) aGroup_Name;
  data->ReadString (num, 1, "group.name", ach, aGroup_Name);

  Handle(TCollection_HAsciiString) aGroup_Description;
  data->ReadString (num, 2, "group.description", ach, aGroup_Description);

  // Inherited fields of FeaGroup

  Handle(StepFEA_FeaModel) aFeaGroup_ModelRef;
  data->ReadEntity (num, 3, "fea_group.model_ref", ach, STANDARD_TYPE(StepFEA_FeaModel), aFeaGroup_ModelRef);

  // Own fields of ElementGroup

  Handle(StepFEA_HArray1OfElementRepresentation) aElements;
  Standard_Integer sub4 = 0;
  if ( data->ReadSubList (num, 4, "elements", ach, sub4) ) {
    Standard_Integer nb0 = data->NbParams(sub4);
    aElements = new StepFEA_HArray1OfElementRepresentation (1, nb0);
    Standard_Integer num2 = sub4;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepFEA_ElementRepresentation) anIt0;
      data->ReadEntity (num2, i0, "element_representation", ach, STANDARD_TYPE(StepFEA_ElementRepresentation), anIt0);
      aElements->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aGroup_Name,
            aGroup_Description,
            aFeaGroup_ModelRef,
            aElements);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWElementGroup::WriteStep (StepData_StepWriter& SW,
                                          const Handle(StepFEA_ElementGroup) &ent) const
{

  // Inherited fields of Group

  SW.Send (ent->StepBasic_Group::Name());

  SW.Send (ent->StepBasic_Group::Description());

  // Inherited fields of FeaGroup

  SW.Send (ent->StepFEA_FeaGroup::ModelRef());

  // Own fields of ElementGroup

  SW.OpenSub();
  for (Standard_Integer i3=1; i3 <= ent->Elements()->Length(); i3++ ) {
    Handle(StepFEA_ElementRepresentation) Var0 = ent->Elements()->Value(i3);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWElementGroup::Share (const Handle(StepFEA_ElementGroup) &ent,
                                      Interface_EntityIterator& iter) const
{

  // Inherited fields of Group

  // Inherited fields of FeaGroup

  iter.AddItem (ent->StepFEA_FeaGroup::ModelRef());

  // Own fields of ElementGroup

  for (Standard_Integer i2=1; i2 <= ent->Elements()->Length(); i2++ ) {
    Handle(StepFEA_ElementRepresentation) Var0 = ent->Elements()->Value(i2);
    iter.AddItem (Var0);
  }
}
