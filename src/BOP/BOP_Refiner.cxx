// Created on: 2001-12-24
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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



#include <BOP_Refiner.ixx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>

#include <BRep_Builder.hxx>


//=======================================================================
// function: BOP_Refiner::BOP_Refiner
// purpose: 
//=======================================================================
BOP_Refiner::BOP_Refiner()
:
  myIsDone(Standard_False),
  myErrorStatus(1),
  myNbRemovedVertices(0),
  myNbRemovedEdges(0)
{}
//=======================================================================
// function: BOP_Refiner::BOP_Refiner
// purpose: 
//=======================================================================
  BOP_Refiner::BOP_Refiner(const TopoDS_Shape& aS)
:
  myIsDone(Standard_False),
  myErrorStatus(1),
  myNbRemovedVertices(0),
  myNbRemovedEdges(0)
{
  myShape=aS;
}

//=======================================================================
// function: SetShape
// purpose: 
//=======================================================================
  void BOP_Refiner::SetShape (const TopoDS_Shape& aS)
{
  myShape=aS;
}
//=======================================================================
// function: SetInternals
// purpose: 
//=======================================================================
  void BOP_Refiner::SetInternals (const TopTools_ListOfShape& aLS)
{
  myInternals.Clear();
  TopTools_ListIteratorOfListOfShape anIt(aLS);
  for(;  anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aS=anIt.Value();
    myInternals.Append(aS);
  }
}
//=======================================================================
// function: IsDone
// purpose: 
//=======================================================================
  Standard_Boolean BOP_Refiner::IsDone() const
{
  return myIsDone;
}
//=======================================================================
// function: ErrorStatus
// purpose: 
//=======================================================================
  Standard_Integer BOP_Refiner::ErrorStatus() const
{
  return myErrorStatus;
}
//=======================================================================
// function: Shape
// purpose: 
//=======================================================================
  const TopoDS_Shape& BOP_Refiner::Shape() const
{
  return myShape;
}
//=======================================================================
// function: NbRemovedVertices
// purpose: 
//=======================================================================
  Standard_Integer BOP_Refiner::NbRemovedVertices() const
{
  return myNbRemovedVertices;
}
//=======================================================================
// function: NbRemovedEdges
// purpose: 
//=======================================================================
  Standard_Integer BOP_Refiner::NbRemovedEdges() const
{
  return myNbRemovedEdges;
}
//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void BOP_Refiner::Do()
{
  DoInternals();
}
//=======================================================================
// function: DoInternals
// purpose: 
//=======================================================================
  void BOP_Refiner::DoInternals()
{
  Standard_Integer i, aNb, aNbF, aNbE;
  BRep_Builder aBB;
  TopTools_IndexedDataMapOfShapeListOfShape aMVF, aMFV, aMVE;
  TopTools_IndexedMapOfShape aMInternals;
  //
  // 0. Source Internals
  TopTools_ListIteratorOfListOfShape anItx(myInternals);
  for(;  anItx.More(); anItx.Next()) {
    const TopoDS_Shape& aS=anItx.Value();
    aMInternals.Add(aS);
  }
  //
  // 1. Vertices
  TopExp::MapShapesAndAncestors(myShape, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
  TopExp::MapShapesAndAncestors(myShape, TopAbs_VERTEX, TopAbs_FACE, aMVF);
  
  aNb=aMVF.Extent();
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Vertex& aV=TopoDS::Vertex(aMVF.FindKey(i));
    
    if (aV.Orientation()!=TopAbs_INTERNAL) {
      continue;
    }
    // how many edges attached to the vertex ?
    aNbE=aMVE.FindFromKey(aV).Extent();
    
    if (aNbE) {
      continue;
    }

    const TopTools_ListOfShape& aLF=aMVF(i);
    // how many edges attached to the vertex ?
    aNbF=aLF.Extent();
    if (aNbF==1) {
      const TopoDS_Face& aF=TopoDS::Face(aLF.First());
      
      if (aMFV.Contains(aF)) {
	TopTools_ListOfShape& aLFWithVertex=aMFV.ChangeFromKey(aF);
	aLFWithVertex.Append(aV);
      }
      else {
	TopTools_ListOfShape aLFWithVertex;
	aLFWithVertex.Append(aV);
	aMFV.Add(aF, aLFWithVertex);
      }
    }//  if (aNbF==1)
  }// for (i=1; i<=aNb; ++i)
  
  aNbF=aMFV.Extent();
  for (i=1; i<=aNbF; ++i) {
    const TopoDS_Face& aF=TopoDS::Face(aMFV.FindKey(i));
    TopoDS_Face* pF=(TopoDS_Face*)&aF;

    const TopTools_ListOfShape& aLV=aMFV(i);
    TopTools_ListIteratorOfListOfShape anIt(aLV);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Vertex& aV=TopoDS::Vertex(anIt.Value());
      //
      if (aMInternals.Contains(aV)) {
	continue;
      }
      //
      pF->Free(Standard_True);
      aBB.Remove(*pF, aV);
      myNbRemovedVertices++;
    }
  }
  //
  // 2. Edges
  TopTools_IndexedDataMapOfShapeListOfShape aMEF, aMFE;
  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, aMEF);
  
  aNb=aMEF.Extent();
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Edge& aE=TopoDS::Edge(aMEF.FindKey(i));
    if (aE.Orientation()!=TopAbs_INTERNAL) {
      continue;
    }
    const TopTools_ListOfShape& aLF=aMEF(i);
    aNbF=aLF.Extent();
    if (aNbF==1) {
      const TopoDS_Face& aF=TopoDS::Face(aLF.First());
      
      if (aMFE.Contains(aF)) {
	TopTools_ListOfShape& aLFWithEdge=aMFE.ChangeFromKey(aF);
	aLFWithEdge.Append(aE);
      }
      else {
	TopTools_ListOfShape aLFWithEdge;
	aLFWithEdge.Append(aE);
	aMFE.Add(aF, aLFWithEdge);
      }
    }//  if (aNbF==1)
  }// for (i=1; i<=aNb; ++i)
  //
  //modified by NIZNHY-PKV Wed Nov 03 14:27:22 2010f
  Standard_Boolean bFound;
  TopoDS_Iterator aItS;
  TopAbs_Orientation aOr;
  //modified by NIZNHY-PKV Wed Nov 03 14:27:24 2010t
  //
  aNbF=aMFE.Extent();
  for (i=1; i<=aNbF; ++i) {
    //modified by NIZNHY-PKV Wed Nov 03 14:29:03 2010f
    TopoDS_Face aF=TopoDS::Face(aMFE.FindKey(i));
    aOr=aF.Orientation();
    if (aOr==TopAbs_INTERNAL) {
      aF.Orientation(TopAbs_FORWARD);
    }
    //const TopoDS_Face& aF=TopoDS::Face(aMFE.FindKey(i));
    //modified by NIZNHY-PKV Wed Nov 03 14:29:06 2010t
    TopoDS_Face* pF=(TopoDS_Face*)&aF;
    
    const TopTools_ListOfShape& aLE=aMFE(i);
    TopTools_ListIteratorOfListOfShape anIt(aLE);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Edge& aE=TopoDS::Edge(anIt.Value());
      //
      if (aMInternals.Contains(aE)) {
	continue;
      }
      //
      TopTools_IndexedDataMapOfShapeListOfShape aMEW;
      TopExp::MapShapesAndAncestors(aF, TopAbs_EDGE, TopAbs_WIRE, aMEW);
      //
      if (aMEW.Contains(aE)) {
	const TopTools_ListOfShape& aLW=aMEW.FindFromKey(aE);
	TopTools_ListIteratorOfListOfShape aWIt(aLW);
	for (; aWIt.More(); aWIt.Next()) {
	  const TopoDS_Wire& aW=TopoDS::Wire(aWIt.Value());
	  //
	  //modified by NIZNHY-PKV Wed Nov 03 14:12:48 2010f
	  bFound=Standard_False;
	  aItS.Initialize(aW);
	  for(; aItS.More(); aItS.Next()) {
	    const TopoDS_Shape& aEW=aItS.Value();
	    if (aEW==aE) {
	      bFound=Standard_True;
	      break;
	    }
	  }
	  //
	  if (!bFound) {
	    continue;
	  }
	  //modified by NIZNHY-PKV Wed Nov 03 14:14:22 2010t
	  //
	  TopoDS_Wire* pW=(TopoDS_Wire*)&aW;
	  pW->Free(Standard_True);
	  //
	  TopExp_Explorer anExp(*pW, TopAbs_EDGE);
	  for (; anExp.More(); anExp.Next()) {
	    const TopoDS_Edge& anEdge=TopoDS::Edge(anExp.Current());
	    aBB.Remove(*pW, anEdge);
	    anExp.Init(*pW, TopAbs_EDGE);
	  }
	  //
	  pF->Free(Standard_True);
	  aBB.Remove(*pF, aW);
	  //modified by NIZNHY-PKV Wed Nov 03 14:29:56 2010f
	  pF->Orientation(aOr);
	  //modified by NIZNHY-PKV Wed Nov 03 14:29:59 2010t
	  myNbRemovedEdges++;
	}
      }
    }// next inernal edge 
  }// for (i=1; i<=aNbF; ++i)
  myIsDone=!myIsDone;
}
// myErrorStatus:
// 0 - OK
// 1 - Nothing has been done after constructor
// 
