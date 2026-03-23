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

#include <NCollection_BaseAllocator.hxx>
#include <Standard_DefineAlloc.hxx>

class TopoDS_Shape;
class BRepGraphInc_Storage;

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

  //! Options controlling which post-passes are executed during population.
  struct Options
  {
    bool ExtractRegularities;    //!< Phase 3b: edge regularities
    bool ExtractVertexPointReps; //!< Phase 3c: vertex point representations

    Options() : ExtractRegularities(true), ExtractVertexPointReps(true) {}
  };

  //! Build incidence storage from a TopoDS_Shape.
  //! @param[out] theStorage  storage to populate (cleared first)
  //! @param[in]  theShape    root shape
  //! @param[in]  theParallel if true, face-level extraction runs in parallel
  //! @param[in]  theOptions  optional post-pass controls
  static Standard_EXPORT void Perform(
    BRepGraphInc_Storage&                    theStorage,
    const TopoDS_Shape&                      theShape,
    bool                                     theParallel,
    const Options&                           theOptions = Options(),
    const Handle(NCollection_BaseAllocator)& theTmpAlloc = Handle(NCollection_BaseAllocator)());

  //! Extend existing storage with additional shapes (no clear).
  //! Flattens hierarchy to face level only (no Solid/Shell/Compound entities created).
  //! Does not run post-passes (regularities, vertex point representations).
  //! @param[in,out] theStorage  storage to extend
  //! @param[in]     theShape    shape to append
  //! @param[in]     theParallel if true, face-level extraction runs in parallel
  //! @param[in]     theTmpAlloc optional allocator for temporary scratch data
  static Standard_EXPORT void Append(
    BRepGraphInc_Storage&                    theStorage,
    const TopoDS_Shape&                      theShape,
    bool                                     theParallel,
    const Handle(NCollection_BaseAllocator)& theTmpAlloc = Handle(NCollection_BaseAllocator)());

private:
  BRepGraphInc_Populate() = delete;
};

#endif // _BRepGraphInc_Populate_HeaderFile
