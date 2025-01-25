// Created on: 1990-12-17
// Created by: Remi Lequette
// Copyright (c) 1990-1999 Matra Datavision
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

#ifndef _TopoDS_Edge_HeaderFile
#define _TopoDS_Edge_HeaderFile

#include <TopoDS_Shape.hxx>

//! Describes an edge which
//! - references an underlying edge with the potential to
//! be given a location and an orientation
//! - has a location for the underlying edge, giving its
//! placement in the local coordinate system
//! - has an orientation for the underlying edge, in terms
//! of its geometry (as opposed to orientation in
//! relation to other shapes).
class TopoDS_Edge : public TopoDS_Shape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Undefined Edge.
  TopoDS_Edge() {}
};

namespace std
{
template <>
struct hash<TopoDS_Edge>
{
  size_t operator()(const TopoDS_Edge& theShape) const
  {
    return std::hash<TopoDS_Shape>{}(theShape);
  }
};
} // namespace std

#endif // _TopoDS_Edge_HeaderFile
