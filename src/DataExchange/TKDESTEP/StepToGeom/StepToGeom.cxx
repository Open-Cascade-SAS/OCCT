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
#include <Geom_BSplineCurve.hxx>
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

#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeAnalysis_Curve.hxx>

#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <StepGeom_Axis2Placement3d.hxx>

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
#include <StepGeom_SuParameters.hxx>
#include <StepKinematics_SpatialRotation.hxx>
#include <StepKinematics_RotationAboutDirection.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#include <StepData_Factors.hxx>
#include <StepBasic_ConversionBasedUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_SiUnitAndPlaneAngleUnit.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepRepr_ReprItemAndMeasureWithUnit.hxx>
#include <STEPConstruct_UnitContext.hxx>

//=============================================================================
// Creation d' un Ax1Placement de Geom a partir d' un axis1_placement de Step
//=============================================================================

occ::handle<Geom_Axis1Placement> StepToGeom::MakeAxis1Placement(
  const occ::handle<StepGeom_Axis1Placement>& SA,
  const StepData_Factors&                     theLocalFactors)
{
  occ::handle<Geom_CartesianPoint> P = MakeCartesianPoint(SA->Location(), theLocalFactors);
  if (!P.IsNull())
  {
    // sln 22.10.2001. CTS23496: If problems with creation of axis direction occur default direction
    // is used
    gp_Dir D(gp_Dir::D::Z);
    if (SA->HasAxis())
    {
      occ::handle<Geom_Direction> D1 = MakeDirection(SA->Axis());
      if (!D1.IsNull())
        D = D1->Dir();
    }
    return new Geom_Axis1Placement(P->Pnt(), D);
  }
  return nullptr;
}

//=============================================================================
// Creation d' un Axis2Placement de Geom a partir d' un axis2_placement_3d de Step
//=============================================================================

occ::handle<Geom_Axis2Placement> StepToGeom::MakeAxis2Placement(
  const occ::handle<StepGeom_Axis2Placement3d>& SA,
  const StepData_Factors&                       theLocalFactors)
{
  occ::handle<Geom_CartesianPoint> P = MakeCartesianPoint(SA->Location(), theLocalFactors);
  if (!P.IsNull())
  {
    const gp_Pnt Pgp = P->Pnt();

    // sln 22.10.2001. CTS23496: If problems with creation of direction occur default direction is
    // used (MakeLine(...) function)
    gp_Dir Ngp(gp_Dir::D::Z);
    if (SA->HasAxis())
    {
      occ::handle<Geom_Direction> D = MakeDirection(SA->Axis());
      if (!D.IsNull())
        Ngp = D->Dir();
    }

    gp_Ax2 gpAx2;
    bool   isDefaultDirectionUsed = true;
    if (SA->HasRefDirection())
    {
      occ::handle<Geom_Direction> D = MakeDirection(SA->RefDirection());
      if (!D.IsNull())
      {
        const gp_Dir Vxgp = D->Dir();
        if (!Ngp.IsParallel(Vxgp, Precision::Angular()))
        {
          gpAx2                  = gp_Ax2(Pgp, Ngp, Vxgp);
          isDefaultDirectionUsed = false;
        }
      }
    }
    if (isDefaultDirectionUsed)
      gpAx2 = gp_Ax2(Pgp, Ngp);

    return new Geom_Axis2Placement(gpAx2);
  }
  return nullptr;
}

//=============================================================================
// Creation of an AxisPlacement from a Kinematic SuParameters for Step
//=============================================================================

occ::handle<Geom_Axis2Placement> StepToGeom::MakeAxis2Placement(
  const occ::handle<StepGeom_SuParameters>& theSP)
{
  double aLocX =
    theSP->A() * cos(theSP->Gamma()) + theSP->B() * sin(theSP->Gamma()) * sin(theSP->Alpha());
  double aLocY =
    theSP->A() * sin(theSP->Gamma()) - theSP->B() * cos(theSP->Gamma()) * sin(theSP->Alpha());
  double aLocZ   = theSP->C() + theSP->B() * cos(theSP->Alpha());
  double anAsisX = sin(theSP->Gamma()) * sin(theSP->Alpha());
  double anAxisY = -cos(theSP->Gamma()) * sin(theSP->Alpha());
  double anAxisZ = cos(theSP->Alpha());
  double aDirX   = cos(theSP->Gamma()) * cos(theSP->Beta())
                 - sin(theSP->Gamma()) * cos(theSP->Alpha()) * sin(theSP->Beta());
  double aDirY = sin(theSP->Gamma()) * cos(theSP->Beta())
                 + cos(theSP->Gamma()) * cos(theSP->Alpha()) * sin(theSP->Beta());
  double       aDirZ = sin(theSP->Alpha()) * sin(theSP->Beta());
  const gp_Pnt Pgp(aLocX, aLocY, aLocZ);
  const gp_Dir Ngp(anAsisX, anAxisY, anAxisZ);
  const gp_Dir Vxgp(aDirX, aDirY, aDirZ);
  gp_Ax2       gpAx2 = gp_Ax2(Pgp, Ngp, Vxgp);
  return new Geom_Axis2Placement(gpAx2);
}

//=============================================================================
// Creation d' un AxisPlacement de Geom2d a partir d' un axis2_placement_3d de Step
//=============================================================================

occ::handle<Geom2d_AxisPlacement> StepToGeom::MakeAxisPlacement(
  const occ::handle<StepGeom_Axis2Placement2d>& SA,
  const StepData_Factors&                       theLocalFactors)
{
  occ::handle<Geom2d_CartesianPoint> P = MakeCartesianPoint2d(SA->Location(), theLocalFactors);
  if (!P.IsNull())
  {
    // sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is
    // used
    gp_Dir2d Vxgp(gp_Dir2d::D::X);
    if (SA->HasRefDirection())
    {
      occ::handle<Geom2d_Direction> Vx = MakeDirection2d(SA->RefDirection());
      if (!Vx.IsNull())
        Vxgp = Vx->Dir2d();
    }

    return new Geom2d_AxisPlacement(P->Pnt2d(), Vxgp);
  }
  return nullptr;
}

//=============================================================================
// Creation d' une BoundedCurve de Geom a partir d' une BoundedCurve de Step
//=============================================================================

occ::handle<Geom_BoundedCurve> StepToGeom::MakeBoundedCurve(
  const occ::handle<StepGeom_BoundedCurve>& SC,
  const StepData_Factors&                   theLocalFactors)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)))
  {
    return MakeBSplineCurve(
      occ::down_cast<StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve>(SC),
      theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnots)))
  {
    return MakeBSplineCurve(occ::down_cast<StepGeom_BSplineCurveWithKnots>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_TrimmedCurve)))
  {
    return MakeTrimmedCurve(occ::down_cast<StepGeom_TrimmedCurve>(SC), theLocalFactors);
  }

  // STEP BezierCurve, UniformCurve and QuasiUniformCurve are transformed into
  // STEP BSplineCurve before being mapped onto CAS.CADE/SF
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BezierCurve)))
  {
    const occ::handle<StepGeom_BezierCurve> BzC     = occ::down_cast<StepGeom_BezierCurve>(SC);
    int                                     aDegree = BzC->Degree();
    if (aDegree < 1 || aDegree > Geom_BSplineCurve::MaxDegree())
      return nullptr;
    const occ::handle<StepGeom_BSplineCurveWithKnots> BSPL = new StepGeom_BSplineCurveWithKnots;
    BSPL->SetDegree(aDegree);
    BSPL->SetControlPointsList(BzC->ControlPointsList());
    BSPL->SetCurveForm(BzC->CurveForm());
    BSPL->SetClosedCurve(BzC->ClosedCurve());
    BSPL->SetSelfIntersect(BzC->SelfIntersect());
    // Compute Knots and KnotsMultiplicity
    const occ::handle<NCollection_HArray1<int>>    Kmult = new NCollection_HArray1<int>(1, 2);
    const occ::handle<NCollection_HArray1<double>> Knots = new NCollection_HArray1<double>(1, 2);
    Kmult->SetValue(1, BzC->Degree() + 1);
    Kmult->SetValue(2, BzC->Degree() + 1);
    Knots->SetValue(1, 0.);
    Knots->SetValue(2, 1.);
    BSPL->SetKnotMultiplicities(Kmult);
    BSPL->SetKnots(Knots);

    return MakeBSplineCurve(BSPL, theLocalFactors);
  }

  if (SC->IsKind(STANDARD_TYPE(StepGeom_UniformCurve)))
  {
    const occ::handle<StepGeom_UniformCurve> UC      = occ::down_cast<StepGeom_UniformCurve>(SC);
    int                                      aDegree = UC->Degree();
    if (aDegree < 1 || aDegree > Geom_BSplineCurve::MaxDegree())
      return nullptr;
    const occ::handle<StepGeom_BSplineCurveWithKnots> BSPL = new StepGeom_BSplineCurveWithKnots;
    BSPL->SetDegree(aDegree);
    BSPL->SetControlPointsList(UC->ControlPointsList());
    BSPL->SetCurveForm(UC->CurveForm());
    BSPL->SetClosedCurve(UC->ClosedCurve());
    BSPL->SetSelfIntersect(UC->SelfIntersect());

    // Compute Knots and KnotsMultiplicity
    const int nbK = BSPL->NbControlPointsList() + BSPL->Degree() + 1;
    const occ::handle<NCollection_HArray1<int>>    Kmult = new NCollection_HArray1<int>(1, nbK);
    const occ::handle<NCollection_HArray1<double>> Knots = new NCollection_HArray1<double>(1, nbK);
    for (int iUC = 1; iUC <= nbK; iUC++)
    {
      Kmult->SetValue(iUC, 1);
      Knots->SetValue(iUC, iUC - 1.);
    }
    BSPL->SetKnotMultiplicities(Kmult);
    BSPL->SetKnots(Knots);

    return MakeBSplineCurve(BSPL, theLocalFactors);
  }

  if (SC->IsKind(STANDARD_TYPE(StepGeom_QuasiUniformCurve)))
  {
    const occ::handle<StepGeom_QuasiUniformCurve> QUC =
      occ::down_cast<StepGeom_QuasiUniformCurve>(SC);
    int aDegree = QUC->Degree();
    if (aDegree < 1 || aDegree > Geom_BSplineCurve::MaxDegree())
      return nullptr;
    const occ::handle<StepGeom_BSplineCurveWithKnots> BSPL = new StepGeom_BSplineCurveWithKnots;
    BSPL->SetDegree(aDegree);
    BSPL->SetControlPointsList(QUC->ControlPointsList());
    BSPL->SetCurveForm(QUC->CurveForm());
    BSPL->SetClosedCurve(QUC->ClosedCurve());
    BSPL->SetSelfIntersect(QUC->SelfIntersect());

    // Compute Knots and KnotsMultiplicity
    const int nbK = BSPL->NbControlPointsList() - BSPL->Degree() + 1;
    const occ::handle<NCollection_HArray1<int>>    Kmult = new NCollection_HArray1<int>(1, nbK);
    const occ::handle<NCollection_HArray1<double>> Knots = new NCollection_HArray1<double>(1, nbK);
    for (int iQUC = 1; iQUC <= nbK; iQUC++)
    {
      Kmult->SetValue(iQUC, 1);
      Knots->SetValue(iQUC, iQUC - 1.);
    }
    Kmult->SetValue(1, BSPL->Degree() + 1);
    Kmult->SetValue(nbK, BSPL->Degree() + 1);
    BSPL->SetKnotMultiplicities(Kmult);
    BSPL->SetKnots(Knots);

    return MakeBSplineCurve(BSPL, theLocalFactors);
  }

  if (SC->IsKind(STANDARD_TYPE(StepGeom_UniformCurveAndRationalBSplineCurve)))
  {
    const occ::handle<StepGeom_UniformCurveAndRationalBSplineCurve> RUC =
      occ::down_cast<StepGeom_UniformCurveAndRationalBSplineCurve>(SC);
    int aDegree = RUC->Degree();
    if (aDegree < 1 || aDegree > Geom_BSplineCurve::MaxDegree())
      return nullptr;
    const occ::handle<StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve> RBSPL =
      new StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve;

    // Compute Knots and KnotsMultiplicity
    const int                                      nbK   = RUC->NbControlPointsList() + aDegree + 1;
    const occ::handle<NCollection_HArray1<int>>    Kmult = new NCollection_HArray1<int>(1, nbK);
    const occ::handle<NCollection_HArray1<double>> Knots = new NCollection_HArray1<double>(1, nbK);
    for (int iUC = 1; iUC <= nbK; iUC++)
    {
      Kmult->SetValue(iUC, 1);
      Knots->SetValue(iUC, iUC - 1.);
    }

    // Initialize the BSplineCurveWithKnotsAndRationalBSplineCurve
    RBSPL->Init(RUC->Name(),
                aDegree,
                RUC->ControlPointsList(),
                RUC->CurveForm(),
                RUC->ClosedCurve(),
                RUC->SelfIntersect(),
                Kmult,
                Knots,
                StepGeom_ktUnspecified,
                RUC->WeightsData());

    return MakeBSplineCurve(RBSPL, theLocalFactors);
  }

  if (SC->IsKind(STANDARD_TYPE(StepGeom_QuasiUniformCurveAndRationalBSplineCurve)))
  {
    const occ::handle<StepGeom_QuasiUniformCurveAndRationalBSplineCurve> RQUC =
      occ::down_cast<StepGeom_QuasiUniformCurveAndRationalBSplineCurve>(SC);
    int aDegree = RQUC->Degree();
    if (aDegree < 1 || aDegree > Geom_BSplineCurve::MaxDegree())
      return nullptr;
    const occ::handle<StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve> RBSPL =
      new StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve;

    // Compute Knots and KnotsMultiplicity
    const int                                      nbK = RQUC->NbControlPointsList() - aDegree + 1;
    const occ::handle<NCollection_HArray1<int>>    Kmult = new NCollection_HArray1<int>(1, nbK);
    const occ::handle<NCollection_HArray1<double>> Knots = new NCollection_HArray1<double>(1, nbK);
    for (int iRQUC = 1; iRQUC <= nbK; iRQUC++)
    {
      Kmult->SetValue(iRQUC, 1);
      Knots->SetValue(iRQUC, iRQUC - 1.);
    }
    Kmult->SetValue(1, aDegree + 1);
    Kmult->SetValue(nbK, aDegree + 1);
    // Initialize the BSplineCurveWithKnotsAndRationalBSplineCurve
    RBSPL->Init(RQUC->Name(),
                aDegree,
                RQUC->ControlPointsList(),
                RQUC->CurveForm(),
                RQUC->ClosedCurve(),
                RQUC->SelfIntersect(),
                Kmult,
                Knots,
                StepGeom_ktUnspecified,
                RQUC->WeightsData());

    return MakeBSplineCurve(RBSPL, theLocalFactors);
  }

  if (SC->IsKind(STANDARD_TYPE(StepGeom_Polyline)))
  { //: n6 abv 15 Feb 99
    return MakePolyline(occ::down_cast<StepGeom_Polyline>(SC), theLocalFactors);
  }

  return nullptr;
}

