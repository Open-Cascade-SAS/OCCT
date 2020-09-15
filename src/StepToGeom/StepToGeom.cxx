// Created on: 1993-06-15
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

#include <StepToGeom.hxx>

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <ElCLib.hxx>

#include <Geom_Axis1Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Conic.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Direction.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_VectorWithMagnitude.hxx>

#include <Geom2d_AxisPlacement.hxx>
#include <Geom2d_BoundedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Direction.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_VectorWithMagnitude.hxx>
#include <Geom2dConvert.hxx>

#include <gp_Trsf.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>

#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeAnalysis_Curve.hxx>

#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <StepGeom_BoundedSurface.hxx>
#include <StepGeom_BSplineCurve.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

#include <StepGeom_BezierCurve.hxx>
#include <StepGeom_BezierSurface.hxx>
#include <StepGeom_BSplineSurface.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <StepGeom_Circle.hxx>
#include <StepGeom_Conic.hxx>
#include <StepGeom_ConicalSurface.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_CurveReplica.hxx>
#include <StepGeom_CylindricalSurface.hxx>
#include <StepGeom_ElementarySurface.hxx>
#include <StepGeom_Ellipse.hxx>
#include <StepGeom_Hyperbola.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_OffsetCurve3d.hxx>
#include <StepGeom_OffsetSurface.hxx>
#include <StepGeom_Parabola.hxx>
#include <StepGeom_Plane.hxx>
#include <StepGeom_Polyline.hxx>
#include <StepGeom_QuasiUniformCurve.hxx>
#include <StepGeom_QuasiUniformCurveAndRationalBSplineCurve.hxx>
#include <StepGeom_QuasiUniformSurface.hxx>
#include <StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface.hxx>
#include <StepGeom_RectangularTrimmedSurface.hxx>
#include <StepGeom_SphericalSurface.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepGeom_SurfaceOfLinearExtrusion.hxx>
#include <StepGeom_SurfaceOfRevolution.hxx>
#include <StepGeom_SurfaceReplica.hxx>
#include <StepGeom_SweptSurface.hxx>
#include <StepGeom_ToroidalSurface.hxx>
#include <StepGeom_CartesianTransformationOperator2d.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepGeom_UniformCurve.hxx>
#include <StepGeom_UniformCurveAndRationalBSplineCurve.hxx>
#include <StepGeom_UniformSurface.hxx>
#include <StepGeom_UniformSurfaceAndRationalBSplineSurface.hxx>
#include <StepGeom_Vector.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un Ax1Placement de Geom a partir d' un axis1_placement de Step
//=============================================================================

Handle(Geom_Axis1Placement) StepToGeom::MakeAxis1Placement (const Handle(StepGeom_Axis1Placement)& SA)
{
  Handle(Geom_CartesianPoint) P = MakeCartesianPoint (SA->Location());
  if (! P.IsNull())
  {
    // sln 22.10.2001. CTS23496: If problems with creation of axis direction occur default direction is used
    gp_Dir D(0.,0.,1.);
    if (SA->HasAxis())
    {
      Handle(Geom_Direction) D1 = MakeDirection (SA->Axis());
      if (! D1.IsNull())
        D = D1->Dir();
    }
    return new Geom_Axis1Placement(P->Pnt(),D);
  }
  return 0;
}

//=============================================================================
// Creation d' un Axis2Placement de Geom a partir d' un axis2_placement_3d de Step
//=============================================================================

Handle(Geom_Axis2Placement) StepToGeom::MakeAxis2Placement (const Handle(StepGeom_Axis2Placement3d)& SA)
{
  Handle(Geom_CartesianPoint) P = MakeCartesianPoint (SA->Location());
  if (! P.IsNull())
  {
    const gp_Pnt Pgp = P->Pnt();

    // sln 22.10.2001. CTS23496: If problems with creation of direction occur default direction is used (MakeLine(...) function)
    gp_Dir Ngp(0.,0.,1.);
    if (SA->HasAxis())
    {
      Handle(Geom_Direction) D = MakeDirection (SA->Axis());
      if (! D.IsNull())
        Ngp = D->Dir();
    }

    gp_Ax2 gpAx2;
    Standard_Boolean isDefaultDirectionUsed = Standard_True;
    if (SA->HasRefDirection())
    {
      Handle(Geom_Direction) D = MakeDirection (SA->RefDirection());
      if (! D.IsNull())
      {
        const gp_Dir Vxgp = D->Dir();
        if (!Ngp.IsParallel(Vxgp,Precision::Angular()))
        {
          gpAx2 = gp_Ax2(Pgp, Ngp, Vxgp);
          isDefaultDirectionUsed = Standard_False;
        }
      }
    }
    if(isDefaultDirectionUsed)
      gpAx2 = gp_Ax2(Pgp, Ngp);

    return new Geom_Axis2Placement(gpAx2);
  }
  return 0;
}

//=============================================================================
// Creation d' un AxisPlacement de Geom2d a partir d' un axis2_placement_3d de Step
//=============================================================================

Handle(Geom2d_AxisPlacement) StepToGeom::MakeAxisPlacement (const Handle(StepGeom_Axis2Placement2d)& SA)
{
  Handle(Geom2d_CartesianPoint) P = MakeCartesianPoint2d (SA->Location());
  if (! P.IsNull())
  {
    // sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is used
    gp_Dir2d Vxgp(1.,0.);
    if (SA->HasRefDirection()) {
      Handle(Geom2d_Direction) Vx = MakeDirection2d (SA->RefDirection());
      if (! Vx.IsNull())
        Vxgp = Vx->Dir2d();
    }

    return new Geom2d_AxisPlacement(P->Pnt2d(),Vxgp);
  }
  return 0;
}

//=============================================================================
// Creation d' une BoundedCurve de Geom a partir d' une BoundedCurve de Step
//=============================================================================

Handle(Geom_BoundedCurve) StepToGeom::MakeBoundedCurve (const Handle(StepGeom_BoundedCurve)& SC)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)))
  {
    return MakeBSplineCurve (Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnots)))
  {
    return MakeBSplineCurve (Handle(StepGeom_BSplineCurveWithKnots)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_TrimmedCurve)))
  {
    return MakeTrimmedCurve (Handle(StepGeom_TrimmedCurve)::DownCast(SC));
  }

  // STEP BezierCurve, UniformCurve and QuasiUniformCurve are transformed into
  // STEP BSplineCurve before being mapped onto CAS.CADE/SF
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BezierCurve)))
  {
    const Handle(StepGeom_BezierCurve) BzC = Handle(StepGeom_BezierCurve)::DownCast(SC);
    Standard_Integer aDegree = BzC->Degree();
    if (aDegree < 1 || aDegree > Geom_BSplineCurve::MaxDegree())
      return 0;
    const Handle(StepGeom_BSplineCurveWithKnots) BSPL = new StepGeom_BSplineCurveWithKnots;
    BSPL->SetDegree(aDegree);
    BSPL->SetControlPointsList(BzC->ControlPointsList());
    BSPL->SetCurveForm(BzC->CurveForm());
    BSPL->SetClosedCurve(BzC->ClosedCurve());
    BSPL->SetSelfIntersect(BzC->SelfIntersect());
    // Compute Knots and KnotsMultiplicity
    const Handle(TColStd_HArray1OfInteger) Kmult = new TColStd_HArray1OfInteger(1,2);
    const Handle(TColStd_HArray1OfReal) Knots = new TColStd_HArray1OfReal(1,2);
    Kmult->SetValue(1, BzC->Degree() + 1);
    Kmult->SetValue(2, BzC->Degree() + 1);
    Knots->SetValue(1, 0.);
    Knots->SetValue(2, 1.);
    BSPL->SetKnotMultiplicities(Kmult);
    BSPL->SetKnots(Knots);

    return MakeBSplineCurve (BSPL);
  }

  if (SC->IsKind(STANDARD_TYPE(StepGeom_UniformCurve)))
  {
    const Handle(StepGeom_UniformCurve) UC = Handle(StepGeom_UniformCurve)::DownCast(SC);
    Standard_Integer aDegree = UC->Degree();
    if (aDegree < 1 || aDegree > Geom_BSplineCurve::MaxDegree())
      return 0;
    const Handle(StepGeom_BSplineCurveWithKnots) BSPL = new StepGeom_BSplineCurveWithKnots;
    BSPL->SetDegree(aDegree);
    BSPL->SetControlPointsList(UC->ControlPointsList());
    BSPL->SetCurveForm(UC->CurveForm());
    BSPL->SetClosedCurve(UC->ClosedCurve());
    BSPL->SetSelfIntersect(UC->SelfIntersect());

    // Compute Knots and KnotsMultiplicity
    const Standard_Integer nbK = BSPL->NbControlPointsList() + BSPL->Degree() + 1;
    const Handle(TColStd_HArray1OfInteger) Kmult = new TColStd_HArray1OfInteger(1,nbK);
    const Handle(TColStd_HArray1OfReal) Knots = new TColStd_HArray1OfReal(1,nbK);
    for (Standard_Integer iUC = 1 ; iUC <= nbK ; iUC ++) {
      Kmult->SetValue(iUC, 1);
      Knots->SetValue(iUC, iUC - 1.);
    }
    BSPL->SetKnotMultiplicities(Kmult);
    BSPL->SetKnots(Knots);

    return MakeBSplineCurve (BSPL);
  }

  if (SC->IsKind(STANDARD_TYPE(StepGeom_QuasiUniformCurve)))
  {
    const Handle(StepGeom_QuasiUniformCurve) QUC =
      Handle(StepGeom_QuasiUniformCurve)::DownCast(SC);
    Standard_Integer aDegree = QUC->Degree();
    if (aDegree < 1 || aDegree > Geom_BSplineCurve::MaxDegree())
      return 0;
    const Handle(StepGeom_BSplineCurveWithKnots) BSPL = new StepGeom_BSplineCurveWithKnots;
    BSPL->SetDegree(aDegree);
    BSPL->SetControlPointsList(QUC->ControlPointsList());
    BSPL->SetCurveForm(QUC->CurveForm());
    BSPL->SetClosedCurve(QUC->ClosedCurve());
    BSPL->SetSelfIntersect(QUC->SelfIntersect());

    // Compute Knots and KnotsMultiplicity
    const Standard_Integer nbK = BSPL->NbControlPointsList() - BSPL->Degree() + 1;
    const Handle(TColStd_HArray1OfInteger) Kmult = new TColStd_HArray1OfInteger(1,nbK);
    const Handle(TColStd_HArray1OfReal) Knots = new TColStd_HArray1OfReal(1,nbK);
    for (Standard_Integer iQUC = 1 ; iQUC <= nbK ; iQUC ++) {
      Kmult->SetValue(iQUC, 1);
      Knots->SetValue(iQUC, iQUC - 1.);
    }
    Kmult->SetValue(1, BSPL->Degree() + 1);
    Kmult->SetValue(nbK, BSPL->Degree() + 1);
    BSPL->SetKnotMultiplicities(Kmult);
    BSPL->SetKnots(Knots);

    return MakeBSplineCurve (BSPL);
  }

  if (SC->IsKind(STANDARD_TYPE(StepGeom_UniformCurveAndRationalBSplineCurve)))
  {
    const Handle(StepGeom_UniformCurveAndRationalBSplineCurve) RUC =
      Handle(StepGeom_UniformCurveAndRationalBSplineCurve)::DownCast(SC);
    Standard_Integer aDegree = RUC->Degree();
    if (aDegree < 1 || aDegree > Geom_BSplineCurve::MaxDegree())
      return 0;
    const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve) RBSPL =
      new StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve;

    // Compute Knots and KnotsMultiplicity
    const Standard_Integer nbK = RUC->NbControlPointsList() + aDegree + 1;
    const Handle(TColStd_HArray1OfInteger) Kmult = new TColStd_HArray1OfInteger(1,nbK);
    const Handle(TColStd_HArray1OfReal) Knots = new TColStd_HArray1OfReal(1,nbK);
    for (Standard_Integer iUC = 1 ; iUC <= nbK ; iUC ++) {
      Kmult->SetValue(iUC, 1);
      Knots->SetValue(iUC, iUC - 1.);
    }

    // Initialize the BSplineCurveWithKnotsAndRationalBSplineCurve
    RBSPL->Init(RUC->Name(), aDegree, RUC->ControlPointsList(), RUC->CurveForm(),
		RUC->ClosedCurve(), RUC->SelfIntersect(), Kmult, Knots, StepGeom_ktUnspecified,
		RUC->WeightsData());

    return MakeBSplineCurve (RBSPL);
  }

  if (SC->IsKind(STANDARD_TYPE(StepGeom_QuasiUniformCurveAndRationalBSplineCurve)))
  {
    const Handle(StepGeom_QuasiUniformCurveAndRationalBSplineCurve) RQUC =
      Handle(StepGeom_QuasiUniformCurveAndRationalBSplineCurve)::DownCast(SC);
    Standard_Integer aDegree = RQUC->Degree();
    if (aDegree < 1 || aDegree > Geom_BSplineCurve::MaxDegree())
      return 0;
    const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve) RBSPL =
      new StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve;

    // Compute Knots and KnotsMultiplicity
    const Standard_Integer nbK = RQUC->NbControlPointsList() - aDegree + 1;
    const Handle(TColStd_HArray1OfInteger) Kmult = new TColStd_HArray1OfInteger(1,nbK);
    const Handle(TColStd_HArray1OfReal) Knots = new TColStd_HArray1OfReal(1,nbK);
    for (Standard_Integer iRQUC = 1 ; iRQUC <= nbK ; iRQUC ++) {
      Kmult->SetValue(iRQUC, 1);
      Knots->SetValue(iRQUC, iRQUC - 1.);
    }
    Kmult->SetValue(1, aDegree + 1);
    Kmult->SetValue(nbK, aDegree + 1);
    // Initialize the BSplineCurveWithKnotsAndRationalBSplineCurve
    RBSPL->Init(RQUC->Name(), aDegree, RQUC->ControlPointsList(), RQUC->CurveForm(),
		RQUC->ClosedCurve(), RQUC->SelfIntersect(), Kmult, Knots, StepGeom_ktUnspecified,
		RQUC->WeightsData());

    return MakeBSplineCurve (RBSPL);
  }

  if (SC->IsKind(STANDARD_TYPE(StepGeom_Polyline)))
  { //:n6 abv 15 Feb 99
    return MakePolyline (Handle(StepGeom_Polyline)::DownCast (SC));
  }

  return 0;
}

