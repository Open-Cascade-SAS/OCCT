// Created on: 1992-04-30
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _gce_ErrorType_HeaderFile
#define _gce_ErrorType_HeaderFile

//! Indicates the outcome of a construction, i.e.
//! whether it is successful or not, as explained below.
//! gce_Done: Construction was successful.
//! gce_ConfusedPoints: Two points are coincident.
//! gce_NegativeRadius: Radius value is negative.
//! gce_ColinearPoints: Three points are collinear.
//! gce_IntersectionError: Intersection cannot be computed.
//! gce_NullAxis: Axis is undefined.
//! gce_NullAngle: Angle value is invalid (usually null).
//! gce_NullRadius: Radius is null.
//! gce_InvertAxis: Axis value is invalid.
//! gce_BadAngle: Angle value is invalid.
//! gce_InvertRadius: Radius value is incorrect
//! (usually with respect to another radius).
//! gce_NullFocusLength: Focal distance is null.
//! gce_NullVector: Vector is null.
//! gce_BadEquation: Coefficients are
//! incorrect (applies to the equation of a geometric object).
enum gce_ErrorType
{
  gce_Done,
  gce_ConfusedPoints,
  gce_NegativeRadius,
  gce_ColinearPoints,
  gce_IntersectionError,
  gce_NullAxis,
  gce_NullAngle,
  gce_NullRadius,
  gce_InvertAxis,
  gce_BadAngle,
  gce_InvertRadius,
  gce_NullFocusLength,
  gce_NullVector,
  gce_BadEquation
};

#endif // _gce_ErrorType_HeaderFile
