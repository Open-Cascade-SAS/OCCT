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
#include <RWStepGeom_RWRationalBSplineSurface.hxx>
#include <StepData_Logical.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BSplineSurfaceForm.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_HArray2OfCartesianPoint.hxx>
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

RWStepGeom_RWRationalBSplineSurface::RWStepGeom_RWRationalBSplineSurface () {}

void RWStepGeom_RWRationalBSplineSurface::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_RationalBSplineSurface)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,9,ach,"rational_b_spline_surface")) return;

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

	// --- own field : weightsData ---

	Handle(TColStd_HArray2OfReal) aWeightsData;
	  Standard_Real aWeightsDataItem;
	Standard_Integer nsub9;
	if (data->ReadSubList (num,9,"weights_data",ach,nsub9)) {
	  Standard_Integer nbi9 = data->NbParams(nsub9);
	  Standard_Integer nbj9 = data->NbParams(data->ParamNumber(nsub9,1));
	  aWeightsData = new TColStd_HArray2OfReal (1,nbi9,1,nbj9);
	  for (Standard_Integer i9 = 1; i9 <= nbi9; i9 ++) {
	    Standard_Integer nsi9;
	    if (data->ReadSubList (nsub9,i9,"sub-part(weights_data)",ach,nsi9)) {
	      for (Standard_Integer j9 =1; j9 <= nbj9; j9 ++) {
		//szv#4:S4163:12Mar99 `Standard_Boolean stat9 =` not needed
	        if (data->ReadReal (nsi9,j9,"weights_data",ach,aWeightsDataItem))
		  aWeightsData->SetValue(i9,j9,aWeightsDataItem);
	      }
	    }
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, aUClosed, aVClosed, aSelfIntersect, aWeightsData);
}


void RWStepGeom_RWRationalBSplineSurface::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_RationalBSplineSurface)& ent) const
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

	// --- own field : weightsData ---

	SW.OpenSub();
	for (Standard_Integer i9 = 1;  i9 <= ent->NbWeightsDataI(); i9 ++) {
	  SW.NewLine(Standard_False);
	  SW.OpenSub();
	  for (Standard_Integer j9 = 1;  j9 <= ent->NbWeightsDataJ(); j9 ++) {
	    SW.Send(ent->WeightsDataValue(i9,j9));
	    SW.JoinLast(Standard_False);
	  }
	  SW.CloseSub();
	}
	SW.CloseSub();
}


void RWStepGeom_RWRationalBSplineSurface::Share(const Handle(StepGeom_RationalBSplineSurface)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbiElem1 = ent->NbControlPointsListI();
	Standard_Integer nbjElem1 = ent->NbControlPointsListJ();
	for (Standard_Integer is1=1; is1<=nbiElem1; is1 ++) {
	  for (Standard_Integer js1=1; js1<=nbjElem1; js1 ++) {
	    iter.GetOneItem(ent->ControlPointsListValue(is1,js1));
	  }
	}

}



void RWStepGeom_RWRationalBSplineSurface::Check
  (const Handle(StepGeom_RationalBSplineSurface)& ent,
   const Interface_ShareTool& ,
   Handle(Interface_Check)& ach) const
{
  Standard_Integer nbWgtU = ent->NbWeightsDataI();
  Standard_Integer nbWgtV = ent->NbWeightsDataJ();
  Standard_Integer nbCPLU = ent->NbControlPointsListI();
  Standard_Integer nbCPLV = ent->NbControlPointsListJ();
  Standard_Integer i;
  Standard_Integer j;
//  std::cout << "RationalBSplineCurve: nbWgtU=" << nbWgtU << " nbCPLU: " << 
//    nbCPLU << std::endl;
//  std::cout << "                      nbWgtV=" << nbWgtV << " nbCPLV: " << 
//    nbCPLV << std::endl;
  if(nbWgtU != nbCPLU) {
    ach->AddFail("ERROR: No.of ControlPoints not equal No.of Weights in U");
  }
  if(nbWgtV != nbCPLV) {
    ach->AddFail("ERROR: No.of ControlPoints not equal No.of Weights in V");
  }
  for(i=1; i<=nbWgtU; i++) {
    for(j=1; j<=nbWgtV; j++) {
      if(ent->WeightsDataValue(i,j) < RealEpsilon()) {
	ach->AddFail("ERROR: WeightsData Value not greater than 0.0");
      }
    }
  }
}