//=============================================================================
// Creation d' une BoundedCurve de Geom a partir d' une BoundedCurve de Step
//=============================================================================

occ::handle<Geom2d_BoundedCurve> StepToGeom::MakeBoundedCurve2d(
  const occ::handle<StepGeom_BoundedCurve>& SC,
  const StepData_Factors&                   theLocalFactors)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)))
  {
    return MakeBSplineCurve2d(
      occ::down_cast<StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve>(SC),
      theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnots)))
  {
    return MakeBSplineCurve2d(occ::down_cast<StepGeom_BSplineCurveWithKnots>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_TrimmedCurve)))
  {
    return MakeTrimmedCurve2d(occ::down_cast<StepGeom_TrimmedCurve>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Polyline)))
  { //: n6 abv 15 Feb 99
    return MakePolyline2d(occ::down_cast<StepGeom_Polyline>(SC), theLocalFactors);
  }
  return occ::handle<Geom2d_BoundedCurve>();
}

//=============================================================================
// Creation d' une BoundedSurface de Geom a partir d' une BoundedSurface de Step
//=============================================================================

occ::handle<Geom_BoundedSurface> StepToGeom::MakeBoundedSurface(
  const occ::handle<StepGeom_BoundedSurface>& SS,
  const StepData_Factors&                     theLocalFactors)
{
  if (SS->IsKind(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)))
  {
    return MakeBSplineSurface(
      occ::down_cast<StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface>(SS),
      theLocalFactors);
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnots)))
  {
    return MakeBSplineSurface(occ::down_cast<StepGeom_BSplineSurfaceWithKnots>(SS),
                              theLocalFactors);
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_RectangularTrimmedSurface)))
  {
    return MakeRectangularTrimmedSurface(occ::down_cast<StepGeom_RectangularTrimmedSurface>(SS),
                                         theLocalFactors);
  }

  // STEP BezierSurface, UniformSurface and QuasiUniformSurface are transformed
  // into STEP BSplineSurface before being mapped onto CAS.CADE/SF
  if (SS->IsKind(STANDARD_TYPE(StepGeom_BezierSurface)))
  {
    const occ::handle<StepGeom_BezierSurface> BzS = occ::down_cast<StepGeom_BezierSurface>(SS);
    const occ::handle<StepGeom_BSplineSurfaceWithKnots> BSPL = new StepGeom_BSplineSurfaceWithKnots;
    BSPL->SetUDegree(BzS->UDegree());
    BSPL->SetVDegree(BzS->VDegree());
    BSPL->SetControlPointsList(BzS->ControlPointsList());
    BSPL->SetSurfaceForm(BzS->SurfaceForm());
    BSPL->SetUClosed(BzS->UClosed());
    BSPL->SetVClosed(BzS->VClosed());
    BSPL->SetSelfIntersect(BzS->SelfIntersect());

    // Compute Knots and KnotsMultiplicity
    const occ::handle<NCollection_HArray1<int>>    UKmult = new NCollection_HArray1<int>(1, 2);
    const occ::handle<NCollection_HArray1<int>>    VKmult = new NCollection_HArray1<int>(1, 2);
    const occ::handle<NCollection_HArray1<double>> UKnots = new NCollection_HArray1<double>(1, 2);
    const occ::handle<NCollection_HArray1<double>> VKnots = new NCollection_HArray1<double>(1, 2);
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

    return MakeBSplineSurface(BSPL, theLocalFactors);
  }

  if (SS->IsKind(STANDARD_TYPE(StepGeom_UniformSurface)))
  {
    const occ::handle<StepGeom_UniformSurface> US = occ::down_cast<StepGeom_UniformSurface>(SS);
    const occ::handle<StepGeom_BSplineSurfaceWithKnots> BSPL = new StepGeom_BSplineSurfaceWithKnots;
    BSPL->SetUDegree(US->UDegree());
    BSPL->SetVDegree(US->VDegree());
    BSPL->SetControlPointsList(US->ControlPointsList());
    BSPL->SetSurfaceForm(US->SurfaceForm());
    BSPL->SetUClosed(US->UClosed());
    BSPL->SetVClosed(US->VClosed());
    BSPL->SetSelfIntersect(US->SelfIntersect());

    // Compute Knots and KnotsMultiplicity for U Direction
    const int nbKU = BSPL->NbControlPointsListI() + BSPL->UDegree() + 1;
    const occ::handle<NCollection_HArray1<int>>    UKmult = new NCollection_HArray1<int>(1, nbKU);
    const occ::handle<NCollection_HArray1<double>> UKnots =
      new NCollection_HArray1<double>(1, nbKU);
    for (int iU = 1; iU <= nbKU; iU++)
    {
      UKmult->SetValue(iU, 1);
      UKnots->SetValue(iU, iU - 1.);
    }
    BSPL->SetUMultiplicities(UKmult);
    BSPL->SetUKnots(UKnots);

    // Compute Knots and KnotsMultiplicity for V Direction
    const int nbKV = BSPL->NbControlPointsListJ() + BSPL->VDegree() + 1;
    const occ::handle<NCollection_HArray1<int>>    VKmult = new NCollection_HArray1<int>(1, nbKV);
    const occ::handle<NCollection_HArray1<double>> VKnots =
      new NCollection_HArray1<double>(1, nbKV);
    for (int iV = 1; iV <= nbKV; iV++)
    {
      VKmult->SetValue(iV, 1);
      VKnots->SetValue(iV, iV - 1.);
    }
    BSPL->SetVMultiplicities(VKmult);
    BSPL->SetVKnots(VKnots);

    return MakeBSplineSurface(BSPL, theLocalFactors);
  }

  if (SS->IsKind(STANDARD_TYPE(StepGeom_QuasiUniformSurface)))
  {
    const occ::handle<StepGeom_QuasiUniformSurface> QUS =
      occ::down_cast<StepGeom_QuasiUniformSurface>(SS);
    const occ::handle<StepGeom_BSplineSurfaceWithKnots> BSPL = new StepGeom_BSplineSurfaceWithKnots;
    BSPL->SetUDegree(QUS->UDegree());
    BSPL->SetVDegree(QUS->VDegree());
    BSPL->SetControlPointsList(QUS->ControlPointsList());
    BSPL->SetSurfaceForm(QUS->SurfaceForm());
    BSPL->SetUClosed(QUS->UClosed());
    BSPL->SetVClosed(QUS->VClosed());
    BSPL->SetSelfIntersect(QUS->SelfIntersect());

    // Compute Knots and KnotsMultiplicity for U Direction
    const int nbKU = BSPL->NbControlPointsListI() - BSPL->UDegree() + 1;
    const occ::handle<NCollection_HArray1<int>>    UKmult = new NCollection_HArray1<int>(1, nbKU);
    const occ::handle<NCollection_HArray1<double>> UKnots =
      new NCollection_HArray1<double>(1, nbKU);
    for (int iU = 1; iU <= nbKU; iU++)
    {
      UKmult->SetValue(iU, 1);
      UKnots->SetValue(iU, iU - 1.);
    }
    UKmult->SetValue(1, BSPL->UDegree() + 1);
    UKmult->SetValue(nbKU, BSPL->UDegree() + 1);
    BSPL->SetUMultiplicities(UKmult);
    BSPL->SetUKnots(UKnots);

    // Compute Knots and KnotsMultiplicity for V Direction
    const int nbKV = BSPL->NbControlPointsListJ() - BSPL->VDegree() + 1;
    const occ::handle<NCollection_HArray1<int>>    VKmult = new NCollection_HArray1<int>(1, nbKV);
    const occ::handle<NCollection_HArray1<double>> VKnots =
      new NCollection_HArray1<double>(1, nbKV);
    for (int iV = 1; iV <= nbKV; iV++)
    {
      VKmult->SetValue(iV, 1);
      VKnots->SetValue(iV, iV - 1.);
    }
    VKmult->SetValue(1, BSPL->VDegree() + 1);
    VKmult->SetValue(nbKV, BSPL->VDegree() + 1);
    BSPL->SetVMultiplicities(VKmult);
    BSPL->SetVKnots(VKnots);

    return MakeBSplineSurface(BSPL, theLocalFactors);
  }

  if (SS->IsKind(STANDARD_TYPE(StepGeom_UniformSurfaceAndRationalBSplineSurface)))
  {
    const occ::handle<StepGeom_UniformSurfaceAndRationalBSplineSurface> RUS =
      occ::down_cast<StepGeom_UniformSurfaceAndRationalBSplineSurface>(SS);
    const occ::handle<StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface> RBSPL =
      new StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface;

    // Compute Knots and KnotsMultiplicity for U Direction
    const int nbKU = RUS->NbControlPointsListI() + RUS->UDegree() + 1;
    const occ::handle<NCollection_HArray1<int>>    UKmult = new NCollection_HArray1<int>(1, nbKU);
    const occ::handle<NCollection_HArray1<double>> UKnots =
      new NCollection_HArray1<double>(1, nbKU);
    for (int iU = 1; iU <= nbKU; iU++)
    {
      UKmult->SetValue(iU, 1);
      UKnots->SetValue(iU, iU - 1.);
    }

    // Compute Knots and KnotsMultiplicity for V Direction
    const int nbKV = RUS->NbControlPointsListJ() + RUS->VDegree() + 1;
    const occ::handle<NCollection_HArray1<int>>    VKmult = new NCollection_HArray1<int>(1, nbKV);
    const occ::handle<NCollection_HArray1<double>> VKnots =
      new NCollection_HArray1<double>(1, nbKV);
    for (int iV = 1; iV <= nbKV; iV++)
    {
      VKmult->SetValue(iV, 1);
      VKnots->SetValue(iV, iV - 1.);
    }

    // Initialize the BSplineSurfaceWithKnotsAndRationalBSplineSurface
    RBSPL->Init(RUS->Name(),
                RUS->UDegree(),
                RUS->VDegree(),
                RUS->ControlPointsList(),
                RUS->SurfaceForm(),
                RUS->UClosed(),
                RUS->VClosed(),
                RUS->SelfIntersect(),
                UKmult,
                VKmult,
                UKnots,
                VKnots,
                StepGeom_ktUnspecified,
                RUS->WeightsData());

    return MakeBSplineSurface(RBSPL, theLocalFactors);
  }

  if (SS->IsKind(STANDARD_TYPE(StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface)))
  {
    const occ::handle<StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface> RQUS =
      occ::down_cast<StepGeom_QuasiUniformSurfaceAndRationalBSplineSurface>(SS);
    const occ::handle<StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface> RBSPL =
      new StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface;

    // Compute Knots and KnotsMultiplicity for U Direction
    const int nbKU = RQUS->NbControlPointsListI() - RQUS->UDegree() + 1;
    const occ::handle<NCollection_HArray1<int>>    UKmult = new NCollection_HArray1<int>(1, nbKU);
    const occ::handle<NCollection_HArray1<double>> UKnots =
      new NCollection_HArray1<double>(1, nbKU);
    for (int iU = 1; iU <= nbKU; iU++)
    {
      UKmult->SetValue(iU, 1);
      UKnots->SetValue(iU, iU - 1.);
    }
    UKmult->SetValue(1, RQUS->UDegree() + 1);
    UKmult->SetValue(nbKU, RQUS->UDegree() + 1);

    // Compute Knots and KnotsMultiplicity for V Direction
    const int nbKV = RQUS->NbControlPointsListJ() - RQUS->VDegree() + 1;
    const occ::handle<NCollection_HArray1<int>>    VKmult = new NCollection_HArray1<int>(1, nbKV);
    const occ::handle<NCollection_HArray1<double>> VKnots =
      new NCollection_HArray1<double>(1, nbKV);
    for (int iV = 1; iV <= nbKV; iV++)
    {
      VKmult->SetValue(iV, 1);
      VKnots->SetValue(iV, iV - 1.);
    }
    VKmult->SetValue(1, RQUS->VDegree() + 1);
    VKmult->SetValue(nbKV, RQUS->VDegree() + 1);

    // Initialize the BSplineSurfaceWithKnotsAndRationalBSplineSurface
    RBSPL->Init(RQUS->Name(),
                RQUS->UDegree(),
                RQUS->VDegree(),
                RQUS->ControlPointsList(),
                RQUS->SurfaceForm(),
                RQUS->UClosed(),
                RQUS->VClosed(),
                RQUS->SelfIntersect(),
                UKmult,
                VKmult,
                UKnots,
                VKnots,
                StepGeom_ktUnspecified,
                RQUS->WeightsData());
    return MakeBSplineSurface(RBSPL, theLocalFactors);
  }

  return nullptr;
}

