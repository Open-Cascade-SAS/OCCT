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

// sln 04.10.2001. BUC61003. Correction of looking for items of complex entity

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <RWStepGeom_RWBSplineCurveWithKnots.hxx>
#include <RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <RWStepGeom_RWRationalBSplineCurve.hxx>
#include <StepData_Logical.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BSplineCurveForm.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepGeom_KnotType.hxx>
#include <StepGeom_RationalBSplineCurve.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

// --- Enum : BSplineCurveForm ---
static TCollection_AsciiString bscfEllipticArc(".ELLIPTIC_ARC.");
static TCollection_AsciiString bscfPolylineForm(".POLYLINE_FORM.");
static TCollection_AsciiString bscfParabolicArc(".PARABOLIC_ARC.");
static TCollection_AsciiString bscfCircularArc(".CIRCULAR_ARC.");
static TCollection_AsciiString bscfUnspecified(".UNSPECIFIED.");
static TCollection_AsciiString bscfHyperbolicArc(".HYPERBOLIC_ARC.");

	// --- Enum : KnotType ---
static TCollection_AsciiString ktUniformKnots(".UNIFORM_KNOTS.");
static TCollection_AsciiString ktQuasiUniformKnots(".QUASI_UNIFORM_KNOTS.");
static TCollection_AsciiString ktPiecewiseBezierKnots(".PIECEWISE_BEZIER_KNOTS.");
static TCollection_AsciiString ktUnspecified(".UNSPECIFIED.");

RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve::RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve () {}

void RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num0,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)& ent) const
{

// sln 04.10.2001. BUC61003. Correction of looking for items of complex entity
	Standard_Integer num = 0;  // num0
	data->NamedForComplex("BOUNDED_CURVE", "BNDCRV",num0,num,ach);

//	num = data->NextForComplex(num);
// sln 04.10.2001. BUC61003. Correction of looking for items of complex entity
//        num =  0; gka TRJ9
  data->NamedForComplex("B_SPLINE_CURVE", "BSPCR",num0,num,ach);

	// --- Instance of common supertype BSplineCurve ---

	if (!data->CheckNbParams(num,5,ach,"b_spline_curve")) return;
	// --- field : degree ---


	Standard_Integer aDegree;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadInteger (num,1,"degree",ach,aDegree);
	// --- field : controlPointsList ---


	Handle(StepGeom_HArray1OfCartesianPoint) aControlPointsList;
	Handle(StepGeom_CartesianPoint) anent2;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"control_points_list",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aControlPointsList = new StepGeom_HArray1OfCartesianPoint (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	    if (data->ReadEntity (nsub2, i2,"cartesian_point", ach,
				  STANDARD_TYPE(StepGeom_CartesianPoint), anent2))
	      aControlPointsList->SetValue(i2, anent2);
	  }
	}

	// --- field : curveForm ---


	StepGeom_BSplineCurveForm aCurveForm = StepGeom_bscfPolylineForm;
	if (data->ParamType(num,3) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,3);
	  if      (bscfEllipticArc.IsEqual(text)) aCurveForm = StepGeom_bscfEllipticArc;
	  else if (bscfPolylineForm.IsEqual(text)) aCurveForm = StepGeom_bscfPolylineForm;
	  else if (bscfParabolicArc.IsEqual(text)) aCurveForm = StepGeom_bscfParabolicArc;
	  else if (bscfCircularArc.IsEqual(text)) aCurveForm = StepGeom_bscfCircularArc;
	  else if (bscfUnspecified.IsEqual(text)) aCurveForm = StepGeom_bscfUnspecified;
	  else if (bscfHyperbolicArc.IsEqual(text)) aCurveForm = StepGeom_bscfHyperbolicArc;
	  else ach->AddFail("Enumeration b_spline_curve_form has not an allowed value");
	}
	else ach->AddFail("Parameter #3 (curve_form) is not an enumeration");
	// --- field : closedCurve ---


	StepData_Logical aClosedCurve;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadLogical (num,4,"closed_curve",ach,aClosedCurve);
	// --- field : selfIntersect ---


	StepData_Logical aSelfIntersect;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	data->ReadLogical (num,5,"self_intersect",ach,aSelfIntersect);

