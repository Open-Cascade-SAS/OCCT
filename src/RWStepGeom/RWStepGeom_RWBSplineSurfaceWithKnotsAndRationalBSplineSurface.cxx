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
#include <RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <RWStepGeom_RWRationalBSplineSurface.hxx>
#include <StepData_Logical.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BSplineSurfaceForm.hxx>
#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_HArray2OfCartesianPoint.hxx>
#include <StepGeom_KnotType.hxx>
#include <StepGeom_RationalBSplineSurface.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

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

	// --- Enum : KnotType ---
static TCollection_AsciiString ktUniformKnots(".UNIFORM_KNOTS.");
static TCollection_AsciiString ktQuasiUniformKnots(".QUASI_UNIFORM_KNOTS.");
static TCollection_AsciiString ktPiecewiseBezierKnots(".PIECEWISE_BEZIER_KNOTS.");
static TCollection_AsciiString ktUnspecified(".UNSPECIFIED.");

RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface::RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface () {}

void RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num0,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)& ent) const
{

	Standard_Integer num = 0;    // num0
	data->NamedForComplex("BOUNDED_SURFACE", "BNDSRF",num0,num,ach);

//	num = data->NextForComplex(num);
	data->NamedForComplex("B_SPLINE_SURFACE", "BSPSR",num0,num,ach);

	// --- Instance of common supertype BSplineSurface ---

	if (!data->CheckNbParams(num,7,ach,"b_spline_surface")) return;
	// --- field : uDegree ---


	Standard_Integer aUDegree;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadInteger (num,1,"u_degree",ach,aUDegree);
	// --- field : vDegree ---


	Standard_Integer aVDegree;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadInteger (num,2,"v_degree",ach,aVDegree);
	// --- field : controlPointsList ---


	Handle(StepGeom_HArray2OfCartesianPoint) aControlPointsList;
	Handle(StepGeom_CartesianPoint) anent3;
	Standard_Integer nsub3;
	if (data->ReadSubList (num,3,"control_points_list",ach,nsub3)) {
	  Standard_Integer nbi3 = data->NbParams(nsub3);
	  Standard_Integer nbj3 = data->NbParams(data->ParamNumber(nsub3,1));
	  aControlPointsList = new StepGeom_HArray2OfCartesianPoint (1, nbi3, 1, nbj3);
	  for (Standard_Integer i3 = 1; i3 <= nbi3; i3 ++) {
	    Standard_Integer nsi3temp;
	    if (data->ReadSubList (nsub3,i3,"sub-part(control_points_list)",ach,nsi3temp)) {
	      Standard_Integer nsi3 = data->ParamNumber(nsub3,i3);
	      for (Standard_Integer j3 =1; j3 <= nbj3; j3 ++) {
		//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	        if (data->ReadEntity (nsi3, j3,"cartesian_point", ach,
				      STANDARD_TYPE(StepGeom_CartesianPoint), anent3))
		  aControlPointsList->SetValue(i3, j3, anent3);
	      }
	    }
	  }
	}

	// --- field : surfaceForm ---


	StepGeom_BSplineSurfaceForm aSurfaceForm = StepGeom_bssfPlaneSurf;
	if (data->ParamType(num,4) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,4);
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
	else ach->AddFail("Parameter #4 (surface_form) is not an enumeration");
	// --- field : uClosed ---


	StepData_Logical aUClosed;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	data->ReadLogical (num,5,"u_closed",ach,aUClosed);
	// --- field : vClosed ---


	StepData_Logical aVClosed;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat6 =` not needed
	data->ReadLogical (num,6,"v_closed",ach,aVClosed);
	// --- field : selfIntersect ---


	StepData_Logical aSelfIntersect;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat7 =` not needed
	data->ReadLogical (num,7,"self_intersect",ach,aSelfIntersect);

