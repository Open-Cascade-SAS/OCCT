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
#include <RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface.hxx>
#include <StepData_Logical.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BSplineSurfaceForm.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_HArray2OfCartesianPoint.hxx>
#include <StepGeom_QuasiUniformSurface.hxx>
#include <StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface.hxx>
#include <StepGeom_RationalBSplineSurface.hxx>
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

RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface::RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface () {}

void RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num0,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface)& ent) const
{

	Standard_Integer num = num0;


	// --- Instance of plex componant BoundedSurface ---

	if (!data->CheckNbParams(num,0,ach,"bounded_surface")) return;

	num = data->NextForComplex(num);

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
	    Standard_Integer nsi3;
	    if (data->ReadSubList (nsub3,i3,"sub-part(control_points_list)",ach,nsi3)) {
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


	StepGeom_BSplineSurfaceForm aSurfaceForm =  StepGeom_bssfPlaneSurf;
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

	num = data->NextForComplex(num);

	// --- Instance of plex componant GeometricRepresentationItem ---

	if (!data->CheckNbParams(num,0,ach,"geometric_representation_item")) return;

	num = data->NextForComplex(num);

	// --- Instance of plex componant QuasiUniformSurface ---

	if (!data->CheckNbParams(num,0,ach,"quasi_uniform_surface")) return;

	num = data->NextForComplex(num);

	// --- Instance of plex componant RationalBSplineSurface ---

	if (!data->CheckNbParams(num,1,ach,"rational_b_spline_surface")) return;

	// --- field : weightsData ---

	Handle(TColStd_HArray2OfReal) aWeightsData;
	  Standard_Real aWeightsDataItem;
	Standard_Integer nsub8;
	if (data->ReadSubList (num,1,"weights_data",ach,nsub8)) {
	  Standard_Integer nbi8 = data->NbParams(nsub8);
	  Standard_Integer nbj8 = data->NbParams(data->ParamNumber(nsub8,1));
	  aWeightsData = new TColStd_HArray2OfReal (1,nbi8,1,nbj8);
	  for (Standard_Integer i8 = 1; i8 <= nbi8; i8 ++) {
	    Standard_Integer nsi8;
	    if (data->ReadSubList (nsub8,i8,"sub-part(weights_data)",ach,nsi8)) {
	      for (Standard_Integer j8 =1; j8 <= nbj8; j8 ++) {
		//szv#4:S4163:12Mar99 `Standard_Boolean stat8 =` not needed
	        if (data->ReadReal (nsi8,j8,"weights_data",ach,aWeightsDataItem))
		  aWeightsData->SetValue(i8,j8,aWeightsDataItem);
	      }
	    }
	  }
	}

	num = data->NextForComplex(num);

	// --- Instance of plex componant RepresentationItem ---

	if (!data->CheckNbParams(num,1,ach,"representation_item")) return;

	// --- field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat9 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	num = data->NextForComplex(num);

	// --- Instance of plex componant Surface ---

	if (!data->CheckNbParams(num,0,ach,"surface")) return;

	//--- Initialisation of the red entity ---

	ent->Init(aName,aUDegree,aVDegree,aControlPointsList,aSurfaceForm,aUClosed,aVClosed,aSelfIntersect,aWeightsData);
}


void RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface)& ent) const
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

	// --- Instance of plex componant GeometricRepresentationItem ---

	SW.StartEntity("GEOMETRIC_REPRESENTATION_ITEM");

	// --- Instance of plex componant QuasiUniformSurface ---

	SW.StartEntity("QUASI_UNIFORM_SURFACE");

	// --- Instance of plex componant RationalBSplineSurface ---

	SW.StartEntity("RATIONAL_B_SPLINE_SURFACE");
	// --- field : weightsData ---

	SW.OpenSub();
	for (Standard_Integer i8 = 1;  i8 <= ent->NbWeightsDataI(); i8 ++) {
	  SW.NewLine(Standard_False);
	  SW.OpenSub();
	  for (Standard_Integer j8 = 1;  j8 <= ent->NbWeightsDataJ(); j8 ++) {
	    SW.Send(ent->WeightsDataValue(i8,j8));
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


void RWStepGeom_RWQuasiUniformSurfaceAndRationalBSplineSurface::Share(const Handle(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbiElem1 = ent->NbControlPointsListI();
	Standard_Integer nbjElem1 = ent->NbControlPointsListJ();
	for (Standard_Integer is1=1; is1<=nbiElem1; is1 ++) {
	  for (Standard_Integer js1=1; js1<=nbjElem1; js1 ++) {
	    iter.GetOneItem(ent->ControlPointsListValue(is1,js1));
	  }
	}

}

