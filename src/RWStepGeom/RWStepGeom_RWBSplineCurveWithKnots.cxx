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
#include <Interface_ShareTool.hxx>
#include <RWStepGeom_RWBSplineCurveWithKnots.hxx>
#include <StepData_Logical.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BSplineCurveForm.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepGeom_KnotType.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

// --- Enum : KnotType ---
static TCollection_AsciiString ktUniformKnots(".UNIFORM_KNOTS.");
static TCollection_AsciiString ktQuasiUniformKnots(".QUASI_UNIFORM_KNOTS.");
static TCollection_AsciiString ktPiecewiseBezierKnots(".PIECEWISE_BEZIER_KNOTS.");
static TCollection_AsciiString ktUnspecified(".UNSPECIFIED.");


	// --- Enum : BSplineCurveForm ---
static TCollection_AsciiString bscfEllipticArc(".ELLIPTIC_ARC.");
static TCollection_AsciiString bscfPolylineForm(".POLYLINE_FORM.");
static TCollection_AsciiString bscfParabolicArc(".PARABOLIC_ARC.");
static TCollection_AsciiString bscfCircularArc(".CIRCULAR_ARC.");
static TCollection_AsciiString bscfUnspecified(".UNSPECIFIED.");
static TCollection_AsciiString bscfHyperbolicArc(".HYPERBOLIC_ARC.");

RWStepGeom_RWBSplineCurveWithKnots::RWStepGeom_RWBSplineCurveWithKnots () {}

void RWStepGeom_RWBSplineCurveWithKnots::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_BSplineCurveWithKnots)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,9,ach,"b_spline_curve_with_knots")) return;

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
    if(nb3 <1)
      ach->AddFail("Number of control points of the b_spline_curve_form is equal to 0");
    else
    {
      aControlPointsList = new StepGeom_HArray1OfCartesianPoint (1, nb3);
      for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
        //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
        if (data->ReadEntity (nsub3, i3,"cartesian_point", ach,
          STANDARD_TYPE(StepGeom_CartesianPoint), anent3))
          aControlPointsList->SetValue(i3, anent3);
      }
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

	// --- own field : knotMultiplicities ---

	Handle(TColStd_HArray1OfInteger) aKnotMultiplicities;
	Standard_Integer aKnotMultiplicitiesItem;
	Standard_Integer nsub7;
	if (data->ReadSubList (num,7,"knot_multiplicities",ach,nsub7)) {
	  Standard_Integer nb7 = data->NbParams(nsub7);
	  aKnotMultiplicities = new TColStd_HArray1OfInteger (1, nb7);
	  for (Standard_Integer i7 = 1; i7 <= nb7; i7 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat7 =` not needed
	    if (data->ReadInteger (nsub7,i7,"knot_multiplicities",ach,aKnotMultiplicitiesItem))
	      aKnotMultiplicities->SetValue(i7,aKnotMultiplicitiesItem);
	  }
	}

	// --- own field : knots ---

	Handle(TColStd_HArray1OfReal) aKnots;
	Standard_Real aKnotsItem;
	Standard_Integer nsub8;
	if (data->ReadSubList (num,8,"knots",ach,nsub8)) {
	  Standard_Integer nb8 = data->NbParams(nsub8);
	  aKnots = new TColStd_HArray1OfReal (1, nb8);
	  for (Standard_Integer i8 = 1; i8 <= nb8; i8 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat8 =` not needed
	    if (data->ReadReal (nsub8,i8,"knots",ach,aKnotsItem))
	      aKnots->SetValue(i8,aKnotsItem);
	  }
	}

	// --- own field : knotSpec ---

	StepGeom_KnotType aKnotSpec = StepGeom_ktUniformKnots;
	if (data->ParamType(num,9) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,9);
	  if      (ktUniformKnots.IsEqual(text)) aKnotSpec = StepGeom_ktUniformKnots;
	  else if (ktQuasiUniformKnots.IsEqual(text)) aKnotSpec = StepGeom_ktQuasiUniformKnots;
	  else if (ktPiecewiseBezierKnots.IsEqual(text)) aKnotSpec = StepGeom_ktPiecewiseBezierKnots;
	  else if (ktUnspecified.IsEqual(text)) aKnotSpec = StepGeom_ktUnspecified;
	  else ach->AddFail("Enumeration knot_type has not an allowed value");
	}
	else ach->AddFail("Parameter #9 (knot_spec) is not an enumeration");

	//--- Initialisation of the read entity ---


	ent->Init(aName, aDegree, aControlPointsList, aCurveForm, aClosedCurve, aSelfIntersect, aKnotMultiplicities, aKnots, aKnotSpec);
}


