// Created on: 2011-06-02
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


#include <BRepMesh_VertexTool.ixx>
#include <gp_XY.hxx>
#include <Precision.hxx>
#include <BRepMesh_Vertex.hxx>
#include <BRepMesh_VertexInspector.hxx>
#include <BRepMesh_BaseAllocator.hxx>

//=======================================================================
//function : BRepMesh_VertexTool
//purpose  : 
//=======================================================================
BRepMesh_VertexTool::BRepMesh_VertexTool(const BRepMesh_BaseAllocator& theAlloc)
: myAllocator(theAlloc),
  myCellFilter(0., myAllocator),
  mySelector(64,myAllocator),
  myTol(0,1)
{
  SetCellSize ( Precision::Confusion()+0.05*Precision::Confusion() );
  SetTolerance( Precision::Confusion(), Precision::Confusion() );
}

//=======================================================================
//function : BRepMesh_VertexTool
//purpose  : 
//=======================================================================
BRepMesh_VertexTool::BRepMesh_VertexTool(const Standard_Integer        nbComp,
                                         const BRepMesh_BaseAllocator& theAlloc)
                                         : myAllocator(theAlloc),
                                           myCellFilter(0., myAllocator),
                                           mySelector(Max(nbComp,64),myAllocator),
                                           myTol(0,1)
{
  SetCellSize ( Precision::Confusion()+0.05*Precision::Confusion() );
  SetTolerance( Precision::Confusion(), Precision::Confusion() );
}

//=======================================================================
//function : SetCellSize
//purpose  : 
//=======================================================================
void BRepMesh_VertexTool::SetCellSize(const Standard_Real theSize)
{
  myCellFilter.Reset(theSize, myAllocator);
  mySelector.Clear();
}

//=======================================================================
//function : SetCellSize
//purpose  : 
//=======================================================================
void BRepMesh_VertexTool::SetCellSize(const Standard_Real theXSize, 
                                      const Standard_Real theYSize)
{
  Standard_Real aCellSize[2];
  aCellSize[0] = theXSize;
  aCellSize[1] = theYSize;
  
  myCellFilter.Reset(aCellSize, myAllocator);
  mySelector.Clear();
}

//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================
void BRepMesh_VertexTool::SetTolerance(const Standard_Real theTol)
{
  mySelector.SetTolerance( theTol );
  myTol(0) = theTol;
  myTol(1) = theTol;
}

//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================
void BRepMesh_VertexTool::SetTolerance(const Standard_Real theTolX, const Standard_Real theTolY)
{
  mySelector.SetTolerance( theTolX, theTolY );
  myTol(0) = theTolX;
  myTol(1) = theTolY;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_VertexTool::Add(const BRepMesh_Vertex& theVertex)
{
  Standard_Integer anIndex = FindIndex(theVertex);
  if ( anIndex == 0 )
  {
    BRepMesh_ListOfInteger thelist(myAllocator);
    anIndex = Add(theVertex, thelist);
  }
  return anIndex;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_VertexTool::Add(const BRepMesh_Vertex& theVertex,
                                          const BRepMesh_ListOfInteger& theParams)
{
  Standard_Integer anIndex = mySelector.Add(theVertex);
  myLinksMap.Bind(anIndex, theParams);
  gp_XY aMinPnt, aMaxPnt;
  ExpandPoint(theVertex.Coord(), aMinPnt, aMaxPnt);
  myCellFilter.Add(anIndex, aMinPnt, aMaxPnt);
  return anIndex;
}

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
void  BRepMesh_VertexTool::Delete(const Standard_Integer theIndex)
{
  BRepMesh_Vertex& aV = mySelector.GetVertex(theIndex);
  gp_XY aMinPnt, aMaxPnt;
  ExpandPoint(aV.Coord(), aMinPnt, aMaxPnt);
  myCellFilter.Remove (theIndex, aMinPnt, aMaxPnt);
  mySelector.Delete(theIndex);
}

//=======================================================================
//function : RemoveLast
//purpose  : 
//=======================================================================
void  BRepMesh_VertexTool::RemoveLast()
{
  Standard_Integer aIndex = mySelector.GetNbVertices();
  Delete( aIndex );
}

//=======================================================================
//function : GetListOfDelNodes
//purpose  : 
//=======================================================================
const BRepMesh_ListOfInteger& BRepMesh_VertexTool::GetListOfDelNodes() const
{
  return mySelector.GetListOfDelNodes();
}

//=======================================================================
//function : FindIndex
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_VertexTool::FindIndex(const BRepMesh_Vertex& theVertex)
{
  mySelector.SetCurrent(theVertex.Coord());
  myCellFilter.Inspect (theVertex.Coord(), mySelector);
  return mySelector.GetCoincidentInd();
}

//=======================================================================
//function : FindKey
//purpose  : 
//=======================================================================
const BRepMesh_Vertex& BRepMesh_VertexTool::FindKey(const Standard_Integer theIndex)
{
  return mySelector.GetVertex(theIndex);
}

//=======================================================================
//function : Substitute
//purpose  : 
//=======================================================================
void BRepMesh_VertexTool::Substitute(const Standard_Integer Index,
                                     const BRepMesh_Vertex& theVertex,
                                     const BRepMesh_ListOfInteger& theData)
{
  BRepMesh_Vertex& aV = mySelector.GetVertex(Index);
  gp_XY aMinPnt, aMaxPnt;
  ExpandPoint(aV.Coord(), aMinPnt, aMaxPnt);
  myCellFilter.Remove (Index, aMinPnt, aMaxPnt);
  aV = theVertex;
  ExpandPoint(aV.Coord(), aMinPnt, aMaxPnt);
  myCellFilter.Add(Index, aMinPnt, aMaxPnt);
  FindFromIndex(Index) = theData;
}

//=======================================================================
//function : Extent
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_VertexTool::Extent() const
{
  return mySelector.GetNbVertices();
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_VertexTool::IsEmpty() const
{
  return mySelector.GetNbVertices() == 0;
}

//=======================================================================
//function : FindFromIndex
//purpose  : 
//=======================================================================
BRepMesh_ListOfInteger& BRepMesh_VertexTool::FindFromIndex(const Standard_Integer theIndex) const
{
  return (BRepMesh_ListOfInteger&) myLinksMap.Find(theIndex);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void BRepMesh_VertexTool::ExpandPoint(const gp_XY& thePnt, gp_XY& theMinPnt, gp_XY& theMaxPnt)
{
  theMinPnt.SetX(thePnt.X() - myTol(0));
  theMinPnt.SetY(thePnt.Y() - myTol(1));
  theMaxPnt.SetX(thePnt.X() + myTol(0));
  theMaxPnt.SetY(thePnt.Y() + myTol(1));
}

//=======================================================================
//function : Statistics
//purpose  : 
//=======================================================================
void BRepMesh_VertexTool::Statistics(Standard_OStream& S) const
{
  S <<"\nStructure Statistics\n---------------\n\n";
  S <<"This structure has "<<mySelector.GetNbVertices()<<" Nodes\n\n";
}
