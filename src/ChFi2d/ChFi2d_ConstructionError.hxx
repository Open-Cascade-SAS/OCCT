// Created on: 1995-06-12
// Created by: Joelle CHAUVET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _ChFi2d_ConstructionError_HeaderFile
#define _ChFi2d_ConstructionError_HeaderFile

//! error     that    can   occur       during   the
//! fillet  construction on planar wire//! the face is not planar//! the face is null//! the two  faces used for the initialisation are
//! uncompatible.//! the  parameters  as  distances   or  angle for
//! chamfer are less or equal to zero.//! the initialization has been succesfull.//! the algorithm could not find a solution.//! the vertex given  to locate the fillet  or the
//! chamfer  is not connected to 2 edges.//! the two edges connected to the vertex are tangent.//! the first edge is degenerated.//! the last edge is degenerated.//! the two edges are degenerated.//! One or the two  edges connected to the  vertex
//! is a fillet or a chamfer
//! One or the two  edges connected to the  vertex
//! is not a line or a circle
enum ChFi2d_ConstructionError
{
ChFi2d_NotPlanar,
ChFi2d_NoFace,
ChFi2d_InitialisationError,
ChFi2d_ParametersError,
ChFi2d_Ready,
ChFi2d_IsDone,
ChFi2d_ComputationError,
ChFi2d_ConnexionError,
ChFi2d_TangencyError,
ChFi2d_FirstEdgeDegenerated,
ChFi2d_LastEdgeDegenerated,
ChFi2d_BothEdgesDegenerated,
ChFi2d_NotAuthorized
};

#endif // _ChFi2d_ConstructionError_HeaderFile