//=============================================================================
// Creation d' une BoundedCurve de Geom a partir d' une BoundedCurve de Step
//=============================================================================

Handle(Geom2d_BoundedCurve) StepToGeom::MakeBoundedCurve2d (const Handle(StepGeom_BoundedCurve)& SC)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)))
  {
    return MakeBSplineCurve2d (Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnots)))
  {
    return MakeBSplineCurve2d (Handle(StepGeom_BSplineCurveWithKnots)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_TrimmedCurve)))
  {
    return MakeTrimmedCurve2d (Handle(StepGeom_TrimmedCurve)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Polyline)))
  { //:n6 abv 15 Feb 99
    return MakePolyline2d (Handle(StepGeom_Polyline)::DownCast(SC));
  }
  return Handle(Geom2d_BoundedCurve)();
}

//=============================================================================
// Creation d' une BoundedSurface de Geom a partir d' une BoundedSurface de Step
//=============================================================================

Handle(Geom_BoundedSurface) StepToGeom::MakeBoundedSurface (const Handle(StepGeom_BoundedSurface)& SS)
{
  if (SS->IsKind(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)))
  {
    return MakeBSplineSurface (Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)::DownCast(SS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnots)))
  {
    return MakeBSplineSurface (Handle(StepGeom_BSplineSurfaceWithKnots)::DownCast(SS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_RectangularTrimmedSurface)))
  {
    return MakeRectangularTrimmedSurface (Handle(StepGeom_RectangularTrimmedSurface)::DownCast(SS));
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

    return MakeBSplineSurface (BSPL);
  }

  if (SS->IsKind(STANDARD_TYPE(StepGeom_UniformSurface)))
  {
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

    return MakeBSplineSurface (BSPL);
  }

  if (SS->IsKind(STANDARD_TYPE(StepGeom_QuasiUniformSurface)))
  {
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

    return MakeBSplineSurface (BSPL);
  }

  if (SS->IsKind(STANDARD_TYPE(StepGeom_UniformSurfaceAndRationalBSplineSurface)))
  {
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

    return MakeBSplineSurface (RBSPL);
  }

  if (SS->IsKind(STANDARD_TYPE(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface)))
  {
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
    return MakeBSplineSurface (RBSPL);
  }

  return 0;
}

//=============================================================================
// Template function for use in MakeBSplineCurve / MakeBSplineCurve2d
//=============================================================================

template
<
  class TPntArray,
  class TCartesianPoint,
  class TGpPnt,
  class TBSplineCurve
