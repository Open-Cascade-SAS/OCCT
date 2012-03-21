// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <RWStepAP214_RWExternallyDefinedGeneralProperty.ixx>
#include <StepBasic_ExternallyDefinedItem.hxx>

//=======================================================================
//function : RWStepAP214_RWExternallyDefinedGeneralProperty
//purpose  : 
//=======================================================================

RWStepAP214_RWExternallyDefinedGeneralProperty::RWStepAP214_RWExternallyDefinedGeneralProperty ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepAP214_RWExternallyDefinedGeneralProperty::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                               const Standard_Integer num,
                                                               Handle(Interface_Check)& ach,
                                                               const Handle(StepAP214_ExternallyDefinedGeneralProperty) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"externally_defined_general_property") ) return;

  // Inherited fields of GeneralProperty

  Handle(TCollection_HAsciiString) aGeneralProperty_Id;
  data->ReadString (num, 1, "general_property.id", ach, aGeneralProperty_Id);

  Handle(TCollection_HAsciiString) aGeneralProperty_Name;
  data->ReadString (num, 2, "general_property.name", ach, aGeneralProperty_Name);

  Handle(TCollection_HAsciiString) aGeneralProperty_Description;
  Standard_Boolean hasGeneralProperty_Description = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "general_property.description", ach, aGeneralProperty_Description);
  }
  else {
    hasGeneralProperty_Description = Standard_False;
  }

  // Inherited fields of ExternallyDefinedItem

  StepBasic_SourceItem aExternallyDefinedItem_ItemId;
  data->ReadEntity (num, 4, "externally_defined_item.item_id", ach, aExternallyDefinedItem_ItemId);

  Handle(StepBasic_ExternalSource) aExternallyDefinedItem_Source;
  data->ReadEntity (num, 5, "externally_defined_item.source", ach, STANDARD_TYPE(StepBasic_ExternalSource), aExternallyDefinedItem_Source);

  // Initialize entity
  ent->Init(aGeneralProperty_Id,
            aGeneralProperty_Name,
            hasGeneralProperty_Description,
            aGeneralProperty_Description,
            aExternallyDefinedItem_ItemId,
            aExternallyDefinedItem_Source);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepAP214_RWExternallyDefinedGeneralProperty::WriteStep (StepData_StepWriter& SW,
                                                                const Handle(StepAP214_ExternallyDefinedGeneralProperty) &ent) const
{

  // Inherited fields of GeneralProperty

  SW.Send (ent->StepBasic_GeneralProperty::Id());

  SW.Send (ent->StepBasic_GeneralProperty::Name());

  if ( ent->StepBasic_GeneralProperty::HasDescription() ) {
    SW.Send (ent->StepBasic_GeneralProperty::Description());
  }
  else SW.SendUndef();

  // Inherited fields of ExternallyDefinedItem

  SW.Send (ent->ExternallyDefinedItem()->ItemId().Value());

  SW.Send (ent->ExternallyDefinedItem()->Source());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepAP214_RWExternallyDefinedGeneralProperty::Share (const Handle(StepAP214_ExternallyDefinedGeneralProperty) &ent,
                                                            Interface_EntityIterator& iter) const
{

  // Inherited fields of GeneralProperty

  // Inherited fields of ExternallyDefinedItem

  iter.AddItem (ent->ExternallyDefinedItem()->ItemId().Value());

  iter.AddItem (ent->ExternallyDefinedItem()->Source());
}
