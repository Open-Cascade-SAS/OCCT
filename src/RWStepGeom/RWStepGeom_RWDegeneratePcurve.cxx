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
#include <RWStepGeom_RWDegeneratePcurve.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_DegeneratePcurve.hxx>
#include <StepGeom_Surface.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>

RWStepGeom_RWDegeneratePcurve::RWStepGeom_RWDegeneratePcurve () {}

void RWStepGeom_RWDegeneratePcurve::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_DegeneratePcurve)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,3,ach,"degenerate_pcurve")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : basisSurface ---

	Handle(StepGeom_Surface) aBasisSurface;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"basis_surface", ach, STANDARD_TYPE(StepGeom_Surface), aBasisSurface);

	// --- own field : referenceToCurve ---

	Handle(StepRepr_DefinitionalRepresentation) aReferenceToCurve;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadEntity(num, 3,"reference_to_curve", ach, STANDARD_TYPE(StepRepr_DefinitionalRepresentation), aReferenceToCurve);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aBasisSurface, aReferenceToCurve);
}


void RWStepGeom_RWDegeneratePcurve::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_DegeneratePcurve)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- own field : basisSurface ---

	SW.Send(ent->BasisSurface());

	// --- own field : referenceToCurve ---

	SW.Send(ent->ReferenceToCurve());
}


void RWStepGeom_RWDegeneratePcurve::Share(const Handle(StepGeom_DegeneratePcurve)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->BasisSurface());


	iter.GetOneItem(ent->ReferenceToCurve());
}

