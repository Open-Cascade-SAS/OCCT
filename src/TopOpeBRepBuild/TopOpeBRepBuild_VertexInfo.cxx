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


#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_VertexInfo.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <stdio.h>
//=======================================================================
// function :TopOpeBRepBuild_VertexInfo
// purpose: 
//=======================================================================
TopOpeBRepBuild_VertexInfo::TopOpeBRepBuild_VertexInfo()
{
  mySmart=Standard_False;
  myFoundOut=0;
  myEdgesPassed.Clear();
}
//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::SetVertex
// purpose: 
//=======================================================================
  void TopOpeBRepBuild_VertexInfo::SetVertex(const TopoDS_Vertex& aV) 
{
  myVertex=aV;
}
//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::Vertex
// purpose: 
//=======================================================================
  const TopoDS_Vertex& TopOpeBRepBuild_VertexInfo::Vertex() const
{
  return myVertex;
}
//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::SetSmart
// purpose: 
//=======================================================================
  void TopOpeBRepBuild_VertexInfo::SetSmart(const Standard_Boolean aFlag) 
{
  mySmart=aFlag;
}
//=======================================================================
// function :TopOpeBRepBuild_VertexInfo:Smart
// purpose: 
//=======================================================================
   Standard_Boolean TopOpeBRepBuild_VertexInfo::Smart() const
{
  return mySmart;
}
//=======================================================================
// function :TopOpeBRepBuild_VertexInfo:NbCases
// purpose: 
//=======================================================================
   Standard_Integer TopOpeBRepBuild_VertexInfo::NbCases() const
{//myCurrentIn
  return myLocalEdgesOut.Extent();
}

//=======================================================================
// function :TopOpeBRepBuild_VertexInfo:FoundOut
// purpose: 
//=======================================================================
   Standard_Integer TopOpeBRepBuild_VertexInfo::FoundOut () const
{
  return myFoundOut;
}

//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::AddIn
// purpose: 
//=======================================================================
  void TopOpeBRepBuild_VertexInfo::AddIn(const TopoDS_Edge& anE) 
{
  myEdgesIn.Add(anE);
}

//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::AddOut
// purpose: 
//=======================================================================
  void TopOpeBRepBuild_VertexInfo::AddOut(const TopoDS_Edge& anE) 
{
  myEdgesOut.Add(anE);
}

//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::SetCurrentIn
// purpose: 
//=======================================================================
  void TopOpeBRepBuild_VertexInfo::SetCurrentIn(const TopoDS_Edge& anE) 
{
  myCurrentIn=anE;
}

//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::EdgesIn
// purpose: 
//=======================================================================
  const TopTools_IndexedMapOfOrientedShape& TopOpeBRepBuild_VertexInfo::EdgesIn() const  
{
  return myEdgesIn;
}

//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::EdgesOut
// purpose: 
//=======================================================================
  const TopTools_IndexedMapOfOrientedShape& TopOpeBRepBuild_VertexInfo::EdgesOut() const  
{
  return myEdgesOut;
}

//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::ChangeEdgesOut
// purpose: 
//=======================================================================
  TopTools_IndexedMapOfOrientedShape& TopOpeBRepBuild_VertexInfo::ChangeEdgesOut()
{
  return myEdgesOut;
}

//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::Dump
// purpose: 
//=======================================================================
  void TopOpeBRepBuild_VertexInfo::Dump() const
{
  printf(" *** Dump the Vertex Info ***\n");
  printf(" mySmart  : %d\n", (mySmart ? 0 : 1));
  printf(" Edges    : %d In, %d Out\n", myEdgesIn.Extent(), myEdgesOut.Extent());
  
  
  printf("\n");
}

//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::AppendPassed
// purpose: 
//=======================================================================
  void TopOpeBRepBuild_VertexInfo::AppendPassed(const TopoDS_Edge& anE) 
{
  myEdgesPassed.Prepend(anE);
}


//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::RemovePassed
// purpose: 
//=======================================================================
  void TopOpeBRepBuild_VertexInfo::RemovePassed() 
{
  myEdgesPassed.RemoveFirst();
}
//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::ListPassed
// purpose: 
//=======================================================================
  const TopTools_ListOfShape&  TopOpeBRepBuild_VertexInfo::ListPassed() const 
{
  return myEdgesPassed;
}
//=======================================================================
// function :TopOpeBRepBuild_VertexInfo::Prepare
// purpose: 
//=======================================================================
  void TopOpeBRepBuild_VertexInfo::Prepare(const TopTools_ListOfShape& aL)  
{
  myLocalEdgesOut.Clear();

  TopTools_IndexedMapOfOrientedShape tmpMap;
  
  TopTools_ListIteratorOfListOfShape anIt(aL);
  for (; anIt.More(); anIt.Next()) {
    tmpMap.Add(anIt.Value());
  }

  Standard_Integer i = 1, nb = myEdgesOut.Extent();
  for(; i <= nb; i++) {
    const TopoDS_Shape& aE = myEdgesOut(i);
    if(!tmpMap.Contains(aE))
      myLocalEdgesOut.Add(aE);
  }

  tmpMap.Clear();
}

//=======================================================================
// function :TopOpeBRepBuild_VertexInfo:CurrentOut
// purpose: 
//=======================================================================
  const TopoDS_Edge& TopOpeBRepBuild_VertexInfo::CurrentOut () 
{

  Standard_Integer i, aNbOut;
  aNbOut =myLocalEdgesOut.Extent();
  
  TopTools_IndexedMapOfOrientedShape aMapPassed;
  TopTools_ListIteratorOfListOfShape anIt(myEdgesPassed);
  for (; anIt.More(); anIt.Next()) {
    aMapPassed.Add (anIt.Value());
  }

  for (i=1; i<=aNbOut; i++) {
    if (!aMapPassed.Contains(myLocalEdgesOut(i))) {
      myCurrent=TopoDS::Edge(myLocalEdgesOut(i));
      myFoundOut=1;
      return myCurrent;
    }
  }
  myFoundOut=0;
  TopoDS_Edge aS;
  myCurrent=aS;
  return myCurrent;
}





