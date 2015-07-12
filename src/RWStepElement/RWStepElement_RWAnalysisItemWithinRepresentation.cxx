// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepElement_RWAnalysisItemWithinRepresentation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_AnalysisItemWithinRepresentation.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationItem.hxx>

//=======================================================================
//function : RWStepElement_RWAnalysisItemWithinRepresentation
//purpose  : 
//=======================================================================
RWStepElement_RWAnalysisItemWithinRepresentation::RWStepElement_RWAnalysisItemWithinRepresentation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWAnalysisItemWithinRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                 const Standard_Integer num,
                                                                 Handle(Interface_Check)& ach,
                                                                 const Handle(StepElement_AnalysisItemWithinRepresentation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"analysis_item_within_representation") ) return;

  // Own fields of AnalysisItemWithinRepresentation

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "description", ach, aDescription);

  Handle(StepRepr_RepresentationItem) aItem;
  data->ReadEntity (num, 3, "item", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aItem);

  Handle(StepRepr_Representation) aRep;
  data->ReadEntity (num, 4, "rep", ach, STANDARD_TYPE(StepRepr_Representation), aRep);

  // Initialize entity
  ent->Init(aName,
            aDescription,
            aItem,
            aRep);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWAnalysisItemWithinRepresentation::WriteStep (StepData_StepWriter& SW,
                                                                  const Handle(StepElement_AnalysisItemWithinRepresentation) &ent) const
{

  // Own fields of AnalysisItemWithinRepresentation

  SW.Send (ent->Name());

  SW.Send (ent->Description());

  SW.Send (ent->Item());

  SW.Send (ent->Rep());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWAnalysisItemWithinRepresentation::Share (const Handle(StepElement_AnalysisItemWithinRepresentation) &ent,
                                                              Interface_EntityIterator& iter) const
{

  // Own fields of AnalysisItemWithinRepresentation

  iter.AddItem (ent->Item());

  iter.AddItem (ent->Rep());
}
