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
#include <RWStepGeom_RWIntersectionCurve.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_HArray1OfPcurveOrSurface.hxx>
#include <StepGeom_IntersectionCurve.hxx>
#include <StepGeom_PcurveOrSurface.hxx>
#include <StepGeom_PreferredSurfaceCurveRepresentation.hxx>

// --- Enum : PreferredSurfaceCurveRepresentation ---
static TCollection_AsciiString pscrPcurveS2(".PCURVE_S2.");
static TCollection_AsciiString pscrPcurveS1(".PCURVE_S1.");
static TCollection_AsciiString pscrCurve3d(".CURVE_3D.");

RWStepGeom_RWIntersectionCurve::RWStepGeom_RWIntersectionCurve () {}

void RWStepGeom_RWIntersectionCurve::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_IntersectionCurve)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"intersection_curve")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : curve3d ---

	Handle(StepGeom_Curve) aCurve3d;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num, 2,"curve_3d", ach, STANDARD_TYPE(StepGeom_Curve), aCurve3d);

	// --- inherited field : associatedGeometry ---

	Handle(StepGeom_HArray1OfPcurveOrSurface) aAssociatedGeometry;
	StepGeom_PcurveOrSurface aAssociatedGeometryItem;
	Standard_Integer nsub3;
	if (data->ReadSubList (num,3,"associated_geometry",ach,nsub3)) {
	  Standard_Integer nb3 = data->NbParams(nsub3);
	  aAssociatedGeometry = new StepGeom_HArray1OfPcurveOrSurface (1, nb3);
	  for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	    if (data->ReadEntity (nsub3,i3,"associated_geometry",ach,aAssociatedGeometryItem))
	      aAssociatedGeometry->SetValue(i3,aAssociatedGeometryItem);
	  }
	}

	// --- inherited field : masterRepresentation ---

	StepGeom_PreferredSurfaceCurveRepresentation aMasterRepresentation = StepGeom_pscrCurve3d;
	if (data->ParamType(num,4) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,4);
	  if      (pscrPcurveS2.IsEqual(text)) aMasterRepresentation = StepGeom_pscrPcurveS2;
	  else if (pscrPcurveS1.IsEqual(text)) aMasterRepresentation = StepGeom_pscrPcurveS1;
	  else if (pscrCurve3d.IsEqual(text)) aMasterRepresentation = StepGeom_pscrCurve3d;
	  else ach->AddFail("Enumeration preferred_surface_curve_representation has not an allowed value");
	}
	else ach->AddFail("Parameter #4 (master_representation) is not an enumeration");

	//--- Initialisation of the read entity ---


	ent->Init(aName, aCurve3d, aAssociatedGeometry, aMasterRepresentation);
}


void RWStepGeom_RWIntersectionCurve::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_IntersectionCurve)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field curve3d ---

	SW.Send(ent->Curve3d());

	// --- inherited field associatedGeometry ---

	SW.OpenSub();
	for (Standard_Integer i3 = 1;  i3 <= ent->NbAssociatedGeometry();  i3 ++) {
	  SW.Send(ent->AssociatedGeometryValue(i3).Value());
	}
	SW.CloseSub();

	// --- inherited field masterRepresentation ---

	switch(ent->MasterRepresentation()) {
	  case StepGeom_pscrPcurveS2 : SW.SendEnum (pscrPcurveS2); break;
	  case StepGeom_pscrPcurveS1 : SW.SendEnum (pscrPcurveS1); break;
	  case StepGeom_pscrCurve3d : SW.SendEnum (pscrCurve3d); break;
	}
}


void RWStepGeom_RWIntersectionCurve::Share(const Handle(StepGeom_IntersectionCurve)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Curve3d());


	Standard_Integer nbElem2 = ent->NbAssociatedGeometry();
	for (Standard_Integer is2=1; is2<=nbElem2; is2 ++) {
	  iter.GetOneItem(ent->AssociatedGeometryValue(is2).Value());
	}

}

