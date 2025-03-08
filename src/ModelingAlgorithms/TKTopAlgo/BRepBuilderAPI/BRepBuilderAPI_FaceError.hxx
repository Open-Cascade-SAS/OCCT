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

#ifndef _BRepBuilderAPI_FaceError_HeaderFile
#define _BRepBuilderAPI_FaceError_HeaderFile

//! Indicates the outcome of the
//! construction of a face, i.e. whether it has been successful or
//! not, as explained below:
//! -      BRepBuilderAPI_FaceDone No error occurred. The face is
//! correctly built.
//! -      BRepBuilderAPI_NoFace No initialization of the
//! algorithm; only an empty constructor was used.
//! -      BRepBuilderAPI_NotPlanar
//! No surface was given and the wire was not planar.
//! -      BRepBuilderAPI_CurveProjectionFailed
//! Not used so far.
//! -      BRepBuilderAPI_ParametersOutOfRange
//! The parameters given to limit the surface are out of its    bounds.
enum BRepBuilderAPI_FaceError
{
  BRepBuilderAPI_FaceDone,
  BRepBuilderAPI_NoFace,
  BRepBuilderAPI_NotPlanar,
  BRepBuilderAPI_CurveProjectionFailed,
  BRepBuilderAPI_ParametersOutOfRange
};

#endif // _BRepBuilderAPI_FaceError_HeaderFile
