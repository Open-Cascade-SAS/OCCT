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

#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_Data.hxx>

//=================================================================================================

Bnd_Box BRepGraph::CacheView::BoundingBox(BRepGraph_NodeId theNode) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theNode);
  if (aDef == nullptr)
    return Bnd_Box();

  return aDef->Cache.BoundingBox.Get([&]() -> Bnd_Box {
    Bnd_Box aBox;
    myGraph->collectVertexPoints(theNode, aBox);
    return aBox;
  });
}

//=================================================================================================

gp_Pnt BRepGraph::CacheView::Centroid(BRepGraph_NodeId theNode) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theNode);
  if (aDef == nullptr)
    return gp_Pnt();

  return aDef->Cache.Centroid.Get([&]() -> gp_Pnt {
    Bnd_Box aBox = BoundingBox(theNode);
    if (aBox.IsVoid())
      return gp_Pnt();
    double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    return gp_Pnt((aXmin + aXmax) * 0.5, (aYmin + aYmax) * 0.5, (aZmin + aZmax) * 0.5);
  });
}

//=================================================================================================

void BRepGraph::CacheView::Invalidate(BRepGraph_NodeId theNode) const
{
  BRepGraph_NodeCache* aCache = myGraph->mutableCache(theNode);
  if (aCache != nullptr)
    aCache->InvalidateAll();
}

//=================================================================================================

void BRepGraph::CacheView::InvalidateSubgraph(BRepGraph_NodeId theNode) const
{
  myGraph->invalidateSubgraphImpl(theNode);
}
