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

#include <BRepGraphAlgo_Transform.hxx>

#include <BRepGraphAlgo_Copy.hxx>

#include <BRep_Builder.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

namespace
{

//! Apply a location change to a shape and all its sub-shapes recursively.
//! This is the cheapest transform mode: no geometry modification, just location.
TopoDS_Shape applyLocation(const TopoDS_Shape& theShape, const TopLoc_Location& theLoc)
{
  TopoDS_Shape aResult = theShape;
  aResult.Location(theShape.Location() * theLoc);
  return aResult;
}

} // namespace

//=================================================================================================

TopoDS_Shape BRepGraphAlgo_Transform::Perform(const BRepGraph& theGraph,
                                              const gp_Trsf&   theTrsf,
                                              bool             theCopyGeom)
{
  if (!theGraph.IsDone())
    return TopoDS_Shape();

  if (!theCopyGeom)
  {
    // Cheap mode: apply as TopLoc_Location change to the reconstructed shape.
    // First reconstruct, then apply location.
    TopoDS_Shape aReconstructed;
    if (theGraph.NbSolids() > 0)
    {
      aReconstructed =
        theGraph.ReconstructShape(BRepGraph_NodeId(BRepGraph_NodeKind::Solid, 0));
    }
    else if (theGraph.NbShells() > 0)
    {
      aReconstructed =
        theGraph.ReconstructShape(BRepGraph_NodeId(BRepGraph_NodeKind::Shell, 0));
    }
    else if (theGraph.NbFaces() > 0)
    {
      // Reconstruct all faces into a compound.
      BRep_Builder    aBB;
      TopoDS_Compound aComp;
      aBB.MakeCompound(aComp);
      for (int aFaceIdx = 0; aFaceIdx < theGraph.NbFaces(); ++aFaceIdx)
      {
        aBB.Add(aComp, theGraph.Face(aFaceIdx).OriginalShape);
      }
      aReconstructed = aComp;
    }
    else
    {
      return TopoDS_Shape();
    }

    TopLoc_Location aLoc(theTrsf);
    return applyLocation(aReconstructed, aLoc);
  }

  // Deep copy mode: copy geometry first, then transform the copy.
  TopoDS_Shape aCopy = BRepGraphAlgo_Copy::Perform(theGraph, true);
  if (aCopy.IsNull())
    return aCopy;

  // Apply location to the copied shape.
  TopLoc_Location aLoc(theTrsf);
  return applyLocation(aCopy, aLoc);
}

//=================================================================================================

TopoDS_Shape BRepGraphAlgo_Transform::TransformFace(const BRepGraph& theGraph,
                                                    int              theFaceIdx,
                                                    const gp_Trsf&   theTrsf,
                                                    bool             theCopyGeom)
{
  if (!theGraph.IsDone() || theFaceIdx < 0 || theFaceIdx >= theGraph.NbFaces())
    return TopoDS_Shape();

  TopoDS_Shape aFace;
  if (theCopyGeom)
  {
    aFace = BRepGraphAlgo_Copy::CopyFace(theGraph, theFaceIdx, true);
  }
  else
  {
    aFace = theGraph.Face(theFaceIdx).OriginalShape;
  }

  if (aFace.IsNull())
    return aFace;

  TopLoc_Location aLoc(theTrsf);
  return applyLocation(aFace, aLoc);
}