>
Handle(TBSplineCurve) MakeBSplineCurveCommon
(
  const Handle(StepGeom_BSplineCurve)& theStepGeom_BSplineCurve,
  TGpPnt(TCartesianPoint::* thePntGetterFunction)() const,
  Handle(TCartesianPoint) (*thePointMakerFunction)(const Handle(StepGeom_CartesianPoint)&)
)
{
  Handle(StepGeom_BSplineCurveWithKnots) aBSplineCurveWithKnots;
  Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve) aBSplineCurveWithKnotsAndRationalBSplineCurve;

  if (theStepGeom_BSplineCurve->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)))
  {
    aBSplineCurveWithKnotsAndRationalBSplineCurve =
      Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)::DownCast(theStepGeom_BSplineCurve);
    aBSplineCurveWithKnots = aBSplineCurveWithKnotsAndRationalBSplineCurve->BSplineCurveWithKnots();
  }
  else
    aBSplineCurveWithKnots = Handle(StepGeom_BSplineCurveWithKnots)::DownCast(theStepGeom_BSplineCurve);

  const Standard_Integer aDegree = aBSplineCurveWithKnots->Degree();
  const Standard_Integer NbPoles = aBSplineCurveWithKnots->NbControlPointsList();
  const Standard_Integer NbKnots = aBSplineCurveWithKnots->NbKnotMultiplicities();

  const Handle(TColStd_HArray1OfInteger)& aKnotMultiplicities = aBSplineCurveWithKnots->KnotMultiplicities();
  const Handle(TColStd_HArray1OfReal)& aKnots = aBSplineCurveWithKnots->Knots();

  // Count number of unique knots
  Standard_Integer NbUniqueKnots = 0;
  Standard_Real lastKnot = RealFirst();
  for (Standard_Integer i = 1; i <= NbKnots; ++i)
  {
    if (aKnots->Value(i) - lastKnot > Epsilon(Abs(lastKnot)))
    {
      NbUniqueKnots++;
      lastKnot = aKnots->Value(i);
    }
  }
  if (NbUniqueKnots <= 1)
  {
    return 0;
  }
  TColStd_Array1OfReal aUniqueKnots(1, NbUniqueKnots);
  TColStd_Array1OfInteger aUniqueKnotMultiplicities(1, NbUniqueKnots);
  lastKnot = aKnots->Value(1);
  aUniqueKnots.SetValue(1, aKnots->Value(1));
  aUniqueKnotMultiplicities.SetValue(1, aKnotMultiplicities->Value(1));
  Standard_Integer aKnotPosition = 1;
  for (Standard_Integer i = 2; i <= NbKnots; i++)
  {
    if (aKnots->Value(i) - lastKnot > Epsilon(Abs(lastKnot)))
    {
      aKnotPosition++;
      aUniqueKnots.SetValue(aKnotPosition, aKnots->Value(i));
      aUniqueKnotMultiplicities.SetValue(aKnotPosition, aKnotMultiplicities->Value(i));
      lastKnot = aKnots->Value(i);
    }
    else
    {
      // Knot not unique, increase multiplicity
      Standard_Integer aCurrentMultiplicity = aUniqueKnotMultiplicities.Value(aKnotPosition);
      aUniqueKnotMultiplicities.SetValue(aKnotPosition, aCurrentMultiplicity + aKnotMultiplicities->Value(i));
    }
  }

  Standard_Integer aFirstMuultypisityDifference = 0;
  Standard_Integer aLastMuultypisityDifference = 0;
  for (Standard_Integer i = 1; i <= NbUniqueKnots; ++i)
  {
    Standard_Integer aCurrentVal = aUniqueKnotMultiplicities.Value(i);
    if (aCurrentVal > aDegree + 1)
    {
      if (i == 1)
        aFirstMuultypisityDifference = aCurrentVal - aDegree - 1;
      if (i == NbUniqueKnots)
        aLastMuultypisityDifference = aCurrentVal - aDegree - 1;
#ifdef OCCT_DEBUG
      std::cout << "\nWrong multiplicity " << aCurrentVal << " on " << i
        << " knot!" << "\nChanged to " << aDegree + 1 << std::endl;
#endif
      aCurrentVal = aDegree + 1;
    }
    aUniqueKnotMultiplicities.SetValue(i, aCurrentVal);
  }

  const Handle(StepGeom_HArray1OfCartesianPoint)& aControlPointsList = aBSplineCurveWithKnots->ControlPointsList();
  Standard_Integer aSummaryMuultypisityDifference = aFirstMuultypisityDifference + aLastMuultypisityDifference;
  Standard_Integer NbUniquePoles = NbPoles - aSummaryMuultypisityDifference;
  if (NbUniquePoles <= 0)
  {
    return 0;
  }
  TPntArray Poles(1, NbPoles - aSummaryMuultypisityDifference);

  for (Standard_Integer i = 1 + aFirstMuultypisityDifference; i <= NbPoles - aLastMuultypisityDifference; ++i)
  {
    Handle(TCartesianPoint) aPoint = (*thePointMakerFunction)(aControlPointsList->Value(i));
    if (!aPoint.IsNull())
    {
      TCartesianPoint* pPoint = aPoint.get();
      TGpPnt aGpPnt = (pPoint->*thePntGetterFunction)();
      Poles.SetValue(i - aFirstMuultypisityDifference, aGpPnt);
    }
    else
    {
      return 0;
    }
  }

  // --- Does the Curve descriptor LOOKS like a periodic descriptor ? ---
  Standard_Integer aSummaryMuultypisity = 0;
  for (Standard_Integer i = 1; i <= NbUniqueKnots; i++)
  {
    aSummaryMuultypisity += aUniqueKnotMultiplicities.Value(i);
  }

  Standard_Boolean shouldBePeriodic;
  if (aSummaryMuultypisity == (NbPoles + aDegree + 1))
  {
    shouldBePeriodic = Standard_False;
  }
  else if ((aUniqueKnotMultiplicities.Value(1) == aUniqueKnotMultiplicities.Value(NbUniqueKnots)) &&
    ((aSummaryMuultypisity - aUniqueKnotMultiplicities.Value(1)) == NbPoles))
  {
    shouldBePeriodic = Standard_True;
  }
  else
  {  
    // --- What is that ??? ---
    shouldBePeriodic = Standard_False;
  }

  Handle(TBSplineCurve) aBSplineCurve;
  if (theStepGeom_BSplineCurve->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)))
  {
    const Handle(TColStd_HArray1OfReal)& aWeights = aBSplineCurveWithKnotsAndRationalBSplineCurve->WeightsData();
    TColStd_Array1OfReal aUniqueWeights(1, NbPoles - aSummaryMuultypisityDifference);
    for (Standard_Integer i = 1 + aFirstMuultypisityDifference; i <= NbPoles - aLastMuultypisityDifference; ++i)
      aUniqueWeights.SetValue(i - aFirstMuultypisityDifference, aWeights->Value(i));
    aBSplineCurve = new TBSplineCurve(Poles, aUniqueWeights, aUniqueKnots, aUniqueKnotMultiplicities, aDegree, shouldBePeriodic);
  }
  else
  {
    aBSplineCurve = new TBSplineCurve(Poles, aUniqueKnots, aUniqueKnotMultiplicities, aDegree, shouldBePeriodic);
  }

  // abv 04.07.00 CAX-IF TRJ4: trj4_k1_top-md-203.stp #716 (face #581):
  // force periodicity on closed curves
  if (theStepGeom_BSplineCurve->ClosedCurve() && aBSplineCurve->Degree() > 1 && aBSplineCurve->IsClosed())
  {
    aBSplineCurve->SetPeriodic();
  }
  return aBSplineCurve;
}

//=============================================================================
// Creation d' une BSplineCurve de Geom a partir d' une BSplineCurve de Step
//=============================================================================

Handle(Geom_BSplineCurve) StepToGeom::MakeBSplineCurve (const Handle(StepGeom_BSplineCurve)& theStepGeom_BSplineCurve)
{
  return MakeBSplineCurveCommon<TColgp_Array1OfPnt, Geom_CartesianPoint, gp_Pnt, Geom_BSplineCurve>
    (theStepGeom_BSplineCurve, &Geom_CartesianPoint::Pnt, &MakeCartesianPoint);
}

//=============================================================================
// Creation d' une BSplineCurve de Geom2d a partir d' une
// BSplineCurveWithKnotsAndRationalBSplineCurve de Step
//=============================================================================

Handle(Geom2d_BSplineCurve) StepToGeom::MakeBSplineCurve2d (const Handle(StepGeom_BSplineCurve)& theStepGeom_BSplineCurve)
{
  return MakeBSplineCurveCommon<TColgp_Array1OfPnt2d, Geom2d_CartesianPoint, gp_Pnt2d, Geom2d_BSplineCurve>
    (theStepGeom_BSplineCurve, &Geom2d_CartesianPoint::Pnt2d, &MakeCartesianPoint2d);
}

//=============================================================================
// Creation d' une BSplineSurface de Geom a partir d' une
// BSplineSurface de Step
//=============================================================================

Handle(Geom_BSplineSurface) StepToGeom::MakeBSplineSurface (const Handle(StepGeom_BSplineSurface)& SS)
{
  Standard_Integer                    i, j;
  Handle(StepGeom_BSplineSurfaceWithKnots) BS;
  Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface) BSR;

  if (SS->
      IsKind(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface))) {
    BSR =
      Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)
	::DownCast(SS);
    BS = BSR->BSplineSurfaceWithKnots();
  }
  else
    BS = Handle(StepGeom_BSplineSurfaceWithKnots)::DownCast(SS);

  const Standard_Integer UDeg = BS->UDegree();
  const Standard_Integer VDeg = BS->VDegree();
  const Standard_Integer NUPoles = BS->NbControlPointsListI();
  const Standard_Integer NVPoles = BS->NbControlPointsListJ();
  const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList = BS->ControlPointsList();
  TColgp_Array2OfPnt Poles(1,NUPoles,1,NVPoles);
  for (i=1; i<=NUPoles; i++) {
    for (j=1; j<=NVPoles; j++) {
      Handle(Geom_CartesianPoint) P = MakeCartesianPoint (aControlPointsList->Value(i,j));
      if (! P.IsNull())
        Poles.SetValue(i,j,P->Pnt());
      else
        return 0;
    }
  }
  const Standard_Integer NUKnots = BS->NbUMultiplicities();
  const Handle(TColStd_HArray1OfInteger)& aUMultiplicities = BS->UMultiplicities();
  const Handle(TColStd_HArray1OfReal)& aUKnots = BS->UKnots();

  // count number of unique uknots
  Standard_Real lastKnot = RealFirst();
  Standard_Integer NUKnotsUnique = 0;
  for (i=1; i<=NUKnots; i++) {
    if (aUKnots->Value(i) - lastKnot > Epsilon (Abs(lastKnot))) {
      NUKnotsUnique++;
      lastKnot = aUKnots->Value(i);
    }
  }

  // set umultiplicities and uknots
  TColStd_Array1OfInteger UMult(1,NUKnotsUnique);
  TColStd_Array1OfReal KUn(1,NUKnotsUnique);
  Standard_Integer pos = 1;
  lastKnot = aUKnots->Value(1);
  KUn.SetValue(1, aUKnots->Value(1));
  UMult.SetValue(1, aUMultiplicities->Value(1));
  for (i=2; i<=NUKnots; i++) {
    if (aUKnots->Value(i) - lastKnot > Epsilon (Abs(lastKnot))) {
      pos++;
      KUn.SetValue(pos, aUKnots->Value(i));
      UMult.SetValue(pos, aUMultiplicities->Value(i));
      lastKnot = aUKnots->Value(i);
    }
    else {
      // Knot not unique, increase multiplicity
      Standard_Integer curMult = UMult.Value(pos);
      UMult.SetValue(pos, curMult + aUMultiplicities->Value(i));
    }
  }
  const Standard_Integer NVKnots = BS->NbVMultiplicities();
  const Handle(TColStd_HArray1OfInteger)& aVMultiplicities = BS->VMultiplicities();
  const Handle(TColStd_HArray1OfReal)& aVKnots = BS->VKnots();

  // count number of unique vknots
  lastKnot = RealFirst();
  Standard_Integer NVKnotsUnique = 0;
  for (i=1; i<=NVKnots; i++) {
    if (aVKnots->Value(i) - lastKnot > Epsilon (Abs(lastKnot))) {
      NVKnotsUnique++;
      lastKnot = aVKnots->Value(i);
    }
  }

  // set vmultiplicities and vknots
  TColStd_Array1OfInteger VMult(1,NVKnotsUnique);
  TColStd_Array1OfReal KVn(1,NVKnotsUnique);
  pos = 1;
  lastKnot = aVKnots->Value(1);
  KVn.SetValue(1, aVKnots->Value(1));
  VMult.SetValue(1, aVMultiplicities->Value(1));
  for (i=2; i<=NVKnots; i++) {
    if (aVKnots->Value(i) - lastKnot > Epsilon (Abs(lastKnot))) {
      pos++;
      KVn.SetValue(pos, aVKnots->Value(i));
      VMult.SetValue(pos, aVMultiplicities->Value(i));
      lastKnot = aVKnots->Value(i);
    }
    else {
      // Knot not unique, increase multiplicity
      Standard_Integer curMult = VMult.Value(pos);
      VMult.SetValue(pos, curMult + aVMultiplicities->Value(i));
    }
  }

  // --- Does the Surface Descriptor LOOKS like a U and/or V Periodic ---
  // --- Descriptor ? ---

  // --- U Periodic ? ---

  Standard_Integer SumMult = 0;
  for (i=1; i<=NUKnotsUnique; i++) {
    SumMult += UMult.Value(i);
  }

  Standard_Boolean shouldBeUPeriodic = Standard_False;
  if (SumMult == (NUPoles + UDeg + 1)) {
    //shouldBeUPeriodic = Standard_False;
  }
  else if ((UMult.Value(1) ==
	    UMult.Value(NUKnotsUnique)) &&
	   ((SumMult - UMult.Value(1))== NUPoles)) {
    shouldBeUPeriodic = Standard_True;
  }

  // --- V Periodic ? ---

  SumMult = 0;
  for (i=1; i<=NVKnotsUnique; i++) {
    SumMult += VMult.Value(i);
  }

  Standard_Boolean shouldBeVPeriodic = Standard_False;
  if (SumMult == (NVPoles + VDeg + 1)) {
    //shouldBeVPeriodic = Standard_False;
  }
  else if ((VMult.Value(1) ==
	    VMult.Value(NVKnotsUnique)) &&
	   ((SumMult - VMult.Value(1)) == NVPoles)) {
    shouldBeVPeriodic = Standard_True;
  }

  Handle(Geom_BSplineSurface) CS;
  if (SS->IsKind(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface))) {
    const Handle(TColStd_HArray2OfReal)& aWeight = BSR->WeightsData();
    TColStd_Array2OfReal W(1,NUPoles,1,NVPoles);
    for (i=1; i<=NUPoles; i++) {
      for (j=1; j<=NVPoles; j++) {
        W.SetValue(i,j,aWeight->Value(i,j));
      }
    }
    CS = new Geom_BSplineSurface(Poles, W, KUn, KVn, UMult,
                                 VMult, UDeg, VDeg,
                                 shouldBeUPeriodic,
                                 shouldBeVPeriodic);
  }
  else
    CS = new Geom_BSplineSurface(Poles, KUn, KVn, UMult,
                                 VMult, UDeg, VDeg,
                                 shouldBeUPeriodic,
                                 shouldBeVPeriodic);
  return CS;
}

