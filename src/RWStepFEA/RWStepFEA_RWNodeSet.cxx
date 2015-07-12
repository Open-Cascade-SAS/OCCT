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
#include <RWStepFEA_RWNodeSet.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_HArray1OfNodeRepresentation.hxx>
#include <StepFEA_NodeRepresentation.hxx>
#include <StepFEA_NodeSet.hxx>

//=======================================================================
//function : RWStepFEA_RWNodeSet
//purpose  : 
//=======================================================================
RWStepFEA_RWNodeSet::RWStepFEA_RWNodeSet ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWNodeSet::ReadStep (const Handle(StepData_StepReaderData)& data,
                                    const Standard_Integer num,
                                    Handle(Interface_Check)& ach,
                                    const Handle(StepFEA_NodeSet) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"node_set") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of NodeSet

  Handle(StepFEA_HArray1OfNodeRepresentation) aNodes;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "nodes", ach, sub2) ) {
    Standard_Integer nb0 = data->NbParams(sub2);
    aNodes = new StepFEA_HArray1OfNodeRepresentation (1, nb0);
    Standard_Integer num2 = sub2;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepFEA_NodeRepresentation) anIt0;
      data->ReadEntity (num2, i0, "node_representation", ach, STANDARD_TYPE(StepFEA_NodeRepresentation), anIt0);
      aNodes->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aNodes);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWNodeSet::WriteStep (StepData_StepWriter& SW,
                                     const Handle(StepFEA_NodeSet) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of NodeSet

  SW.OpenSub();
  for (Standard_Integer i1=1; i1 <= ent->Nodes()->Length(); i1++ ) {
    Handle(StepFEA_NodeRepresentation) Var0 = ent->Nodes()->Value(i1);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWNodeSet::Share (const Handle(StepFEA_NodeSet) &ent,
                                 Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of NodeSet

  for (Standard_Integer i1=1; i1 <= ent->Nodes()->Length(); i1++ ) {
    Handle(StepFEA_NodeRepresentation) Var0 = ent->Nodes()->Value(i1);
    iter.AddItem (Var0);
  }
}
