// Created on: 1990-12-13
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

#ifndef _TopoDS_TVertex_HeaderFile
#define _TopoDS_TVertex_HeaderFile

#include <Standard.hxx>
#include <Standard_OutOfRange.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_TShape.hxx>

// resolve name collisions with X11 headers
#ifdef Convex
  #undef Convex
#endif

//! A Vertex is a topological point in two or three dimensions.
//! TVertex has no children (sub-shapes).
class TopoDS_TVertex : public TopoDS_TShape
{
public:
  //! Returns the number of direct sub-shapes (children).
  //! A vertex has no children, so this always returns 0.
  int NbChildren() const final { return 0; }

  //! Returns the child shape at the given index.
  //! A vertex has no children, so this always throws Standard_OutOfRange.
  //! @param theIndex the 0-based index (unused)
  //! @return never returns, always throws
  const TopoDS_Shape& GetChild(size_t theIndex) const final
  {
    (void)theIndex;
    throw Standard_OutOfRange("TopoDS_TVertex::GetChild - vertex has no children");
  }

  DEFINE_STANDARD_RTTIEXT(TopoDS_TVertex, TopoDS_TShape)

protected:
  //! Construct a vertex.
  TopoDS_TVertex()
      : TopoDS_TShape(TopAbs_VERTEX)
  {
    Closed(true);
    Convex(true);
  }
};

#endif // _TopoDS_TVertex_HeaderFile
