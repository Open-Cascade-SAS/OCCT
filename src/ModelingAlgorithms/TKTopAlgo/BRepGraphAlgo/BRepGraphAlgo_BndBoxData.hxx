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

#ifndef _BRepGraphAlgo_BndBoxData_HeaderFile
#define _BRepGraphAlgo_BndBoxData_HeaderFile

#include <BRepGraphAlgo_BndBoxPrecision.hxx>

#include <Bnd_Box.hxx>

//! Cached bounding box data for a single graph node.
//!
//! Stores the computed box together with metadata about how it was computed,
//! allowing precision-aware cache lookups.
struct BRepGraphAlgo_BndBoxData
{
  Bnd_Box                       Box; //!< The computed bounding box.
  BRepGraphAlgo_BndBoxPrecision Precision =
    BRepGraphAlgo_BndBoxPrecision_VertexOnly; //!< Precision level used.
  bool UsedTriangulation  = false;            //!< True if triangulation was used.
  bool UsedShapeTolerance = false;            //!< True if shape tolerances were applied.
};

#endif // _BRepGraphAlgo_BndBoxData_HeaderFile
