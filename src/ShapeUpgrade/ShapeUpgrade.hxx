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


//! This package provides tools for splitting and converting shapes by some criteria.
//! It provides modifications of the kind when one topological
//! object can be converted or split in to several ones.
//! In particular this package contains high level API classes which perform:
//! converting geometry of shapes up to given continuity,
//! splitting revolutions by U to segments less than given value,
//! converting to beziers, splitting closed faces.
class ShapeUpgrade 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Unifies same domain faces and edges of specified shape
  Standard_EXPORT static Standard_Boolean C0BSplineToSequenceOfC1BSplineCurve (const Handle(Geom_BSplineCurve)& BS,
                                                                               Handle(TColGeom_HSequenceOfBoundedCurve)& seqBS);

  //! Converts C0 B-Spline curve into sequence of C1 B-Spline curves.
  //! This method splits B-Spline at the knots with multiplicities equal to degree,
  //! i.e. unlike method GeomConvert::C0BSplineToArrayOfC1BSplineCurve
  //! this one does not use any tolerance and therefore does not change the geometry of B-Spline.
  //! Returns True if C0 B-Spline was successfully split,
  //! else returns False (if BS is C1 B-Spline).
  Standard_EXPORT static Standard_Boolean C0BSplineToSequenceOfC1BSplineCurve (const Handle(Geom2d_BSplineCurve)& BS,
                                                                               Handle(TColGeom2d_HSequenceOfBoundedCurve)& seqBS);

};

#endif // _ShapeUpgrade_HeaderFile