//=============================================================================
// Creation d' un CartesianPoint de Geom a partir d' un CartesianPoint de Step
//=============================================================================

Handle(Geom_CartesianPoint) StepToGeom::MakeCartesianPoint (const Handle(StepGeom_CartesianPoint)& SP)
{
  if (SP->NbCoordinates() == 3)
  {
    const Standard_Real LF = UnitsMethods::LengthFactor();
    const Standard_Real X = SP->CoordinatesValue(1) * LF;
    const Standard_Real Y = SP->CoordinatesValue(2) * LF;
    const Standard_Real Z = SP->CoordinatesValue(3) * LF;
    return new Geom_CartesianPoint(X, Y, Z);
  }
  return 0;
}

//=============================================================================
// Creation d' un CartesianPoint de Geom2d a partir d' un CartesianPoint de
// Step
//=============================================================================

Handle(Geom2d_CartesianPoint) StepToGeom::MakeCartesianPoint2d (const Handle(StepGeom_CartesianPoint)& SP)
{
  if (SP->NbCoordinates() == 2)
  {
    const Standard_Real X = SP->CoordinatesValue(1);
    const Standard_Real Y = SP->CoordinatesValue(2);
    return new Geom2d_CartesianPoint(X, Y);
  }
  return 0;
}

//=============================================================================
// Creation d' un Circle de Geom a partir d' un Circle de Step
//=============================================================================

Handle(Geom_Circle) StepToGeom::MakeCircle (const Handle(StepGeom_Circle)& SC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2)
  {
    Handle(Geom_Axis2Placement) A =
      MakeAxis2Placement (Handle(StepGeom_Axis2Placement3d)::DownCast(AxisSelect.Value()));
    if (! A.IsNull())
    {
      return new Geom_Circle(A->Ax2(),SC->Radius() * UnitsMethods::LengthFactor());
    }
  }
  return 0;
}

//=============================================================================
// Creation d' un Circle de Geom2d a partir d' un Circle de Step
//=============================================================================

Handle(Geom2d_Circle) StepToGeom::MakeCircle2d (const Handle(StepGeom_Circle)& SC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 1) {
    Handle(Geom2d_AxisPlacement) A1 =
      MakeAxisPlacement (Handle(StepGeom_Axis2Placement2d)::DownCast(AxisSelect.Value()));
    if (! A1.IsNull())
    {
      return new Geom2d_Circle (A1->Ax2d(), SC->Radius());
    }
  }
  return 0;
}

//=============================================================================
// Creation d' une Conic de Geom a partir d' une Conic de Step
//=============================================================================

Handle(Geom_Conic) StepToGeom::MakeConic (const Handle(StepGeom_Conic)& SC)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Circle))) {
    return MakeCircle (Handle(StepGeom_Circle)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Ellipse))) {
    return MakeEllipse (Handle(StepGeom_Ellipse)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Hyperbola))) {
    return MakeHyperbola (Handle(StepGeom_Hyperbola)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Parabola))) {
    return MakeParabola (Handle(StepGeom_Parabola)::DownCast(SC));
  }
  // Attention : Other conic shall be implemented !
  return 0;
}

//=============================================================================
// Creation d' une Conic de Geom2d a partir d' une Conic de Step
//=============================================================================

Handle(Geom2d_Conic) StepToGeom::MakeConic2d (const Handle(StepGeom_Conic)& SC)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Circle))) {
    return MakeCircle2d (Handle(StepGeom_Circle)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Ellipse))) {
    return MakeEllipse2d (Handle(StepGeom_Ellipse)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Hyperbola))) {
    return MakeHyperbola2d (Handle(StepGeom_Hyperbola)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Parabola))) {
    return MakeParabola2d (Handle(StepGeom_Parabola)::DownCast(SC));
  }
  // Attention : Other conic shall be implemented !
  return Handle(Geom2d_Conic)();
}

//=============================================================================
// Creation d' une ConicalSurface de Geom a partir d' une ConicalSurface de
// Step
//=============================================================================

Handle(Geom_ConicalSurface) StepToGeom::MakeConicalSurface (const Handle(StepGeom_ConicalSurface)& SS)
{
  Handle(Geom_Axis2Placement) A = MakeAxis2Placement (SS->Position());
  if (! A.IsNull())
  {
    const Standard_Real R = SS->Radius() * UnitsMethods::LengthFactor();
    const Standard_Real Ang = SS->SemiAngle() * UnitsMethods::PlaneAngleFactor();
    //#2(K3-3) rln 12/02/98 ProSTEP ct_turbine-A.stp entity #518, #3571 (gp::Resolution() is too little)
    return new Geom_ConicalSurface(A->Ax2(), Max(Ang, Precision::Angular()), R);
  }
  return 0;
}

//=============================================================================
// Creation d' une Curve de Geom a partir d' une Curve de Step
//=============================================================================

Handle(Geom_Curve) StepToGeom::MakeCurve (const Handle(StepGeom_Curve)& SC)
{
  if (SC.IsNull()){
    return Handle(Geom_Curve)();
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Line))) {
    return MakeLine (Handle(StepGeom_Line)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_TrimmedCurve))) {
    return MakeTrimmedCurve (Handle(StepGeom_TrimmedCurve)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Conic))) {
    return MakeConic (Handle(StepGeom_Conic)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BoundedCurve))) {
    return MakeBoundedCurve (Handle(StepGeom_BoundedCurve)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_CurveReplica))) { //:n7 abv 16 Feb 99
    const Handle(StepGeom_CurveReplica) CR = Handle(StepGeom_CurveReplica)::DownCast(SC);
    const Handle(StepGeom_Curve) PC = CR->ParentCurve();
    const Handle(StepGeom_CartesianTransformationOperator3d) T =
      Handle(StepGeom_CartesianTransformationOperator3d)::DownCast(CR->Transformation());
    // protect against cyclic references and wrong type of cartop
    if ( !T.IsNull() && PC != SC )
    {
      Handle(Geom_Curve) C1 = MakeCurve (PC);
      if (! C1.IsNull())
      {
        gp_Trsf T1;
        if (MakeTransformation3d(T,T1))
        {
          C1->Transform ( T1 );
          return C1;
        }
      }
    }
  }
  else if (SC->IsKind(STANDARD_TYPE(StepGeom_OffsetCurve3d))) { //:o2 abv 17 Feb 99
    const Handle(StepGeom_OffsetCurve3d) OC = Handle(StepGeom_OffsetCurve3d)::DownCast(SC);
    const Handle(StepGeom_Curve) BC = OC->BasisCurve();
    if ( BC != SC ) { // protect against loop
      Handle(Geom_Curve) C1 = MakeCurve (BC);
      if (! C1.IsNull())
      {
        Handle(Geom_Direction) RD = MakeDirection(OC->RefDirection());
        if (! RD.IsNull())
        {
          return new Geom_OffsetCurve ( C1, -OC->Distance(), RD->Dir() );
        }
      }
    }
  }
  else if (SC->IsKind(STANDARD_TYPE(StepGeom_SurfaceCurve))) { //:o5 abv 17 Feb 99
    const Handle(StepGeom_SurfaceCurve) SurfC = Handle(StepGeom_SurfaceCurve)::DownCast(SC);
    return MakeCurve (SurfC->Curve3d());
  }
  return 0;
}

//=============================================================================
// Creation d' une Curve de Geom2d a partir d' une Curve de Step
//=============================================================================

Handle(Geom2d_Curve) StepToGeom::MakeCurve2d (const Handle(StepGeom_Curve)& SC)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Line))) {
    return MakeLine2d (Handle(StepGeom_Line)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Conic))) {
    return MakeConic2d (Handle(StepGeom_Conic)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BoundedCurve))) {
    return MakeBoundedCurve2d (Handle(StepGeom_BoundedCurve)::DownCast(SC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_CurveReplica))) { //:n7 abv 16 Feb 99
    const Handle(StepGeom_CurveReplica) CR = Handle(StepGeom_CurveReplica)::DownCast(SC);
    const Handle(StepGeom_Curve) PC = CR->ParentCurve();
    const Handle(StepGeom_CartesianTransformationOperator2d) T =
      Handle(StepGeom_CartesianTransformationOperator2d)::DownCast(CR->Transformation());
    // protect against cyclic references and wrong type of cartop
    if ( !T.IsNull() && PC != SC )
    {
      Handle(Geom2d_Curve) C1 = MakeCurve2d (PC);
      if (! C1.IsNull())
      {
        gp_Trsf2d T1;
        if (MakeTransformation2d(T,T1))
        {
          C1->Transform ( T1 );
          return C1;
        }
      }
    }
  }
  return 0;
}

