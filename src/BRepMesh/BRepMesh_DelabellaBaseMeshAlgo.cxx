// Created on: 2019-07-05
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <BRepMesh_DelabellaBaseMeshAlgo.hxx>

#include <BRepMesh_MeshTool.hxx>
#include <BRepMesh_Delaun.hxx>
#include <Message.hxx>

#include <string.h>
#include <stdarg.h>

#include "delabella.pxx"

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_DelabellaBaseMeshAlgo, BRepMesh_CustomBaseMeshAlgo)

namespace
{
  //! Redirect algorithm messages to OCCT messenger.
  static int logDelabella2Occ (void* theStream, const char* theFormat, ...)
  {
    (void )theStream;
    char aBuffer[1024]; // should be more than enough for Delabella messages

    va_list anArgList;
    va_start(anArgList, theFormat);
    Vsprintf(aBuffer, theFormat, anArgList);
    va_end(anArgList);

    Message_Gravity aGravity = Message_Warning;
    switch ((int )theFormat[1])
    {
      case int('E'): aGravity = Message_Fail;  break; // [ERR]
      case int('W'): aGravity = Message_Trace; break; // [WRN]
      case int('N'): aGravity = Message_Trace; break; // [NFO]
    }
    Message::Send (aBuffer, aGravity);
    return 0;
  }
}

//=======================================================================
// Function: Constructor
// Purpose :
//=======================================================================
BRepMesh_DelabellaBaseMeshAlgo::BRepMesh_DelabellaBaseMeshAlgo ()
{
}

//=======================================================================
// Function: Destructor
// Purpose :
//=======================================================================
BRepMesh_DelabellaBaseMeshAlgo::~BRepMesh_DelabellaBaseMeshAlgo ()
{
}

//=======================================================================
//function : buildBaseTriangulation
//purpose  :
//=======================================================================
void BRepMesh_DelabellaBaseMeshAlgo::buildBaseTriangulation()
{
  const Handle(BRepMesh_DataStructureOfDelaun)& aStructure = this->getStructure();

  Bnd_B2d aBox;
  const Standard_Integer aNodesNb = aStructure->NbNodes ();
  std::vector<Standard_Real> aPoints (2 * (aNodesNb + 4));
  for (Standard_Integer aNodeIt = 0; aNodeIt < aNodesNb; ++aNodeIt)
  {
    const BRepMesh_Vertex& aVertex = aStructure->GetNode (aNodeIt + 1);

    const size_t aBaseIdx = 2 * static_cast<size_t> (aNodeIt);
    aPoints[aBaseIdx + 0] = aVertex.Coord ().X ();
    aPoints[aBaseIdx + 1] = aVertex.Coord ().Y ();

    aBox.Add (gp_Pnt2d(aVertex.Coord ()));
  }

  aBox.Enlarge (0.1 * (aBox.CornerMax () - aBox.CornerMin ()).Modulus ());
  const gp_XY aMin = aBox.CornerMin ();
  const gp_XY aMax = aBox.CornerMax ();

  aPoints[2 * aNodesNb + 0] = aMin.X ();
  aPoints[2 * aNodesNb + 1] = aMin.Y ();
  aStructure->AddNode (BRepMesh_Vertex (
    aPoints[2 * aNodesNb + 0],
    aPoints[2 * aNodesNb + 1], BRepMesh_Free));

  aPoints[2 * aNodesNb + 2] = aMax.X ();
  aPoints[2 * aNodesNb + 3] = aMin.Y ();
  aStructure->AddNode (BRepMesh_Vertex (
    aPoints[2 * aNodesNb + 2],
    aPoints[2 * aNodesNb + 3], BRepMesh_Free));

  aPoints[2 * aNodesNb + 4] = aMax.X ();
  aPoints[2 * aNodesNb + 5] = aMax.Y ();
  aStructure->AddNode (BRepMesh_Vertex (
    aPoints[2 * aNodesNb + 4],
    aPoints[2 * aNodesNb + 5], BRepMesh_Free));

  aPoints[2 * aNodesNb + 6] = aMin.X ();
  aPoints[2 * aNodesNb + 7] = aMax.Y ();
  aStructure->AddNode (BRepMesh_Vertex (
    aPoints[2 * aNodesNb + 6],
    aPoints[2 * aNodesNb + 7], BRepMesh_Free));

  const Standard_Real aDiffX = (aMax.X () - aMin.X ());
  const Standard_Real aDiffY = (aMax.Y () - aMin.Y ());
  for (size_t i = 0; i < aPoints.size(); i += 2)
  {
    aPoints[i + 0] = (aPoints[i + 0] - aMin.X ()) / aDiffX - 0.5;
    aPoints[i + 1] = (aPoints[i + 1] - aMin.Y ()) / aDiffY - 0.5;
  }

  IDelaBella* aTriangulator = IDelaBella::Create();
  if (aTriangulator == NULL) // should never happen
  {
    throw Standard_ProgramError ("BRepMesh_DelabellaBaseMeshAlgo::buildBaseTriangulation: unable creating a triangulation algorithm");
  }

  aTriangulator->SetErrLog (logDelabella2Occ, NULL);
  try
  {
    const int aVerticesNb = aTriangulator->Triangulate (
      static_cast<int>(aPoints.size () / 2),
      &aPoints[0], &aPoints[1], 2 * sizeof (Standard_Real));

    if (aVerticesNb > 0)
    {
      const DelaBella_Triangle* aTrianglePtr = aTriangulator->GetFirstDelaunayTriangle();
      while (aTrianglePtr != NULL)
      {
        Standard_Integer aNodes[3] = {
          aTrianglePtr->v[0]->i + 1,
          aTrianglePtr->v[2]->i + 1,
          aTrianglePtr->v[1]->i + 1
        };

        Standard_Integer aEdges       [3];
        Standard_Boolean aOrientations[3];
        for (Standard_Integer k = 0; k < 3; ++k)
        {
          const BRepMesh_Edge aLink (aNodes[k], aNodes[(k + 1) % 3], BRepMesh_Free);

          const Standard_Integer aLinkInfo = aStructure->AddLink (aLink);
          aEdges       [k] = Abs (aLinkInfo);
          aOrientations[k] = aLinkInfo > 0;
        }

        const BRepMesh_Triangle aTriangle (aEdges, aOrientations, BRepMesh_Free);
        aStructure->AddElement (aTriangle);

        aTrianglePtr = aTrianglePtr->next;
      }
    }

    aTriangulator->Destroy ();
    aTriangulator = NULL;
  }
  catch (Standard_Failure const& theException)
  {
    if (aTriangulator != NULL)
    {
      aTriangulator->Destroy ();
      aTriangulator = NULL;
    }

    throw Standard_Failure (theException);
  }
  catch (...)
  {
    if (aTriangulator != NULL)
    {
      aTriangulator->Destroy ();
      aTriangulator = NULL;
    }

    throw Standard_Failure ("BRepMesh_DelabellaBaseMeshAlgo::buildBaseTriangulation: exception in triangulation algorithm");
  }
}