void RWStepGeom_RWBSplineCurveWithKnots::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_BSplineCurveWithKnots)& ent) const
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

	// --- own field : knotMultiplicities ---

	SW.OpenSub();
	for (Standard_Integer i7 = 1;  i7 <= ent->NbKnotMultiplicities();  i7 ++) {
	  SW.Send(ent->KnotMultiplicitiesValue(i7));
	}
	SW.CloseSub();

	// --- own field : knots ---

	SW.OpenSub();
	for (Standard_Integer i8 = 1;  i8 <= ent->NbKnots();  i8 ++) {
	  SW.Send(ent->KnotsValue(i8));
	}
	SW.CloseSub();

	// --- own field : knotSpec ---

	switch(ent->KnotSpec()) {
	  case StepGeom_ktUniformKnots : SW.SendEnum (ktUniformKnots); break;
	  case StepGeom_ktQuasiUniformKnots : SW.SendEnum (ktQuasiUniformKnots); break;
	  case StepGeom_ktPiecewiseBezierKnots : SW.SendEnum (ktPiecewiseBezierKnots); break;
	  case StepGeom_ktUnspecified : SW.SendEnum (ktUnspecified); break;
	}
}


void RWStepGeom_RWBSplineCurveWithKnots::Share(const Handle(StepGeom_BSplineCurveWithKnots)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbControlPointsList();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->ControlPointsListValue(is1));
	}

}

void RWStepGeom_RWBSplineCurveWithKnots::Check
(const Handle(StepGeom_BSplineCurveWithKnots)& ent,
 const Interface_ShareTool& ,
 Handle(Interface_Check)& ach) const
{
  Standard_Integer nbCPL  = ent->NbControlPointsList();
  Standard_Integer dgBSC  = ent->Degree();
  Standard_Integer nbMult = ent->NbKnotMultiplicities();
  Standard_Integer nbKno  = ent->NbKnots();
  Standard_Integer sumMult = 0;
//  std::cout << "BSplineCurveWithKnots: nbMult=" << nbMult << " nbKno= " << 
//    nbKno << " nbCPL= " << nbCPL << " degree= " << dgBSC << std::endl;
  if(nbMult != nbKno) {
    ach->AddFail("ERROR: No.of KnotMultiplicities not equal No.of Knots");
  }
  Standard_Integer i;//svv Jan 10 2000: porting on DEC 
  for (i=1; i<=nbMult-1; i++) {
    sumMult = sumMult + ent->KnotMultiplicitiesValue(i);
  }
  Standard_Integer sumNonP = nbCPL + dgBSC + 1;
  Standard_Integer mult1 = ent->KnotMultiplicitiesValue(1);
  Standard_Integer multN = ent->KnotMultiplicitiesValue(nbMult);
//  std::cout << "BSplineCurveWithKnots: mult1=" << mult1 << " multN= " <<
//    multN << " sumMult= " << sumMult << std::endl;
  if((sumMult + multN) == sumNonP) {
  }
  else if((sumMult == nbCPL) && (mult1 == multN)) {
  }
  else {
    ach->AddFail("ERROR: wrong number of Knot Multiplicities");
  }
  for(i=2; i<=nbKno; i++) {
    Standard_Real distKn  = ent->KnotsValue(i-1) - ent->KnotsValue(i);
    if(Abs(distKn) <= RealEpsilon())
      ach->AddWarning("WARNING: Curve contains identical KnotsValues");
    else if(distKn > RealEpsilon())
      ach->AddFail("ERROR: Curve contains descending KnotsValues");
  }
}

