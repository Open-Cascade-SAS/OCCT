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

#include <RWStepVisual_RWExternallyDefinedCurveFont.ixx>

//=======================================================================
//function : RWStepVisual_RWExternallyDefinedCurveFont
//purpose  : 
//=======================================================================

RWStepVisual_RWExternallyDefinedCurveFont::RWStepVisual_RWExternallyDefinedCurveFont ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWExternallyDefinedCurveFont::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                          const Standard_Integer num,
                                                          Handle(Interface_Check)& ach,
                                                          const Handle(StepVisual_ExternallyDefinedCurveFont) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"externally_defined_curve_font") ) return;

  // Inherited fields of ExternallyDefinedItem

  StepBasic_SourceItem aExternallyDefinedItem_ItemId;
  data->ReadEntity (num, 1, "externally_defined_item.item_id", ach, aExternallyDefinedItem_ItemId);

  Handle(StepBasic_ExternalSource) aExternallyDefinedItem_Source;
  data->ReadEntity (num, 2, "externally_defined_item.source", ach, STANDARD_TYPE(StepBasic_ExternalSource), aExternallyDefinedItem_Source);

  // Initialize entity
  ent->Init(aExternallyDefinedItem_ItemId,
            aExternallyDefinedItem_Source);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWExternallyDefinedCurveFont::WriteStep (StepData_StepWriter& SW,
                                                           const Handle(StepVisual_ExternallyDefinedCurveFont) &ent) const
{

  // Inherited fields of ExternallyDefinedItem

  SW.Send (ent->StepBasic_ExternallyDefinedItem::ItemId().Value());

  SW.Send (ent->StepBasic_ExternallyDefinedItem::Source());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepVisual_RWExternallyDefinedCurveFont::Share (const Handle(StepVisual_ExternallyDefinedCurveFont) &ent,
                                                       Interface_EntityIterator& iter) const
{

  // Inherited fields of ExternallyDefinedItem

  iter.AddItem (ent->StepBasic_ExternallyDefinedItem::ItemId().Value());

  iter.AddItem (ent->StepBasic_ExternallyDefinedItem::Source());
}
