// Created on: 2015-10-29
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepVisual_RWTessellatedGeometricSet.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_HArray1OfPresentationStyleAssignment.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_TessellatedGeometricSet.hxx>
#include <StepVisual_TessellatedItem.hxx>

//=======================================================================
//function : RWStepVisual_RWTessellatedGeometricSet
//purpose  : 
//=======================================================================
RWStepVisual_RWTessellatedGeometricSet::RWStepVisual_RWTessellatedGeometricSet () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWTessellatedGeometricSet::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num,
   Handle(Interface_Check)& ach,
   const Handle(StepVisual_TessellatedGeometricSet)& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 2, ach, "tessellated_geometric_set")) return;

  // Inherited field : name
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);
  
  NCollection_Handle<StepVisual_Array1OfTessellatedItem> anItems;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"items",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  anItems = new StepVisual_Array1OfTessellatedItem(1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    Handle(StepVisual_TessellatedItem) anItem;// = new StepVisual_TesselatedItem;
	    if (data->ReadEntity (nsub2,i2,"item",ach,STANDARD_TYPE(StepVisual_TessellatedItem), anItem))
	      anItems->SetValue(i2,anItem);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, anItems);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWTessellatedGeometricSet::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepVisual_TessellatedGeometricSet)& ent) const
{
  // Inherited field : name
  SW.Send(ent->Name());

  // Own field : children
  SW.OpenSub();
  for (Standard_Integer i = 1; i <= ent->Items()->Length(); i++)
      SW.Send(ent->Items()->Value(i));
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================
void RWStepVisual_RWTessellatedGeometricSet::Share(const Handle(StepVisual_TessellatedGeometricSet)& ent, Interface_EntityIterator& iter) const
{
  // Own field : children
  for (Standard_Integer i = 1; i <= ent->Items()->Length(); i++)
      iter.AddItem(ent->Items()->Value(i));
}