//	num = data->NextForComplex(num);
	data->NamedForComplex("B_SPLINE_SURFACE_WITH_KNOTS", "BSSWK",num0,num,ach);

	// --- Instance of plex componant BSplineSurfaceWithKnots ---

	if (!data->CheckNbParams(num,5,ach,"b_spline_surface_with_knots")) return;

	// --- field : uMultiplicities ---

	Handle(TColStd_HArray1OfInteger) aUMultiplicities;
	Standard_Integer aUMultiplicitiesItem;
	Standard_Integer nsub8;
	if (data->ReadSubList (num,1,"u_multiplicities",ach,nsub8)) {
	  Standard_Integer nb8 = data->NbParams(nsub8);
	  aUMultiplicities = new TColStd_HArray1OfInteger (1, nb8);
	  for (Standard_Integer i8 = 1; i8 <= nb8; i8 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat8 =` not needed
	    if (data->ReadInteger (nsub8,i8,"u_multiplicities",ach,aUMultiplicitiesItem))
	      aUMultiplicities->SetValue(i8,aUMultiplicitiesItem);
	  }
	}

	// --- field : vMultiplicities ---

	Handle(TColStd_HArray1OfInteger) aVMultiplicities;
	Standard_Integer aVMultiplicitiesItem;
	Standard_Integer nsub9;
	if (data->ReadSubList (num,2,"v_multiplicities",ach,nsub9)) {
	  Standard_Integer nb9 = data->NbParams(nsub9);
	  aVMultiplicities = new TColStd_HArray1OfInteger (1, nb9);
	  for (Standard_Integer i9 = 1; i9 <= nb9; i9 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat9 =` not needed
	    if (data->ReadInteger (nsub9,i9,"v_multiplicities",ach,aVMultiplicitiesItem))
	      aVMultiplicities->SetValue(i9,aVMultiplicitiesItem);
	  }
	}

	// --- field : uKnots ---

	Handle(TColStd_HArray1OfReal) aUKnots;
	Standard_Real aUKnotsItem;
	Standard_Integer nsub10;
	if (data->ReadSubList (num,3,"u_knots",ach,nsub10)) {
	  Standard_Integer nb10 = data->NbParams(nsub10);
	  aUKnots = new TColStd_HArray1OfReal (1, nb10);
	  for (Standard_Integer i10 = 1; i10 <= nb10; i10 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat10 =` not needed
	    if (data->ReadReal (nsub10,i10,"u_knots",ach,aUKnotsItem))
	      aUKnots->SetValue(i10,aUKnotsItem);
	  }
	}

	// --- field : vKnots ---

	Handle(TColStd_HArray1OfReal) aVKnots;
	Standard_Real aVKnotsItem;
	Standard_Integer nsub11;
	if (data->ReadSubList (num,4,"v_knots",ach,nsub11)) {
	  Standard_Integer nb11 = data->NbParams(nsub11);
	  aVKnots = new TColStd_HArray1OfReal (1, nb11);
	  for (Standard_Integer i11 = 1; i11 <= nb11; i11 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat11 =` not needed
	    if (data->ReadReal (nsub11,i11,"v_knots",ach,aVKnotsItem))
	      aVKnots->SetValue(i11,aVKnotsItem);
	  }
	}

	// --- field : knotSpec ---

	StepGeom_KnotType aKnotSpec =  StepGeom_ktUniformKnots;
	if (data->ParamType(num,5) == Interface_ParamEnum) {
	  Standard_CString text = data->ParamCValue(num,5);
	  if      (ktUniformKnots.IsEqual(text)) aKnotSpec = StepGeom_ktUniformKnots;
	  else if (ktQuasiUniformKnots.IsEqual(text)) aKnotSpec = StepGeom_ktQuasiUniformKnots;
	  else if (ktPiecewiseBezierKnots.IsEqual(text)) aKnotSpec = StepGeom_ktPiecewiseBezierKnots;
	  else if (ktUnspecified.IsEqual(text)) aKnotSpec = StepGeom_ktUnspecified;
	  else ach->AddFail("Enumeration knot_type has not an allowed value");
	}
	else ach->AddFail("Parameter #5 (knot_spec) is not an enumeration");

