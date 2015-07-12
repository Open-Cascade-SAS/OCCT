// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepVisual_RWExternallyDefinedCurveFont.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_ExternallyDefinedCurveFont.hxx>

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
