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

//! Defines status codes returned by `gce` construction algorithms.
//! - `gce_Done`: construction completed successfully.
//! - `gce_ConfusedPoints`: two points are coincident.
//! - `gce_NegativeRadius`: a radius value is negative.
//! - `gce_ColinearPoints`: three points are collinear.
//! - `gce_IntersectionError`: intersection cannot be computed.
//! - `gce_NullAxis`: axis is undefined.
//! - `gce_NullAngle`: angle value is invalid (usually null).
//! - `gce_NullRadius`: radius is null.
//! - `gce_InvertAxis`: axis value is invalid.
//! - `gce_BadAngle`: angle value is invalid.
//! - `gce_InvertRadius`: radius values are inconsistent.
//! - `gce_NullFocusLength`: focal distance is null.
//! - `gce_NullVector`: vector is null.
//! - `gce_BadEquation`: coefficients of an equation are invalid.
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