//=============================================================================
// Creation d' une CylindricalSurface de Geom a partir d' une
// CylindricalSurface de Step
//=============================================================================

Handle(Geom_CylindricalSurface) StepToGeom::MakeCylindricalSurface (const Handle(StepGeom_CylindricalSurface)& SS)
{
  Handle(Geom_Axis2Placement) A = MakeAxis2Placement(SS->Position());
  if (! A.IsNull())
  {
    return new Geom_CylindricalSurface(A->Ax2(), SS->Radius() * UnitsMethods::LengthFactor());
  }
  return 0;
}

//=============================================================================
// Creation d' un Direction de Geom a partir d' un Direction de Step
//=============================================================================

Handle(Geom_Direction) StepToGeom::MakeDirection (const Handle(StepGeom_Direction)& SD)
{
  if (SD->NbDirectionRatios() >= 3)
  {
    const Standard_Real X = SD->DirectionRatiosValue(1);
    const Standard_Real Y = SD->DirectionRatiosValue(2);
    const Standard_Real Z = SD->DirectionRatiosValue(3);
    // sln 22.10.2001. CTS23496: Direction is not created if it has null magnitude
    if (gp_XYZ(X, Y, Z).SquareModulus() > gp::Resolution()*gp::Resolution())
    {
      return new Geom_Direction(X, Y, Z);
    }
  }
  return 0;
}

//=============================================================================
// Creation d' un Direction de Geom2d a partir d' un Direction de Step
//=============================================================================

Handle(Geom2d_Direction) StepToGeom::MakeDirection2d (const Handle(StepGeom_Direction)& SD)
{
  if (SD->NbDirectionRatios() >= 2)
  {
    const Standard_Real X = SD->DirectionRatiosValue(1);
    const Standard_Real Y = SD->DirectionRatiosValue(2);
    // sln 23.10.2001. CTS23496: Direction is not created if it has null magnitude
    if(gp_XY(X,Y).SquareModulus() > gp::Resolution()*gp::Resolution())
    {
      return  new Geom2d_Direction(X, Y);
    }
  }
  return 0;
}

//=============================================================================
// Creation d' une ElementarySurface de Geom a partir d' une
// ElementarySurface de Step
//=============================================================================

Handle(Geom_ElementarySurface) StepToGeom::MakeElementarySurface (const Handle(StepGeom_ElementarySurface)& SS)
{
  if (SS->IsKind(STANDARD_TYPE(StepGeom_Plane))) {
    return MakePlane (Handle(StepGeom_Plane)::DownCast(SS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_CylindricalSurface))) {
    return MakeCylindricalSurface (Handle(StepGeom_CylindricalSurface)::DownCast(SS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_ConicalSurface))) {
    return MakeConicalSurface (Handle(StepGeom_ConicalSurface)::DownCast(SS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_SphericalSurface))) {
    return MakeSphericalSurface (Handle(StepGeom_SphericalSurface)::DownCast(SS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_ToroidalSurface))) {
    return MakeToroidalSurface (Handle(StepGeom_ToroidalSurface)::DownCast(SS));
  }
  return 0;
}

//=============================================================================
// Creation d' un Ellipse de Geom a partir d' un Ellipse de Step
//=============================================================================

Handle(Geom_Ellipse) StepToGeom::MakeEllipse (const Handle(StepGeom_Ellipse)& SC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2) {
    Handle(Geom_Axis2Placement) A1 = MakeAxis2Placement (Handle(StepGeom_Axis2Placement3d)::DownCast(AxisSelect.Value()));
    if (! A1.IsNull())
    {
      gp_Ax2 A( A1->Ax2() );
      const Standard_Real LF = UnitsMethods::LengthFactor();
      const Standard_Real majorR = SC->SemiAxis1() * LF;
      const Standard_Real minorR = SC->SemiAxis2() * LF;
      if ( majorR - minorR >= 0. ) { //:o9 abv 19 Feb 99
        return new Geom_Ellipse(A, majorR, minorR);
      }
      //:o9 abv 19 Feb 99
      else {
        A.SetXDirection ( A.XDirection() ^ A.Direction() );
        return new Geom_Ellipse(A, minorR, majorR);
      }
    }
  }
  return 0;
}

//=============================================================================
// Creation d' un Ellipse de Geom2d a partir d' un Ellipse de Step
//=============================================================================

Handle(Geom2d_Ellipse) StepToGeom::MakeEllipse2d (const Handle(StepGeom_Ellipse)& SC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 1) {
    Handle(Geom2d_AxisPlacement) A1 = MakeAxisPlacement (Handle(StepGeom_Axis2Placement2d)::DownCast(AxisSelect.Value()));
    if (! A1.IsNull())
    {
      gp_Ax22d A( A1->Ax2d() );
      const Standard_Real majorR = SC->SemiAxis1();
      const Standard_Real minorR = SC->SemiAxis2();
      if ( majorR - minorR >= 0. ) { //:o9 abv 19 Feb 99: bm4_id_punch_b.stp #678: protection
        return new Geom2d_Ellipse(A, majorR, minorR);
      }
      else {
        const gp_Dir2d X = A.XDirection();
        A.SetXDirection ( gp_Dir2d ( X.X(), -X.Y() ) );
        return new Geom2d_Ellipse(A, minorR, majorR);
      }
    }
  }
  return 0;
}

//=============================================================================
// Creation d' un Hyperbola de Geom a partir d' un Hyperbola de Step
//=============================================================================

Handle(Geom_Hyperbola) StepToGeom::MakeHyperbola (const Handle(StepGeom_Hyperbola)& SC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2)
  {
    Handle(Geom_Axis2Placement) A1 = MakeAxis2Placement (Handle(StepGeom_Axis2Placement3d)::DownCast(AxisSelect.Value()));
    if (! A1.IsNull())
    {
      const gp_Ax2 A( A1->Ax2() );
      const Standard_Real LF = UnitsMethods::LengthFactor();
      return new Geom_Hyperbola(A, SC->SemiAxis() * LF, SC->SemiImagAxis() * LF);
    }
  }
  return 0;
}

//=============================================================================
// Creation d' un Hyperbola de Geom2d a partir d' un Hyperbola de Step
//=============================================================================

Handle(Geom2d_Hyperbola) StepToGeom::MakeHyperbola2d (const Handle(StepGeom_Hyperbola)& SC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 1)
  {
    Handle(Geom2d_AxisPlacement) A1 = MakeAxisPlacement (Handle(StepGeom_Axis2Placement2d)::DownCast(AxisSelect.Value()));
    if (! A1.IsNull())
    {
      const gp_Ax22d A( A1->Ax2d() );
      return new Geom2d_Hyperbola(A, SC->SemiAxis(), SC->SemiImagAxis());
    }
  }
  return 0;
}

//=============================================================================
// Creation d' une Line de Geom a partir d' une Line de Step
//=============================================================================

Handle(Geom_Line) StepToGeom::MakeLine (const Handle(StepGeom_Line)& SC)
{
  Handle(Geom_CartesianPoint) P = MakeCartesianPoint(SC->Pnt());
  if (! P.IsNull())
  {
    // sln 22.10.2001. CTS23496: Line is not created if direction have not been succesfully created
    Handle(Geom_VectorWithMagnitude) D = MakeVectorWithMagnitude (SC->Dir());
    if (! D.IsNull())
    {
      if( D->Vec().SquareMagnitude() < Precision::Confusion() * Precision::Confusion())
        return 0;
      const gp_Dir V(D->Vec());
      return new Geom_Line(P->Pnt(), V);
    }
  }
  return 0;
}

//=============================================================================
// Creation d' une Line de Geom2d a partir d' une Line de Step
//=============================================================================

Handle(Geom2d_Line) StepToGeom::MakeLine2d (const Handle(StepGeom_Line)& SC)
{
  Handle(Geom2d_CartesianPoint) P = MakeCartesianPoint2d(SC->Pnt());
  if (! P.IsNull())
  {
    // sln 23.10.2001. CTS23496: Line is not created if direction have not been succesfully created
    Handle(Geom2d_VectorWithMagnitude) D = MakeVectorWithMagnitude2d (SC->Dir());
    if (! D.IsNull())
    {
      const gp_Dir2d D1(D->Vec2d());
      return new Geom2d_Line(P->Pnt2d(), D1);
    }
  }
  return 0;
}

//=============================================================================
// Creation d' un Parabola de Geom a partir d' un Parabola de Step
//=============================================================================

Handle(Geom_Parabola) StepToGeom::MakeParabola (const Handle(StepGeom_Parabola)& SC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2)
  {
    Handle(Geom_Axis2Placement) A = MakeAxis2Placement (Handle(StepGeom_Axis2Placement3d)::DownCast(AxisSelect.Value()));
    if (! A.IsNull())
    {
      return new Geom_Parabola(A->Ax2(), SC->FocalDist() * UnitsMethods::LengthFactor());
    }
  }
  return 0;
}

//=============================================================================
// Creation d' un Parabola de Geom2d a partir d' un Parabola de Step
//=============================================================================

Handle(Geom2d_Parabola) StepToGeom::MakeParabola2d (const Handle(StepGeom_Parabola)& SC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 1) {
    Handle(Geom2d_AxisPlacement) A1 = MakeAxisPlacement (Handle(StepGeom_Axis2Placement2d)::DownCast(AxisSelect.Value()));
    if (! A1.IsNull())
    {
      const gp_Ax22d A( A1->Ax2d() );
      return new Geom2d_Parabola(A, SC->FocalDist());
    }
  }
  return 0;
}

//=============================================================================
// Creation d' un Plane de Geom a partir d' un plane de Step
//=============================================================================

Handle(Geom_Plane) StepToGeom::MakePlane (const Handle(StepGeom_Plane)& SP)
{
  Handle(Geom_Axis2Placement) A = MakeAxis2Placement (SP->Position());
  if (! A.IsNull())
  {
    return new Geom_Plane(A->Ax2());
  }
  return 0;
}