//=============================================================================
// Template function for use in MakeBSplineCurve / MakeBSplineCurve2d
//=============================================================================

template <class TPntArray, class TCartesianPoint, class TGpPnt, class TBSplineCurve>
occ::handle<TBSplineCurve> MakeBSplineCurveCommon(
  const occ::handle<StepGeom_BSplineCurve>& theStepGeom_BSplineCurve,
  const StepData_Factors&                   theLocalFactors,
  TGpPnt (TCartesianPoint::*thePntGetterFunction)() const,
  occ::handle<TCartesianPoint> (*thePointMakerFunction)(const occ::handle<StepGeom_CartesianPoint>&,
                                                        const StepData_Factors&))
{
  occ::handle<StepGeom_BSplineCurveWithKnots> aBSplineCurveWithKnots;
  occ::handle<StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve>
    aBSplineCurveWithKnotsAndRationalBSplineCurve;

  if (theStepGeom_BSplineCurve->IsKind(
        STANDARD_TYPE(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)))
  {
    aBSplineCurveWithKnotsAndRationalBSplineCurve =
      occ::down_cast<StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve>(
        theStepGeom_BSplineCurve);
    aBSplineCurveWithKnots = aBSplineCurveWithKnotsAndRationalBSplineCurve->BSplineCurveWithKnots();
  }
  else
    aBSplineCurveWithKnots =
      occ::down_cast<StepGeom_BSplineCurveWithKnots>(theStepGeom_BSplineCurve);

  const int aDegree = aBSplineCurveWithKnots->Degree();
  const int NbPoles = aBSplineCurveWithKnots->NbControlPointsList();
  const int NbKnots = aBSplineCurveWithKnots->NbKnotMultiplicities();

  const occ::handle<NCollection_HArray1<int>>& aKnotMultiplicities =
    aBSplineCurveWithKnots->KnotMultiplicities();
  const occ::handle<NCollection_HArray1<double>>& aKnots = aBSplineCurveWithKnots->Knots();

  // Count number of unique knots
  int    NbUniqueKnots = 0;
  double lastKnot      = RealFirst();
  for (int i = 1; i <= NbKnots; ++i)
  {
    if (aKnots->Value(i) - lastKnot > Epsilon(std::abs(lastKnot)))
    {
      NbUniqueKnots++;
      lastKnot = aKnots->Value(i);
    }
  }
  if (NbUniqueKnots <= 1)
  {
    return nullptr;
  }
  NCollection_Array1<double> aUniqueKnots(1, NbUniqueKnots);
  NCollection_Array1<int>    aUniqueKnotMultiplicities(1, NbUniqueKnots);
  lastKnot = aKnots->Value(1);
  aUniqueKnots.SetValue(1, aKnots->Value(1));
  aUniqueKnotMultiplicities.SetValue(1, aKnotMultiplicities->Value(1));
  int aKnotPosition = 1;
  for (int i = 2; i <= NbKnots; i++)
  {
    if (aKnots->Value(i) - lastKnot > Epsilon(std::abs(lastKnot)))
    {
      aKnotPosition++;
      aUniqueKnots.SetValue(aKnotPosition, aKnots->Value(i));
      aUniqueKnotMultiplicities.SetValue(aKnotPosition, aKnotMultiplicities->Value(i));
      lastKnot = aKnots->Value(i);
    }
    else
    {
      // Knot not unique, increase multiplicity
      int aCurrentMultiplicity = aUniqueKnotMultiplicities.Value(aKnotPosition);
      aUniqueKnotMultiplicities.SetValue(aKnotPosition,
                                         aCurrentMultiplicity + aKnotMultiplicities->Value(i));
    }
  }

  int aFirstMuultypisityDifference = 0;
  int aLastMuultypisityDifference  = 0;
  for (int i = 1; i <= NbUniqueKnots; ++i)
  {
    int aCurrentVal = aUniqueKnotMultiplicities.Value(i);
    if (aCurrentVal > aDegree + 1)
    {
      if (i == 1)
        aFirstMuultypisityDifference = aCurrentVal - aDegree - 1;
      if (i == NbUniqueKnots)
        aLastMuultypisityDifference = aCurrentVal - aDegree - 1;
#ifdef OCCT_DEBUG
      std::cout << "\nWrong multiplicity " << aCurrentVal << " on " << i << " knot!"
                << "\nChanged to " << aDegree + 1 << std::endl;
#endif
      aCurrentVal = aDegree + 1;
    }
    aUniqueKnotMultiplicities.SetValue(i, aCurrentVal);
  }

  const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList =
    aBSplineCurveWithKnots->ControlPointsList();
  int aSummaryMuultypisityDifference = aFirstMuultypisityDifference + aLastMuultypisityDifference;
  int NbUniquePoles                  = NbPoles - aSummaryMuultypisityDifference;
  if (NbUniquePoles <= 0)
  {
    return nullptr;
  }
  TPntArray Poles(1, NbPoles - aSummaryMuultypisityDifference);

  for (int i = 1 + aFirstMuultypisityDifference; i <= NbPoles - aLastMuultypisityDifference; ++i)
  {
    occ::handle<TCartesianPoint> aPoint =
      (*thePointMakerFunction)(aControlPointsList->Value(i), theLocalFactors);
    if (!aPoint.IsNull())
    {
      TCartesianPoint* pPoint = aPoint.get();
      TGpPnt           aGpPnt = (pPoint->*thePntGetterFunction)();
      Poles.SetValue(i - aFirstMuultypisityDifference, aGpPnt);
    }
    else
    {
      return nullptr;
    }
  }

  // --- Does the Curve descriptor LOOKS like a periodic descriptor ? ---
  int aSummaryMuultypisity = 0;
  for (int i = 1; i <= NbUniqueKnots; i++)
  {
    aSummaryMuultypisity += aUniqueKnotMultiplicities.Value(i);
  }

  bool shouldBePeriodic;
  if (aSummaryMuultypisity == (NbPoles + aDegree + 1))
  {
    shouldBePeriodic = false;
  }
  else if ((aUniqueKnotMultiplicities.Value(1) == aUniqueKnotMultiplicities.Value(NbUniqueKnots))
           && ((aSummaryMuultypisity - aUniqueKnotMultiplicities.Value(1)) == NbPoles))
  {
    shouldBePeriodic = true;
  }
  else
  {
    // --- What is that ??? ---
    shouldBePeriodic = false;
  }

  occ::handle<TBSplineCurve> aBSplineCurve;
  if (theStepGeom_BSplineCurve->IsKind(
        STANDARD_TYPE(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)))
  {
    const occ::handle<NCollection_HArray1<double>>& aWeights =
      aBSplineCurveWithKnotsAndRationalBSplineCurve->WeightsData();
    NCollection_Array1<double> aUniqueWeights(1, NbPoles - aSummaryMuultypisityDifference);
    for (int i = 1 + aFirstMuultypisityDifference; i <= NbPoles - aLastMuultypisityDifference; ++i)
      aUniqueWeights.SetValue(i - aFirstMuultypisityDifference, aWeights->Value(i));
    aBSplineCurve = new TBSplineCurve(Poles,
                                      aUniqueWeights,
                                      aUniqueKnots,
                                      aUniqueKnotMultiplicities,
                                      aDegree,
                                      shouldBePeriodic);
  }
  else
  {
    aBSplineCurve =
      new TBSplineCurve(Poles, aUniqueKnots, aUniqueKnotMultiplicities, aDegree, shouldBePeriodic);
  }

  // abv 04.07.00 CAX-IF TRJ4: trj4_k1_top-md-203.stp #716 (face #581):
  // force periodicity on closed curves
  if (theStepGeom_BSplineCurve->ClosedCurve() && aBSplineCurve->Degree() > 1
      && aBSplineCurve->IsClosed())
  {
    aBSplineCurve->SetPeriodic();
  }
  return aBSplineCurve;
}

//=============================================================================
// Creation d' une BSplineCurve de Geom a partir d' une BSplineCurve de Step
//=============================================================================

occ::handle<Geom_BSplineCurve> StepToGeom::MakeBSplineCurve(
  const occ::handle<StepGeom_BSplineCurve>& theStepGeom_BSplineCurve,
  const StepData_Factors&                   theLocalFactors)
{
  return MakeBSplineCurveCommon<NCollection_Array1<gp_Pnt>,
                                Geom_CartesianPoint,
                                gp_Pnt,
                                Geom_BSplineCurve>(theStepGeom_BSplineCurve,
                                                   theLocalFactors,
                                                   &Geom_CartesianPoint::Pnt,
                                                   &MakeCartesianPoint);
}

//=============================================================================
// Creation d' une BSplineCurve de Geom2d a partir d' une
// BSplineCurveWithKnotsAndRationalBSplineCurve de Step
//=============================================================================

occ::handle<Geom2d_BSplineCurve> StepToGeom::MakeBSplineCurve2d(
  const occ::handle<StepGeom_BSplineCurve>& theStepGeom_BSplineCurve,
  const StepData_Factors&                   theLocalFactors)
{
  return MakeBSplineCurveCommon<NCollection_Array1<gp_Pnt2d>,
                                Geom2d_CartesianPoint,
                                gp_Pnt2d,
                                Geom2d_BSplineCurve>(theStepGeom_BSplineCurve,
                                                     theLocalFactors,
                                                     &Geom2d_CartesianPoint::Pnt2d,
                                                     &MakeCartesianPoint2d);
}

//=============================================================================
// Creation d' une BSplineSurface de Geom a partir d' une
// BSplineSurface de Step
//=============================================================================

