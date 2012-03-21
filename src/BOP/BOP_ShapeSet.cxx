// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#include <BOP_ShapeSet.ixx>

#include <Standard_ProgramError.hxx>

#include <TopExp_Explorer.hxx>

#include <TopAbs.hxx>


//=======================================================================
//function : BOP_ShapeSet::BOP_ShapeSet
//purpose  : 
//=======================================================================
BOP_ShapeSet::BOP_ShapeSet(const TopAbs_ShapeEnum SubShapeType)
: 
  mySubShapeType(SubShapeType)
{
  switch (SubShapeType) {
  case  TopAbs_EDGE:
    myShapeType = TopAbs_FACE;
    break;
  case TopAbs_VERTEX:
    myShapeType = TopAbs_EDGE;
    break;
  default:
    Standard_ProgramError::Raise("ShapeSet : bad ShapeType");
    break;
  }
}
//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
  void BOP_ShapeSet::Delete()
{}

//=======================================================================
//function : ClearContents
//purpose  : 
//=======================================================================
  void BOP_ShapeSet::ClearContents()
{
  myStartShapes.Clear();
  mySubShapeMap.Clear();
  myShapes.Clear();
  myCurrentShapeNeighbours.Clear();
}

//=======================================================================
//function : AddShape
//purpose  : 
//=======================================================================
  void BOP_ShapeSet::AddShape(const TopoDS_Shape& S)
{
  ProcessAddShape(S);
}

//=======================================================================
//function : AddStartElement
//purpose  : 
//=======================================================================
  void BOP_ShapeSet::AddStartElement(const TopoDS_Shape& S)
{
  ProcessAddStartElement(S);
}

//=======================================================================
//function : AddElement
//purpose  : 
//=======================================================================
  void BOP_ShapeSet::AddElement(const TopoDS_Shape& S)
{
  ProcessAddElement(S);
}

//=======================================================================
//function : ProcessAddShape
//purpose  : 
//=======================================================================
  void BOP_ShapeSet::ProcessAddShape(const TopoDS_Shape& S)
{
  myShapes.Append(S);
}

//=======================================================================
//function : ProcessAddStartElement
//purpose  : 
//=======================================================================
  void BOP_ShapeSet::ProcessAddStartElement(const TopoDS_Shape& S)
{
  TopTools_ListIteratorOfListOfShape anIt(myStartShapes);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aSInner=anIt.Value();
    if (aSInner==S) {
      return;
    }
  }
  myStartShapes.Append(S);
  ProcessAddElement(S);
}

//=======================================================================
//function : ProcessAddElement
//purpose  : 
//=======================================================================
  void BOP_ShapeSet::ProcessAddElement(const TopoDS_Shape& S)
{
  Standard_Boolean b;
  TopTools_ListOfShape  Lemp;
  
  TopExp_Explorer Ex(S, mySubShapeType);
  for (; Ex.More(); Ex.Next()) {
    const TopoDS_Shape& subshape = Ex.Current();
    b = ( ! mySubShapeMap.Contains(subshape) );
    if ( b ) {
      mySubShapeMap.Add(subshape, Lemp);
    }
    mySubShapeMap.ChangeFromKey(subshape).Append(S);
  }
}

//=======================================================================
//function : StartElements
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& BOP_ShapeSet::StartElements()const 
{
  return myStartShapes;
}

//=======================================================================
//function : InitShapes
//purpose  : 
//=======================================================================
  void  BOP_ShapeSet::InitShapes()
{
  myShapesIter.Initialize(myShapes);
}

//=======================================================================
//function : MoreShapes
//purpose  : 
//=======================================================================
  Standard_Boolean  BOP_ShapeSet::MoreShapes()const 
{
  Standard_Boolean b = myShapesIter.More();
  return b;
}

