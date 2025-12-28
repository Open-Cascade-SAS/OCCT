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
#include <TopOpeBRepBuild_VertexInfo.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

#include <stdio.h>

//=================================================================================================

TopOpeBRepBuild_VertexInfo::TopOpeBRepBuild_VertexInfo()
{
  mySmart    = false;
  myFoundOut = 0;
  myEdgesPassed.Clear();
}

//=================================================================================================

void TopOpeBRepBuild_VertexInfo::SetVertex(const TopoDS_Vertex& aV)
{
  myVertex = aV;
}

//=================================================================================================

const TopoDS_Vertex& TopOpeBRepBuild_VertexInfo::Vertex() const
{
  return myVertex;
}

//=================================================================================================

void TopOpeBRepBuild_VertexInfo::SetSmart(const bool aFlag)
{
  mySmart = aFlag;
}

//=================================================================================================

bool TopOpeBRepBuild_VertexInfo::Smart() const
{
  return mySmart;
}

//=================================================================================================

int TopOpeBRepBuild_VertexInfo::NbCases() const
{ // myCurrentIn
  return myLocalEdgesOut.Extent();
}

//=================================================================================================

int TopOpeBRepBuild_VertexInfo::FoundOut() const
{
  return myFoundOut;
}

//=================================================================================================

void TopOpeBRepBuild_VertexInfo::AddIn(const TopoDS_Edge& anE)
{
  myEdgesIn.Add(anE);
}

//=================================================================================================

void TopOpeBRepBuild_VertexInfo::AddOut(const TopoDS_Edge& anE)
{
  myEdgesOut.Add(anE);
}

//=================================================================================================

void TopOpeBRepBuild_VertexInfo::SetCurrentIn(const TopoDS_Edge& anE)
{
  myCurrentIn = anE;
}

//=================================================================================================

const NCollection_IndexedMap<TopoDS_Shape>& TopOpeBRepBuild_VertexInfo::EdgesIn() const
{
  return myEdgesIn;
}

//=================================================================================================

const NCollection_IndexedMap<TopoDS_Shape>& TopOpeBRepBuild_VertexInfo::EdgesOut() const
{
  return myEdgesOut;
}

//=================================================================================================

NCollection_IndexedMap<TopoDS_Shape>& TopOpeBRepBuild_VertexInfo::ChangeEdgesOut()
{
  return myEdgesOut;
}

//=================================================================================================

void TopOpeBRepBuild_VertexInfo::Dump() const
{
  printf(" *** Dump the Vertex Info ***\n");
  printf(" mySmart  : %d\n", (mySmart ? 0 : 1));
  printf(" Edges    : %d In, %d Out\n", myEdgesIn.Extent(), myEdgesOut.Extent());

  printf("\n");
}

//=================================================================================================

void TopOpeBRepBuild_VertexInfo::AppendPassed(const TopoDS_Edge& anE)
{
  myEdgesPassed.Prepend(anE);
}

//=================================================================================================

void TopOpeBRepBuild_VertexInfo::RemovePassed()
{
  myEdgesPassed.RemoveFirst();
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_VertexInfo::ListPassed() const
{
  return myEdgesPassed;
}

//=================================================================================================

void TopOpeBRepBuild_VertexInfo::Prepare(const NCollection_List<TopoDS_Shape>& aL)
{
  myLocalEdgesOut.Clear();

  NCollection_IndexedMap<TopoDS_Shape> tmpMap;

  NCollection_List<TopoDS_Shape>::Iterator anIt(aL);
  for (; anIt.More(); anIt.Next())
  {
    tmpMap.Add(anIt.Value());
  }

  int i = 1, nb = myEdgesOut.Extent();
  for (; i <= nb; i++)
  {
    const TopoDS_Shape& aE = myEdgesOut(i);
    if (!tmpMap.Contains(aE))
      myLocalEdgesOut.Add(aE);
  }

  tmpMap.Clear();
}

//=================================================================================================

const TopoDS_Edge& TopOpeBRepBuild_VertexInfo::CurrentOut()
{

  int i, aNbOut;
  aNbOut = myLocalEdgesOut.Extent();

  NCollection_IndexedMap<TopoDS_Shape> aMapPassed;
  NCollection_List<TopoDS_Shape>::Iterator anIt(myEdgesPassed);
  for (; anIt.More(); anIt.Next())
  {
    aMapPassed.Add(anIt.Value());
  }

  for (i = 1; i <= aNbOut; i++)
  {
    if (!aMapPassed.Contains(myLocalEdgesOut(i)))
    {
      myCurrent  = TopoDS::Edge(myLocalEdgesOut(i));
      myFoundOut = 1;
      return myCurrent;
    }
  }
  myFoundOut = 0;
  TopoDS_Edge aS;
  myCurrent = aS;
  return myCurrent;
}
