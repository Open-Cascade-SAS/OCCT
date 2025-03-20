// Created on: 2016-04-19
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_DelaunayBaseMeshAlgo.hxx>
#include <BRepMesh_MeshTool.hxx>
#include <BRepMesh_Delaun.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_DelaunayBaseMeshAlgo, BRepMesh_ConstrainedBaseMeshAlgo)

//=================================================================================================

BRepMesh_DelaunayBaseMeshAlgo::BRepMesh_DelaunayBaseMeshAlgo() {}

//=================================================================================================

BRepMesh_DelaunayBaseMeshAlgo::~BRepMesh_DelaunayBaseMeshAlgo() {}

//=================================================================================================

void BRepMesh_DelaunayBaseMeshAlgo::generateMesh(const Message_ProgressRange& theRange)
{
  const Handle(BRepMesh_DataStructureOfDelaun)& aStructure = getStructure();
  const Handle(VectorOfPnt)&                    aNodesMap  = getNodesMap();

  IMeshData::VectorOfInteger aVerticesOrder(aNodesMap->Size(), getAllocator());
  for (Standard_Integer i = 1; i <= aNodesMap->Size(); ++i)
  {
    aVerticesOrder.Append(i);
  }

  std::pair<Standard_Integer, Standard_Integer> aCellsCount = getCellsCount(aVerticesOrder.Size());
  BRepMesh_Delaun   aMesher(aStructure, aVerticesOrder, aCellsCount.first, aCellsCount.second);
  BRepMesh_MeshTool aCleaner(aStructure);
  aCleaner.EraseFreeLinks();

  if (!theRange.More())
  {
    return;
  }
  postProcessMesh(aMesher, theRange);
}
