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
#include <RWStepGeom_RWUniformCurve.hxx>
#include <StepData_Logical.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BSplineCurveForm.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepGeom_UniformCurve.hxx>

// --- Enum : BSplineCurveForm ---
static TCollection_AsciiString bscfEllipticArc(".ELLIPTIC_ARC.");
static TCollection_AsciiString bscfPolylineForm(".POLYLINE_FORM.");
static TCollection_AsciiString bscfParabolicArc(".PARABOLIC_ARC.");
static TCollection_AsciiString bscfCircularArc(".CIRCULAR_ARC.");
static TCollection_AsciiString bscfUnspecified(".UNSPECIFIED.");
static TCollection_AsciiString bscfHyperbolicArc(".HYPERBOLIC_ARC.");

RWStepGeom_RWUniformCurve::RWStepGeom_RWUniformCurve () {}

void RWStepGeom_RWUniformCurve::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_UniformCurve)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,6,ach,"uniform_curve")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : degree ---

	Standard_Integer aDegree;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadInteger (num,2,"degree",ach,aDegree);

	// --- inherited field : controlPointsList ---

	Handle(StepGeom_HArray1OfCartesianPoint) aControlPointsList;
	Handle(StepGeom_CartesianPoint) anent3;
	Standard_Integer nsub3;
	if (data->ReadSubList (num,3,"control_points_list",ach,nsub3)) {
	  Standard_Integer nb3 = data->NbParams(nsub3);
	  aControlPointsList = new StepGeom_HArray1OfCartesianPoint (1, nb3);
	  for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	    if (data->ReadEntity (nsub3, i3,"cartesian_point", ach,
				  STANDARD_TYPE(StepGeom_CartesianPoint), anent3))
	      aControlPointsList->SetValue(i3, anent3);
	  }
	}

	// --- inherited field : curveForm ---

	StepGeom_BSplineCurveForm aCurveForm = StepGeom_bscfPolylineForm;
	if (data->ParamType(num,4) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,4);
	  if      (bscfEllipticArc.IsEqual(text)) aCurveForm = StepGeom_bscfEllipticArc;
	  else if (bscfPolylineForm.IsEqual(text)) aCurveForm = StepGeom_bscfPolylineForm;
	  else if (bscfParabolicArc.IsEqual(text)) aCurveForm = StepGeom_bscfParabolicArc;
	  else if (bscfCircularArc.IsEqual(text)) aCurveForm = StepGeom_bscfCircularArc;
	  else if (bscfUnspecified.IsEqual(text)) aCurveForm = StepGeom_bscfUnspecified;
	  else if (bscfHyperbolicArc.IsEqual(text)) aCurveForm = StepGeom_bscfHyperbolicArc;
	  else ach->AddFail("Enumeration b_spline_curve_form has not an allowed value");
	}
	else ach->AddFail("Parameter #4 (curve_form) is not an enumeration");

	// --- inherited field : closedCurve ---

	StepData_Logical aClosedCurve;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	data->ReadLogical (num,5,"closed_curve",ach,aClosedCurve);

	// --- inherited field : selfIntersect ---

	StepData_Logical aSelfIntersect;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat6 =` not needed
	data->ReadLogical (num,6,"self_intersect",ach,aSelfIntersect);

	//--- Initialisation of the read entity ---


	ent->Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect);
}


void RWStepGeom_RWUniformCurve::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_UniformCurve)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field degree ---

	SW.Send(ent->Degree());

	// --- inherited field controlPointsList ---

	SW.OpenSub();
	for (Standard_Integer i3 = 1;  i3 <= ent->NbControlPointsList();  i3 ++) {
	  SW.Send(ent->ControlPointsListValue(i3));
	}
	SW.CloseSub();

	// --- inherited field curveForm ---

	switch(ent->CurveForm()) {
	  case StepGeom_bscfEllipticArc : SW.SendEnum (bscfEllipticArc); break;
	  case StepGeom_bscfPolylineForm : SW.SendEnum (bscfPolylineForm); break;
	  case StepGeom_bscfParabolicArc : SW.SendEnum (bscfParabolicArc); break;
	  case StepGeom_bscfCircularArc : SW.SendEnum (bscfCircularArc); break;
	  case StepGeom_bscfUnspecified : SW.SendEnum (bscfUnspecified); break;
	  case StepGeom_bscfHyperbolicArc : SW.SendEnum (bscfHyperbolicArc); break;
	}

	// --- inherited field closedCurve ---

	SW.SendLogical(ent->ClosedCurve());

	// --- inherited field selfIntersect ---

	SW.SendLogical(ent->SelfIntersect());
}


void RWStepGeom_RWUniformCurve::Share(const Handle(StepGeom_UniformCurve)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbControlPointsList();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->ControlPointsListValue(is1));
	}

}

