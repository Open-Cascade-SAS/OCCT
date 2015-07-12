// Created on: 1999-02-15
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
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

//S4132: reading complex type bounded_curve + surface_curve 
//       which is necessary for reading curve_bounded_surfaces
//       This class is based on RWSurfaceCurve

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepGeom_RWSurfaceCurve.hxx>
#include <RWStepGeom_RWSurfaceCurveAndBoundedCurve.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_HArray1OfPcurveOrSurface.hxx>
#include <StepGeom_PcurveOrSurface.hxx>
#include <StepGeom_PreferredSurfaceCurveRepresentation.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepGeom_SurfaceCurveAndBoundedCurve.hxx>
#include <TCollection_AsciiString.hxx>

// --- Enum : PreferredSurfaceCurveRepresentation ---
static TCollection_AsciiString pscrPcurveS2(".PCURVE_S2.");
static TCollection_AsciiString pscrPcurveS1(".PCURVE_S1.");
static TCollection_AsciiString pscrCurve3d(".CURVE_3D.");

RWStepGeom_RWSurfaceCurveAndBoundedCurve::RWStepGeom_RWSurfaceCurveAndBoundedCurve () {}

void RWStepGeom_RWSurfaceCurveAndBoundedCurve::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_SurfaceCurveAndBoundedCurve)& ent) const
{

  // BOUNDED_CURVE: skip 
  Standard_Integer num1 = num;

  // CURVE: skip 
  num1 = data->NextForComplex (num1);

  // GEOMETRIC_REPRESENTATION_ITEM: skip 
  num1 = data->NextForComplex (num1);

  // REPRESENTATION_ITEM: read name
  num1 = data->NextForComplex (num1);
  if ( ! data->CheckNbParams ( num1, 1, ach, "representation_item" ) ) return;

  Handle(TCollection_HAsciiString) aName;
  data->ReadString ( num1, 1, "name", ach, aName );

  // SURFACE_CURVE: read data
  num1 = data->NextForComplex (num1);
  if ( ! data->CheckNbParams ( num1, 3, ach, "surface_curve" ) ) return;

  // --- own field : curve3d ---
  Handle(StepGeom_Curve) aCurve3d;
  data->ReadEntity ( num1, 1, "curve_3d", ach, STANDARD_TYPE(StepGeom_Curve), aCurve3d );

  // --- own field : associatedGeometry ---
  Handle(StepGeom_HArray1OfPcurveOrSurface) aAssociatedGeometry;
  StepGeom_PcurveOrSurface aAssociatedGeometryItem;
  Standard_Integer nsub3;
  if ( data->ReadSubList ( num1, 2, "associated_geometry", ach, nsub3 ) ) {
    Standard_Integer nb3 = data->NbParams(nsub3);
    aAssociatedGeometry = new StepGeom_HArray1OfPcurveOrSurface (1, nb3);
    for (Standard_Integer i3 = 1; i3 <= nb3; i3 ++) {
      //szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
      if (data->ReadEntity (nsub3,i3,"associated_geometry",ach,aAssociatedGeometryItem))
	aAssociatedGeometry->SetValue(i3,aAssociatedGeometryItem);
    }
  }

  // --- own field : masterRepresentation ---
  StepGeom_PreferredSurfaceCurveRepresentation aMasterRepresentation = StepGeom_pscrCurve3d;
  if ( data->ParamType ( num1, 3 ) == Interface_ParamEnum ) {
    Standard_CString text = data->ParamCValue ( num1, 3 );
    if      (pscrPcurveS2.IsEqual(text)) aMasterRepresentation = StepGeom_pscrPcurveS2;
    else if (pscrPcurveS1.IsEqual(text)) aMasterRepresentation = StepGeom_pscrPcurveS1;
    else if ( pscrCurve3d.IsEqual(text)) aMasterRepresentation = StepGeom_pscrCurve3d;
    else ach->AddFail("Enumeration preferred_surface_curve_representation has not an allowed value");
  }
  else ach->AddFail("Parameter #3 (master_representation) is not an enumeration");

  //--- Initialisation of the read entity ---

  ent->Init(aName, aCurve3d, aAssociatedGeometry, aMasterRepresentation);
  ent->BoundedCurve() = new StepGeom_BoundedCurve;
  ent->BoundedCurve()->Init (aName);
}


void RWStepGeom_RWSurfaceCurveAndBoundedCurve::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_SurfaceCurveAndBoundedCurve)& ent) const
{

  SW.StartEntity("BOUNDED_CURVE");
  SW.StartEntity("CURVE");
  SW.StartEntity("GEOMETRIC_REPRESENTATION_ITEM");

  SW.StartEntity("REPRESENTATION_ITEM");
  SW.Send(ent->Name());

  // --- Instance of plex componant BoundedCurve ---
  SW.StartEntity("SURFACE_CURVE");
  
  // --- own field : curve3d ---
  SW.Send(ent->Curve3d());

  // --- own field : associatedGeometry ---
  SW.OpenSub();
  for (Standard_Integer i3 = 1;  i3 <= ent->NbAssociatedGeometry();  i3 ++) {
    if (!ent->AssociatedGeometryValue(i3).Value().IsNull()) {
      SW.Send(ent->AssociatedGeometryValue(i3).Value());
    }
  }
  SW.CloseSub();

  // --- own field : masterRepresentation ---
  switch(ent->MasterRepresentation()) {
  case StepGeom_pscrPcurveS2 : SW.SendEnum (pscrPcurveS2); break;
  case StepGeom_pscrPcurveS1 : SW.SendEnum (pscrPcurveS1); break;
  case StepGeom_pscrCurve3d : SW.SendEnum (pscrCurve3d); break;
  }
}


void RWStepGeom_RWSurfaceCurveAndBoundedCurve::Share(const Handle(StepGeom_SurfaceCurveAndBoundedCurve)& ent, 
						     Interface_EntityIterator& iter) const
{
  RWStepGeom_RWSurfaceCurve tool;
  tool.Share ( ent, iter );
}

