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

#ifndef _BRepGraph_PCurveContext_HeaderFile
#define _BRepGraph_PCurveContext_HeaderFile

#include <Standard_HashUtils.hxx>
#include <TopAbs_Orientation.hxx>

#include <cstddef>
#include <functional>

//! Lightweight typed composite key identifying the unique context of a PCurve
//! within a BRepGraph: the (EdgeDef index, FaceDef index, Orientation) triple.
//!
//! PCurveContext is a value type: cheap to copy, compare, hash (12 bytes,
//! zero heap allocation).  It is usable as an NCollection_DataMap key via
//! the std::hash specialization below.
struct BRepGraph_PCurveContext
{
  int                EdgeDefIndex;
  int                FaceDefIndex;
  TopAbs_Orientation Orientation;

  BRepGraph_PCurveContext()
    : EdgeDefIndex(-1), FaceDefIndex(-1), Orientation(TopAbs_FORWARD) {}

  BRepGraph_PCurveContext(const int theEdge, const int theFace, const TopAbs_Orientation theOri)
    : EdgeDefIndex(theEdge), FaceDefIndex(theFace), Orientation(theOri) {}

  bool operator==(const BRepGraph_PCurveContext& theOther) const
  {
    return EdgeDefIndex == theOther.EdgeDefIndex
        && FaceDefIndex == theOther.FaceDefIndex
        && Orientation  == theOther.Orientation;
  }

  bool operator!=(const BRepGraph_PCurveContext& theOther) const
  { return !(*this == theOther); }
};

//! std::hash specialization for NCollection_DefaultHasher support.
template<>
struct std::hash<BRepGraph_PCurveContext>
{
  size_t operator()(const BRepGraph_PCurveContext& theCtx) const noexcept
  {
    size_t aCombination[3];
    aCombination[0] = opencascade::hash(theCtx.EdgeDefIndex);
    aCombination[1] = opencascade::hash(theCtx.FaceDefIndex);
    aCombination[2] = opencascade::hash(static_cast<int>(theCtx.Orientation));
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

#endif // _BRepGraph_PCurveContext_HeaderFile
