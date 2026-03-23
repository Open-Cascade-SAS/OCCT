// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _BRepGraph_Builder_HeaderFile
#define _BRepGraph_Builder_HeaderFile

#include <Standard_DefineAlloc.hxx>

class BRepGraph;
class TopoDS_Shape;

//! @brief Static helper that populates a BRepGraph from a TopoDS_Shape.
//!
//! BRepGraph_Builder extracts the build logic out of BRepGraph itself,
//! keeping the graph class focused on queries and mutation.
//! It is declared as a friend of BRepGraph to access private storage.
class BRepGraph_Builder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Build the full graph from a TopoDS_Shape.
  //! @param[in,out] theGraph   graph to populate
  //! @param[in] theShape       root shape
  //! @param[in] theParallel    if true, face-level construction runs in parallel
  static Standard_EXPORT void Perform(BRepGraph&          theGraph,
                                      const TopoDS_Shape& theShape,
                                      bool                theParallel);

private:
  BRepGraph_Builder() = delete;
};

#endif // _BRepGraph_Builder_HeaderFile
