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

#include <BRepGraphInc_Storage.hxx>

//=================================================================================================

void BRepGraphInc_Storage::Clear()
{
  Vertices.Clear();
  Edges.Clear();
  Wires.Clear();
  Faces.Clear();
  Shells.Clear();
  Solids.Clear();
  Compounds.Clear();
  CompSolids.Clear();
  ReverseIdx.Clear();
  TShapeToNodeId.Clear();
  OriginalShapes.Clear();
  IsDone = false;
}

//=================================================================================================

void BRepGraphInc_Storage::BuildReverseIndex()
{
  ReverseIdx.Build(Edges, Wires, Faces, Shells, Solids);
}
