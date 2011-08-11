// File:        BRepMesh_VertexInspector.cxx
// Created:     Jun 1 18:32:12 2011
// Author:      Oleg AGASHIN
// Copyright:   Open CASCADE SAS 2011

#include <gp_XY.hxx>
#include <Precision.hxx>
#include <BRepMesh_VertexInspector.hxx>
#include <BRepMesh_Vertex.hxx>


//=======================================================================
//function : BRepMesh_VertexInspector
//purpose  : Constructor
//
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