//=======================================================================
//function : MakePolyline
//purpose  :
//=======================================================================

Handle(Geom_BSplineCurve) StepToGeom::MakePolyline (const Handle(StepGeom_Polyline)& SPL)
{
  if (SPL.IsNull())
    return Handle(Geom_BSplineCurve)();

  const Standard_Integer nbp = SPL->NbPoints();
  if (nbp > 1)
  {
    TColgp_Array1OfPnt Poles ( 1, nbp );
    TColStd_Array1OfReal Knots ( 1, nbp );
    TColStd_Array1OfInteger Mults ( 1, nbp );

    for ( Standard_Integer i=1; i <= nbp; i++ )
    {
      Handle(Geom_CartesianPoint) P = MakeCartesianPoint (SPL->PointsValue(i));
      if (! P.IsNull())
        Poles.SetValue ( i, P->Pnt() );
      else
        return 0;
      Knots.SetValue ( i, Standard_Real(i-1) );
      Mults.SetValue ( i, 1 );
    }
    Mults.SetValue ( 1, 2 );
    Mults.SetValue ( nbp, 2 );

    return new Geom_BSplineCurve ( Poles, Knots, Mults, 1 );
  }
  return 0;
}

//=======================================================================
//function : MakePolyline2d
//purpose  :
//=======================================================================

Handle(Geom2d_BSplineCurve) StepToGeom::MakePolyline2d (const Handle(StepGeom_Polyline)& SPL)
{
  if (SPL.IsNull())
    return Handle(Geom2d_BSplineCurve)();

  const Standard_Integer nbp = SPL->NbPoints();
  if (nbp > 1)
  {
    TColgp_Array1OfPnt2d Poles ( 1, nbp );
    TColStd_Array1OfReal Knots ( 1, nbp );
    TColStd_Array1OfInteger Mults ( 1, nbp );

    for ( Standard_Integer i=1; i <= nbp; i++ )
    {
    Handle(Geom2d_CartesianPoint) P = MakeCartesianPoint2d (SPL->PointsValue(i));
      if (! P.IsNull())
        Poles.SetValue ( i, P->Pnt2d() );
      else
        return 0;
      Knots.SetValue ( i, Standard_Real(i-1) );
      Mults.SetValue ( i, 1 );
    }
    Mults.SetValue ( 1, 2 );
    Mults.SetValue ( nbp, 2 );

    return new Geom2d_BSplineCurve ( Poles, Knots, Mults, 1 );
  }
  return 0;
}

//=============================================================================
// Creation d' une RectangularTrimmedSurface de Geom a partir d' une
// RectangularTrimmedSurface de Step
//=============================================================================

Handle(Geom_RectangularTrimmedSurface) StepToGeom::MakeRectangularTrimmedSurface (const Handle(StepGeom_RectangularTrimmedSurface)& SS)
{
  Handle(Geom_Surface) theBasis = MakeSurface (SS->BasisSurface());
  if (! theBasis.IsNull())
  {
    // -----------------------------------------
    // Modification of the Trimming Parameters ?
    // -----------------------------------------

    Standard_Real uFact = 1.;
    Standard_Real vFact = 1.;
    const Standard_Real LengthFact  = UnitsMethods::LengthFactor();
    const Standard_Real AngleFact   = UnitsMethods::PlaneAngleFactor(); // abv 30.06.00 trj4_k1_geo-tc-214.stp #1477: PI/180.;

    if (theBasis->IsKind(STANDARD_TYPE(Geom_SphericalSurface)) ||
        theBasis->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))) {
      uFact = vFact = AngleFact;
    }
    else if (theBasis->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) {
      uFact = AngleFact;
      vFact = LengthFact;
    }
    else if ( theBasis->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
      uFact = AngleFact;
    }
    else if (theBasis->IsKind(STANDARD_TYPE(Geom_ConicalSurface))) {
      const Handle(Geom_ConicalSurface) conicS = Handle(Geom_ConicalSurface)::DownCast(theBasis);
      uFact = AngleFact;
      vFact = LengthFact / Cos(conicS->SemiAngle());
    }
    else if (theBasis->IsKind(STANDARD_TYPE(Geom_Plane))) {
      uFact = vFact = LengthFact;
    }

    const Standard_Real U1 = SS->U1() * uFact;
    const Standard_Real U2 = SS->U2() * uFact;
    const Standard_Real V1 = SS->V1() * vFact;
    const Standard_Real V2 = SS->V2() * vFact;

    return new Geom_RectangularTrimmedSurface(theBasis, U1, U2, V1, V2, SS->Usense(), SS->Vsense());
  }
  return 0;
}

//=============================================================================
// Creation d' une SphericalSurface de Geom a partir d' une
// SphericalSurface de Step
//=============================================================================

Handle(Geom_SphericalSurface) StepToGeom::MakeSphericalSurface (const Handle(StepGeom_SphericalSurface)& SS)
{
  Handle(Geom_Axis2Placement) A = MakeAxis2Placement (SS->Position());
  if (! A.IsNull())
  {
    return new Geom_SphericalSurface(A->Ax2(), SS->Radius() * UnitsMethods::LengthFactor());
  }
  return 0;
}

//=============================================================================
// Creation d' une Surface de Geom a partir d' une Surface de Step
//=============================================================================

Handle(Geom_Surface) StepToGeom::MakeSurface (const Handle(StepGeom_Surface)& SS)
{
   // sln 01.10.2001 BUC61003. If entry shell is NULL do nothing
  if(SS.IsNull()) {
    return Handle(Geom_Surface)();
  }

  try {
    OCC_CATCH_SIGNALS
    if (SS->IsKind(STANDARD_TYPE(StepGeom_BoundedSurface))) {
      return MakeBoundedSurface (Handle(StepGeom_BoundedSurface)::DownCast(SS));
    }
    if (SS->IsKind(STANDARD_TYPE(StepGeom_ElementarySurface))) {
      const Handle(StepGeom_ElementarySurface) S1 = Handle(StepGeom_ElementarySurface)::DownCast(SS);
      if(S1->Position().IsNull())
        return Handle(Geom_Surface)();

      return MakeElementarySurface (S1);
    }
    if (SS->IsKind(STANDARD_TYPE(StepGeom_SweptSurface))) {
      return MakeSweptSurface (Handle(StepGeom_SweptSurface)::DownCast(SS));
    }
    if (SS->IsKind(STANDARD_TYPE(StepGeom_OffsetSurface))) { //:d4 abv 12 Mar 98
      const Handle(StepGeom_OffsetSurface) OS = Handle(StepGeom_OffsetSurface)::DownCast(SS);

      Handle(Geom_Surface) aBasisSurface = MakeSurface (OS->BasisSurface());
      if (! aBasisSurface.IsNull())
      {
        // sln 03.10.01. BUC61003. creation of  offset surface is corrected
        const Standard_Real anOffset = OS->Distance() * UnitsMethods::LengthFactor();
        if (aBasisSurface->Continuity() == GeomAbs_C0)
        {
          const BRepBuilderAPI_MakeFace aBFace(aBasisSurface, Precision::Confusion());
          if (aBFace.IsDone())
          {
            const TopoDS_Shape aResult = ShapeAlgo::AlgoContainer()->C0ShapeToC1Shape(aBFace.Face(), Abs(anOffset));
            if (aResult.ShapeType() == TopAbs_FACE)
            {
              aBasisSurface = BRep_Tool::Surface(TopoDS::Face(aResult));
            }
          }
        }
        if(aBasisSurface->Continuity() != GeomAbs_C0)
        {
          return new Geom_OffsetSurface ( aBasisSurface, anOffset );
        }
      }
    }
    else if (SS->IsKind(STANDARD_TYPE(StepGeom_SurfaceReplica))) { //:n7 abv 16 Feb 99
      const Handle(StepGeom_SurfaceReplica) SR = Handle(StepGeom_SurfaceReplica)::DownCast(SS);
      const Handle(StepGeom_Surface) PS = SR->ParentSurface();
      const Handle(StepGeom_CartesianTransformationOperator3d) T = SR->Transformation();
      // protect against cyclic references and wrong type of cartop
      if ( !T.IsNull() && PS != SS ) {
        Handle(Geom_Surface) S1 = MakeSurface (PS);
        if (! S1.IsNull())
        {
          gp_Trsf T1;
          if (MakeTransformation3d(T,T1))
          {
            S1->Transform ( T1 );
            return S1;
          }
        }
      }
    }
  }
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
//   ShapeTool_DB ?
//:s5
    std::cout<<"Warning: MakeSurface: Exception:";
    anException.Print(std::cout); std::cout << std::endl;
#endif
    (void)anException;
  }
  return 0;
}

//=============================================================================
// Creation d' une SurfaceOfLinearExtrusion de Geom a partir d' une
// SurfaceOfLinearExtrusion de Step
//=============================================================================

Handle(Geom_SurfaceOfLinearExtrusion) StepToGeom::MakeSurfaceOfLinearExtrusion (const Handle(StepGeom_SurfaceOfLinearExtrusion)& SS)
{
  Handle(Geom_Curve) C = MakeCurve (SS->SweptCurve());
  if (! C.IsNull())
  {
    // sln 23.10.2001. CTS23496: Surface is not created if extrusion axis have not been succesfully created
    Handle(Geom_VectorWithMagnitude) V = MakeVectorWithMagnitude (SS->ExtrusionAxis());
    if (! V.IsNull())
    {
      const gp_Dir D(V->Vec());
      Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast(C);
      if (!aLine.IsNull() && aLine->Lin().Direction().IsParallel(D, Precision::Angular()))
        return Handle(Geom_SurfaceOfLinearExtrusion)();
      return new Geom_SurfaceOfLinearExtrusion(C,D);
    }
  }
  return 0;
}

//=============================================================================
// Creation d' une SurfaceOfRevolution de Geom a partir d' une
// SurfaceOfRevolution de Step
//=============================================================================

