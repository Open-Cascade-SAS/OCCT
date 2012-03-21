// Created on: 2011-06-01
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