occ::handle<Geom_BSplineSurface> StepToGeom::MakeBSplineSurface(
  const occ::handle<StepGeom_BSplineSurface>& SS,
  const StepData_Factors&                     theLocalFactors)
{
  int                                                                    i, j;
  occ::handle<StepGeom_BSplineSurfaceWithKnots>                          BS;
  occ::handle<StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface> BSR;

  if (SS->IsKind(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)))
  {
    BSR = occ::down_cast<StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface>(SS);
    BS  = BSR->BSplineSurfaceWithKnots();
  }
  else
    BS = occ::down_cast<StepGeom_BSplineSurfaceWithKnots>(SS);

  const int UDeg    = BS->UDegree();
  const int VDeg    = BS->VDegree();
  const int NUPoles = BS->NbControlPointsListI();
  const int NVPoles = BS->NbControlPointsListJ();
  const occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>>& aControlPointsList =
    BS->ControlPointsList();
  NCollection_Array2<gp_Pnt> Poles(1, NUPoles, 1, NVPoles);
  for (i = 1; i <= NUPoles; i++)
  {
    for (j = 1; j <= NVPoles; j++)
    {
      occ::handle<Geom_CartesianPoint> P =
        MakeCartesianPoint(aControlPointsList->Value(i, j), theLocalFactors);
      if (!P.IsNull())
        Poles.SetValue(i, j, P->Pnt());
      else
        return nullptr;
    }
  }
  const int                                       NUKnots          = BS->NbUMultiplicities();
  const occ::handle<NCollection_HArray1<int>>&    aUMultiplicities = BS->UMultiplicities();
  const occ::handle<NCollection_HArray1<double>>& aUKnots          = BS->UKnots();

  // count number of unique uknots
  double lastKnot      = RealFirst();
  int    NUKnotsUnique = 0;
  for (i = 1; i <= NUKnots; i++)
  {
    if (aUKnots->Value(i) - lastKnot > Epsilon(std::abs(lastKnot)))
    {
      NUKnotsUnique++;
      lastKnot = aUKnots->Value(i);
    }
  }

  // set umultiplicities and uknots
  NCollection_Array1<int>    UMult(1, NUKnotsUnique);
  NCollection_Array1<double> KUn(1, NUKnotsUnique);
  int                        pos = 1;
  lastKnot                       = aUKnots->Value(1);
  KUn.SetValue(1, aUKnots->Value(1));
  UMult.SetValue(1, aUMultiplicities->Value(1));
  for (i = 2; i <= NUKnots; i++)
  {
    if (aUKnots->Value(i) - lastKnot > Epsilon(std::abs(lastKnot)))
    {
      pos++;
      KUn.SetValue(pos, aUKnots->Value(i));
      UMult.SetValue(pos, aUMultiplicities->Value(i));
      lastKnot = aUKnots->Value(i);
    }
    else
    {
      // Knot not unique, increase multiplicity
      int curMult = UMult.Value(pos);
      UMult.SetValue(pos, curMult + aUMultiplicities->Value(i));
    }
  }
  const int                                       NVKnots          = BS->NbVMultiplicities();
  const occ::handle<NCollection_HArray1<int>>&    aVMultiplicities = BS->VMultiplicities();
  const occ::handle<NCollection_HArray1<double>>& aVKnots          = BS->VKnots();

  // count number of unique vknots
  lastKnot          = RealFirst();
  int NVKnotsUnique = 0;
  for (i = 1; i <= NVKnots; i++)
  {
    if (aVKnots->Value(i) - lastKnot > Epsilon(std::abs(lastKnot)))
    {
      NVKnotsUnique++;
      lastKnot = aVKnots->Value(i);
    }
  }

  // set vmultiplicities and vknots
  NCollection_Array1<int>    VMult(1, NVKnotsUnique);
  NCollection_Array1<double> KVn(1, NVKnotsUnique);
  pos      = 1;
  lastKnot = aVKnots->Value(1);
  KVn.SetValue(1, aVKnots->Value(1));
  VMult.SetValue(1, aVMultiplicities->Value(1));
  for (i = 2; i <= NVKnots; i++)
  {
    if (aVKnots->Value(i) - lastKnot > Epsilon(std::abs(lastKnot)))
    {
      pos++;
      KVn.SetValue(pos, aVKnots->Value(i));
      VMult.SetValue(pos, aVMultiplicities->Value(i));
      lastKnot = aVKnots->Value(i);
    }
    else
    {
      // Knot not unique, increase multiplicity
      int curMult = VMult.Value(pos);
      VMult.SetValue(pos, curMult + aVMultiplicities->Value(i));
    }
  }

  // --- Does the Surface Descriptor LOOKS like a U and/or V Periodic ---
  // --- Descriptor ? ---

  // --- U Periodic ? ---

  int SumMult = 0;
  for (i = 1; i <= NUKnotsUnique; i++)
  {
    SumMult += UMult.Value(i);
  }

  bool shouldBeUPeriodic = false;
  if (SumMult == (NUPoles + UDeg + 1))
  {
    // shouldBeUPeriodic = false;
  }
  else if ((UMult.Value(1) == UMult.Value(NUKnotsUnique))
           && ((SumMult - UMult.Value(1)) == NUPoles))
  {
    shouldBeUPeriodic = true;
  }

  // --- V Periodic ? ---

  SumMult = 0;
  for (i = 1; i <= NVKnotsUnique; i++)
  {
    SumMult += VMult.Value(i);
  }

  bool shouldBeVPeriodic = false;
  if (SumMult == (NVPoles + VDeg + 1))
  {
    // shouldBeVPeriodic = false;
  }
  else if ((VMult.Value(1) == VMult.Value(NVKnotsUnique))
           && ((SumMult - VMult.Value(1)) == NVPoles))
  {
    shouldBeVPeriodic = true;
  }

  occ::handle<Geom_BSplineSurface> CS;
  if (SS->IsKind(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)))
  {
    const occ::handle<NCollection_HArray2<double>>& aWeight = BSR->WeightsData();
    NCollection_Array2<double>                      W(1, NUPoles, 1, NVPoles);
    for (i = 1; i <= NUPoles; i++)
    {
      for (j = 1; j <= NVPoles; j++)
      {
        W.SetValue(i, j, aWeight->Value(i, j));
      }
    }
    CS = new Geom_BSplineSurface(Poles,
                                 W,
                                 KUn,
                                 KVn,
                                 UMult,
                                 VMult,
                                 UDeg,
                                 VDeg,
                                 shouldBeUPeriodic,
                                 shouldBeVPeriodic);
  }
  else
    CS = new Geom_BSplineSurface(Poles,
                                 KUn,
                                 KVn,
                                 UMult,
                                 VMult,
                                 UDeg,
                                 VDeg,
                                 shouldBeUPeriodic,
                                 shouldBeVPeriodic);
  return CS;
}

//=============================================================================
// Creation d' un CartesianPoint de Geom a partir d' un CartesianPoint de Step
//=============================================================================

occ::handle<Geom_CartesianPoint> StepToGeom::MakeCartesianPoint(
  const occ::handle<StepGeom_CartesianPoint>& SP,
  const StepData_Factors&                     theLocalFactors)
{
  if (!SP.IsNull() && SP->NbCoordinates() == 3)
  {
    const double LF = theLocalFactors.LengthFactor();
    const double X  = SP->CoordinatesValue(1) * LF;
    const double Y  = SP->CoordinatesValue(2) * LF;
    const double Z  = SP->CoordinatesValue(3) * LF;
    return new Geom_CartesianPoint(X, Y, Z);
  }
  return nullptr;
}

//=============================================================================
// Creation d' un CartesianPoint de Geom2d a partir d' un CartesianPoint de
// Step
//=============================================================================

occ::handle<Geom2d_CartesianPoint> StepToGeom::MakeCartesianPoint2d(
  const occ::handle<StepGeom_CartesianPoint>& SP,
  const StepData_Factors&                     theLocalFactors)
{
  (void)theLocalFactors;
  if (SP->NbCoordinates() == 2)
  {
    const double X = SP->CoordinatesValue(1);
    const double Y = SP->CoordinatesValue(2);
    return new Geom2d_CartesianPoint(X, Y);
  }
  return nullptr;
}

//=============================================================================
// Creation d' un Circle de Geom a partir d' un Circle de Step
//=============================================================================

occ::handle<Geom_Circle> StepToGeom::MakeCircle(const occ::handle<StepGeom_Circle>& SC,
                                                const StepData_Factors&             theLocalFactors)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2)
  {
    occ::handle<Geom_Axis2Placement> A =
      MakeAxis2Placement(occ::down_cast<StepGeom_Axis2Placement3d>(AxisSelect.Value()),
                         theLocalFactors);
    if (!A.IsNull())
    {
      return new Geom_Circle(A->Ax2(), SC->Radius() * theLocalFactors.LengthFactor());
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' un Circle de Geom2d a partir d' un Circle de Step
//=============================================================================

occ::handle<Geom2d_Circle> StepToGeom::MakeCircle2d(const occ::handle<StepGeom_Circle>& SC,
                                                    const StepData_Factors& theLocalFactors)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 1)
  {
    occ::handle<Geom2d_AxisPlacement> A1 =
      MakeAxisPlacement(occ::down_cast<StepGeom_Axis2Placement2d>(AxisSelect.Value()),
                        theLocalFactors);
    if (!A1.IsNull())
    {
      return new Geom2d_Circle(A1->Ax2d(), SC->Radius());
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' une Conic de Geom a partir d' une Conic de Step
//=============================================================================

occ::handle<Geom_Conic> StepToGeom::MakeConic(const occ::handle<StepGeom_Conic>& SC,
                                              const StepData_Factors&            theLocalFactors)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Circle)))
  {
    return MakeCircle(occ::down_cast<StepGeom_Circle>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Ellipse)))
  {
    return MakeEllipse(occ::down_cast<StepGeom_Ellipse>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Hyperbola)))
  {
    return MakeHyperbola(occ::down_cast<StepGeom_Hyperbola>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Parabola)))
  {
    return MakeParabola(occ::down_cast<StepGeom_Parabola>(SC), theLocalFactors);
  }
  // Attention : Other conic shall be implemented !
  return nullptr;
}

//=============================================================================
// Creation d' une Conic de Geom2d a partir d' une Conic de Step
//=============================================================================

occ::handle<Geom2d_Conic> StepToGeom::MakeConic2d(const occ::handle<StepGeom_Conic>& SC,
                                                  const StepData_Factors& theLocalFactors)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Circle)))
  {
    return MakeCircle2d(occ::down_cast<StepGeom_Circle>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Ellipse)))
  {
    return MakeEllipse2d(occ::down_cast<StepGeom_Ellipse>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Hyperbola)))
  {
    return MakeHyperbola2d(occ::down_cast<StepGeom_Hyperbola>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Parabola)))
  {
    return MakeParabola2d(occ::down_cast<StepGeom_Parabola>(SC), theLocalFactors);
  }
  // Attention : Other conic shall be implemented !
  return occ::handle<Geom2d_Conic>();
}

//=============================================================================
// Creation d' une ConicalSurface de Geom a partir d' une ConicalSurface de
// Step
//=============================================================================

occ::handle<Geom_ConicalSurface> StepToGeom::MakeConicalSurface(
  const occ::handle<StepGeom_ConicalSurface>& SS,
  const StepData_Factors&                     theLocalFactors)
{
  occ::handle<Geom_Axis2Placement> A = MakeAxis2Placement(SS->Position(), theLocalFactors);
  if (!A.IsNull())
  {
    const double R   = SS->Radius() * theLocalFactors.LengthFactor();
    const double Ang = SS->SemiAngle() * theLocalFactors.PlaneAngleFactor();
    // #2(K3-3) rln 12/02/98 ProSTEP ct_turbine-A.stp entity #518, #3571 (gp::Resolution() is too
    // little)
    return new Geom_ConicalSurface(A->Ax2(), std::max(Ang, Precision::Angular()), R);
  }
  return nullptr;
}

//=============================================================================
// Creation d' une Curve de Geom a partir d' une Curve de Step
//=============================================================================

occ::handle<Geom_Curve> StepToGeom::MakeCurve(const occ::handle<StepGeom_Curve>& SC,
                                              const StepData_Factors&            theLocalFactors)
{
  if (SC.IsNull())
  {
    return occ::handle<Geom_Curve>();
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Line)))
  {
    return MakeLine(occ::down_cast<StepGeom_Line>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_TrimmedCurve)))
  {
    return MakeTrimmedCurve(occ::down_cast<StepGeom_TrimmedCurve>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Conic)))
  {
    return MakeConic(occ::down_cast<StepGeom_Conic>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BoundedCurve)))
  {
    return MakeBoundedCurve(occ::down_cast<StepGeom_BoundedCurve>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_CurveReplica)))
  { //: n7 abv 16 Feb 99
    const occ::handle<StepGeom_CurveReplica> CR = occ::down_cast<StepGeom_CurveReplica>(SC);
    const occ::handle<StepGeom_Curve>        PC = CR->ParentCurve();
    const occ::handle<StepGeom_CartesianTransformationOperator3d> T =
      occ::down_cast<StepGeom_CartesianTransformationOperator3d>(CR->Transformation());
    // protect against cyclic references and wrong type of cartop
    if (!T.IsNull() && PC != SC)
    {
      occ::handle<Geom_Curve> C1 = MakeCurve(PC, theLocalFactors);
      if (!C1.IsNull())
      {
        gp_Trsf T1;
        if (MakeTransformation3d(T, T1, theLocalFactors))
        {
          C1->Transform(T1);
          return C1;
        }
      }
    }
  }
  else if (SC->IsKind(STANDARD_TYPE(StepGeom_OffsetCurve3d)))
  { //: o2 abv 17 Feb 99
    const occ::handle<StepGeom_OffsetCurve3d> OC = occ::down_cast<StepGeom_OffsetCurve3d>(SC);
    const occ::handle<StepGeom_Curve>         BC = OC->BasisCurve();
    if (BC != SC)
    { // protect against loop
      occ::handle<Geom_Curve> C1 = MakeCurve(BC, theLocalFactors);
      if (!C1.IsNull())
      {
        occ::handle<Geom_Direction> RD = MakeDirection(OC->RefDirection());
        if (!RD.IsNull())
        {
          return new Geom_OffsetCurve(C1, -OC->Distance(), RD->Dir());
        }
      }
    }
  }
  else if (SC->IsKind(STANDARD_TYPE(StepGeom_SurfaceCurve)))
  { //: o5 abv 17 Feb 99
    const occ::handle<StepGeom_SurfaceCurve> SurfC = occ::down_cast<StepGeom_SurfaceCurve>(SC);
    return MakeCurve(SurfC->Curve3d(), theLocalFactors);
  }
  return nullptr;
}

//=============================================================================
// Creation d' une Curve de Geom2d a partir d' une Curve de Step
//=============================================================================