Handle(Geom_SurfaceOfRevolution) StepToGeom::MakeSurfaceOfRevolution (const Handle(StepGeom_SurfaceOfRevolution)& SS)
{
  Handle(Geom_Curve) C = MakeCurve (SS->SweptCurve());
  if (! C.IsNull())
  {
    Handle(Geom_Axis1Placement) A1 = MakeAxis1Placement (SS->AxisPosition());
    if (! A1.IsNull())
    {
      const gp_Ax1 A( A1->Ax1() );
      //skl for OCC952 (one bad case revolution of circle)
      if ( C->IsKind(STANDARD_TYPE(Geom_Circle)) || C->IsKind(STANDARD_TYPE(Geom_Ellipse)) )
      {
        const Handle(Geom_Conic) conic = Handle(Geom_Conic)::DownCast(C);
        const gp_Pnt pc = conic->Location();
        const gp_Lin rl (A);
        if (rl.Distance(pc) < Precision::Confusion()) { //pc lies on A2
          const gp_Dir dirline = A.Direction();
          const gp_Dir norm = conic->Axis().Direction();
          const gp_Dir xAxis = conic->XAxis().Direction();
          //checking A2 lies on plane of circle
          if( dirline.IsNormal(norm,Precision::Angular()) && (dirline.IsParallel(xAxis,Precision::Angular()) || C->IsKind(STANDARD_TYPE(Geom_Circle)))) {
            //change parametrization for trimming
            gp_Ax2 axnew(pc,norm,dirline.Reversed());
            conic->SetPosition(axnew);
            C = new Geom_TrimmedCurve(conic, 0., M_PI);
          }
        }
      }
      return new Geom_SurfaceOfRevolution(C, A);
    }
  }
  return 0;
}

//=============================================================================
// Creation d' une SweptSurface de prostep a partir d' une
// SweptSurface de Geom
//=============================================================================

Handle(Geom_SweptSurface) StepToGeom::MakeSweptSurface (const Handle(StepGeom_SweptSurface)& SS)
{
  if (SS->IsKind(STANDARD_TYPE(StepGeom_SurfaceOfLinearExtrusion))) {
    return MakeSurfaceOfLinearExtrusion (Handle(StepGeom_SurfaceOfLinearExtrusion)::DownCast(SS));
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_SurfaceOfRevolution))) {
    return MakeSurfaceOfRevolution (Handle(StepGeom_SurfaceOfRevolution)::DownCast(SS));
  }
  return Handle(Geom_SweptSurface)();
}

//=============================================================================
// Creation d' une ToroidalSurface de Geom a partir d' une
// ToroidalSurface de Step
//=============================================================================

Handle(Geom_ToroidalSurface) StepToGeom::MakeToroidalSurface (const Handle(StepGeom_ToroidalSurface)& SS)
{
  Handle(Geom_Axis2Placement) A = MakeAxis2Placement (SS->Position());
  if (! A.IsNull())
  {
    const Standard_Real LF = UnitsMethods::LengthFactor();
    return new Geom_ToroidalSurface(A->Ax2(), Abs(SS->MajorRadius() * LF), Abs(SS->MinorRadius() * LF));
  }
  return 0;
}

