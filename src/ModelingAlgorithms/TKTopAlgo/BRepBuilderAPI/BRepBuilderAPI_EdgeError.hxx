// Created on: 1993-07-06
// Created by: Remi LEQUETTE
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

#ifndef _BRepBuilderAPI_EdgeError_HeaderFile
#define _BRepBuilderAPI_EdgeError_HeaderFile

//! Indicates the outcome of the
//! construction of an edge, i.e. whether it has been successful or
//! not, as explained below:
//! -      BRepBuilderAPI_EdgeDone No    error occurred; The edge is
//! correctly built.
//! -      BRepBuilderAPI_PointProjectionFailed No parameters were given but
//! the projection of the 3D points on the curve failed. This
//! happens when the point distance to the curve is greater than
//! the precision value.
//! -      BRepBuilderAPI_ParameterOutOfRange
//! The given parameters are not in the parametric range
//! C->FirstParameter(), C->LastParameter()
//! -      BRepBuilderAPI_DifferentPointsOnClosedCurve
//! The two vertices or points are the extremities of a closed
//! curve but have different locations.
//! -      BRepBuilderAPI_PointWithInfiniteParameter
//! A finite coordinate point was associated with an infinite
//! parameter (see the Precision package for a definition of    infinite values).
//! -      BRepBuilderAPI_DifferentsPointAndParameter
//! The distance between the 3D point and the point evaluated
//! on the curve with the parameter is greater than the precision.
//! -      BRepBuilderAPI_LineThroughIdenticPoints
//! Two identical points were given to define a line (construction
//! of an edge without curve); gp::Resolution is used for the    confusion test.
enum BRepBuilderAPI_EdgeError
{
  BRepBuilderAPI_EdgeDone,
  BRepBuilderAPI_PointProjectionFailed,
  BRepBuilderAPI_ParameterOutOfRange,
  BRepBuilderAPI_DifferentPointsOnClosedCurve,
  BRepBuilderAPI_PointWithInfiniteParameter,
  BRepBuilderAPI_DifferentsPointAndParameter,
  BRepBuilderAPI_LineThroughIdenticPoints
};

#endif // _BRepBuilderAPI_EdgeError_HeaderFile
