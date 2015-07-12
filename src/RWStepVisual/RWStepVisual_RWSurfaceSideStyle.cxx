// Copyright (c) 1999-2014 OPEN CASCADE SAS
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
#include <RWStepVisual_RWSurfaceSideStyle.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_HArray1OfSurfaceStyleElementSelect.hxx>
#include <StepVisual_SurfaceSideStyle.hxx>
#include <StepVisual_SurfaceStyleElementSelect.hxx>

RWStepVisual_RWSurfaceSideStyle::RWStepVisual_RWSurfaceSideStyle () {}

void RWStepVisual_RWSurfaceSideStyle::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_SurfaceSideStyle)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"surface_side_style")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : styles ---

	Handle(StepVisual_HArray1OfSurfaceStyleElementSelect) aStyles;
	StepVisual_SurfaceStyleElementSelect aStylesItem;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"styles",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aStyles = new StepVisual_HArray1OfSurfaceStyleElementSelect (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	    if (data->ReadEntity (nsub2,i2,"styles",ach,aStylesItem))
	      aStyles->SetValue(i2,aStylesItem);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, aStyles);
}


void RWStepVisual_RWSurfaceSideStyle::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_SurfaceSideStyle)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : styles ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbStyles();  i2 ++) {
	  SW.Send(ent->StylesValue(i2).Value());
	}
	SW.CloseSub();
}


void RWStepVisual_RWSurfaceSideStyle::Share(const Handle(StepVisual_SurfaceSideStyle)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbStyles();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->StylesValue(is1).Value());
	}

}

