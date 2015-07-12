// Created on: 1998-06-03
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeUpgrade_HeaderFile
#define _ShapeUpgrade_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TColGeom_HSequenceOfBoundedCurve.hxx>
#include <TColGeom2d_HSequenceOfBoundedCurve.hxx>
class Geom_BSplineCurve;
class Geom2d_BSplineCurve;
class ShapeUpgrade_Tool;
class ShapeUpgrade_EdgeDivide;
class ShapeUpgrade_ClosedEdgeDivide;
class ShapeUpgrade_WireDivide;
class ShapeUpgrade_FaceDivide;
class ShapeUpgrade_ClosedFaceDivide;
class ShapeUpgrade_FaceDivideArea;
class ShapeUpgrade_ShapeDivide;
class ShapeUpgrade_ShapeDivideArea;
class ShapeUpgrade_ShapeDivideContinuity;
class ShapeUpgrade_ShapeDivideAngle;
class ShapeUpgrade_ShapeConvertToBezier;
class ShapeUpgrade_ShapeDivideClosed;
class ShapeUpgrade_ShapeDivideClosedEdges;
class ShapeUpgrade_SplitCurve;
class ShapeUpgrade_SplitCurve2d;
class ShapeUpgrade_SplitCurve2dContinuity;
class ShapeUpgrade_ConvertCurve2dToBezier;
class ShapeUpgrade_SplitCurve3d;
class ShapeUpgrade_SplitCurve3dContinuity;
class ShapeUpgrade_ConvertCurve3dToBezier;
class ShapeUpgrade_SplitSurface;
class ShapeUpgrade_SplitSurfaceContinuity;
class ShapeUpgrade_SplitSurfaceAngle;
class ShapeUpgrade_ConvertSurfaceToBezierBasis;
class ShapeUpgrade_SplitSurfaceArea;
class ShapeUpgrade_ShellSewing;
class ShapeUpgrade_FixSmallCurves;
class ShapeUpgrade_FixSmallBezierCurves;
class ShapeUpgrade_RemoveLocations;
class ShapeUpgrade_RemoveInternalWires;
class ShapeUpgrade_UnifySameDomain;


//! This package provides tools
//! for splitting and converting shapes by some criteria. It
//! provides modifications of the kind when one topological
//! object can be converted or splitted to several ones. In
//! particular this package contains high level API classes which perform:
//! converting geometry of shapes up to given continuity,
//! splitting revolutions by U to segments less than given value,
//! converting to beziers,
//! splitting closed faces.
class ShapeUpgrade 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Unifies same domain faces and edges of specified shape
  Standard_EXPORT static Standard_Boolean C0BSplineToSequenceOfC1BSplineCurve (const Handle(Geom_BSplineCurve)& BS, Handle(TColGeom_HSequenceOfBoundedCurve)& seqBS);
  
  //! Converts C0 B-Spline curve into sequence of C1 B-Spline curves.
  //! This method splits B-Spline at the knots with multiplicities
  //! equal to degree, i.e. unlike method
  //! GeomConvert::C0BSplineToArrayOfC1BSplineCurve this one does not
  //! use any tolerance and therefore does not change the geometry of
  //! B-Spline.
  //! Returns True if C0 B-Spline was successfully splitted, else
  //! returns False (if BS is C1 B-Spline).
  Standard_EXPORT static Standard_Boolean C0BSplineToSequenceOfC1BSplineCurve (const Handle(Geom2d_BSplineCurve)& BS, Handle(TColGeom2d_HSequenceOfBoundedCurve)& seqBS);




protected:





private:




friend class ShapeUpgrade_Tool;
friend class ShapeUpgrade_EdgeDivide;
friend class ShapeUpgrade_ClosedEdgeDivide;
friend class ShapeUpgrade_WireDivide;
friend class ShapeUpgrade_FaceDivide;
friend class ShapeUpgrade_ClosedFaceDivide;
friend class ShapeUpgrade_FaceDivideArea;
friend class ShapeUpgrade_ShapeDivide;
friend class ShapeUpgrade_ShapeDivideArea;
friend class ShapeUpgrade_ShapeDivideContinuity;
friend class ShapeUpgrade_ShapeDivideAngle;
friend class ShapeUpgrade_ShapeConvertToBezier;
friend class ShapeUpgrade_ShapeDivideClosed;
friend class ShapeUpgrade_ShapeDivideClosedEdges;
friend class ShapeUpgrade_SplitCurve;
friend class ShapeUpgrade_SplitCurve2d;
friend class ShapeUpgrade_SplitCurve2dContinuity;
friend class ShapeUpgrade_ConvertCurve2dToBezier;
friend class ShapeUpgrade_SplitCurve3d;
friend class ShapeUpgrade_SplitCurve3dContinuity;
friend class ShapeUpgrade_ConvertCurve3dToBezier;
friend class ShapeUpgrade_SplitSurface;
friend class ShapeUpgrade_SplitSurfaceContinuity;
friend class ShapeUpgrade_SplitSurfaceAngle;
friend class ShapeUpgrade_ConvertSurfaceToBezierBasis;
friend class ShapeUpgrade_SplitSurfaceArea;
friend class ShapeUpgrade_ShellSewing;
friend class ShapeUpgrade_FixSmallCurves;
friend class ShapeUpgrade_FixSmallBezierCurves;
friend class ShapeUpgrade_RemoveLocations;
friend class ShapeUpgrade_RemoveInternalWires;
friend class ShapeUpgrade_UnifySameDomain;

};







#endif // _ShapeUpgrade_HeaderFile
