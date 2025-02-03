// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <RWMesh_TriangulationSource.hxx>

#include <RWMesh_TriangulationReader.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWMesh_TriangulationSource, Poly_Triangulation)

//=================================================================================================

RWMesh_TriangulationSource::RWMesh_TriangulationSource()
    : myNbDefNodes(0),
      myNbDefTriangles(0),
      myStatisticOfDegeneratedTriNb(0)
{
}

//=================================================================================================

RWMesh_TriangulationSource::~RWMesh_TriangulationSource()
{
  //
}

//=================================================================================================

Standard_Boolean RWMesh_TriangulationSource::loadDeferredData(
  const Handle(OSD_FileSystem)&     theFileSystem,
  const Handle(Poly_Triangulation)& theDestTriangulation) const
{
  myStatisticOfDegeneratedTriNb = 0;
  if (myReader.IsNull())
  {
    return false;
  }
  if (myReader->Load(this, theDestTriangulation, theFileSystem))
  {
    return true;
  }
  return false;
}

//=================================================================================================

void RWMesh_TriangulationSource::ResizeEdges(Standard_Integer theNbEdges,
                                             Standard_Boolean theToCopyOld)
{
  myEdges.Resize(1, theNbEdges, theToCopyOld);
}
