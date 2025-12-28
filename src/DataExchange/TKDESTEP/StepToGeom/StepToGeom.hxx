// Created on: 1993-06-11
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

#ifndef _StepToGeom_HeaderFile
#define _StepToGeom_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <StepData_Factors.hxx>

class Geom_Axis1Placement;
class Geom_Axis2Placement;
class Geom_BoundedCurve;
class Geom_BoundedSurface;
class Geom_BSplineCurve;
class Geom_BSplineSurface;
class Geom_CartesianPoint;
class Geom_Circle;
class Geom_Conic;
class Geom_ConicalSurface;
class Geom_Curve;
class Geom_CylindricalSurface;
class Geom_Direction;
class Geom_ElementarySurface;
class Geom_Ellipse;
class Geom_Hyperbola;
class Geom_Line;
class Geom_Parabola;
class Geom_Plane;
class Geom_RectangularTrimmedSurface;
class Geom_SphericalSurface;
class Geom_Surface;
class Geom_SurfaceOfLinearExtrusion;
class Geom_SurfaceOfRevolution;
class Geom_SweptSurface;
class Geom_ToroidalSurface;
class Geom_TrimmedCurve;
class Geom_VectorWithMagnitude;

class Geom2d_AxisPlacement;
class Geom2d_BoundedCurve;
class Geom2d_BSplineCurve;
class Geom2d_CartesianPoint;
class Geom2d_Circle;
class Geom2d_Conic;
class Geom2d_Curve;
class Geom2d_Direction;
class Geom2d_Ellipse;
class Geom2d_Hyperbola;
class Geom2d_Line;
class Geom2d_Parabola;
class Geom2d_VectorWithMagnitude;

class gp_Trsf;
class gp_Trsf2d;

class StepGeom_Axis1Placement;
class StepGeom_Axis2Placement2d;
class StepGeom_Axis2Placement3d;
class StepGeom_BoundedCurve;
class StepGeom_BoundedSurface;
class StepGeom_BSplineCurve;
class StepGeom_CartesianPoint;
class StepGeom_Direction;

class StepGeom_BSplineSurface;
class StepGeom_Circle;
class StepGeom_Conic;
class StepGeom_ConicalSurface;
class StepGeom_Curve;
class StepGeom_CylindricalSurface;
class StepGeom_ElementarySurface;
class StepGeom_Ellipse;
class StepGeom_Hyperbola;
class StepGeom_Line;
class StepGeom_Parabola;
class StepGeom_Plane;
class StepGeom_Polyline;
class StepGeom_RectangularTrimmedSurface;
class StepGeom_SphericalSurface;
class StepGeom_Surface;
class StepGeom_SurfaceOfLinearExtrusion;
class StepGeom_SurfaceOfRevolution;
class StepGeom_SweptSurface;
class StepGeom_ToroidalSurface;
class StepGeom_CartesianTransformationOperator2d;
class StepGeom_CartesianTransformationOperator3d;
class StepGeom_TrimmedCurve;
class StepGeom_Vector;
class StepGeom_SuParameters;
class StepKinematics_SpatialRotation;
class StepRepr_GlobalUnitAssignedContext;
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//! This class provides static methods to convert STEP geometric entities to OCCT.
//! The methods returning handles will return null handle in case of error.
//! The methods returning boolean will return True if succeeded and False if error.