//	num = data->NextForComplex(num);
// sln 04.10.2001. BUC61003. Correction of looking for items of complex entity
//        num =  0; //gka TRJ9
	data->NamedForComplex("B_SPLINE_CURVE_WITH_KNOTS", "BSCWK",num0,num,ach);

	// --- Instance of plex componant BSplineCurveWithKnots ---

	if (!data->CheckNbParams(num,3,ach,"b_spline_curve_with_knots")) return;

	// --- field : knotMultiplicities ---

	Handle(TColStd_HArray1OfInteger) aKnotMultiplicities;
	Standard_Integer aKnotMultiplicitiesItem;
	Standard_Integer nsub6;
	if (data->ReadSubList (num,1,"knot_multiplicities",ach,nsub6)) {
	  Standard_Integer nb6 = data->NbParams(nsub6);
	  aKnotMultiplicities = new TColStd_HArray1OfInteger (1, nb6);
	  for (Standard_Integer i6 = 1; i6 <= nb6; i6 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat6 =` not needed
	    if (data->ReadInteger (nsub6,i6,"knot_multiplicities",ach,aKnotMultiplicitiesItem))
	      aKnotMultiplicities->SetValue(i6,aKnotMultiplicitiesItem);
	  }
	}

	// --- field : knots ---

	Handle(TColStd_HArray1OfReal) aKnots;
	Standard_Real aKnotsItem;
	Standard_Integer nsub7;
	if (data->ReadSubList (num,2,"knots",ach,nsub7)) {
	  Standard_Integer nb7 = data->NbParams(nsub7);
	  aKnots = new TColStd_HArray1OfReal (1, nb7);
	  for (Standard_Integer i7 = 1; i7 <= nb7; i7 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat7 =` not needed
	    if (data->ReadReal (nsub7,i7,"knots",ach,aKnotsItem))
	      aKnots->SetValue(i7,aKnotsItem);
	  }
	}

	// --- field : knotSpec ---

	StepGeom_KnotType aKnotSpec = StepGeom_ktUniformKnots;
	if (data->ParamType(num,3) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,3);
	  if      (ktUniformKnots.IsEqual(text)) aKnotSpec = StepGeom_ktUniformKnots;
	  else if (ktQuasiUniformKnots.IsEqual(text)) aKnotSpec = StepGeom_ktQuasiUniformKnots;
	  else if (ktPiecewiseBezierKnots.IsEqual(text)) aKnotSpec = StepGeom_ktPiecewiseBezierKnots;
	  else if (ktUnspecified.IsEqual(text)) aKnotSpec = StepGeom_ktUnspecified;
	  else ach->AddFail("Enumeration knot_type has not an allowed value");
	}
	else ach->AddFail("Parameter #3 (knot_spec) is not an enumeration");

//	num = data->NextForComplex(num);
// sln 04.10.2001. BUC61003. Correction of looking for items of complex entity
//        num =  0; gka TRJ9
	data->NamedForComplex("CURVE",num0,num,ach);

//	num = data->NextForComplex(num);
// sln 04.10.2001. BUC61003. Correction of looking for items of complex entity
        //num =  0;
	data->NamedForComplex("GEOMETRIC_REPRESENTATION_ITEM", "GMRPIT",num0,num,ach);

