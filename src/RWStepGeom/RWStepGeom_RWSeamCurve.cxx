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


#include <RWStepGeom_RWSeamCurve.ixx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_HArray1OfPcurveOrSurface.hxx>
#include <StepGeom_PcurveOrSurface.hxx>
#include <StepGeom_PreferredSurfaceCurveRepresentation.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_SeamCurve.hxx>



// --- Enum : PreferredSurfaceCurveRepresentation ---
static TCollection_AsciiString pscrPcurveS2(".PCURVE_S2.");
static TCollection_AsciiString pscrPcurveS1(".PCURVE_S1.");
static TCollection_AsciiString pscrCurve3d(".CURVE_3D.");

RWStepGeom_RWSeamCurve::RWStepGeom_RWSeamCurve () {}

void RWStepGeom_RWSeamCurve::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_SeamCurve)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,4,ach,"seam_curve")) return;

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
	Handle(Standard_Transient) assgeomval;
	Standard_Integer nsub3;
	if (data->ReadSubList (num,3,"associated_geometry",ach,nsub3)) {
	  Standard_Integer nb3 = data->NbParams(nsub3);
	  aAssociatedGeometry = new StepGeom_HArray1OfPcurveOrSurface (1, nb3);
	  for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	    if (data->ReadEntity (nsub3,i3,"associated_geometry",ach,aAssociatedGeometryItem))
	      aAssociatedGeometry->SetValue(i3,aAssociatedGeometryItem);
	    if (i3 == 1)  assgeomval  = aAssociatedGeometryItem.Value();
	    else if      (assgeomval == aAssociatedGeometryItem.Value()) //:a9 abv
	      ach->AddFail ("Seam Curve with twice the same geom");
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


void RWStepGeom_RWSeamCurve::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_SeamCurve)& ent) const
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


void RWStepGeom_RWSeamCurve::Share(const Handle(StepGeom_SeamCurve)& ent, Interface_EntityIterator& iter) const
{

	iter.GetOneItem(ent->Curve3d());


	Standard_Integer nbElem2 = ent->NbAssociatedGeometry();
	for (Standard_Integer is2=1; is2<=nbElem2; is2 ++) {
	  iter.GetOneItem(ent->AssociatedGeometryValue(is2).Value());
	}

}

