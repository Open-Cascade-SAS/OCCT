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

//! This class provides static methods to convert STEP geometric entities to OCCT.
//! The methods returning handles will return null handle in case of error.
//! The methods returning boolean will return True if succeeded and False if error.

class StepToGeom
{
public:

  Standard_EXPORT static Handle(Geom_Axis1Placement) MakeAxis1Placement (const Handle(StepGeom_Axis1Placement)& SA);
  Standard_EXPORT static Handle(Geom_Axis2Placement) MakeAxis2Placement (const Handle(StepGeom_Axis2Placement3d)& SA);
  Standard_EXPORT static Handle(Geom2d_AxisPlacement) MakeAxisPlacement (const Handle(StepGeom_Axis2Placement2d)& SA);
  Standard_EXPORT static Handle(Geom_BoundedCurve) MakeBoundedCurve (const Handle(StepGeom_BoundedCurve)& SC);
  Standard_EXPORT static Handle(Geom2d_BoundedCurve) MakeBoundedCurve2d (const Handle(StepGeom_BoundedCurve)& SC);
  Standard_EXPORT static Handle(Geom_BoundedSurface) MakeBoundedSurface (const Handle(StepGeom_BoundedSurface)& SS);
  Standard_EXPORT static Handle(Geom_BSplineCurve) MakeBSplineCurve (const Handle(StepGeom_BSplineCurve)& SC);
  Standard_EXPORT static Handle(Geom2d_BSplineCurve) MakeBSplineCurve2d (const Handle(StepGeom_BSplineCurve)& SC);
  Standard_EXPORT static Handle(Geom_BSplineSurface) MakeBSplineSurface (const Handle(StepGeom_BSplineSurface)& SS);
  Standard_EXPORT static Handle(Geom_CartesianPoint) MakeCartesianPoint (const Handle(StepGeom_CartesianPoint)& SP);
  Standard_EXPORT static Handle(Geom2d_CartesianPoint) MakeCartesianPoint2d (const Handle(StepGeom_CartesianPoint)& SP);
  Standard_EXPORT static Handle(Geom_Circle) MakeCircle (const Handle(StepGeom_Circle)& SC);
  Standard_EXPORT static Handle(Geom2d_Circle) MakeCircle2d (const Handle(StepGeom_Circle)& SC);
  Standard_EXPORT static Handle(Geom_Conic) MakeConic (const Handle(StepGeom_Conic)& SC);
  Standard_EXPORT static Handle(Geom2d_Conic) MakeConic2d (const Handle(StepGeom_Conic)& SC);
  Standard_EXPORT static Handle(Geom_ConicalSurface) MakeConicalSurface (const Handle(StepGeom_ConicalSurface)& SS);
  Standard_EXPORT static Handle(Geom_Curve) MakeCurve (const Handle(StepGeom_Curve)& SC);
  Standard_EXPORT static Handle(Geom2d_Curve) MakeCurve2d (const Handle(StepGeom_Curve)& SC);
  Standard_EXPORT static Handle(Geom_CylindricalSurface) MakeCylindricalSurface (const Handle(StepGeom_CylindricalSurface)& SS);
  Standard_EXPORT static Handle(Geom_Direction) MakeDirection (const Handle(StepGeom_Direction)& SD);
  Standard_EXPORT static Handle(Geom2d_Direction) MakeDirection2d (const Handle(StepGeom_Direction)& SD);
  Standard_EXPORT static Handle(Geom_ElementarySurface) MakeElementarySurface (const Handle(StepGeom_ElementarySurface)& SS);
  Standard_EXPORT static Handle(Geom_Ellipse) MakeEllipse (const Handle(StepGeom_Ellipse)& SC);
  Standard_EXPORT static Handle(Geom2d_Ellipse) MakeEllipse2d (const Handle(StepGeom_Ellipse)& SC);
  Standard_EXPORT static Handle(Geom_Hyperbola) MakeHyperbola (const Handle(StepGeom_Hyperbola)& SC);
  Standard_EXPORT static Handle(Geom2d_Hyperbola) MakeHyperbola2d (const Handle(StepGeom_Hyperbola)& SC);
  Standard_EXPORT static Handle(Geom_Line) MakeLine (const Handle(StepGeom_Line)& SC);
  Standard_EXPORT static Handle(Geom2d_Line) MakeLine2d (const Handle(StepGeom_Line)& SC);
  Standard_EXPORT static Handle(Geom_Parabola) MakeParabola (const Handle(StepGeom_Parabola)& SC);
  Standard_EXPORT static Handle(Geom2d_Parabola) MakeParabola2d (const Handle(StepGeom_Parabola)& SC);
  Standard_EXPORT static Handle(Geom_Plane) MakePlane (const Handle(StepGeom_Plane)& SP);
  Standard_EXPORT static Handle(Geom_BSplineCurve) MakePolyline (const Handle(StepGeom_Polyline)& SPL);
  Standard_EXPORT static Handle(Geom2d_BSplineCurve) MakePolyline2d (const Handle(StepGeom_Polyline)& SPL);
  Standard_EXPORT static Handle(Geom_RectangularTrimmedSurface) MakeRectangularTrimmedSurface (const Handle(StepGeom_RectangularTrimmedSurface)& SS);
  Standard_EXPORT static Handle(Geom_SphericalSurface) MakeSphericalSurface (const Handle(StepGeom_SphericalSurface)& SS);
  Standard_EXPORT static Handle(Geom_Surface) MakeSurface (const Handle(StepGeom_Surface)& SS);
  Standard_EXPORT static Handle(Geom_SurfaceOfLinearExtrusion) MakeSurfaceOfLinearExtrusion (const Handle(StepGeom_SurfaceOfLinearExtrusion)& SS);
  Standard_EXPORT static Handle(Geom_SurfaceOfRevolution) MakeSurfaceOfRevolution (const Handle(StepGeom_SurfaceOfRevolution)& SS);
  Standard_EXPORT static Handle(Geom_SweptSurface) MakeSweptSurface (const Handle(StepGeom_SweptSurface)& SS);
  Standard_EXPORT static Handle(Geom_ToroidalSurface) MakeToroidalSurface (const Handle(StepGeom_ToroidalSurface)& SS);
  Standard_EXPORT static Standard_Boolean MakeTransformation2d (const Handle(StepGeom_CartesianTransformationOperator2d)& SCTO, gp_Trsf2d& CT);
  Standard_EXPORT static Standard_Boolean MakeTransformation3d (const Handle(StepGeom_CartesianTransformationOperator3d)& SCTO, gp_Trsf& CT);
  Standard_EXPORT static Handle(Geom_TrimmedCurve) MakeTrimmedCurve (const Handle(StepGeom_TrimmedCurve)& SC);
  Standard_EXPORT static Handle(Geom2d_BSplineCurve) MakeTrimmedCurve2d (const Handle(StepGeom_TrimmedCurve)& SC);
  Standard_EXPORT static Handle(Geom_VectorWithMagnitude) MakeVectorWithMagnitude (const Handle(StepGeom_Vector)& SV);
  Standard_EXPORT static Handle(Geom2d_VectorWithMagnitude) MakeVectorWithMagnitude2d (const Handle(StepGeom_Vector)& SV);
};

#endif // _StepToGeom_HeaderFile
