// Created on: 1993-07-02
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

//:p0 abv 19.02.99: management of 'done' flag improved
//:j7 abv 05.04.99: S4136: ass-tol2.stp #9861: avoid using CheckSurfaceClosure
//    rln 02.06.99 removing #include <StepToGeom_CheckSurfaceClosure.hxx>

#include <StepToGeom_MakeBoundedSurface.ixx>

#include <Geom_BSplineSurface.hxx>

//rln 02.06.99 #include <StepToGeom_CheckSurfaceClosure.hxx>

#include <StepToGeom_MakeBoundedSurface.hxx>
#include <StepToGeom_MakeBSplineSurface.hxx>
#include <StepToGeom_MakeRectangularTrimmedSurface.hxx>

#include <StepGeom_RectangularTrimmedSurface.hxx>
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <StepGeom_BSplineSurfaceWithKnots.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

#include <StepGeom_BezierSurface.hxx>
#include <StepGeom_UniformSurface.hxx>
#include <StepGeom_QuasiUniformSurface.hxx>
#include <StepGeom_UniformSurfaceAndRationalBSplineSurface.hxx>
#include <StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface.hxx>
#include <StepGeom_KnotType.hxx>

//=============================================================================
// Creation d' une BoundedSurface de Geom a partir d' une BoundedSurface
// de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeBoundedSurface::Convert (const Handle(StepGeom_BoundedSurface)& SS, Handle(Geom_BoundedSurface)& CS)
{
  if (SS->IsKind(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface))) { 
    const Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface) BS =
      Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)::DownCast(SS);
	return StepToGeom_MakeBSplineSurface::Convert(BS,*((Handle(Geom_BSplineSurface)*)&CS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnots))) {
    const Handle(StepGeom_BSplineSurfaceWithKnots) BS
      = Handle(StepGeom_BSplineSurfaceWithKnots)::DownCast(SS);
	return StepToGeom_MakeBSplineSurface::Convert(BS,*((Handle(Geom_BSplineSurface)*)&CS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_RectangularTrimmedSurface))) {
    const Handle(StepGeom_RectangularTrimmedSurface) Sur = 
      Handle(StepGeom_RectangularTrimmedSurface)::DownCast(SS);
    return StepToGeom_MakeRectangularTrimmedSurface::Convert(Sur,*((Handle(Geom_RectangularTrimmedSurface)*)&CS));
  }
  // STEP BezierSurface, UniformSurface and QuasiUniformSurface are transformed
  // into STEP BSplineSurface before being mapped onto CAS.CADE/SF
  if (SS->IsKind(STANDARD_TYPE(StepGeom_BezierSurface))) {
    const Handle(StepGeom_BezierSurface) BzS = Handle(StepGeom_BezierSurface)::DownCast(SS);
    const Handle(StepGeom_BSplineSurfaceWithKnots) BSPL = new StepGeom_BSplineSurfaceWithKnots;
    BSPL->SetUDegree(BzS->UDegree());
    BSPL->SetVDegree(BzS->VDegree());
    BSPL->SetControlPointsList(BzS->ControlPointsList());
    BSPL->SetSurfaceForm(BzS->SurfaceForm());
    BSPL->SetUClosed(BzS->UClosed());
    BSPL->SetVClosed(BzS->VClosed());
    BSPL->SetSelfIntersect(BzS->SelfIntersect());
    // Compute Knots and KnotsMultiplicity
    const Handle(TColStd_HArray1OfInteger) UKmult = new TColStd_HArray1OfInteger(1,2);
    const Handle(TColStd_HArray1OfInteger) VKmult = new TColStd_HArray1OfInteger(1,2);
    const Handle(TColStd_HArray1OfReal) UKnots = new TColStd_HArray1OfReal(1,2);
    const Handle(TColStd_HArray1OfReal) VKnots = new TColStd_HArray1OfReal(1,2);
    UKmult->SetValue(1, BzS->UDegree() + 1);
    UKmult->SetValue(2, BzS->UDegree() + 1);
    VKmult->SetValue(1, BzS->VDegree() + 1);
    VKmult->SetValue(2, BzS->VDegree() + 1);
    UKnots->SetValue(1, 0.);
    UKnots->SetValue(2, 1.);
    VKnots->SetValue(1, 0.);
    VKnots->SetValue(2, 1.);
    BSPL->SetUMultiplicities(UKmult);
    BSPL->SetVMultiplicities(VKmult);
    BSPL->SetUKnots(UKnots);
    BSPL->SetVKnots(VKnots);
	return StepToGeom_MakeBSplineSurface::Convert(BSPL,*((Handle(Geom_BSplineSurface)*)&CS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_UniformSurface))) {
    const Handle(StepGeom_UniformSurface) US = Handle(StepGeom_UniformSurface)::DownCast(SS);
    const Handle(StepGeom_BSplineSurfaceWithKnots) BSPL = new StepGeom_BSplineSurfaceWithKnots;
    BSPL->SetUDegree(US->UDegree());
    BSPL->SetVDegree(US->VDegree());
    BSPL->SetControlPointsList(US->ControlPointsList());
    BSPL->SetSurfaceForm(US->SurfaceForm());
    BSPL->SetUClosed(US->UClosed());
    BSPL->SetVClosed(US->VClosed());
    BSPL->SetSelfIntersect(US->SelfIntersect());
    // Compute Knots and KnotsMultiplicity for U Direction
    const Standard_Integer nbKU = BSPL->NbControlPointsListI() + BSPL->UDegree() + 1;
    const Handle(TColStd_HArray1OfInteger) UKmult = new TColStd_HArray1OfInteger(1,nbKU);
    const Handle(TColStd_HArray1OfReal) UKnots = new TColStd_HArray1OfReal(1,nbKU);
    for (Standard_Integer iU = 1 ; iU <= nbKU ; iU ++) {
      UKmult->SetValue(iU, 1);
      UKnots->SetValue(iU, iU - 1.);
    }
    BSPL->SetUMultiplicities(UKmult);
    BSPL->SetUKnots(UKnots);
    // Compute Knots and KnotsMultiplicity for V Direction
    const Standard_Integer nbKV = BSPL->NbControlPointsListJ() + BSPL->VDegree() + 1;
    const Handle(TColStd_HArray1OfInteger) VKmult = new TColStd_HArray1OfInteger(1,nbKV);
    const Handle(TColStd_HArray1OfReal) VKnots = new TColStd_HArray1OfReal(1,nbKV);
    for (Standard_Integer iV = 1 ; iV <= nbKV ; iV ++) {
      VKmult->SetValue(iV, 1);
      VKnots->SetValue(iV, iV - 1.);
    }
    BSPL->SetVMultiplicities(VKmult);
    BSPL->SetVKnots(VKnots);
	return StepToGeom_MakeBSplineSurface::Convert(BSPL,*((Handle(Geom_BSplineSurface)*)&CS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_QuasiUniformSurface))) {
    const Handle(StepGeom_QuasiUniformSurface) QUS = 
      Handle(StepGeom_QuasiUniformSurface)::DownCast(SS);
    const Handle(StepGeom_BSplineSurfaceWithKnots) BSPL = new StepGeom_BSplineSurfaceWithKnots;
    BSPL->SetUDegree(QUS->UDegree());
    BSPL->SetVDegree(QUS->VDegree());
    BSPL->SetControlPointsList(QUS->ControlPointsList());
    BSPL->SetSurfaceForm(QUS->SurfaceForm());
    BSPL->SetUClosed(QUS->UClosed());
    BSPL->SetVClosed(QUS->VClosed());
    BSPL->SetSelfIntersect(QUS->SelfIntersect());
    // Compute Knots and KnotsMultiplicity for U Direction
    const Standard_Integer nbKU = BSPL->NbControlPointsListI() - BSPL->UDegree() + 1;
    const Handle(TColStd_HArray1OfInteger) UKmult = new TColStd_HArray1OfInteger(1,nbKU);
    const Handle(TColStd_HArray1OfReal) UKnots = new TColStd_HArray1OfReal(1,nbKU);
    for (Standard_Integer iU = 1 ; iU <= nbKU ; iU ++) {
      UKmult->SetValue(iU, 1);
      UKnots->SetValue(iU, iU - 1.);
    }
    UKmult->SetValue(1, BSPL->UDegree() + 1);
    UKmult->SetValue(nbKU, BSPL->UDegree() + 1);
    BSPL->SetUMultiplicities(UKmult);
    BSPL->SetUKnots(UKnots);
    // Compute Knots and KnotsMultiplicity for V Direction
    const Standard_Integer nbKV = BSPL->NbControlPointsListJ() - BSPL->VDegree() + 1;
    const Handle(TColStd_HArray1OfInteger) VKmult = new TColStd_HArray1OfInteger(1,nbKV);
    const Handle(TColStd_HArray1OfReal) VKnots = new TColStd_HArray1OfReal(1,nbKV);
    for (Standard_Integer iV = 1 ; iV <= nbKV ; iV ++) {
      VKmult->SetValue(iV, 1);
      VKnots->SetValue(iV, iV - 1.);
    }
    VKmult->SetValue(1, BSPL->VDegree() + 1);
    VKmult->SetValue(nbKV, BSPL->VDegree() + 1);
    BSPL->SetVMultiplicities(VKmult);
    BSPL->SetVKnots(VKnots);
	return StepToGeom_MakeBSplineSurface::Convert(BSPL,*((Handle(Geom_BSplineSurface)*)&CS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_UniformSurfaceAndRationalBSplineSurface))) {
    const Handle(StepGeom_UniformSurfaceAndRationalBSplineSurface) RUS = 
      Handle(StepGeom_UniformSurfaceAndRationalBSplineSurface)::DownCast(SS);
    const Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface) RBSPL = 
      new StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface;
    // Compute Knots and KnotsMultiplicity for U Direction
    const Standard_Integer nbKU = RUS->NbControlPointsListI() + RUS->UDegree() + 1;
    const Handle(TColStd_HArray1OfInteger) UKmult = new TColStd_HArray1OfInteger(1,nbKU);
    const Handle(TColStd_HArray1OfReal) UKnots = new TColStd_HArray1OfReal(1,nbKU);
    for (Standard_Integer iU = 1 ; iU <= nbKU ; iU ++) {
      UKmult->SetValue(iU, 1);
      UKnots->SetValue(iU, iU - 1.);
    }
    // Compute Knots and KnotsMultiplicity for V Direction
    const Standard_Integer nbKV = RUS->NbControlPointsListJ() + RUS->VDegree() + 1;
    const Handle(TColStd_HArray1OfInteger) VKmult = new TColStd_HArray1OfInteger(1,nbKV);
    const Handle(TColStd_HArray1OfReal) VKnots = new TColStd_HArray1OfReal(1,nbKV);
    for (Standard_Integer iV = 1 ; iV <= nbKV ; iV ++) {
      VKmult->SetValue(iV, 1);
      VKnots->SetValue(iV, iV - 1.);
    }
    // Initialize the BSplineSurfaceWithKnotsAndRationalBSplineSurface
    RBSPL->Init(RUS->Name(), RUS->UDegree(), RUS->VDegree(), 
		RUS->ControlPointsList(), RUS->SurfaceForm(),
		RUS->UClosed(), RUS->VClosed(), RUS->SelfIntersect(), 
		UKmult, VKmult, UKnots, VKnots, StepGeom_ktUnspecified,
		RUS->WeightsData());
	return StepToGeom_MakeBSplineSurface::Convert(RBSPL,*((Handle(Geom_BSplineSurface)*)&CS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface))) {
    const Handle(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface) RQUS = 
      Handle(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface)::DownCast(SS);
    const Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface) RBSPL = 
      new StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface;
    // Compute Knots and KnotsMultiplicity for U Direction
    const Standard_Integer nbKU = RQUS->NbControlPointsListI() - RQUS->UDegree() + 1;
    const Handle(TColStd_HArray1OfInteger) UKmult = new TColStd_HArray1OfInteger(1,nbKU);
    const Handle(TColStd_HArray1OfReal) UKnots = new TColStd_HArray1OfReal(1,nbKU);
    for (Standard_Integer iU = 1 ; iU <= nbKU ; iU ++) {
      UKmult->SetValue(iU, 1);
      UKnots->SetValue(iU, iU - 1.);
    }
    UKmult->SetValue(1, RQUS->UDegree() + 1);
    UKmult->SetValue(nbKU, RQUS->UDegree() + 1);
    // Compute Knots and KnotsMultiplicity for V Direction
    const Standard_Integer nbKV = RQUS->NbControlPointsListJ() - RQUS->VDegree() + 1;
    const Handle(TColStd_HArray1OfInteger) VKmult = new TColStd_HArray1OfInteger(1,nbKV);
    const Handle(TColStd_HArray1OfReal) VKnots = new TColStd_HArray1OfReal(1,nbKV);
    for (Standard_Integer iV = 1 ; iV <= nbKV ; iV ++) {
      VKmult->SetValue(iV, 1);
      VKnots->SetValue(iV, iV - 1.);
    }
    VKmult->SetValue(1, RQUS->VDegree() + 1);
    VKmult->SetValue(nbKV, RQUS->VDegree() + 1);
    // Initialize the BSplineSurfaceWithKnotsAndRationalBSplineSurface
    RBSPL->Init(RQUS->Name(), RQUS->UDegree(), RQUS->VDegree(), RQUS->ControlPointsList(), 
		RQUS->SurfaceForm(), RQUS->UClosed(), RQUS->VClosed(), 
		RQUS->SelfIntersect(), UKmult, VKmult, UKnots, VKnots, StepGeom_ktUnspecified,
		RQUS->WeightsData());
	return StepToGeom_MakeBSplineSurface::Convert(RBSPL,*((Handle(Geom_BSplineSurface)*)&CS));
  }
/* //:S4136: ass-tol2.stp #9861
  // UPDATE FMA 15-03-96
  // For BSplineSurface, the surface could be U and/or VClosed within the 
  // file tolerance. In this case, the closure flag is enforced
  
  if (done && //:i6
      theBoundedSurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) {

    Handle(Geom_BSplineSurface) theBSP = 
      Handle(Geom_BSplineSurface)::DownCast(theBoundedSurface);
    if (!theBSP->IsURational() &&
	!theBSP->IsVRational()) {
      StepToGeom_CheckSurfaceClosure CheckClose(theBSP);
      theBoundedSurface = CheckClose.Surface();
    }
  }
*/  
  return Standard_False;
}	 