//=======================================================================
//function : MakeTransformation2d
//purpose  :
//=======================================================================
Standard_Boolean StepToGeom::MakeTransformation2d (const Handle(StepGeom_CartesianTransformationOperator2d)& SCTO, gp_Trsf2d& CT)
{
  //  NB : on ne s interesse ici qu au deplacement rigide
  Handle(Geom2d_CartesianPoint) CP = MakeCartesianPoint2d (SCTO->LocalOrigin());
  if (! CP.IsNull())
  {
    gp_Dir2d D1(1.,0.);
    // sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is used
    const Handle(StepGeom_Direction) A = SCTO->Axis1();
    if (!A.IsNull())
    {
      Handle(Geom2d_Direction) D = MakeDirection2d (A);
      if (! D.IsNull())
        D1 = D->Dir2d();
    }
    const gp_Ax2d result(CP->Pnt2d(),D1);
    CT.SetTransformation(result);
    CT = CT.Inverted();
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : MakeTransformation3d
//purpose  :
//=======================================================================

Standard_Boolean StepToGeom::MakeTransformation3d (const Handle(StepGeom_CartesianTransformationOperator3d)& SCTO, gp_Trsf& CT)
{
  Handle(Geom_CartesianPoint) CP = MakeCartesianPoint (SCTO->LocalOrigin());
  if (! CP.IsNull())
  {
    const gp_Pnt Pgp = CP->Pnt();

    // sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is used
    gp_Dir D1(1.,0.,0.);
    const Handle(StepGeom_Direction) A1 = SCTO->Axis1();
    if (!A1.IsNull()) {
      Handle(Geom_Direction) D = MakeDirection (A1);
      if (! D.IsNull())
        D1 = D->Dir();
    }

    gp_Dir D2(0.,1.,0.);
    const Handle(StepGeom_Direction) A2 = SCTO->Axis2();
    if (!A2.IsNull()) {
      Handle(Geom_Direction) D = MakeDirection (A2);
      if (! D.IsNull())
        D2 = D->Dir();
    }

    Standard_Boolean isDefaultDirectionUsed = Standard_True;
    gp_Dir D3;
    const Handle(StepGeom_Direction) A3 = SCTO->Axis3();
    if (!A3.IsNull()) {
      Handle(Geom_Direction) D = MakeDirection (A3);
      if (! D.IsNull())
      {
        D3 = D->Dir();
        isDefaultDirectionUsed = Standard_False;
      }
    }
    if(isDefaultDirectionUsed)
      D3 = D1.Crossed(D2);

    const gp_Ax3 result(Pgp,D3,D1);
    CT.SetTransformation(result);
    CT = CT.Inverted(); //:n8 abv 16 Feb 99: tr8_as2_db.stp: reverse for accordance with LV tool
    return Standard_True;
  }
  return Standard_False;
}

// ----------------------------------------------------------------
// ExtractParameter
// ----------------------------------------------------------------
//:o6 abv 18 Feb 99: parameter Factor added
//:p3 abv 23 Feb 99: parameter Shift added
static Standard_Boolean  ExtractParameter
(const Handle(Geom_Curve) &  aGeomCurve,
 const Handle(StepGeom_HArray1OfTrimmingSelect) & TS,
 const Standard_Integer nbSel,
 const Standard_Integer MasterRep,
 const Standard_Real Factor,
 const Standard_Real Shift,
 Standard_Real & aParam)
{
  Handle(StepGeom_CartesianPoint) aPoint;
  Standard_Integer i;
//:S4136  Standard_Real precBrep = BRepAPI::Precision();
  for ( i = 1 ; i <= nbSel ; i++) {
    StepGeom_TrimmingSelect theSel = TS->Value(i);
    if (MasterRep == 2 && theSel.CaseMember() > 0) {
      aParam = Shift + Factor * theSel.ParameterValue();
      return Standard_True;
    }
    else if (MasterRep == 1 && theSel.CaseNumber() > 0) {
      aPoint = theSel.CartesianPoint();
      Handle(Geom_CartesianPoint) theGeomPnt = StepToGeom::MakeCartesianPoint (aPoint);
      gp_Pnt thegpPnt = theGeomPnt->Pnt();

      //:S4136: use advanced algorithm
      ShapeAnalysis_Curve sac;
      gp_Pnt p;
      sac.Project ( aGeomCurve, thegpPnt, Precision::Confusion(), p, aParam );
/* //:S4136
      //Trim == natural boundary ?
      if(aGeomCurve->IsKind(STANDARD_TYPE(Geom_BoundedCurve))) {
	Standard_Real frstPar = aGeomCurve->FirstParameter();
	Standard_Real lstPar = aGeomCurve->LastParameter();
	gp_Pnt frstPnt = aGeomCurve->Value(frstPar);
	gp_Pnt lstPnt = aGeomCurve->Value(lstPar);
	if(frstPnt.IsEqual(thegpPnt,precBrep)) {
	  aParam = frstPar;
	  return Standard_True;
	}
	if(lstPnt.IsEqual(thegpPnt,precBrep)) {
	  aParam = lstPar;
	  return Standard_True;
	}
      }
      // Project Point On Curve
      GeomAPI_ProjectPointOnCurve PPOC(thegpPnt, aGeomCurve);
      if (PPOC.NbPoints() == 0) {
	return Standard_False;
      }
      aParam = PPOC.LowerDistanceParameter();
*/
      return Standard_True;
    }
  }
// if the MasterRepresentation is unspecified:
// if a ParameterValue exists, it is prefered

  for ( i = 1 ; i <= nbSel ; i++) {
    StepGeom_TrimmingSelect theSel = TS->Value(i);
    if (theSel.CaseMember() > 0) {
      aParam = Shift + Factor * theSel.ParameterValue();

      return Standard_True;
    }
  }
// if no ParameterValue exists, it is created from the CartesianPointValue

  for ( i = 1 ; i <= nbSel ; i++) {
    StepGeom_TrimmingSelect theSel = TS->Value(i);
    if (theSel.CaseNumber() > 0) {
      aPoint = theSel.CartesianPoint();
      Handle(Geom_CartesianPoint) theGeomPnt = StepToGeom::MakeCartesianPoint (aPoint);
      gp_Pnt thegpPnt = theGeomPnt->Pnt();
      // Project Point On Curve
      ShapeAnalysis_Curve sac;
      gp_Pnt p;
      sac.Project ( aGeomCurve, thegpPnt, Precision::Confusion(), p, aParam );
/*
      GeomAPI_ProjectPointOnCurve PPOC(thegpPnt, aGeomCurve);
      if (PPOC.NbPoints() == 0) {
	return Standard_False;
      }
      aParam = PPOC.LowerDistanceParameter();
*/
      return Standard_True;
    }
  }
  return Standard_False;  // I suppose
}


//=============================================================================
// Creation d' une Trimmed Curve de Geom a partir d' une Trimmed Curve de Step
//=============================================================================

Handle(Geom_TrimmedCurve) StepToGeom::MakeTrimmedCurve (const Handle(StepGeom_TrimmedCurve)& SC)
{
  const Handle(StepGeom_Curve) theSTEPCurve = SC->BasisCurve();
  Handle(Geom_Curve) theCurve = MakeCurve (theSTEPCurve);
  if (theCurve.IsNull())
    return Handle(Geom_TrimmedCurve)();

  const Handle(StepGeom_HArray1OfTrimmingSelect)& theTrimSel1 = SC->Trim1();
  const Handle(StepGeom_HArray1OfTrimmingSelect)& theTrimSel2 = SC->Trim2();
  const Standard_Integer nbSel1 = SC->NbTrim1();
  const Standard_Integer nbSel2 = SC->NbTrim2();

  Standard_Integer MasterRep;
  switch (SC->MasterRepresentation())
  {
    case StepGeom_tpCartesian: MasterRep = 1; break;
    case StepGeom_tpParameter: MasterRep = 2; break;
    default: MasterRep = 0;
  }

  //gka 18.02.04 analysis for case when MasterRep = .Unspecified
  //and parameters are specified as CARTESIAN_POINT
  Standard_Boolean isPoint = Standard_False;
  if(MasterRep == 0 || (MasterRep == 2 && nbSel1 >1 && nbSel2 > 1)) {
    Standard_Integer ii;
    for(ii = 1; ii <= nbSel1; ii++)
    {
      if (!(theTrimSel1->Value(ii).CartesianPoint().IsNull()))
      {
        for(ii = 1; ii <= nbSel2; ii++)
        {
          if (!(theTrimSel2->Value(ii).CartesianPoint().IsNull()))
          {
            isPoint = Standard_True;
            break;
          }
        }
        break;
      }
    }
  }

  //:o6 abv 18 Feb 99: computation of factor moved
  Standard_Real fact = 1., shift = 0.;
  if (theSTEPCurve->IsKind(STANDARD_TYPE(StepGeom_Line))) {
    const Handle(StepGeom_Line) theLine =
      Handle(StepGeom_Line)::DownCast(theSTEPCurve);
    fact = theLine->Dir()->Magnitude() * UnitsMethods::LengthFactor();
  }
  else if (theSTEPCurve->IsKind(STANDARD_TYPE(StepGeom_Circle)) ||
           theSTEPCurve->IsKind(STANDARD_TYPE(StepGeom_Ellipse))) {
//    if (trim1 > 2.1*M_PI || trim2 > 2.1*M_PI) fact = M_PI / 180.;
    fact = UnitsMethods::PlaneAngleFactor();
    //:p3 abv 23 Feb 99: shift on pi/2 on ellipse with R1 < R2
    const Handle(StepGeom_Ellipse) ellipse = Handle(StepGeom_Ellipse)::DownCast(theSTEPCurve);
    if ( !ellipse.IsNull() && ellipse->SemiAxis1() - ellipse->SemiAxis2() < 0. )
      shift = 0.5 * M_PI;

    // skl 04.02.2002 for OCC133: we can not make TrimmedCurve if
    // there is no X-direction in StepGeom_Axis2Placement3d
    const Handle(StepGeom_Conic) conic = Handle(StepGeom_Conic)::DownCast(theSTEPCurve);
    // CKY 6-FEB-2004 for Airbus-MedialAxis :
    // this restriction does not apply for trimming by POINTS
    if(!conic.IsNull() && MasterRep != 1) {
      const StepGeom_Axis2Placement a2p = conic->Position();
      if(a2p.CaseNum(a2p.Value())==2) {
        if( !a2p.Axis2Placement3d()->HasRefDirection() ) {
          ////gka 18.02.04 analysis for case when MasterRep = .Unspecified
          //and parameters are specified as CARTESIAN_POINT
          if(isPoint /*&& !MasterRep*/)
            MasterRep =1;
          else {
            if ( SC->SenseAgreement() )
              return new Geom_TrimmedCurve(theCurve, 0., 2.*M_PI, Standard_True);
            else
              return new Geom_TrimmedCurve(theCurve, 2.*M_PI, 0., Standard_False);
          }
        }
      }
    }
  }

  Standard_Real trim1 = 0.;
  Standard_Real trim2 = 0.;
  Handle(StepGeom_CartesianPoint) TrimCP1, TrimCP2;
  const Standard_Boolean FoundParam1 = ExtractParameter(theCurve, theTrimSel1, nbSel1, MasterRep, fact, shift, trim1);
  const Standard_Boolean FoundParam2 = ExtractParameter(theCurve, theTrimSel2, nbSel2, MasterRep, fact, shift, trim2);

  if (FoundParam1 && FoundParam2) {
    const Standard_Real cf = theCurve->FirstParameter();
    const Standard_Real cl = theCurve->LastParameter();
    //: abv 09.04.99: S4136: bm2_ug_t4-B.stp #70610: protect against OutOfRange
    if ( !theCurve->IsPeriodic() ) {
      if ( trim1 < cf ) trim1 = cf;
      else if ( trim1 > cl ) trim1 = cl;
      if ( trim2 < cf ) trim2 = cf;
      else if ( trim2 > cl ) trim2 = cl;
    }
    if (Abs(trim1 - trim2) < Precision::PConfusion()) {
      if (theCurve->IsPeriodic()) {
        ElCLib::AdjustPeriodic(cf,cl,Precision::PConfusion(),trim1,trim2);
      }
      else if (theCurve->IsClosed()) {
        if (Abs(trim1 - cf) < Precision::PConfusion()) {
          trim2 += cl;
        }
        else {
          trim1 -= cl;
        }
      }
      else {
        return 0;
      }
    }
//  CKY 16-DEC-1997 : USA60035 le texte de Part42 parle de degres
//    mais des systemes ecrivent en radians. Exploiter UnitsMethods
//:o6    trim1 = trim1 * fact;
//:o6    trim2 = trim2 * fact;
    if ( SC->SenseAgreement() )
      return new Geom_TrimmedCurve(theCurve, trim1, trim2, Standard_True);
    else //:abv 29.09.00 PRO20362: reverse parameters in case of reversed curve
      return new Geom_TrimmedCurve(theCurve, trim2, trim1, Standard_False);
  }
  return 0;
}

//=============================================================================
// Creation d'une Trimmed Curve de Geom2d a partir d' une Trimmed Curve de Step
//=============================================================================
// Shall be completed to treat trimming with points

Handle(Geom2d_BSplineCurve) StepToGeom::MakeTrimmedCurve2d (const Handle(StepGeom_TrimmedCurve)& SC)
{
  const Handle(StepGeom_Curve) BasisCurve = SC->BasisCurve();
  Handle(Geom2d_Curve) theGeomBasis = MakeCurve2d (BasisCurve);
  if (theGeomBasis.IsNull())
    return Handle(Geom2d_BSplineCurve)();

  if (theGeomBasis->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
    return Handle(Geom2d_BSplineCurve)::DownCast(theGeomBasis);
  }

  const Handle(StepGeom_HArray1OfTrimmingSelect)& theTrimSel1 = SC->Trim1();
  const Handle(StepGeom_HArray1OfTrimmingSelect)& theTrimSel2 = SC->Trim2();
  const Standard_Integer nbSel1 = SC->NbTrim1();
  const Standard_Integer nbSel2 = SC->NbTrim2();
  if ((nbSel1 == 1) && (nbSel2 == 1) &&
      (theTrimSel1->Value(1).CaseMember() > 0) &&
      (theTrimSel2->Value(1).CaseMember() > 0))
  {
    const Standard_Real u1 = theTrimSel1->Value(1).ParameterValue();
    const Standard_Real u2 = theTrimSel2->Value(1).ParameterValue();
    Standard_Real fact = 1., shift = 0.;

    if (BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Line))) {
      const Handle(StepGeom_Line) theLine = Handle(StepGeom_Line)::DownCast(BasisCurve);
      fact = theLine->Dir()->Magnitude();
    }
    else if (BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Circle)) ||
             BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Ellipse))) {
//      if (u1 > 2.1*M_PI || u2 > 2.1*M_PI) fact = M_PI / 180.;
      fact = UnitsMethods::PlaneAngleFactor();
      //:p3 abv 23 Feb 99: shift on pi/2 on ellipse with R1 < R2
      const Handle(StepGeom_Ellipse) ellipse = Handle(StepGeom_Ellipse)::DownCast(BasisCurve);
      if ( !ellipse.IsNull() && ellipse->SemiAxis1() - ellipse->SemiAxis2() < 0. )
        shift = 0.5 * M_PI;
    }
    else if (BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Parabola)) ||
             BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Hyperbola))) {
      // LATER !!!
    }
//    CKY 16-DEC-1997 : USA60035 le texte de Part42 parle de degres
//      mais des systemes ecrivent en radians. Exploiter UnitsMethods

    const Standard_Real newU1 = shift + u1 * fact;
    const Standard_Real newU2 = shift + u2 * fact;

    const Handle(Geom2d_TrimmedCurve) theTrimmed =
      new Geom2d_TrimmedCurve(theGeomBasis, newU1, newU2, SC->SenseAgreement());
    return Geom2dConvert::CurveToBSplineCurve(theTrimmed);
  }
  return 0;
}

//=============================================================================
// Creation d' un VectorWithMagnitude de Geom a partir d' un Vector de Step
//=============================================================================

Handle(Geom_VectorWithMagnitude) StepToGeom::MakeVectorWithMagnitude (const Handle(StepGeom_Vector)& SV)
{
  // sln 22.10.2001. CTS23496: Vector is not created if direction have not been succesfully created
  Handle(Geom_Direction) D = MakeDirection (SV->Orientation());
  if (! D.IsNull())
  {
    const gp_Vec V(D->Dir().XYZ() * SV->Magnitude() * UnitsMethods::LengthFactor());
    return new Geom_VectorWithMagnitude(V);
  }
  return 0;
}

//=============================================================================
// Creation d' un VectorWithMagnitude de Geom2d a partir d' un Vector de Step
//=============================================================================

Handle(Geom2d_VectorWithMagnitude) StepToGeom::MakeVectorWithMagnitude2d (const Handle(StepGeom_Vector)& SV)
{
  // sln 23.10.2001. CTS23496: Vector is not created if direction have not been succesfully created (MakeVectorWithMagnitude2d(...) function)
  Handle(Geom2d_Direction) D = MakeDirection2d (SV->Orientation());
  if (! D.IsNull())
  {
    const gp_Vec2d V(D->Dir2d().XY() * SV->Magnitude());
    return new Geom2d_VectorWithMagnitude(V);
  }
  return 0;
}