class StepToGeom
{
public:
  Standard_EXPORT static occ::handle<Geom_Axis1Placement> MakeAxis1Placement(
    const occ::handle<StepGeom_Axis1Placement>& SA,
    const StepData_Factors&                theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_Axis2Placement> MakeAxis2Placement(
    const occ::handle<StepGeom_Axis2Placement3d>& SA,
    const StepData_Factors&                  theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_Axis2Placement> MakeAxis2Placement(
    const occ::handle<StepGeom_SuParameters>& SP);
  Standard_EXPORT static occ::handle<Geom2d_AxisPlacement> MakeAxisPlacement(
    const occ::handle<StepGeom_Axis2Placement2d>& SA,
    const StepData_Factors&                  theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_BoundedCurve> MakeBoundedCurve(
    const occ::handle<StepGeom_BoundedCurve>& SC,
    const StepData_Factors&              theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_BoundedCurve> MakeBoundedCurve2d(
    const occ::handle<StepGeom_BoundedCurve>& SC,
    const StepData_Factors&              theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_BoundedSurface> MakeBoundedSurface(
    const occ::handle<StepGeom_BoundedSurface>& SS,
    const StepData_Factors&                theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_BSplineCurve> MakeBSplineCurve(
    const occ::handle<StepGeom_BSplineCurve>& SC,
    const StepData_Factors&              theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_BSplineCurve> MakeBSplineCurve2d(
    const occ::handle<StepGeom_BSplineCurve>& SC,
    const StepData_Factors&              theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_BSplineSurface> MakeBSplineSurface(
    const occ::handle<StepGeom_BSplineSurface>& SS,
    const StepData_Factors&                theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_CartesianPoint> MakeCartesianPoint(
    const occ::handle<StepGeom_CartesianPoint>& SP,
    const StepData_Factors&                theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_CartesianPoint> MakeCartesianPoint2d(
    const occ::handle<StepGeom_CartesianPoint>& SP,
    const StepData_Factors&                theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_Circle> MakeCircle(
    const occ::handle<StepGeom_Circle>& SC,
    const StepData_Factors&        theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_Circle> MakeCircle2d(
    const occ::handle<StepGeom_Circle>& SC,
    const StepData_Factors&        theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_Conic> MakeConic(
    const occ::handle<StepGeom_Conic>& SC,
    const StepData_Factors&       theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_Conic> MakeConic2d(
    const occ::handle<StepGeom_Conic>& SC,
    const StepData_Factors&       theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_ConicalSurface> MakeConicalSurface(
    const occ::handle<StepGeom_ConicalSurface>& SS,
    const StepData_Factors&                theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_Curve> MakeCurve(
    const occ::handle<StepGeom_Curve>& SC,
    const StepData_Factors&       theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_Curve> MakeCurve2d(
    const occ::handle<StepGeom_Curve>& SC,
    const StepData_Factors&       theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_CylindricalSurface> MakeCylindricalSurface(
    const occ::handle<StepGeom_CylindricalSurface>& SS,
    const StepData_Factors&                    theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_Direction> MakeDirection(const occ::handle<StepGeom_Direction>& SD);
  Standard_EXPORT static occ::handle<Geom2d_Direction> MakeDirection2d(
    const occ::handle<StepGeom_Direction>& SD);
  Standard_EXPORT static occ::handle<Geom_ElementarySurface> MakeElementarySurface(
    const occ::handle<StepGeom_ElementarySurface>& SS,
    const StepData_Factors&                   theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_Ellipse> MakeEllipse(
    const occ::handle<StepGeom_Ellipse>& SC,
    const StepData_Factors&         theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_Ellipse> MakeEllipse2d(
    const occ::handle<StepGeom_Ellipse>& SC,
    const StepData_Factors&         theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_Hyperbola> MakeHyperbola(
    const occ::handle<StepGeom_Hyperbola>& SC,
    const StepData_Factors&           theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_Hyperbola> MakeHyperbola2d(
    const occ::handle<StepGeom_Hyperbola>& SC,
    const StepData_Factors&           theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_Line> MakeLine(
    const occ::handle<StepGeom_Line>& SC,
    const StepData_Factors&      theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_Line> MakeLine2d(
    const occ::handle<StepGeom_Line>& SC,
    const StepData_Factors&      theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_Parabola> MakeParabola(
    const occ::handle<StepGeom_Parabola>& SC,
    const StepData_Factors&          theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_Parabola> MakeParabola2d(
    const occ::handle<StepGeom_Parabola>& SC,
    const StepData_Factors&          theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_Plane> MakePlane(
    const occ::handle<StepGeom_Plane>& SP,
    const StepData_Factors&       theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_BSplineCurve> MakePolyline(
    const occ::handle<StepGeom_Polyline>& SPL,
    const StepData_Factors&          theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_BSplineCurve> MakePolyline2d(
    const occ::handle<StepGeom_Polyline>& SPL,
    const StepData_Factors&          theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_RectangularTrimmedSurface> MakeRectangularTrimmedSurface(
    const occ::handle<StepGeom_RectangularTrimmedSurface>& SS,
    const StepData_Factors&                           theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_SphericalSurface> MakeSphericalSurface(
    const occ::handle<StepGeom_SphericalSurface>& SS,
    const StepData_Factors&                  theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_Surface> MakeSurface(
    const occ::handle<StepGeom_Surface>& SS,
    const StepData_Factors&         theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_SurfaceOfLinearExtrusion> MakeSurfaceOfLinearExtrusion(
    const occ::handle<StepGeom_SurfaceOfLinearExtrusion>& SS,
    const StepData_Factors&                          theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_SurfaceOfRevolution> MakeSurfaceOfRevolution(
    const occ::handle<StepGeom_SurfaceOfRevolution>& SS,
    const StepData_Factors&                     theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_SweptSurface> MakeSweptSurface(
    const occ::handle<StepGeom_SweptSurface>& SS,
    const StepData_Factors&              theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_ToroidalSurface> MakeToroidalSurface(
    const occ::handle<StepGeom_ToroidalSurface>& SS,
    const StepData_Factors&                 theLocalFactors = StepData_Factors());
  Standard_EXPORT static bool MakeTransformation2d(
    const occ::handle<StepGeom_CartesianTransformationOperator2d>& SCTO,
    gp_Trsf2d&                                                CT,
    const StepData_Factors&                                   theLocalFactors = StepData_Factors());
  Standard_EXPORT static bool MakeTransformation3d(
    const occ::handle<StepGeom_CartesianTransformationOperator3d>& SCTO,
    gp_Trsf&                                                  CT,
    const StepData_Factors&                                   theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_TrimmedCurve> MakeTrimmedCurve(
    const occ::handle<StepGeom_TrimmedCurve>& SC,
    const StepData_Factors&              theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_BSplineCurve> MakeTrimmedCurve2d(
    const occ::handle<StepGeom_TrimmedCurve>& SC,
    const StepData_Factors&              theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom_VectorWithMagnitude> MakeVectorWithMagnitude(
    const occ::handle<StepGeom_Vector>& SV,
    const StepData_Factors&        theLocalFactors = StepData_Factors());
  Standard_EXPORT static occ::handle<Geom2d_VectorWithMagnitude> MakeVectorWithMagnitude2d(
    const occ::handle<StepGeom_Vector>& SV);
  Standard_EXPORT static occ::handle<NCollection_HArray1<double>> MakeYprRotation(
    const StepKinematics_SpatialRotation&             SR,
    const occ::handle<StepRepr_GlobalUnitAssignedContext>& theCntxt);
};

#endif // _StepToGeom_HeaderFile