//	num = data->NextForComplex(num);
// sln 04.10.2001. BUC61003. Correction of looking for items of complex entity
        //num =  0;
	data->NamedForComplex("RATIONAL_B_SPLINE_CURVE", "RBSC",num0,num,ach);

	// --- Instance of plex componant RationalBSplineCurve ---

	if (!data->CheckNbParams(num,1,ach,"rational_b_spline_curve")) return;

	// --- field : weightsData ---

	Handle(TColStd_HArray1OfReal) aWeightsData;
	Standard_Real aWeightsDataItem;
	Standard_Integer nsub9;
	if (data->ReadSubList (num,1,"weights_data",ach,nsub9)) {
	  Standard_Integer nb9 = data->NbParams(nsub9);
	  aWeightsData = new TColStd_HArray1OfReal (1, nb9);
	  for (Standard_Integer i9 = 1; i9 <= nb9; i9 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat9 =` not needed
	    if (data->ReadReal (nsub9,i9,"weights_data",ach,aWeightsDataItem))
	      aWeightsData->SetValue(i9,aWeightsDataItem);
	  }
	}

//	num = data->NextForComplex(num);
// sln 04.10.2001. BUC61003. Correction of looking for items of complex entity
        //num =  0;
	data->NamedForComplex("REPRESENTATION_ITEM", "RPRITM",num0,num,ach);

	// --- Instance of plex componant RepresentationItem ---

	if (!data->CheckNbParams(num,1,ach,"representation_item")) return;

	// --- field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat10 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	//--- Initialisation of the red entity ---

	ent->Init(aName,aDegree,aControlPointsList,aCurveForm,aClosedCurve,aSelfIntersect,aKnotMultiplicities,aKnots,aKnotSpec,aWeightsData);
}


void RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)& ent) const
{

	// --- Instance of plex componant BoundedCurve ---

	SW.StartEntity("BOUNDED_CURVE");

	// --- Instance of common supertype BSplineCurve ---

	SW.StartEntity("B_SPLINE_CURVE");
	// --- field : degree ---

	SW.Send(ent->Degree());
	// --- field : controlPointsList ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbControlPointsList();  i2 ++) {
	  SW.Send(ent->ControlPointsListValue(i2));
	}
	SW.CloseSub();
	// --- field : curveForm ---

	switch(ent->CurveForm()) {
	  case StepGeom_bscfEllipticArc : SW.SendEnum (bscfEllipticArc); break;
	  case StepGeom_bscfPolylineForm : SW.SendEnum (bscfPolylineForm); break;
	  case StepGeom_bscfParabolicArc : SW.SendEnum (bscfParabolicArc); break;
	  case StepGeom_bscfCircularArc : SW.SendEnum (bscfCircularArc); break;
	  case StepGeom_bscfUnspecified : SW.SendEnum (bscfUnspecified); break;
	  case StepGeom_bscfHyperbolicArc : SW.SendEnum (bscfHyperbolicArc); break;
	}
	// --- field : closedCurve ---

	SW.SendLogical(ent->ClosedCurve());
	// --- field : selfIntersect ---

	SW.SendLogical(ent->SelfIntersect());

	// --- Instance of plex componant BSplineCurveWithKnots ---

	SW.StartEntity("B_SPLINE_CURVE_WITH_KNOTS");
	// --- field : knotMultiplicities ---

	SW.OpenSub();
	for (Standard_Integer i6 = 1;  i6 <= ent->NbKnotMultiplicities();  i6 ++) {
	  SW.Send(ent->KnotMultiplicitiesValue(i6));
	}
	SW.CloseSub();
	// --- field : knots ---

	SW.OpenSub();
	for (Standard_Integer i7 = 1;  i7 <= ent->NbKnots();  i7 ++) {
	  SW.Send(ent->KnotsValue(i7));
	}
	SW.CloseSub();
	// --- field : knotSpec ---

	switch(ent->KnotSpec()) {
	  case StepGeom_ktUniformKnots : SW.SendEnum (ktUniformKnots); break;
	  case StepGeom_ktQuasiUniformKnots : SW.SendEnum (ktQuasiUniformKnots); break;
	  case StepGeom_ktPiecewiseBezierKnots : SW.SendEnum (ktPiecewiseBezierKnots); break;
	  case StepGeom_ktUnspecified : SW.SendEnum (ktUnspecified); break;
	}

	// --- Instance of plex componant Curve ---

	SW.StartEntity("CURVE");

	// --- Instance of plex componant GeometricRepresentationItem ---

	SW.StartEntity("GEOMETRIC_REPRESENTATION_ITEM");

	// --- Instance of plex componant RationalBSplineCurve ---

	SW.StartEntity("RATIONAL_B_SPLINE_CURVE");
	// --- field : weightsData ---

	SW.OpenSub();
	for (Standard_Integer i9 = 1;  i9 <= ent->NbWeightsData();  i9 ++) {
	  SW.Send(ent->WeightsDataValue(i9));
	}
	SW.CloseSub();

	// --- Instance of plex componant RepresentationItem ---

	SW.StartEntity("REPRESENTATION_ITEM");
	// --- field : name ---

	SW.Send(ent->Name());
}


void RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve::Share(const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbControlPointsList();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->ControlPointsListValue(is1));
	}

}



void RWStepGeom_RWBSplineCurveWithKnotsAndRationalBSplineCurve::Check
  (const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)& ent,
   const Interface_ShareTool& aShto,
   Handle(Interface_Check)& ach) const
{
  Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve) aRationalBSC = ent;
  Handle(StepGeom_BSplineCurveWithKnots) aBSCWK =
    aRationalBSC->BSplineCurveWithKnots();
  RWStepGeom_RWBSplineCurveWithKnots t1;
  t1.Check(aBSCWK,aShto,ach);
  Handle(StepGeom_RationalBSplineCurve) aRBSC =
    aRationalBSC->RationalBSplineCurve();
  RWStepGeom_RWRationalBSplineCurve t2;
  t2.Check(aRBSC,aShto,ach);
}
