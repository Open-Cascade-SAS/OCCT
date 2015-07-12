// Created on: 1999-11-29
// Created by: Peter KURNEV
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopOpeBRepBuild_Tools2d.hxx>
#include <TopOpeBRepBuild_VertexInfo.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <stdio.h>
static 
  void BuildPath (const TopoDS_Vertex& myVertex0,
		  const TopoDS_Edge&   myEdge,
		  const TopoDS_Vertex& myVertex,
		  const Standard_Integer aNbEdges,
		  TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo& M,
		  Standard_Integer anEdgesCount,
		  Standard_Integer& aBreakFlag,
		  TopTools_ListOfShape& myResList);

//=======================================================================
// function :TopOpeBRepBuild_Tools2d::Path
// purpose: 
//=======================================================================
  void TopOpeBRepBuild_Tools2d::Path(const TopoDS_Wire& aWire, TopTools_ListOfShape& aResList)
{
  Standard_Integer anEdgesCount=0, aNbEdges=0, aBreakFlag=0;
  TopTools_ListOfShape myResList;
  TopoDS_Vertex  myVertex, myVertex0;
  TopoDS_Edge    myEdge, aNullEdge;

  TopExp_Explorer ex(aWire,TopAbs_EDGE);
  for (; ex.More(); ex.Next()) aNbEdges++;
  
  myResList.Clear();

  TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo M;
  TopOpeBRepBuild_Tools2d::MakeMapOfShapeVertexInfo(aWire, M);
  
  
  myEdge=aNullEdge;
  myVertex0=TopoDS::Vertex(M.FindKey(1));
  myVertex=myVertex0;

  BuildPath (myVertex0, myEdge, myVertex, aNbEdges, M, anEdgesCount, aBreakFlag, myResList);
  //
  aResList.Clear();
  aResList=myResList;
}
//=======================================================================
// function :BuildPath
// purpose: 
//=======================================================================
  void BuildPath (const TopoDS_Vertex& myVertex0,
		  const TopoDS_Edge&   myInputEdge,
		  const TopoDS_Vertex& myInputVertex,
		  const Standard_Integer aNbEdges,
		  TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo& M,
		  Standard_Integer anEdgesCount,
		  Standard_Integer& aBreakFlag, 
		  TopTools_ListOfShape& myResList)
{
  Standard_Integer  j = 1, aFoundOut, aNbCases, stopFlag=0;
  TopoDS_Edge myEdge;
  TopoDS_Vertex myVertex;

  if (aBreakFlag==1) return;
  
  TopOpeBRepBuild_VertexInfo& aVInfo=M.ChangeFromKey(myInputVertex);
  //
  aVInfo.SetCurrentIn (myInputEdge);
  aVInfo.Prepare(myResList);
  aNbCases=aVInfo.NbCases();
  if (!aNbCases) aBreakFlag=2; 

  for (j=1; j<=aNbCases; j++) {

    myEdge=aVInfo.CurrentOut();

    aFoundOut=aVInfo.FoundOut();
    if (!aFoundOut) { //FondOut=0 TUPICK
      aBreakFlag=2;
      return;
    }

    else {
      if (stopFlag) { // if previous path was wrong
	aVInfo.RemovePassed();
	myResList.RemoveFirst();
	stopFlag=0;
	anEdgesCount--;
      }

      aVInfo.AppendPassed (myEdge);
      myResList.Prepend (myEdge);
      anEdgesCount++;
      myVertex=(myEdge.Orientation()==TopAbs_FORWARD)? 
	TopExp::LastVertex(myEdge) : TopExp::FirstVertex(myEdge);
      
      if (myVertex.IsSame(myVertex0) && anEdgesCount==aNbEdges) {
	aBreakFlag=1;
	return;
      }
      
      BuildPath (myVertex0, myEdge, myVertex, aNbEdges, M, anEdgesCount, aBreakFlag, myResList);
      ////
      if (aBreakFlag==1) {
	return;
      }

      if (aBreakFlag==2) {// Come back
	if (j==aNbCases) {
	  aVInfo.RemovePassed();
	  myResList.RemoveFirst();
	  anEdgesCount--;
	  ////
	  return;
	}
	else {
	  stopFlag=1;
	  aBreakFlag=0; // Next j if possible
	}
      } // end of if (aBreakFlag==2) 
    } // end of else .i.e. aFoundOut#0
  } // end of for (j=1; j<=aNbCases; j++)
}

//=======================================================================
// function :TopOpeBRepBuild_Tools2d::MakeMapOfShapeVertexInfo
// purpose: 
//=======================================================================
  void TopOpeBRepBuild_Tools2d::MakeMapOfShapeVertexInfo(const TopoDS_Wire& aWire,
						      TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo& M)
{
  TopOpeBRepBuild_VertexInfo empty;
  TopExp_Explorer exa(aWire,TopAbs_EDGE);
  for (; exa.More(); exa.Next()) {
    const TopoDS_Edge& anEdge = TopoDS::Edge(exa.Current());
    TopExp_Explorer exs(anEdge, TopAbs_VERTEX);
    for (; exs.More(); exs.Next()) {
      const TopoDS_Vertex& aVertex= TopoDS::Vertex(exs.Current());
      Standard_Integer index = M.FindIndex(aVertex);
      if (!index) 
	index = M.Add(aVertex, empty);
      
      TopOpeBRepBuild_VertexInfo& aVInfo= M(index);
      aVInfo.SetVertex (aVertex);
      TopAbs_Orientation anOr=aVertex.Orientation();
      if      (anOr==TopAbs_FORWARD ) 	aVInfo.AddOut  (anEdge);
      else if (anOr==TopAbs_REVERSED)   aVInfo.AddIn   (anEdge);
    }
  }

  Standard_Integer i, aNb;
  aNb=M.Extent();
  for (i=1; i<=aNb; i++) {
    TopOpeBRepBuild_VertexInfo& aVInfo=M(i);
    const TopTools_IndexedMapOfOrientedShape& EdgesIn   =aVInfo.EdgesIn();
    const TopTools_IndexedMapOfOrientedShape& EdgesOut  =aVInfo.EdgesOut();
    Standard_Integer aNbEdgesIn, aNbEdgesOut;
    aNbEdgesIn   =EdgesIn.Extent();
    aNbEdgesOut  =EdgesOut.Extent();
    if (aNbEdgesIn!=1 && aNbEdgesOut!=1) {
      aVInfo.SetSmart (Standard_True);
    }
  }
}

//=======================================================================
// function :TopOpeBRepBuild_Tools2d::DumpMapOfShapeVertexInfo
// purpose: 
//=======================================================================
  void TopOpeBRepBuild_Tools2d::DumpMapOfShapeVertexInfo(const TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo& M)
{
  Standard_Integer i, aNb;
  aNb=M.Extent();
  for (i=1; i<=aNb; i++) {
    const TopOpeBRepBuild_VertexInfo& aVInfo=M(i);
    
    printf(" Vert.#%d, ", i);
    const TopTools_ListOfShape& aList=aVInfo.ListPassed();
    
    if (aList.Extent()) {
      TopTools_ListIteratorOfListOfShape anIt(aList);
      for (; anIt.More(); anIt.Next()) {
	printf("pass,");
      }
    }
    
    else {
      printf("none");
    }
    printf("\n");
  }
}

