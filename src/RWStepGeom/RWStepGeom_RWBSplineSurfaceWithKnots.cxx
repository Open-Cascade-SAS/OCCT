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
#include <RWStepGeom_RWBSplineSurfaceWithKnots.hxx>
#include <StepData_Logical.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BSplineSurfaceForm.hxx>
#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_HArray2OfCartesianPoint.hxx>
#include <StepGeom_KnotType.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

// --- Enum : KnotType ---
static TCollection_AsciiString ktUniformKnots(".UNIFORM_KNOTS.");
static TCollection_AsciiString ktQuasiUniformKnots(".QUASI_UNIFORM_KNOTS.");
static TCollection_AsciiString ktPiecewiseBezierKnots(".PIECEWISE_BEZIER_KNOTS.");
static TCollection_AsciiString ktUnspecified(".UNSPECIFIED.");


	// --- Enum : BSplineSurfaceForm ---
static TCollection_AsciiString bssfSurfOfLinearExtrusion(".SURF_OF_LINEAR_EXTRUSION.");
static TCollection_AsciiString bssfPlaneSurf(".PLANE_SURF.");
static TCollection_AsciiString bssfGeneralisedCone(".GENERALISED_CONE.");
static TCollection_AsciiString bssfToroidalSurf(".TOROIDAL_SURF.");
static TCollection_AsciiString bssfConicalSurf(".CONICAL_SURF.");
static TCollection_AsciiString bssfSphericalSurf(".SPHERICAL_SURF.");
static TCollection_AsciiString bssfUnspecified(".UNSPECIFIED.");
static TCollection_AsciiString bssfRuledSurf(".RULED_SURF.");
static TCollection_AsciiString bssfSurfOfRevolution(".SURF_OF_REVOLUTION.");
static TCollection_AsciiString bssfCylindricalSurf(".CYLINDRICAL_SURF.");
static TCollection_AsciiString bssfQuadricSurf(".QUADRIC_SURF.");

RWStepGeom_RWBSplineSurfaceWithKnots::RWStepGeom_RWBSplineSurfaceWithKnots () {}