occ::handle<Geom2d_Curve> StepToGeom::MakeCurve2d(const occ::handle<StepGeom_Curve>& SC,
                                                  const StepData_Factors& theLocalFactors)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Line)))
  {
    return MakeLine2d(occ::down_cast<StepGeom_Line>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Conic)))
  {
    return MakeConic2d(occ::down_cast<StepGeom_Conic>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BoundedCurve)))
  {
    return MakeBoundedCurve2d(occ::down_cast<StepGeom_BoundedCurve>(SC), theLocalFactors);
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_CurveReplica)))
  { //: n7 abv 16 Feb 99
    const occ::handle<StepGeom_CurveReplica> CR = occ::down_cast<StepGeom_CurveReplica>(SC);
    const occ::handle<StepGeom_Curve>        PC = CR->ParentCurve();
    const occ::handle<StepGeom_CartesianTransformationOperator2d> T =
      occ::down_cast<StepGeom_CartesianTransformationOperator2d>(CR->Transformation());
    // protect against cyclic references and wrong type of cartop
    if (!T.IsNull() && PC != SC)
    {
      occ::handle<Geom2d_Curve> C1 = MakeCurve2d(PC, theLocalFactors);
      if (!C1.IsNull())
      {
        gp_Trsf2d T1;
        if (MakeTransformation2d(T, T1, theLocalFactors))
        {
          C1->Transform(T1);
          return C1;
        }
      }
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' une CylindricalSurface de Geom a partir d' une
// CylindricalSurface de Step
//=============================================================================

occ::handle<Geom_CylindricalSurface> StepToGeom::MakeCylindricalSurface(
  const occ::handle<StepGeom_CylindricalSurface>& SS,
  const StepData_Factors&                         theLocalFactors)
{
  occ::handle<Geom_Axis2Placement> A = MakeAxis2Placement(SS->Position(), theLocalFactors);
  if (!A.IsNull())
  {
    return new Geom_CylindricalSurface(A->Ax2(), SS->Radius() * theLocalFactors.LengthFactor());
  }
  return nullptr;
}

//=============================================================================
// Creation d' un Direction de Geom a partir d' un Direction de Step
//=============================================================================

occ::handle<Geom_Direction> StepToGeom::MakeDirection(const occ::handle<StepGeom_Direction>& SD)
{
  if (SD->NbDirectionRatios() >= 3)
  {
    const double X = SD->DirectionRatiosValue(1);
    const double Y = SD->DirectionRatiosValue(2);
    const double Z = SD->DirectionRatiosValue(3);
    // 5.08.2021. Unstable test bugs xde bug24759: Y is very large value - FPE in SquareModulus
    if (Precision::IsInfinite(X) || Precision::IsInfinite(Y) || Precision::IsInfinite(Z))
    {
      return nullptr;
    }
    // sln 22.10.2001. CTS23496: Direction is not created if it has null magnitude
    if (gp_XYZ(X, Y, Z).SquareModulus() > gp::Resolution() * gp::Resolution())
    {
      return new Geom_Direction(X, Y, Z);
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' un Direction de Geom2d a partir d' un Direction de Step
//=============================================================================

occ::handle<Geom2d_Direction> StepToGeom::MakeDirection2d(const occ::handle<StepGeom_Direction>& SD)
{
  if (SD->NbDirectionRatios() >= 2)
  {
    const double X = SD->DirectionRatiosValue(1);
    const double Y = SD->DirectionRatiosValue(2);
    // sln 23.10.2001. CTS23496: Direction is not created if it has null magnitude
    if (gp_XY(X, Y).SquareModulus() > gp::Resolution() * gp::Resolution())
    {
      return new Geom2d_Direction(X, Y);
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' une ElementarySurface de Geom a partir d' une
// ElementarySurface de Step
//=============================================================================

occ::handle<Geom_ElementarySurface> StepToGeom::MakeElementarySurface(
  const occ::handle<StepGeom_ElementarySurface>& SS,
  const StepData_Factors&                        theLocalFactors)
{
  if (SS->IsKind(STANDARD_TYPE(StepGeom_Plane)))
  {
    return MakePlane(occ::down_cast<StepGeom_Plane>(SS), theLocalFactors);
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_CylindricalSurface)))
  {
    return MakeCylindricalSurface(occ::down_cast<StepGeom_CylindricalSurface>(SS), theLocalFactors);
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_ConicalSurface)))
  {
    return MakeConicalSurface(occ::down_cast<StepGeom_ConicalSurface>(SS), theLocalFactors);
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_SphericalSurface)))
  {
    return MakeSphericalSurface(occ::down_cast<StepGeom_SphericalSurface>(SS), theLocalFactors);
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_ToroidalSurface)))
  {
    return MakeToroidalSurface(occ::down_cast<StepGeom_ToroidalSurface>(SS), theLocalFactors);
  }
  return nullptr;
}

//=============================================================================
// Creation d' un Ellipse de Geom a partir d' un Ellipse de Step
//=============================================================================

