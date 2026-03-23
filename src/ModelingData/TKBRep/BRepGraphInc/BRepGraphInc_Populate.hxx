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

#ifndef _BRepGraphInc_Populate_HeaderFile
#define _BRepGraphInc_Populate_HeaderFile

#include <Standard_DefineAlloc.hxx>

class TopoDS_Shape;
struct BRepGraphInc_Storage;

//! @brief Populates a BRepGraphInc_Storage from a TopoDS_Shape.
//!
//! Adapted from BRepGraph_Builder, but writes to incidence-table storage
//! instead of Def/Usage two-layer storage.  Entity structs carry forward
//! child references directly (no separate Usage objects).
//!
//! The population pipeline:
//! 1. Sequential hierarchy traversal (Compound/CompSolid/Solid/Shell)
//! 2. Parallel per-face geometry extraction
//! 3. Sequential registration with TShape deduplication
//! 4. Reverse index construction
class BRepGraphInc_Populate
{
public:
  DEFINE_STANDARD_ALLOC

  //! Build incidence storage from a TopoDS_Shape.
  //! @param[out] theStorage  storage to populate (cleared first)
  //! @param[in]  theShape    root shape
  //! @param[in]  theParallel if true, face-level extraction runs in parallel
  static Standard_EXPORT void Perform(BRepGraphInc_Storage& theStorage,
                                      const TopoDS_Shape&   theShape,
                                      bool                  theParallel);

  //! Extend existing storage with additional shapes (no clear).
  //! Flattens hierarchy to face level only (no Solid/Shell/Compound entities created).
  //! @param[in,out] theStorage  storage to extend
  //! @param[in]     theShape    shape to append
  //! @param[in]     theParallel if true, face-level extraction runs in parallel
  static Standard_EXPORT void Append(BRepGraphInc_Storage& theStorage,
                                     const TopoDS_Shape&   theShape,
                                     bool                  theParallel);

private:
  BRepGraphInc_Populate() = delete;
};

#endif // _BRepGraphInc_Populate_HeaderFile