//=======================================================================
//function : NextShape
//purpose  : 
//=======================================================================
  void  BOP_ShapeSet::NextShape()
{
  myShapesIter.Next();
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
  const TopoDS_Shape&  BOP_ShapeSet::Shape()const 
{
  const TopoDS_Shape& S = myShapesIter.Value();
  return S;
}

//=======================================================================
//function : InitStartElements
//purpose  : 
//=======================================================================
  void  BOP_ShapeSet::InitStartElements()
{
  myStartShapesIter.Initialize(myStartShapes);
}

//=======================================================================
//function : MoreStartElements
//purpose  : 
//=======================================================================
  Standard_Boolean  BOP_ShapeSet::MoreStartElements()const 
{
  Standard_Boolean b = myStartShapesIter.More();
  return b;
}

//=======================================================================
//function : NextStartElement
//purpose  : 
//=======================================================================
  void  BOP_ShapeSet::NextStartElement()
{
  myStartShapesIter.Next();
}

//=======================================================================
//function : StartElement
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BOP_ShapeSet::StartElement()const 
{
  const TopoDS_Shape& S = myStartShapesIter.Value();
  return S;
}

//=======================================================================
//function : InitNeighbours
//purpose  : 
//=======================================================================
  void  BOP_ShapeSet::InitNeighbours(const TopoDS_Shape& S)
{
  mySubShapeExplorer.Init(S, mySubShapeType);
  myCurrentShape = S;
  FindNeighbours();
}

//=======================================================================
//function : MoreNeighbours
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_ShapeSet::MoreNeighbours()
{
  Standard_Boolean b = myIncidentShapesIter.More();
  return b;
}

//=======================================================================
//function : NextNeighbour
//purpose  : 
//=======================================================================
  void BOP_ShapeSet::NextNeighbour()
{
  Standard_Boolean noisimore, ssemore;
 
  myIncidentShapesIter.Next();
  noisimore = ! myIncidentShapesIter.More();
  if ( noisimore ) {
    ssemore = mySubShapeExplorer.More();
    if ( ssemore ) {
      mySubShapeExplorer.Next();
      FindNeighbours();
    }
  }
}

//=======================================================================
//function : Neighbour
//purpose  : 
//=======================================================================
  const TopoDS_Shape&  BOP_ShapeSet::Neighbour()const 
{
  const TopoDS_Shape& S = myIncidentShapesIter.Value();
  return S;
}

//=======================================================================
//function : ChangeStartShapes
//purpose  : 
//=======================================================================
  TopTools_ListOfShape& BOP_ShapeSet::ChangeStartShapes()
{
  return myStartShapes;
}

//=======================================================================
//function : FindNeighbours
//purpose  : 
//=======================================================================
  void BOP_ShapeSet::FindNeighbours()
{
  while (mySubShapeExplorer.More()) {
     // l = list of edges neighbour of edge myCurrentShape trough
    // the vertex mySubShapeExplorer.Current(), which is a vertex of the
    // edge myCurrentShape.
    const TopoDS_Shape& V = mySubShapeExplorer.Current();
    const TopTools_ListOfShape & l = MakeNeighboursList(myCurrentShape,V);
    // myIncidentShapesIter iterates on the neighbour edges of the edge
    // given as InitNeighbours() argument (this edge has been stored 
    // in the field myCurrentShape).
    myIncidentShapesIter.Initialize(l);
    if (myIncidentShapesIter.More()) {
      break;
    }    
    else {
      mySubShapeExplorer.Next();
    }
  }
}

//=======================================================================
//function : MakeNeighboursList
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape & BOP_ShapeSet::MakeNeighboursList(const TopoDS_Shape& ,//Earg, 
								const TopoDS_Shape& Varg)
{
  const TopTools_ListOfShape& l = mySubShapeMap.FindFromKey(Varg);
  return l;
}

//=======================================================================
//function : MaxNumberSubShape
//purpose  : 
//=======================================================================
  Standard_Integer BOP_ShapeSet::MaxNumberSubShape(const TopoDS_Shape& Shape)
{
  Standard_Integer i, m = 0;
 
  TopExp_Explorer SE(Shape, mySubShapeType);

  while(SE.More()) {
    const TopoDS_Shape& SubShape = SE.Current();
    if(!mySubShapeMap.Contains(SubShape)) {
      SE.Next();
      continue;
    }
    
    const TopTools_ListOfShape& l = mySubShapeMap.FindFromKey(SubShape);
    i=l.Extent();
    m = Max(m, i);
    SE.Next();
  }
  return m;
}