occ::handle<Geom_Ellipse> StepToGeom::MakeEllipse(const occ::handle<StepGeom_Ellipse>& SC,
                                                  const StepData_Factors& theLocalFactors)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2)
  {
    occ::handle<Geom_Axis2Placement> A1 =
      MakeAxis2Placement(occ::down_cast<StepGeom_Axis2Placement3d>(AxisSelect.Value()),
                         theLocalFactors);
    if (!A1.IsNull())
    {
      gp_Ax2       A(A1->Ax2());
      const double LF     = theLocalFactors.LengthFactor();
      const double majorR = SC->SemiAxis1() * LF;
      const double minorR = SC->SemiAxis2() * LF;
      if (majorR - minorR >= 0.)
      { //: o9 abv 19 Feb 99
        return new Geom_Ellipse(A, majorR, minorR);
      }
      //: o9 abv 19 Feb 99
      else
      {
        A.SetXDirection(A.XDirection() ^ A.Direction());
        return new Geom_Ellipse(A, minorR, majorR);
      }
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' un Ellipse de Geom2d a partir d' un Ellipse de Step
//=============================================================================

occ::handle<Geom2d_Ellipse> StepToGeom::MakeEllipse2d(const occ::handle<StepGeom_Ellipse>& SC,
                                                      const StepData_Factors& theLocalFactors)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 1)
  {
    occ::handle<Geom2d_AxisPlacement> A1 =
      MakeAxisPlacement(occ::down_cast<StepGeom_Axis2Placement2d>(AxisSelect.Value()),
                        theLocalFactors);
    if (!A1.IsNull())
    {
      gp_Ax22d     A(A1->Ax2d());
      const double majorR = SC->SemiAxis1();
      const double minorR = SC->SemiAxis2();
      if (majorR - minorR >= 0.)
      { //: o9 abv 19 Feb 99: bm4_id_punch_b.stp #678: protection
        return new Geom2d_Ellipse(A, majorR, minorR);
      }
      else
      {
        const gp_Dir2d X = A.XDirection();
        A.SetXDirection(gp_Dir2d(X.X(), -X.Y()));
        return new Geom2d_Ellipse(A, minorR, majorR);
      }
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' un Hyperbola de Geom a partir d' un Hyperbola de Step
//=============================================================================

occ::handle<Geom_Hyperbola> StepToGeom::MakeHyperbola(const occ::handle<StepGeom_Hyperbola>& SC,
                                                      const StepData_Factors& theLocalFactors)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2)
  {
    occ::handle<Geom_Axis2Placement> A1 =
      MakeAxis2Placement(occ::down_cast<StepGeom_Axis2Placement3d>(AxisSelect.Value()),
                         theLocalFactors);
    if (!A1.IsNull())
    {
      const gp_Ax2 A(A1->Ax2());
      const double LF = theLocalFactors.LengthFactor();
      return new Geom_Hyperbola(A, SC->SemiAxis() * LF, SC->SemiImagAxis() * LF);
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' un Hyperbola de Geom2d a partir d' un Hyperbola de Step
//=============================================================================

occ::handle<Geom2d_Hyperbola> StepToGeom::MakeHyperbola2d(const occ::handle<StepGeom_Hyperbola>& SC,
                                                          const StepData_Factors& theLocalFactors)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 1)
  {
    occ::handle<Geom2d_AxisPlacement> A1 =
      MakeAxisPlacement(occ::down_cast<StepGeom_Axis2Placement2d>(AxisSelect.Value()),
                        theLocalFactors);
    if (!A1.IsNull())
    {
      const gp_Ax22d A(A1->Ax2d());
      return new Geom2d_Hyperbola(A, SC->SemiAxis(), SC->SemiImagAxis());
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' une Line de Geom a partir d' une Line de Step
//=============================================================================

occ::handle<Geom_Line> StepToGeom::MakeLine(const occ::handle<StepGeom_Line>& SC,
                                            const StepData_Factors&           theLocalFactors)
{
  occ::handle<Geom_CartesianPoint> P = MakeCartesianPoint(SC->Pnt(), theLocalFactors);
  if (!P.IsNull())
  {
    // sln 22.10.2001. CTS23496: Line is not created if direction have not been successfully created
    occ::handle<Geom_VectorWithMagnitude> D = MakeVectorWithMagnitude(SC->Dir(), theLocalFactors);
    if (!D.IsNull())
    {
      if (D->Vec().SquareMagnitude() < Precision::Confusion() * Precision::Confusion())
        return nullptr;
      const gp_Dir V(D->Vec());
      return new Geom_Line(P->Pnt(), V);
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' une Line de Geom2d a partir d' une Line de Step
//=============================================================================

occ::handle<Geom2d_Line> StepToGeom::MakeLine2d(const occ::handle<StepGeom_Line>& SC,
                                                const StepData_Factors&           theLocalFactors)
{
  occ::handle<Geom2d_CartesianPoint> P = MakeCartesianPoint2d(SC->Pnt(), theLocalFactors);
  if (!P.IsNull())
  {
    // sln 23.10.2001. CTS23496: Line is not created if direction have not been successfully created
    occ::handle<Geom2d_VectorWithMagnitude> D = MakeVectorWithMagnitude2d(SC->Dir());
    if (!D.IsNull())
    {
      const gp_Dir2d D1(D->Vec2d());
      return new Geom2d_Line(P->Pnt2d(), D1);
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' un Parabola de Geom a partir d' un Parabola de Step
//=============================================================================

occ::handle<Geom_Parabola> StepToGeom::MakeParabola(const occ::handle<StepGeom_Parabola>& SC,
                                                    const StepData_Factors& theLocalFactors)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2)
  {
    occ::handle<Geom_Axis2Placement> A =
      MakeAxis2Placement(occ::down_cast<StepGeom_Axis2Placement3d>(AxisSelect.Value()),
                         theLocalFactors);
    if (!A.IsNull())
    {
      return new Geom_Parabola(A->Ax2(), SC->FocalDist() * theLocalFactors.LengthFactor());
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' un Parabola de Geom2d a partir d' un Parabola de Step
//=============================================================================

occ::handle<Geom2d_Parabola> StepToGeom::MakeParabola2d(const occ::handle<StepGeom_Parabola>& SC,
                                                        const StepData_Factors& theLocalFactors)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 1)
  {
    occ::handle<Geom2d_AxisPlacement> A1 =
      MakeAxisPlacement(occ::down_cast<StepGeom_Axis2Placement2d>(AxisSelect.Value()),
                        theLocalFactors);
    if (!A1.IsNull())
    {
      const gp_Ax22d A(A1->Ax2d());
      return new Geom2d_Parabola(A, SC->FocalDist());
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' un Plane de Geom a partir d' un plane de Step
//=============================================================================

occ::handle<Geom_Plane> StepToGeom::MakePlane(const occ::handle<StepGeom_Plane>& SP,
                                              const StepData_Factors&            theLocalFactors)
{
  occ::handle<Geom_Axis2Placement> A = MakeAxis2Placement(SP->Position(), theLocalFactors);
  if (!A.IsNull())
  {
    return new Geom_Plane(A->Ax2());
  }
  return nullptr;
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> StepToGeom::MakePolyline(const occ::handle<StepGeom_Polyline>& SPL,
                                                        const StepData_Factors& theLocalFactors)
{
  if (SPL.IsNull())
    return occ::handle<Geom_BSplineCurve>();

  const int nbp = SPL->NbPoints();
  if (nbp > 1)
  {
    NCollection_Array1<gp_Pnt> Poles(1, nbp);
    NCollection_Array1<double> Knots(1, nbp);
    NCollection_Array1<int>    Mults(1, nbp);

    for (int i = 1; i <= nbp; i++)
    {
      occ::handle<Geom_CartesianPoint> P = MakeCartesianPoint(SPL->PointsValue(i), theLocalFactors);
      if (!P.IsNull())
        Poles.SetValue(i, P->Pnt());
      else
        return nullptr;
      Knots.SetValue(i, double(i - 1));
      Mults.SetValue(i, 1);
    }
    Mults.SetValue(1, 2);
    Mults.SetValue(nbp, 2);

    return new Geom_BSplineCurve(Poles, Knots, Mults, 1);
  }
  return nullptr;
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> StepToGeom::MakePolyline2d(
  const occ::handle<StepGeom_Polyline>& SPL,
  const StepData_Factors&               theLocalFactors)
{
  if (SPL.IsNull())
    return occ::handle<Geom2d_BSplineCurve>();

  const int nbp = SPL->NbPoints();
  if (nbp > 1)
  {
    NCollection_Array1<gp_Pnt2d> Poles(1, nbp);
    NCollection_Array1<double>   Knots(1, nbp);
    NCollection_Array1<int>      Mults(1, nbp);

    for (int i = 1; i <= nbp; i++)
    {
      occ::handle<Geom2d_CartesianPoint> P =
        MakeCartesianPoint2d(SPL->PointsValue(i), theLocalFactors);
      if (!P.IsNull())
        Poles.SetValue(i, P->Pnt2d());
      else
        return nullptr;
      Knots.SetValue(i, double(i - 1));
      Mults.SetValue(i, 1);
    }
    Mults.SetValue(1, 2);
    Mults.SetValue(nbp, 2);

    return new Geom2d_BSplineCurve(Poles, Knots, Mults, 1);
  }
  return nullptr;
}

//=============================================================================
// Creation d' une RectangularTrimmedSurface de Geom a partir d' une
// RectangularTrimmedSurface de Step
//=============================================================================

occ::handle<Geom_RectangularTrimmedSurface> StepToGeom::MakeRectangularTrimmedSurface(
  const occ::handle<StepGeom_RectangularTrimmedSurface>& SS,
  const StepData_Factors&                                theLocalFactors)
{
  occ::handle<Geom_Surface> theBasis = MakeSurface(SS->BasisSurface(), theLocalFactors);
  if (!theBasis.IsNull())
  {
    // -----------------------------------------
    // Modification of the Trimming Parameters ?
    // -----------------------------------------

    double       uFact      = 1.;
    double       vFact      = 1.;
    const double LengthFact = theLocalFactors.LengthFactor();
    // clang-format off
    const double AngleFact = theLocalFactors.PlaneAngleFactor(); // abv 30.06.00 trj4_k1_geo-tc-214.stp #1477: PI/180.;
    // clang-format on

    if (theBasis->IsKind(STANDARD_TYPE(Geom_SphericalSurface))
        || theBasis->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
    {
      uFact = vFact = AngleFact;
    }
    else if (theBasis->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
    {
      uFact = AngleFact;
      vFact = LengthFact;
    }
    else if (theBasis->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
    {
      uFact = AngleFact;
    }
    else if (theBasis->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
    {
      const occ::handle<Geom_ConicalSurface> conicS = occ::down_cast<Geom_ConicalSurface>(theBasis);
      uFact                                         = AngleFact;
      vFact                                         = LengthFact / std::cos(conicS->SemiAngle());
    }
    else if (theBasis->IsKind(STANDARD_TYPE(Geom_Plane)))
    {
      uFact = vFact = LengthFact;
    }

    const double U1 = SS->U1() * uFact;
    const double U2 = SS->U2() * uFact;
    const double V1 = SS->V1() * vFact;
    const double V2 = SS->V2() * vFact;

    return new Geom_RectangularTrimmedSurface(theBasis, U1, U2, V1, V2, SS->Usense(), SS->Vsense());
  }
  return nullptr;
}

//=============================================================================
// Creation d' une SphericalSurface de Geom a partir d' une
// SphericalSurface de Step
//=============================================================================

occ::handle<Geom_SphericalSurface> StepToGeom::MakeSphericalSurface(
  const occ::handle<StepGeom_SphericalSurface>& SS,
  const StepData_Factors&                       theLocalFactors)
{
  occ::handle<Geom_Axis2Placement> A = MakeAxis2Placement(SS->Position(), theLocalFactors);
  if (!A.IsNull())
  {
    return new Geom_SphericalSurface(A->Ax2(), SS->Radius() * theLocalFactors.LengthFactor());
  }
  return nullptr;
}

//=============================================================================
// Creation d' une Surface de Geom a partir d' une Surface de Step
//=============================================================================

occ::handle<Geom_Surface> StepToGeom::MakeSurface(const occ::handle<StepGeom_Surface>& SS,
                                                  const StepData_Factors& theLocalFactors)
{
  // sln 01.10.2001 BUC61003. If entry shell is NULL do nothing
  if (SS.IsNull())
  {
    return occ::handle<Geom_Surface>();
  }

  try
  {
    OCC_CATCH_SIGNALS
    if (SS->IsKind(STANDARD_TYPE(StepGeom_BoundedSurface)))
    {
      return MakeBoundedSurface(occ::down_cast<StepGeom_BoundedSurface>(SS), theLocalFactors);
    }
    if (SS->IsKind(STANDARD_TYPE(StepGeom_ElementarySurface)))
    {
      const occ::handle<StepGeom_ElementarySurface> S1 =
        occ::down_cast<StepGeom_ElementarySurface>(SS);
      if (S1->Position().IsNull())
        return occ::handle<Geom_Surface>();

      return MakeElementarySurface(S1, theLocalFactors);
    }
    if (SS->IsKind(STANDARD_TYPE(StepGeom_SweptSurface)))
    {
      return MakeSweptSurface(occ::down_cast<StepGeom_SweptSurface>(SS), theLocalFactors);
    }
    if (SS->IsKind(STANDARD_TYPE(StepGeom_OffsetSurface)))
    { //: d4 abv 12 Mar 98
      const occ::handle<StepGeom_OffsetSurface> OS = occ::down_cast<StepGeom_OffsetSurface>(SS);

      occ::handle<Geom_Surface> aBasisSurface = MakeSurface(OS->BasisSurface(), theLocalFactors);
      if (!aBasisSurface.IsNull())
      {
        // sln 03.10.01. BUC61003. creation of  offset surface is corrected
        const double anOffset = OS->Distance() * theLocalFactors.LengthFactor();
        if (aBasisSurface->Continuity() == GeomAbs_C0)
        {
          const BRepBuilderAPI_MakeFace aBFace(aBasisSurface, Precision::Confusion());
          if (aBFace.IsDone())
          {
            const TopoDS_Shape aResult =
              ShapeAlgo::AlgoContainer()->C0ShapeToC1Shape(aBFace.Face(), std::abs(anOffset));
            if (aResult.ShapeType() == TopAbs_FACE)
            {
              aBasisSurface = BRep_Tool::Surface(TopoDS::Face(aResult));
            }
          }
        }
        if (aBasisSurface->Continuity() != GeomAbs_C0)
        {
          return new Geom_OffsetSurface(aBasisSurface, anOffset);
        }
      }
    }
    else if (SS->IsKind(STANDARD_TYPE(StepGeom_SurfaceReplica)))
    { //: n7 abv 16 Feb 99
      const occ::handle<StepGeom_SurfaceReplica> SR = occ::down_cast<StepGeom_SurfaceReplica>(SS);
      const occ::handle<StepGeom_Surface>        PS = SR->ParentSurface();
      const occ::handle<StepGeom_CartesianTransformationOperator3d> T = SR->Transformation();
      // protect against cyclic references and wrong type of cartop
      if (!T.IsNull() && PS != SS)
      {
        occ::handle<Geom_Surface> S1 = MakeSurface(PS, theLocalFactors);
        if (!S1.IsNull())
        {
          gp_Trsf T1;
          if (MakeTransformation3d(T, T1, theLocalFactors))
          {
            S1->Transform(T1);
            return S1;
          }
        }
      }
    }
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    //   ShapeTool_DB ?
    //: s5
    std::cout << "Warning: MakeSurface: Exception:";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
  }
  return nullptr;
}

//=============================================================================
// Creation d' une SurfaceOfLinearExtrusion de Geom a partir d' une
// SurfaceOfLinearExtrusion de Step
//=============================================================================

occ::handle<Geom_SurfaceOfLinearExtrusion> StepToGeom::MakeSurfaceOfLinearExtrusion(
  const occ::handle<StepGeom_SurfaceOfLinearExtrusion>& SS,
  const StepData_Factors&                               theLocalFactors)
{
  occ::handle<Geom_Curve> C = MakeCurve(SS->SweptCurve(), theLocalFactors);
  if (!C.IsNull())
  {
    // sln 23.10.2001. CTS23496: Surface is not created if extrusion axis have not been successfully
    // created
    occ::handle<Geom_VectorWithMagnitude> V =
      MakeVectorWithMagnitude(SS->ExtrusionAxis(), theLocalFactors);
    if (!V.IsNull())
    {
      const gp_Dir           D(V->Vec());
      occ::handle<Geom_Line> aLine = occ::down_cast<Geom_Line>(C);
      if (!aLine.IsNull() && aLine->Lin().Direction().IsParallel(D, Precision::Angular()))
        return occ::handle<Geom_SurfaceOfLinearExtrusion>();
      return new Geom_SurfaceOfLinearExtrusion(C, D);
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' une SurfaceOfRevolution de Geom a partir d' une
// SurfaceOfRevolution de Step
//=============================================================================

occ::handle<Geom_SurfaceOfRevolution> StepToGeom::MakeSurfaceOfRevolution(
  const occ::handle<StepGeom_SurfaceOfRevolution>& SS,
  const StepData_Factors&                          theLocalFactors)
{
  occ::handle<Geom_Curve> C = MakeCurve(SS->SweptCurve(), theLocalFactors);
  if (!C.IsNull())
  {
    occ::handle<Geom_Axis1Placement> A1 = MakeAxis1Placement(SS->AxisPosition(), theLocalFactors);
    if (!A1.IsNull())
    {
      const gp_Ax1 A(A1->Ax1());
      // skl for OCC952 (one bad case revolution of circle)
      if (C->IsKind(STANDARD_TYPE(Geom_Circle)) || C->IsKind(STANDARD_TYPE(Geom_Ellipse)))
      {
        const occ::handle<Geom_Conic> conic = occ::down_cast<Geom_Conic>(C);
        const gp_Pnt                  pc    = conic->Location();
        const gp_Lin                  rl(A);
        if (rl.Distance(pc) < Precision::Confusion())
        { // pc lies on A2
          const gp_Dir dirline = A.Direction();
          const gp_Dir norm    = conic->Axis().Direction();
          const gp_Dir xAxis   = conic->XAxis().Direction();
          // checking A2 lies on plane of circle
          if (dirline.IsNormal(norm, Precision::Angular())
              && (dirline.IsParallel(xAxis, Precision::Angular())
                  || C->IsKind(STANDARD_TYPE(Geom_Circle))))
          {
            // change parametrization for trimming
            gp_Ax2 axnew(pc, norm, dirline.Reversed());
            conic->SetPosition(axnew);
            C = new Geom_TrimmedCurve(conic, 0., M_PI);
          }
        }
      }
      return new Geom_SurfaceOfRevolution(C, A);
    }
  }
  return nullptr;
}

//=============================================================================
// Creation d' une SweptSurface de prostep a partir d' une
// SweptSurface de Geom
//=============================================================================

occ::handle<Geom_SweptSurface> StepToGeom::MakeSweptSurface(
  const occ::handle<StepGeom_SweptSurface>& SS,
  const StepData_Factors&                   theLocalFactors)
{
  if (SS->IsKind(STANDARD_TYPE(StepGeom_SurfaceOfLinearExtrusion)))
  {
    return MakeSurfaceOfLinearExtrusion(occ::down_cast<StepGeom_SurfaceOfLinearExtrusion>(SS),
                                        theLocalFactors);
  }
  if (SS->IsKind(STANDARD_TYPE(StepGeom_SurfaceOfRevolution)))
  {
    return MakeSurfaceOfRevolution(occ::down_cast<StepGeom_SurfaceOfRevolution>(SS),
                                   theLocalFactors);
  }
  return occ::handle<Geom_SweptSurface>();
}

//=============================================================================
// Creation d' une ToroidalSurface de Geom a partir d' une
// ToroidalSurface de Step
//=============================================================================

occ::handle<Geom_ToroidalSurface> StepToGeom::MakeToroidalSurface(
  const occ::handle<StepGeom_ToroidalSurface>& SS,
  const StepData_Factors&                      theLocalFactors)
{
  occ::handle<Geom_Axis2Placement> A = MakeAxis2Placement(SS->Position(), theLocalFactors);
  if (!A.IsNull())
  {
    const double LF = theLocalFactors.LengthFactor();
    return new Geom_ToroidalSurface(A->Ax2(),
                                    std::abs(SS->MajorRadius() * LF),
                                    std::abs(SS->MinorRadius() * LF));
  }
  return nullptr;
}

//=================================================================================================

bool StepToGeom::MakeTransformation2d(
  const occ::handle<StepGeom_CartesianTransformationOperator2d>& SCTO,
  gp_Trsf2d&                                                     CT,
  const StepData_Factors&                                        theLocalFactors)
{
  //  NB : on ne s interesse ici qu au deplacement rigide
  occ::handle<Geom2d_CartesianPoint> CP =
    MakeCartesianPoint2d(SCTO->LocalOrigin(), theLocalFactors);
  if (!CP.IsNull())
  {
    gp_Dir2d D1(gp_Dir2d::D::X);
    // sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is
    // used
    const occ::handle<StepGeom_Direction> A = SCTO->Axis1();
    if (!A.IsNull())
    {
      occ::handle<Geom2d_Direction> D = MakeDirection2d(A);
      if (!D.IsNull())
        D1 = D->Dir2d();
    }
    const gp_Ax2d result(CP->Pnt2d(), D1);
    CT.SetTransformation(result);
    CT = CT.Inverted();
    return true;
  }
  return false;
}

//=================================================================================================

bool StepToGeom::MakeTransformation3d(
  const occ::handle<StepGeom_CartesianTransformationOperator3d>& SCTO,
  gp_Trsf&                                                       CT,
  const StepData_Factors&                                        theLocalFactors)
{
  occ::handle<Geom_CartesianPoint> CP = MakeCartesianPoint(SCTO->LocalOrigin(), theLocalFactors);
  if (!CP.IsNull())
  {
    const gp_Pnt Pgp = CP->Pnt();

    // sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is
    // used
    gp_Dir                                D1(gp_Dir::D::X);
    const occ::handle<StepGeom_Direction> A1 = SCTO->Axis1();
    if (!A1.IsNull())
    {
      occ::handle<Geom_Direction> D = MakeDirection(A1);
      if (!D.IsNull())
        D1 = D->Dir();
    }

    gp_Dir                                D2(gp_Dir::D::Y);
    const occ::handle<StepGeom_Direction> A2 = SCTO->Axis2();
    if (!A2.IsNull())
    {
      occ::handle<Geom_Direction> D = MakeDirection(A2);
      if (!D.IsNull())
        D2 = D->Dir();
    }

    bool                                  isDefaultDirectionUsed = true;
    gp_Dir                                D3;
    const occ::handle<StepGeom_Direction> A3 = SCTO->Axis3();
    if (!A3.IsNull())
    {
      occ::handle<Geom_Direction> D = MakeDirection(A3);
      if (!D.IsNull())
      {
        D3                     = D->Dir();
        isDefaultDirectionUsed = false;
      }
    }
    if (isDefaultDirectionUsed)
      D3 = D1.Crossed(D2);

    const gp_Ax3 result(Pgp, D3, D1);
    CT.SetTransformation(result);
    if (SCTO->HasScale())
    {
      CT.SetScaleFactor(SCTO->Scale());
    }
    CT = CT.Inverted(); //: n8 abv 16 Feb 99: tr8_as2_db.stp: reverse for accordance with LV tool
    return true;
  }
  return false;
}

// ----------------------------------------------------------------
// ExtractParameter
// ----------------------------------------------------------------
//: o6 abv 18 Feb 99: parameter Factor added
//: p3 abv 23 Feb 99: parameter Shift added
static bool ExtractParameter(const occ::handle<Geom_Curve>& aGeomCurve,
                             const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& TS,
                             const int                                                        nbSel,
                             const int               MasterRep,
                             const double            Factor,
                             const double            Shift,
                             double&                 aParam,
                             const StepData_Factors& theLocalFactors)
{
  occ::handle<StepGeom_CartesianPoint> aPoint;
  int                                  i;
  //: S4136  double precBrep = BRepAPI::Precision();
  for (i = 1; i <= nbSel; i++)
  {
    StepGeom_TrimmingSelect theSel = TS->Value(i);
    if (MasterRep == 2 && theSel.CaseMember() > 0)
    {
      aParam = Shift + Factor * theSel.ParameterValue();
      return true;
    }
    else if (MasterRep == 1 && theSel.CaseNumber() > 0)
    {
      aPoint = theSel.CartesianPoint();
      occ::handle<Geom_CartesianPoint> theGeomPnt =
        StepToGeom::MakeCartesianPoint(aPoint, theLocalFactors);
      gp_Pnt thegpPnt = theGeomPnt->Pnt();

      //: S4136: use advanced algorithm
      ShapeAnalysis_Curve sac;
      gp_Pnt              p;
      sac.Project(aGeomCurve, thegpPnt, Precision::Confusion(), p, aParam);
      /* //:S4136
            //Trim == natural boundary ?
            if(aGeomCurve->IsKind(STANDARD_TYPE(Geom_BoundedCurve))) {
          double frstPar = aGeomCurve->FirstParameter();
          double lstPar = aGeomCurve->LastParameter();
          gp_Pnt frstPnt = aGeomCurve->Value(frstPar);
          gp_Pnt lstPnt = aGeomCurve->Value(lstPar);
          if(frstPnt.IsEqual(thegpPnt,precBrep)) {
            aParam = frstPar;
            return true;
          }
          if(lstPnt.IsEqual(thegpPnt,precBrep)) {
            aParam = lstPar;
            return true;
          }
            }
            // Project Point On Curve
            GeomAPI_ProjectPointOnCurve PPOC(thegpPnt, aGeomCurve);
            if (PPOC.NbPoints() == 0) {
          return false;
            }
            aParam = PPOC.LowerDistanceParameter();
      */
      return true;
    }
  }
  // if the MasterRepresentation is unspecified:
  // if a ParameterValue exists, it is preferred

  for (i = 1; i <= nbSel; i++)
  {
    StepGeom_TrimmingSelect theSel = TS->Value(i);
    if (theSel.CaseMember() > 0)
    {
      aParam = Shift + Factor * theSel.ParameterValue();

      return true;
    }
  }
  // if no ParameterValue exists, it is created from the CartesianPointValue

  for (i = 1; i <= nbSel; i++)
  {
    StepGeom_TrimmingSelect theSel = TS->Value(i);
    if (theSel.CaseNumber() > 0)
    {
      aPoint = theSel.CartesianPoint();
      occ::handle<Geom_CartesianPoint> theGeomPnt =
        StepToGeom::MakeCartesianPoint(aPoint, theLocalFactors);
      gp_Pnt thegpPnt = theGeomPnt->Pnt();
      // Project Point On Curve
      ShapeAnalysis_Curve sac;
      gp_Pnt              p;
      sac.Project(aGeomCurve, thegpPnt, Precision::Confusion(), p, aParam);
      /*
            GeomAPI_ProjectPointOnCurve PPOC(thegpPnt, aGeomCurve);
            if (PPOC.NbPoints() == 0) {
          return false;
            }
            aParam = PPOC.LowerDistanceParameter();
      */
      return true;
    }
  }
  return false; // I suppose
}

//=============================================================================
// Creation d' une Trimmed Curve de Geom a partir d' une Trimmed Curve de Step
//=============================================================================

occ::handle<Geom_TrimmedCurve> StepToGeom::MakeTrimmedCurve(
  const occ::handle<StepGeom_TrimmedCurve>& SC,
  const StepData_Factors&                   theLocalFactors)
{
  const occ::handle<StepGeom_Curve> theSTEPCurve = SC->BasisCurve();
  occ::handle<Geom_Curve>           theCurve     = MakeCurve(theSTEPCurve, theLocalFactors);
  if (theCurve.IsNull())
    return occ::handle<Geom_TrimmedCurve>();

  const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& theTrimSel1 = SC->Trim1();
  const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& theTrimSel2 = SC->Trim2();
  const int                                                        nbSel1      = SC->NbTrim1();
  const int                                                        nbSel2      = SC->NbTrim2();

  int MasterRep;
  switch (SC->MasterRepresentation())
  {
    case StepGeom_tpCartesian:
      MasterRep = 1;
      break;
    case StepGeom_tpParameter:
      MasterRep = 2;
      break;
    default:
      MasterRep = 0;
  }

  // gka 18.02.04 analysis for case when MasterRep = .Unspecified
  // and parameters are specified as CARTESIAN_POINT
  bool isPoint = false;
  if (MasterRep == 0 || (MasterRep == 2 && nbSel1 > 1 && nbSel2 > 1))
  {
    int ii;
    for (ii = 1; ii <= nbSel1; ii++)
    {
      if (!(theTrimSel1->Value(ii).CartesianPoint().IsNull()))
      {
        for (ii = 1; ii <= nbSel2; ii++)
        {
          if (!(theTrimSel2->Value(ii).CartesianPoint().IsNull()))
          {
            isPoint = true;
            break;
          }
        }
        break;
      }
    }
  }

  //: o6 abv 18 Feb 99: computation of factor moved
  double fact = 1., shift = 0.;
  if (theSTEPCurve->IsKind(STANDARD_TYPE(StepGeom_Line)))
  {
    const occ::handle<StepGeom_Line> theLine = occ::down_cast<StepGeom_Line>(theSTEPCurve);
    fact = theLine->Dir()->Magnitude() * theLocalFactors.LengthFactor();
  }
  else if (theSTEPCurve->IsKind(STANDARD_TYPE(StepGeom_Circle))
           || theSTEPCurve->IsKind(STANDARD_TYPE(StepGeom_Ellipse)))
  {
    //    if (trim1 > 2.1*M_PI || trim2 > 2.1*M_PI) fact = M_PI / 180.;
    fact = theLocalFactors.PlaneAngleFactor();
    //: p3 abv 23 Feb 99: shift on pi/2 on ellipse with R1 < R2
    const occ::handle<StepGeom_Ellipse> ellipse = occ::down_cast<StepGeom_Ellipse>(theSTEPCurve);
    if (!ellipse.IsNull() && ellipse->SemiAxis1() - ellipse->SemiAxis2() < 0.)
      shift = 0.5 * M_PI;

    // skl 04.02.2002 for OCC133: we can not make TrimmedCurve if
    // there is no X-direction in StepGeom_Axis2Placement3d
    const occ::handle<StepGeom_Conic> conic = occ::down_cast<StepGeom_Conic>(theSTEPCurve);
    // CKY 6-FEB-2004 for Airbus-MedialAxis :
    // this restriction does not apply for trimming by POINTS
    if (!conic.IsNull() && MasterRep != 1)
    {
      const StepGeom_Axis2Placement a2p = conic->Position();
      if (a2p.CaseNum(a2p.Value()) == 2)
      {
        if (!a2p.Axis2Placement3d()->HasRefDirection())
        {
          ////gka 18.02.04 analysis for case when MasterRep = .Unspecified
          // and parameters are specified as CARTESIAN_POINT
          if (isPoint /*&& !MasterRep*/)
            MasterRep = 1;
          else
          {
            if (SC->SenseAgreement())
              return new Geom_TrimmedCurve(theCurve, 0., 2. * M_PI, true);
            else
              return new Geom_TrimmedCurve(theCurve, 2. * M_PI, 0., false);
          }
        }
      }
    }
  }

  double                               trim1 = 0.;
  double                               trim2 = 0.;
  occ::handle<StepGeom_CartesianPoint> TrimCP1, TrimCP2;
  const bool                           FoundParam1 =
    ExtractParameter(theCurve, theTrimSel1, nbSel1, MasterRep, fact, shift, trim1, theLocalFactors);
  const bool FoundParam2 =
    ExtractParameter(theCurve, theTrimSel2, nbSel2, MasterRep, fact, shift, trim2, theLocalFactors);

  if (FoundParam1 && FoundParam2)
  {
    const double cf = theCurve->FirstParameter();
    const double cl = theCurve->LastParameter();
    //: abv 09.04.99: S4136: bm2_ug_t4-B.stp #70610: protect against OutOfRange
    if (!theCurve->IsPeriodic())
    {
      if (trim1 < cf)
        trim1 = cf;
      else if (trim1 > cl)
        trim1 = cl;
      if (trim2 < cf)
        trim2 = cf;
      else if (trim2 > cl)
        trim2 = cl;
    }
    if (std::abs(trim1 - trim2) < Precision::PConfusion())
    {
      if (theCurve->IsPeriodic())
      {
        ElCLib::AdjustPeriodic(cf, cl, Precision::PConfusion(), trim1, trim2);
      }
      else if (theCurve->IsClosed())
      {
        if (std::abs(trim1 - cf) < Precision::PConfusion())
        {
          trim2 += cl;
        }
        else
        {
          trim1 -= cl;
        }
      }
      else
      {
        return nullptr;
      }
    }
    //  CKY 16-DEC-1997 : USA60035 le texte de Part42 parle de degres
    //    mais des systemes ecrivent en radians. Exploiter UnitsMethods
    //: o6    trim1 = trim1 * fact;
    //: o6    trim2 = trim2 * fact;
    if (SC->SenseAgreement())
      return new Geom_TrimmedCurve(theCurve, trim1, trim2, true);
    else //: abv 29.09.00 PRO20362: reverse parameters in case of reversed curve
      return new Geom_TrimmedCurve(theCurve, trim2, trim1, false);
  }
  return nullptr;
}

//=============================================================================
// Creation d'une Trimmed Curve de Geom2d a partir d' une Trimmed Curve de Step
//=============================================================================
// Shall be completed to treat trimming with points

occ::handle<Geom2d_BSplineCurve> StepToGeom::MakeTrimmedCurve2d(
  const occ::handle<StepGeom_TrimmedCurve>& SC,
  const StepData_Factors&                   theLocalFactors)
{
  const occ::handle<StepGeom_Curve> BasisCurve   = SC->BasisCurve();
  occ::handle<Geom2d_Curve>         theGeomBasis = MakeCurve2d(BasisCurve, theLocalFactors);
  if (theGeomBasis.IsNull())
    return occ::handle<Geom2d_BSplineCurve>();

  if (theGeomBasis->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    return occ::down_cast<Geom2d_BSplineCurve>(theGeomBasis);
  }

  const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& theTrimSel1 = SC->Trim1();
  const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& theTrimSel2 = SC->Trim2();
  const int                                                        nbSel1      = SC->NbTrim1();
  const int                                                        nbSel2      = SC->NbTrim2();
  if ((nbSel1 == 1) && (nbSel2 == 1) && (theTrimSel1->Value(1).CaseMember() > 0)
      && (theTrimSel2->Value(1).CaseMember() > 0))
  {
    const double u1   = theTrimSel1->Value(1).ParameterValue();
    const double u2   = theTrimSel2->Value(1).ParameterValue();
    double       fact = 1., shift = 0.;

    if (BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Line)))
    {
      const occ::handle<StepGeom_Line> theLine = occ::down_cast<StepGeom_Line>(BasisCurve);
      fact                                     = theLine->Dir()->Magnitude();
    }
    else if (BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Circle))
             || BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Ellipse)))
    {
      //      if (u1 > 2.1*M_PI || u2 > 2.1*M_PI) fact = M_PI / 180.;
      fact = theLocalFactors.PlaneAngleFactor();
      //: p3 abv 23 Feb 99: shift on pi/2 on ellipse with R1 < R2
      const occ::handle<StepGeom_Ellipse> ellipse = occ::down_cast<StepGeom_Ellipse>(BasisCurve);
      if (!ellipse.IsNull() && ellipse->SemiAxis1() - ellipse->SemiAxis2() < 0.)
        shift = 0.5 * M_PI;
    }
    else if (BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Parabola))
             || BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Hyperbola)))
    {
      // LATER !!!
    }
    //    CKY 16-DEC-1997 : USA60035 le texte de Part42 parle de degres
    //      mais des systemes ecrivent en radians. Exploiter UnitsMethods

    const double newU1 = shift + u1 * fact;
    const double newU2 = shift + u2 * fact;

    const occ::handle<Geom2d_TrimmedCurve> theTrimmed =
      new Geom2d_TrimmedCurve(theGeomBasis, newU1, newU2, SC->SenseAgreement());
    return Geom2dConvert::CurveToBSplineCurve(theTrimmed);
  }
  return nullptr;
}

//=============================================================================
// Creation d' un VectorWithMagnitude de Geom a partir d' un Vector de Step
//=============================================================================

occ::handle<Geom_VectorWithMagnitude> StepToGeom::MakeVectorWithMagnitude(
  const occ::handle<StepGeom_Vector>& SV,
  const StepData_Factors&             theLocalFactors)
{
  // sln 22.10.2001. CTS23496: Vector is not created if direction have not been successfully created
  occ::handle<Geom_Direction> D = MakeDirection(SV->Orientation());
  if (!D.IsNull())
  {
    const gp_Vec V(D->Dir().XYZ() * SV->Magnitude() * theLocalFactors.LengthFactor());
    return new Geom_VectorWithMagnitude(V);
  }
  return nullptr;
}

//=============================================================================
// Creation d' un VectorWithMagnitude de Geom2d a partir d' un Vector de Step
//=============================================================================

occ::handle<Geom2d_VectorWithMagnitude> StepToGeom::MakeVectorWithMagnitude2d(
  const occ::handle<StepGeom_Vector>& SV)
{
  // sln 23.10.2001. CTS23496: Vector is not created if direction have not been successfully created
  // (MakeVectorWithMagnitude2d(...) function)
  occ::handle<Geom2d_Direction> D = MakeDirection2d(SV->Orientation());
  if (!D.IsNull())
  {
    const gp_Vec2d V(D->Dir2d().XY() * SV->Magnitude());
    return new Geom2d_VectorWithMagnitude(V);
  }
  return nullptr;
}

//=============================================================================
// Creation of a YptRotation from a Kinematic SpatialRotation for Step
//=============================================================================

occ::handle<NCollection_HArray1<double>> StepToGeom::MakeYprRotation(
  const StepKinematics_SpatialRotation&                  SR,
  const occ::handle<StepRepr_GlobalUnitAssignedContext>& theCntxt)
{
  // If rotation is already a ypr_rotation, return it immediately
  occ::handle<NCollection_HArray1<double>> anYPRRotation;
  if (!SR.YprRotation().IsNull() && SR.YprRotation()->Length() == 3)
  {
    return SR.YprRotation();
  }

  if (SR.RotationAboutDirection().IsNull()
      || SR.RotationAboutDirection()->DirectionOfAxis()->NbDirectionRatios() != 3
      || theCntxt.IsNull())
  {
    return nullptr;
  }
  // rotation is a rotation_about_direction
  occ::handle<Geom_Direction> anAxis;
  anAxis =
    new Geom_Direction(SR.RotationAboutDirection()->DirectionOfAxis()->DirectionRatiosValue(1),
                       SR.RotationAboutDirection()->DirectionOfAxis()->DirectionRatiosValue(2),
                       SR.RotationAboutDirection()->DirectionOfAxis()->DirectionRatiosValue(3));
  double anAngle = SR.RotationAboutDirection()->RotationAngle();
  if (std::abs(anAngle) < Precision::Angular())
  {
    // a zero rotation is converted trivially
    anYPRRotation = new NCollection_HArray1<double>(1, 3);
    anYPRRotation->SetValue(1, 0.);
    anYPRRotation->SetValue(2, 0.);
    anYPRRotation->SetValue(3, 0.);
    return anYPRRotation;
  }
  double                                                 dx = anAxis->X();
  double                                                 dy = anAxis->Y();
  double                                                 dz = anAxis->Z();
  NCollection_Sequence<occ::handle<StepBasic_NamedUnit>> aPaUnits;
  for (int anInd = 1; anInd <= theCntxt->Units()->Length(); ++anInd)
  {
    if (theCntxt->UnitsValue(anInd)->IsKind(
          STANDARD_TYPE(StepBasic_ConversionBasedUnitAndPlaneAngleUnit))
        || theCntxt->UnitsValue(anInd)->IsKind(STANDARD_TYPE(StepBasic_SiUnitAndPlaneAngleUnit)))
    {
      aPaUnits.Append(theCntxt->UnitsValue(anInd));
    }
  }
  if (aPaUnits.Length() != 1)
  {
    return anYPRRotation;
  }
  occ::handle<StepBasic_NamedUnit> aPau = aPaUnits.Value(1);
  while (!aPau.IsNull()
         && aPau->IsKind((STANDARD_TYPE(StepBasic_ConversionBasedUnitAndPlaneAngleUnit))))
  {
    occ::handle<StepBasic_ConversionBasedUnitAndPlaneAngleUnit> aConverUnit =
      occ::down_cast<StepBasic_ConversionBasedUnitAndPlaneAngleUnit>(aPau);
    occ::handle<StepBasic_MeasureWithUnit> aConversionFactor;
    occ::handle<Standard_Transient>        aTransientFactor = aConverUnit->ConversionFactor();
    if (aTransientFactor->IsKind(STANDARD_TYPE(StepBasic_MeasureWithUnit)))
    {
      aConversionFactor = occ::down_cast<StepBasic_MeasureWithUnit>(aTransientFactor);
    }
    else if (aTransientFactor->IsKind(STANDARD_TYPE(StepRepr_ReprItemAndMeasureWithUnit)))
    {
      occ::handle<StepRepr_ReprItemAndMeasureWithUnit> aReprMeasureItem =
        occ::down_cast<StepRepr_ReprItemAndMeasureWithUnit>(aTransientFactor);
      aConversionFactor = aReprMeasureItem->GetMeasureWithUnit();
    }
    anAngle = anAngle * aConversionFactor->ValueComponent();
    aPau    = aConversionFactor->UnitComponent().NamedUnit();
  }
  if (aPau.IsNull())
  {
    return anYPRRotation;
  }
  occ::handle<StepBasic_SiUnitAndPlaneAngleUnit> aSiUnit =
    occ::down_cast<StepBasic_SiUnitAndPlaneAngleUnit>(aPau);
  if (aSiUnit.IsNull() || aSiUnit->Name() != StepBasic_sunRadian)
  {
    return anYPRRotation;
  }
  anAngle =
    (!aSiUnit->HasPrefix() ? 1. : STEPConstruct_UnitContext::ConvertSiPrefix(aSiUnit->Prefix()))
    * anAngle;
  double anUcf = SR.RotationAboutDirection()->RotationAngle() / anAngle;
  double aSA   = std::sin(anAngle);
  double aCA   = std::cos(anAngle);
  double aYaw = 0, aPitch = 0, aRoll = 0;

  // axis parallel either to x-axis or to z-axis?
  if (std::abs(dy) < Precision::Confusion() && std::abs(dx * dz) < Precision::SquareConfusion())
  {
    while (anAngle <= -M_PI)
    {
      anAngle = anAngle + 2 * M_PI;
    }
    while (anAngle > M_PI)
    {
      anAngle = anAngle - 2 * M_PI;
    }

    aYaw = anUcf * anAngle;
    if (std::abs(anAngle - M_PI) >= Precision::Angular())
    {
      aRoll = -aYaw;
    }
    else
    {
      aRoll = aYaw;
    }
    anYPRRotation = new NCollection_HArray1<double>(1, 3);
    anYPRRotation->SetValue(1, 0.);
    anYPRRotation->SetValue(2, 0.);
    anYPRRotation->SetValue(3, 0.);
    if (std::abs(dx) >= Precision::Confusion())
    {
      if (dx > 0.)
        anYPRRotation->SetValue(3, aYaw);
      else
        anYPRRotation->SetValue(3, aRoll);
    }
    else
    {
      if (dz > 0.)
        anYPRRotation->SetValue(1, aYaw);
      else
        anYPRRotation->SetValue(1, aRoll);
    }
    return anYPRRotation;
  }

  // axis parallel to y-axis - use y-axis as pitch axis
  if (std::abs(dy) >= Precision::Confusion() && std::abs(dx) < Precision::Confusion()
      && std::abs(dz) < Precision::Confusion())
  {
    if (aCA >= 0.)
    {
      aYaw  = 0.0;
      aRoll = 0.0;
    }
    else
    {
      aYaw  = anUcf * M_PI;
      aRoll = aYaw;
    }
    aPitch = anUcf * std::atan2(aSA, std::abs(aCA));
    if (dy < 0.)
    {
      aPitch = -aPitch;
    }
    anYPRRotation = new NCollection_HArray1<double>(1, 3);
    anYPRRotation->SetValue(1, aYaw);
    anYPRRotation->SetValue(2, aPitch);
    anYPRRotation->SetValue(3, aRoll);
    return anYPRRotation;
  }
  // axis not parallel to any axis of coordinate system
  // compute rotation matrix
  double aCm1 = 1 - aCA;

  double aRotMat[3][3] = {
    {dx * dx * aCm1 + aCA, dx * dy * aCm1 - dz * aSA, dx * dz * aCm1 + dy * aSA},
    {dx * dy * aCm1 + dz * aSA, dy * dy * aCm1 + aCA, dy * dz * aCm1 - dx * aSA},
    {dx * dz * aCm1 - dy * aSA, dy * dz * aCm1 + dx * aSA, dz * dz * aCm1 + aCA}};

  // aRotMat[1][3] equals SIN(pitch_angle)
  if (std::abs(std::abs(aRotMat[0][2] - 1.)) < Precision::Confusion())
  {
    // |aPitch| = PI/2
    if (std::abs(aRotMat[0][2] - 1.) < Precision::Confusion())
      aPitch = M_PI_2;
    else
      aPitch = -M_PI_2;
    // In this case, only the sum or difference of roll and yaw angles
    // is relevant and can be evaluated from the matrix.
    // According to IP `rectangular pitch angle' for ypr_rotation,
    // the roll angle is set to zero.
    aRoll = 0.;
    aYaw  = std::atan2(aRotMat[1][0], aRotMat[1][1]);
    // result of ATAN is in the range[-PI / 2, PI / 2].
    // Here all four quadrants are needed.

    if (aRotMat[1][1] < 0.)
    {
      if (aYaw <= 0.)
        aYaw = aYaw + M_PI;
      else
        aYaw = aYaw - M_PI;
    }
  }
  else
  {
    // COS (pitch_angle) not equal to zero
    aYaw = std::atan2(-aRotMat[0][1], aRotMat[0][0]);

    if (aRotMat[0][0] < 0.)
    {
      if (aYaw < 0. || std::abs(aYaw) < Precision::Angular())
        aYaw = aYaw + M_PI;
      else
        aYaw = aYaw - M_PI;
    }
    double aSY = std::sin(aYaw);
    double aCY = std::cos(aYaw);
    double aSR = std::sin(aRoll);
    double aCR = std::cos(aRoll);

    if (std::abs(aSY) > std::abs(aCY) && std::abs(aSY) > std::abs(aSR)
        && std::abs(aSY) > std::abs(aCR))
    {
      aCm1 = -aRotMat[0][1] / aSY;
    }
    else
    {
      if (std::abs(aCY) > std::abs(aSR) && std::abs(aCY) > std::abs(aCR))
        aCm1 = aRotMat[0][0] / aCY;
      else if (std::abs(aSR) > std::abs(aCR))
        aCm1 = -aRotMat[1][2] / aSR;
      else
        aCm1 = aRotMat[2][2] / aCR;
    }
    aPitch = std::atan2(aRotMat[0][2], aCm1);
  }
  aYaw          = aYaw * anUcf;
  aPitch        = aPitch * anUcf;
  aRoll         = aRoll * anUcf;
  anYPRRotation = new NCollection_HArray1<double>(1, 3);
  anYPRRotation->SetValue(1, aYaw);
  anYPRRotation->SetValue(2, aPitch);
  anYPRRotation->SetValue(3, aRoll);

  return anYPRRotation;
}
