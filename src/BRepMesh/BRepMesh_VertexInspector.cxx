// Created on: 2011-06-01
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <gp_XY.hxx>
#include <Precision.hxx>
#include <BRepMesh_VertexInspector.hxx>
#include <BRepMesh_Vertex.hxx>


//=======================================================================
//function : BRepMesh_VertexInspector
//purpose  : Constructor
//=======================================================================

BRepMesh_VertexInspector::BRepMesh_VertexInspector (const Standard_Integer nbComp,
                                                    const BRepMesh_BaseAllocator& theAlloc)
                                                    : myTol(0,1),
                                                    myResInd(theAlloc),
                                                    myVertices(nbComp),
                                                    myDelNodes(theAlloc)
{
  SetTolerance( Precision::Confusion() );
}

BRepMesh_VertexInspector::BRepMesh_VertexInspector (const Standard_Integer nbComp,
                                                    const Standard_Real    aTol,
                                                    const BRepMesh_BaseAllocator& theAlloc)
                                                    : myTol(0,1),
                                                    myResInd(theAlloc),
                                                    myVertices(nbComp),
                                                    myDelNodes(theAlloc)
{
  SetTolerance( aTol );
}

BRepMesh_VertexInspector::BRepMesh_VertexInspector (const Standard_Integer nbComp,
                                                    const Standard_Real    aTolX,
                                                    const Standard_Real    aTolY,
                                                    const BRepMesh_BaseAllocator& theAlloc)
                                                    : myTol(0,1),
                                                    myResInd(theAlloc),
                                                    myVertices(nbComp),
                                                    myDelNodes(theAlloc)
{
  SetTolerance( aTolX, aTolY );
}

//=======================================================================
//function : Inspect
//purpose  : 
//
//=======================================================================
NCollection_CellFilter_Action BRepMesh_VertexInspector::Inspect (const Standard_Integer theTarget)
{
  const BRepMesh_Vertex& aVertex = myVertices(theTarget-1);
  if( aVertex.Movability() == BRepMesh_Deleted )
  {
    myDelNodes.Append(theTarget);
    return CellFilter_Purge;
  }
  
  const gp_XY& aPos = aVertex.Coord();
  Standard_Real dx,dy;
  dx = myCurrent.X() - aPos.X();
  dy = myCurrent.Y() - aPos.Y();
  
  Standard_Boolean inTol;
  if ( myTol(1) == 0. )
  {
    inTol = (dx*dx + dy*dy) <= myTol(0);
  }
  else
  {
    inTol = ( (dx*dx) <= myTol(0) ) && 
            ( (dy*dy) <= myTol(1) );
  }
  if ( inTol )
    myResInd.Append(theTarget);
  return CellFilter_Keep;
}

//=======================================================================
//function : Add
//purpose  : 
//
//=======================================================================
Standard_Integer BRepMesh_VertexInspector::Add(const BRepMesh_Vertex& theVertex)
{
  if( myDelNodes.IsEmpty() )
  {
    myVertices.Append(theVertex);
    return myVertices.Length();
  }
  
  Standard_Integer aNodeIndex = myDelNodes.First();
  myVertices(aNodeIndex-1) = theVertex;
  myDelNodes.RemoveFirst();
  return aNodeIndex;
}
