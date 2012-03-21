// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <RWStepRepr_RWConfigurationItem.ixx>

//=======================================================================
//function : RWStepRepr_RWConfigurationItem
//purpose  : 
//=======================================================================

RWStepRepr_RWConfigurationItem::RWStepRepr_RWConfigurationItem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWConfigurationItem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                               const Standard_Integer num,
                                               Handle(Interface_Check)& ach,
                                               const Handle(StepRepr_ConfigurationItem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"configuration_item") ) return;

  // Own fields of ConfigurationItem

  Handle(TCollection_HAsciiString) aId;
  data->ReadString (num, 1, "id", ach, aId);

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 2, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean hasDescription = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "description", ach, aDescription);
  }
  else {
    hasDescription = Standard_False;
  }

  Handle(StepRepr_ProductConcept) aItemConcept;
  data->ReadEntity (num, 4, "item_concept", ach, STANDARD_TYPE(StepRepr_ProductConcept), aItemConcept);

  Handle(TCollection_HAsciiString) aPurpose;
  Standard_Boolean hasPurpose = Standard_True;
  if ( data->IsParamDefined (num,5) ) {
    data->ReadString (num, 5, "purpose", ach, aPurpose);
  }
  else {
    hasPurpose = Standard_False;
  }

  // Initialize entity
  ent->Init(aId,
            aName,
            hasDescription,
            aDescription,
            aItemConcept,
            hasPurpose,
            aPurpose);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWConfigurationItem::WriteStep (StepData_StepWriter& SW,
                                                const Handle(StepRepr_ConfigurationItem) &ent) const
{

  // Own fields of ConfigurationItem

  SW.Send (ent->Id());

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->ItemConcept());

  if ( ent->HasPurpose() ) {
    SW.Send (ent->Purpose());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWConfigurationItem::Share (const Handle(StepRepr_ConfigurationItem) &ent,
                                            Interface_EntityIterator& iter) const
{

  // Own fields of ConfigurationItem

  iter.AddItem (ent->ItemConcept());
}
