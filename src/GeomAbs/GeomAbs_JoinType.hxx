// Created on: 1993-02-22
// Created by: Modelistation
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

#ifndef _GeomAbs_JoinType_HeaderFile
#define _GeomAbs_JoinType_HeaderFile

//! Characterizes the type of a join, built by an algorithm for
//! constructing parallel curves, between two consecutive
//! arcs of a contour parallel to a given contour.
enum GeomAbs_JoinType
{
  GeomAbs_Arc,
  GeomAbs_Tangent,
  GeomAbs_Intersection
};

#endif // _GeomAbs_JoinType_HeaderFile