void RWStepGeom_RWBSplineSurfaceWithKnots::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_BSplineSurfaceWithKnots)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,13,ach,"b_spline_surface_with_knots")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : uDegree ---

	Standard_Integer aUDegree;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadInteger (num,2,"u_degree",ach,aUDegree);

	// --- inherited field : vDegree ---

	Standard_Integer aVDegree;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadInteger (num,3,"v_degree",ach,aVDegree);

	// --- inherited field : controlPointsList ---

	Handle(StepGeom_HArray2OfCartesianPoint) aControlPointsList;
	Handle(StepGeom_CartesianPoint) anent4;
	Standard_Integer nsub4;
	if (data->ReadSubList (num,4,"control_points_list",ach,nsub4)) {
	  Standard_Integer nbi4 = data->NbParams(nsub4);
	  Standard_Integer nbj4 = data->NbParams(data->ParamNumber(nsub4,1));
	  aControlPointsList = new StepGeom_HArray2OfCartesianPoint (1, nbi4, 1, nbj4);
	  for (Standard_Integer i4 = 1; i4 <= nbi4; i4 ++) {
	    Standard_Integer nsi4;
	    if (data->ReadSubList (nsub4,i4,"sub-part(control_points_list)",ach,nsi4)) {
	      for (Standard_Integer j4 =1; j4 <= nbj4; j4 ++) {
		//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	        if (data->ReadEntity (nsi4, j4,"cartesian_point", ach,
				      STANDARD_TYPE(StepGeom_CartesianPoint), anent4))
		  aControlPointsList->SetValue(i4, j4, anent4);
	      }
	    }
	  }
	}

	// --- inherited field : surfaceForm ---

	StepGeom_BSplineSurfaceForm aSurfaceForm = StepGeom_bssfPlaneSurf;
	if (data->ParamType(num,5) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,5);
	  if      (bssfSurfOfLinearExtrusion.IsEqual(text)) aSurfaceForm = StepGeom_bssfSurfOfLinearExtrusion;
	  else if (bssfPlaneSurf.IsEqual(text)) aSurfaceForm = StepGeom_bssfPlaneSurf;
	  else if (bssfGeneralisedCone.IsEqual(text)) aSurfaceForm = StepGeom_bssfGeneralisedCone;
	  else if (bssfToroidalSurf.IsEqual(text)) aSurfaceForm = StepGeom_bssfToroidalSurf;
	  else if (bssfConicalSurf.IsEqual(text)) aSurfaceForm = StepGeom_bssfConicalSurf;
	  else if (bssfSphericalSurf.IsEqual(text)) aSurfaceForm = StepGeom_bssfSphericalSurf;
	  else if (bssfUnspecified.IsEqual(text)) aSurfaceForm = StepGeom_bssfUnspecified;
	  else if (bssfRuledSurf.IsEqual(text)) aSurfaceForm = StepGeom_bssfRuledSurf;
	  else if (bssfSurfOfRevolution.IsEqual(text)) aSurfaceForm = StepGeom_bssfSurfOfRevolution;
	  else if (bssfCylindricalSurf.IsEqual(text)) aSurfaceForm = StepGeom_bssfCylindricalSurf;
	  else if (bssfQuadricSurf.IsEqual(text)) aSurfaceForm = StepGeom_bssfQuadricSurf;
	  else ach->AddFail("Enumeration b_spline_surface_form has not an allowed value");
	}
	else ach->AddFail("Parameter #5 (surface_form) is not an enumeration");

	// --- inherited field : uClosed ---

	StepData_Logical aUClosed;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat6 =` not needed
	data->ReadLogical (num,6,"u_closed",ach,aUClosed);

	// --- inherited field : vClosed ---

	StepData_Logical aVClosed;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat7 =` not needed
	data->ReadLogical (num,7,"v_closed",ach,aVClosed);

	// --- inherited field : selfIntersect ---

	StepData_Logical aSelfIntersect;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat8 =` not needed
	data->ReadLogical (num,8,"self_intersect",ach,aSelfIntersect);

	// --- own field : uMultiplicities ---

	Handle(TColStd_HArray1OfInteger) aUMultiplicities;
	Standard_Integer aUMultiplicitiesItem;
	Standard_Integer nsub9;
	if (data->ReadSubList (num,9,"u_multiplicities",ach,nsub9)) {
	  Standard_Integer nb9 = data->NbParams(nsub9);
	  aUMultiplicities = new TColStd_HArray1OfInteger (1, nb9);
	  for (Standard_Integer i9 = 1; i9 <= nb9; i9 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat9 =` not needed
	    if (data->ReadInteger (nsub9,i9,"u_multiplicities",ach,aUMultiplicitiesItem))
	      aUMultiplicities->SetValue(i9,aUMultiplicitiesItem);
	  }
	}

	// --- own field : vMultiplicities ---

	Handle(TColStd_HArray1OfInteger) aVMultiplicities;
	Standard_Integer aVMultiplicitiesItem;
	Standard_Integer nsub10;
	if (data->ReadSubList (num,10,"v_multiplicities",ach,nsub10)) {
	  Standard_Integer nb10 = data->NbParams(nsub10);
	  aVMultiplicities = new TColStd_HArray1OfInteger (1, nb10);
	  for (Standard_Integer i10 = 1; i10 <= nb10; i10 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat10 =` not needed
	    if (data->ReadInteger (nsub10,i10,"v_multiplicities",ach,aVMultiplicitiesItem))
	      aVMultiplicities->SetValue(i10,aVMultiplicitiesItem);
	  }
	}

	// --- own field : uKnots ---

	Handle(TColStd_HArray1OfReal) aUKnots;
	Standard_Real aUKnotsItem;
	Standard_Integer nsub11;
	if (data->ReadSubList (num,11,"u_knots",ach,nsub11)) {
	  Standard_Integer nb11 = data->NbParams(nsub11);
	  aUKnots = new TColStd_HArray1OfReal (1, nb11);
	  for (Standard_Integer i11 = 1; i11 <= nb11; i11 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat11 =` not needed
	    if (data->ReadReal (nsub11,i11,"u_knots",ach,aUKnotsItem))
	      aUKnots->SetValue(i11,aUKnotsItem);
	  }
	}

	// --- own field : vKnots ---

	Handle(TColStd_HArray1OfReal) aVKnots;
	Standard_Real aVKnotsItem;
	Standard_Integer nsub12;
	if (data->ReadSubList (num,12,"v_knots",ach,nsub12)) {
	  Standard_Integer nb12 = data->NbParams(nsub12);
	  aVKnots = new TColStd_HArray1OfReal (1, nb12);
	  for (Standard_Integer i12 = 1; i12 <= nb12; i12 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat12 =` not needed
	    if (data->ReadReal (nsub12,i12,"v_knots",ach,aVKnotsItem))
	      aVKnots->SetValue(i12,aVKnotsItem);
	  }
	}

	// --- own field : knotSpec ---

	StepGeom_KnotType aKnotSpec = StepGeom_ktUniformKnots;
	if (data->ParamType(num,13) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,13);
	  if      (ktUniformKnots.IsEqual(text)) aKnotSpec = StepGeom_ktUniformKnots;
	  else if (ktQuasiUniformKnots.IsEqual(text)) aKnotSpec = StepGeom_ktQuasiUniformKnots;
	  else if (ktPiecewiseBezierKnots.IsEqual(text)) aKnotSpec = StepGeom_ktPiecewiseBezierKnots;
	  else if (ktUnspecified.IsEqual(text)) aKnotSpec = StepGeom_ktUnspecified;
	  else ach->AddFail("Enumeration knot_type has not an allowed value");
	}
	else ach->AddFail("Parameter #13 (knot_spec) is not an enumeration");

	//--- Initialisation of the read entity ---


	ent->Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect, aUMultiplicities, aVMultiplicities, aUKnots, aVKnots, aKnotSpec);
}


void RWStepGeom_RWBSplineSurfaceWithKnots::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_BSplineSurfaceWithKnots)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field uDegree ---

	SW.Send(ent->UDegree());

	// --- inherited field vDegree ---

	SW.Send(ent->VDegree());

	// --- inherited field controlPointsList ---

	SW.OpenSub();
	for (Standard_Integer i4 = 1;  i4 <= ent->NbControlPointsListI(); i4 ++) {
	  SW.NewLine(Standard_False);
	  SW.OpenSub();
	  for (Standard_Integer j4 = 1;  j4 <= ent->NbControlPointsListJ(); j4 ++) {
	    SW.Send(ent->ControlPointsListValue(i4,j4));
	    SW.JoinLast(Standard_False);
	  }
	  SW.CloseSub();
	}
	SW.CloseSub();

	// --- inherited field surfaceForm ---

	switch(ent->SurfaceForm()) {
	  case StepGeom_bssfSurfOfLinearExtrusion : SW.SendEnum (bssfSurfOfLinearExtrusion); break;
	  case StepGeom_bssfPlaneSurf : SW.SendEnum (bssfPlaneSurf); break;
	  case StepGeom_bssfGeneralisedCone : SW.SendEnum (bssfGeneralisedCone); break;
	  case StepGeom_bssfToroidalSurf : SW.SendEnum (bssfToroidalSurf); break;
	  case StepGeom_bssfConicalSurf : SW.SendEnum (bssfConicalSurf); break;
	  case StepGeom_bssfSphericalSurf : SW.SendEnum (bssfSphericalSurf); break;
	  case StepGeom_bssfUnspecified : SW.SendEnum (bssfUnspecified); break;
	  case StepGeom_bssfRuledSurf : SW.SendEnum (bssfRuledSurf); break;
	  case StepGeom_bssfSurfOfRevolution : SW.SendEnum (bssfSurfOfRevolution); break;
	  case StepGeom_bssfCylindricalSurf : SW.SendEnum (bssfCylindricalSurf); break;
	  case StepGeom_bssfQuadricSurf : SW.SendEnum (bssfQuadricSurf); break;
	}

	// --- inherited field uClosed ---

	SW.SendLogical(ent->UClosed());

	// --- inherited field vClosed ---

	SW.SendLogical(ent->VClosed());

	// --- inherited field selfIntersect ---

	SW.SendLogical(ent->SelfIntersect());

	// --- own field : uMultiplicities ---

	SW.OpenSub();
	for (Standard_Integer i9 = 1;  i9 <= ent->NbUMultiplicities();  i9 ++) {
	  SW.Send(ent->UMultiplicitiesValue(i9));
	}
	SW.CloseSub();

	// --- own field : vMultiplicities ---

	SW.OpenSub();
	for (Standard_Integer i10 = 1;  i10 <= ent->NbVMultiplicities();  i10 ++) {
	  SW.Send(ent->VMultiplicitiesValue(i10));
	}
	SW.CloseSub();

	// --- own field : uKnots ---

	SW.OpenSub();
	for (Standard_Integer i11 = 1;  i11 <= ent->NbUKnots();  i11 ++) {
	  SW.Send(ent->UKnotsValue(i11));
	}
	SW.CloseSub();

	// --- own field : vKnots ---

	SW.OpenSub();
	for (Standard_Integer i12 = 1;  i12 <= ent->NbVKnots();  i12 ++) {
	  SW.Send(ent->VKnotsValue(i12));
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


void RWStepGeom_RWBSplineSurfaceWithKnots::Share(const Handle(StepGeom_BSplineSurfaceWithKnots)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbiElem1 = ent->NbControlPointsListI();
	Standard_Integer nbjElem1 = ent->NbControlPointsListJ();
	for (Standard_Integer is1=1; is1<=nbiElem1; is1 ++) {
	  for (Standard_Integer js1=1; js1<=nbjElem1; js1 ++) {
	    iter.GetOneItem(ent->ControlPointsListValue(is1,js1));
	  }
	}

}



void RWStepGeom_RWBSplineSurfaceWithKnots::Check
  (const Handle(StepGeom_BSplineSurfaceWithKnots)& ent,
   const Interface_ShareTool& ,
   Handle(Interface_Check)& ach) const
{
  Standard_Integer nbCPLU  = ent->NbControlPointsListI();
  Standard_Integer nbCPLV  = ent->NbControlPointsListJ();
  Standard_Integer dgBSSU  = ent->UDegree();
  Standard_Integer dgBSSV  = ent->VDegree();
  Standard_Integer nbMulU  = ent->NbUMultiplicities();
  Standard_Integer nbMulV  = ent->NbVMultiplicities();
  Standard_Integer nbKnoU  = ent->NbUKnots();
  Standard_Integer nbKnoV  = ent->NbVKnots();
  Standard_Integer sumMulU = 0;
  Standard_Integer sumMulV = 0;
  Standard_Integer i;
//  cout << "BSplineSurfaceWithKnots: nbMulU=" << nbMulU << " nbKnoU= " << 
//    nbKnoU << " nbCPLU= " << nbCPLU << " degreeU= " << dgBSSU << endl;
//  cout << "                         nbMulV=" << nbMulV << " nbKnoV= " << 
//    nbKnoV << " nbCPLV= " << nbCPLV << " degreeV= " << dgBSSV << endl;
  if(nbMulU != nbKnoU) {
    ach->AddFail("ERROR: No.of KnotMultiplicities not equal No.of Knots in U");
  }
  if(nbMulV != nbKnoV) {
    ach->AddFail("ERROR: No.of KnotMultiplicities not equal No.of Knots in V");
  }

  // check in U direction

  for(i=1; i<=nbMulU-1; i++) {
    sumMulU = sumMulU + ent->UMultiplicitiesValue(i);
  }
  Standard_Integer sumNonPU = nbCPLU + dgBSSU + 1;
  Standard_Integer mult1U = ent->UMultiplicitiesValue(1);
  Standard_Integer multNU = ent->UMultiplicitiesValue(nbMulU);
//  cout << "BSplineSurfaceWithKnots: mult1U=" << mult1U << " multNU= " <<
//    multNU << " sumMulU= " << sumMulU << endl;
  if((sumMulU + multNU) == sumNonPU) {
  }
  else if((sumMulU == nbCPLU) && (mult1U == multNU)) {
  }
  else {
    ach->AddFail("ERROR: wrong number of Knot Multiplicities in U");
  }
  for(i=2; i<=nbKnoU; i++) {
    Standard_Real distKn  = ent->UKnotsValue(i-1) - ent->UKnotsValue(i);
    if(Abs(distKn) <= RealEpsilon())
      ach->AddWarning("WARNING: Surface contains identical KnotsValues in U");
    else if(distKn > RealEpsilon())
      ach->AddFail("ERROR: Surface contains descending KnotsValues in U");
  }

  // check in V direction

  for(i=1; i<=nbMulV-1; i++) {
    sumMulV = sumMulV + ent->VMultiplicitiesValue(i);
  }
  Standard_Integer sumNonPV = nbCPLV + dgBSSV + 1;
  Standard_Integer mult1V = ent->VMultiplicitiesValue(1);
  Standard_Integer multNV = ent->VMultiplicitiesValue(nbMulV);
//  cout << "                       : mult1V=" << mult1V << " multNV= " <<
//    multNV << " sumMulV= " << sumMulV << endl;
  if((sumMulV + multNV) == sumNonPV) {
  }
  else if((sumMulV == nbCPLV) && (mult1V == multNV)) {
  }
  else {
    ach->AddFail("ERROR: wrong number of Knot Multiplicities in V");
  }
  for(i=2; i<=nbKnoV; i++) {
    Standard_Real distKn  = ent->VKnotsValue(i-1) - ent->VKnotsValue(i);
    if(Abs(distKn) <= RealEpsilon())
      ach->AddWarning("WARNING: Surface contains identical KnotsValues in V");
    else if(distKn > RealEpsilon())
      ach->AddFail("ERROR: Surface contains descending KnotsValues in V");
  }
}