//	num = data->NextForComplex(num);
	data->NamedForComplex("GEOMETRIC_REPRESENTATION_ITEM", "GMRPIT",num0,num,ach);

//	num = data->NextForComplex(num);
	data->NamedForComplex("RATIONAL_B_SPLINE_SURFACE", "RBSS",num0,num,ach);

	// --- Instance of plex componant RationalBSplineSurface ---

	if (!data->CheckNbParams(num,1,ach,"rational_b_spline_surface")) return;

	// --- field : weightsData ---

	Handle(TColStd_HArray2OfReal) aWeightsData;
	  Standard_Real aWeightsDataItem;
	Standard_Integer nsub13;
	if (data->ReadSubList (num,1,"weights_data",ach,nsub13)) {
	  Standard_Integer nbi13 = data->NbParams(nsub13);
	  Standard_Integer nbj13 = data->NbParams(data->ParamNumber(nsub13,1));
	  aWeightsData = new TColStd_HArray2OfReal (1,nbi13,1,nbj13);
	  for (Standard_Integer i13 = 1; i13 <= nbi13; i13 ++) {
	    Standard_Integer nsi13temp;
	    if (data->ReadSubList (nsub13,i13,"sub-part(weights_data)",ach,nsi13temp)) {
	      Standard_Integer nsi13 = data->ParamNumber(nsub13,i13);
	      for (Standard_Integer j13 =1; j13 <= nbj13; j13 ++) {
		//szv#4:S4163:12Mar99 `Standard_Boolean stat13 =` not needed
	        if (data->ReadReal (nsi13,j13,"weights_data",ach,aWeightsDataItem))
		  aWeightsData->SetValue(i13,j13,aWeightsDataItem);
	      }
	    }
	  }
	}

//	num = data->NextForComplex(num);
	data->NamedForComplex("REPRESENTATION_ITEM", "RPRITM",num0,num,ach);

	// --- Instance of plex componant RepresentationItem ---

	if (!data->CheckNbParams(num,1,ach,"representation_item")) return;

	// --- field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat14 =` not needed
	data->ReadString (num,1,"name",ach,aName);

//	num = data->NextForComplex(num);
	data->NamedForComplex("SURFACE", "SRFC",num0,num,ach);

	//--- Initialisation of the red entity ---

	ent->Init(aName,aUDegree,aVDegree,aControlPointsList,aSurfaceForm,aUClosed,aVClosed,aSelfIntersect,aUMultiplicities,aVMultiplicities,aUKnots,aVKnots,aKnotSpec,aWeightsData);
}


void RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)& ent) const
{

	// --- Instance of plex componant BoundedSurface ---

	SW.StartEntity("BOUNDED_SURFACE");

	// --- Instance of common supertype BSplineSurface ---

	SW.StartEntity("B_SPLINE_SURFACE");
	// --- field : uDegree ---

	SW.Send(ent->UDegree());
	// --- field : vDegree ---

	SW.Send(ent->VDegree());
	// --- field : controlPointsList ---

	SW.OpenSub();
	for (Standard_Integer i3 = 1;  i3 <= ent->NbControlPointsListI(); i3 ++) {
	  SW.NewLine(Standard_False);
	  SW.OpenSub();
	  for (Standard_Integer j3 = 1;  j3 <= ent->NbControlPointsListJ(); j3 ++) {
	    SW.Send(ent->ControlPointsListValue(i3,j3));
	    SW.JoinLast(Standard_False);
	  }
	  SW.CloseSub();
	}
	SW.CloseSub();
	// --- field : surfaceForm ---

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
	// --- field : uClosed ---

	SW.SendLogical(ent->UClosed());
	// --- field : vClosed ---

	SW.SendLogical(ent->VClosed());
	// --- field : selfIntersect ---

	SW.SendLogical(ent->SelfIntersect());

	// --- Instance of plex componant BSplineSurfaceWithKnots ---

	SW.StartEntity("B_SPLINE_SURFACE_WITH_KNOTS");
	// --- field : uMultiplicities ---

	SW.OpenSub();
	for (Standard_Integer i8 = 1;  i8 <= ent->NbUMultiplicities();  i8 ++) {
	  SW.Send(ent->UMultiplicitiesValue(i8));
	}
	SW.CloseSub();
	// --- field : vMultiplicities ---

	SW.OpenSub();
	for (Standard_Integer i9 = 1;  i9 <= ent->NbVMultiplicities();  i9 ++) {
	  SW.Send(ent->VMultiplicitiesValue(i9));
	}
	SW.CloseSub();
	// --- field : uKnots ---

	SW.OpenSub();
	for (Standard_Integer i10 = 1;  i10 <= ent->NbUKnots();  i10 ++) {
	  SW.Send(ent->UKnotsValue(i10));
	}
	SW.CloseSub();
	// --- field : vKnots ---

	SW.OpenSub();
	for (Standard_Integer i11 = 1;  i11 <= ent->NbVKnots();  i11 ++) {
	  SW.Send(ent->VKnotsValue(i11));
	}
	SW.CloseSub();
	// --- field : knotSpec ---

	switch(ent->KnotSpec()) {
	  case StepGeom_ktUniformKnots : SW.SendEnum (ktUniformKnots); break;
	  case StepGeom_ktQuasiUniformKnots : SW.SendEnum (ktQuasiUniformKnots); break;
	  case StepGeom_ktPiecewiseBezierKnots : SW.SendEnum (ktPiecewiseBezierKnots); break;
	  case StepGeom_ktUnspecified : SW.SendEnum (ktUnspecified); break;
	}

	// --- Instance of plex componant GeometricRepresentationItem ---

	SW.StartEntity("GEOMETRIC_REPRESENTATION_ITEM");

	// --- Instance of plex componant RationalBSplineSurface ---

	SW.StartEntity("RATIONAL_B_SPLINE_SURFACE");
	// --- field : weightsData ---

	SW.OpenSub();
	for (Standard_Integer i13 = 1;  i13 <= ent->NbWeightsDataI(); i13 ++) {
	  SW.NewLine(Standard_False);
	  SW.OpenSub();
	  for (Standard_Integer j13 = 1;  j13 <= ent->NbWeightsDataJ(); j13 ++) {
	    SW.Send(ent->WeightsDataValue(i13,j13));
	    SW.JoinLast(Standard_False);
	  }
	  SW.CloseSub();
	}
	SW.CloseSub();

	// --- Instance of plex componant RepresentationItem ---

	SW.StartEntity("REPRESENTATION_ITEM");
	// --- field : name ---

	SW.Send(ent->Name());

	// --- Instance of plex componant Surface ---

	SW.StartEntity("SURFACE");
}


void RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface::Share(const Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbiElem1 = ent->NbControlPointsListI();
	Standard_Integer nbjElem1 = ent->NbControlPointsListJ();
	for (Standard_Integer is1=1; is1<=nbiElem1; is1 ++) {
	  for (Standard_Integer js1=1; js1<=nbjElem1; js1 ++) {
	    iter.GetOneItem(ent->ControlPointsListValue(is1,js1));
	  }
	}

}



void RWStepGeom_RWBSplineSurfaceWithKnotsAndRationalBSplineSurface::Check
  (const Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)& ent,
   const Interface_ShareTool& aShto,
   Handle(Interface_Check)& ach) const
{
  Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface) aRationalBSS = ent;
  Handle(StepGeom_BSplineSurfaceWithKnots) aBSSWK =
    aRationalBSS->BSplineSurfaceWithKnots();
  RWStepGeom_RWBSplineSurfaceWithKnots t1;
  t1.Check(aBSSWK,aShto,ach);
  Handle(StepGeom_RationalBSplineSurface) aRBSS =
    aRationalBSS->RationalBSplineSurface();
  RWStepGeom_RWRationalBSplineSurface t2;
  t2.Check(aRBSS,aShto,ach);
}
